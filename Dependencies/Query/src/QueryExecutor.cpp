#include "QueryExecutor.h"

#include <mutex>
#include <condition_variable>
#include <QTimer>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QNetworkCookieJar>
#include <QJsonObject>
#include <QCoreApplication>

#include <QLibrary>
#include <QUrlQuery>
#include <QPointer>

class rest::CancellationTokenImpl : public QObject
{
    Q_OBJECT
    
    std::mutex m_lock;
    bool m_isAborted = false;
    
    std::mutex m_cvLock;
    std::condition_variable m_cv;
    
    friend class QueryExecutor;
    friend class CancellationToken;
    
    std::atomic<int> m_unfinishedRepliesCount = 0;
    
    void Increase()
    {
        m_unfinishedRepliesCount.fetch_add(1);
    }
    void ConnectToReply(QNetworkReply* reply)
    {
        std::unique_lock<std::mutex> lock{m_lock};
        if(m_isAborted)
            reply->abort();
        else
            QObject::connect(this, &CancellationTokenImpl::sig_abort, reply, &QNetworkReply::abort);
    }
    
    void RemoveReply(QNetworkReply* reply)
    {
        m_unfinishedRepliesCount.fetch_sub(1);
    }
    
    bool notEmpty()
    {
        return m_unfinishedRepliesCount > 0;
    }
signals:
    void sig_abort();
    
public:
    void Abort()
    {
        std::unique_lock<std::mutex> lock{m_lock};
        m_isAborted = true;
        emit sig_abort();
    }
    
    void Wait()
    {
        while(notEmpty())
        {
           std::unique_lock<std::mutex> cvlock{m_cvLock};
           m_cv.wait(cvlock);
        }
    }
    
private:
    CancellationTokenImpl() {}
};


class AsyncRequestCompleteNotifier : public QObject
{
    Q_OBJECT
signals:
    void requestFinished(rest::Response resp);
    void requestCanceled();
};


namespace {
struct Cancellations{
    QHash<QThread*, QSet<rest::CancellationToken>> threadIdToCancellationToken;
    std::mutex mtx;
    
    Cancellations() {}
    Cancellations(const Cancellations& other) = delete;
    Cancellations* operator=(const Cancellations& other) = delete;
    
    bool AddCancellationToken(QThread* thread, rest::CancellationToken ct)
    {
        if(!thread)
            return true;
        
        std::unique_lock lock{mtx};
        
        if(thread->isInterruptionRequested())
            return true;
        
        threadIdToCancellationToken[thread].insert(ct);
        
        return false;
    }
    void RemoveCancellationToken(QThread* thread, rest::CancellationToken ct)
    {
        std::unique_lock lock{mtx};
        threadIdToCancellationToken[thread].remove(ct);
    }
    void RemoveAllCancellationTokens(rest::CancellationToken ct)
    {
        std::unique_lock lock{mtx};
        for(auto& tokens : threadIdToCancellationToken)
            tokens.remove(ct);
    }
    void Cancel(QThread* thread)
    {
        if(!thread)
            return;
        std::unique_lock lock{mtx};
        thread->requestInterruption();
        for(auto ct : threadIdToCancellationToken[thread])
            ct.Cancel();
    }
} RequestCancellations;
}

rest::QueryExecutor::QueryExecutor()
{
    qRegisterMetaType<Response>();
    
    m_thread = new QThread();
    m_accessManager = new QNetworkAccessManager();
    m_accessManager->moveToThread(m_thread);
    QObject::connect(m_thread, &QThread::finished, m_accessManager, &QObject::deleteLater);
    QObject::connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);

    m_thread->start();
    
    QLibrary lib("Query");
    if(lib.load())
    {
        using queryExportedFunc = void(*)(std::function<void(QString, QString, QString, QDateTime)>);
        auto fptr = (queryExportedFunc)lib.resolve("SubscribeOnQueryExecutorDataChanged");
        if(fptr)
        {
           fptr([this](QString url, QString authToken, QString refreshToken, QDateTime exp)
                {
                    this->SetBaseUrl(url);
                    this->SetAuthToken(authToken, refreshToken, exp);
                });
        }
    }
}

void rest::QueryExecutor::SetBaseUrl(QString baseUrl)
{
    std::unique_lock lock{m_lock};
    m_baseUrl = baseUrl;
}

QString rest::QueryExecutor::GetBaseUrl()
{
    std::unique_lock lock{m_lock};
    return m_baseUrl;
}

QString rest::QueryExecutor::GetServer()
{
    std::unique_lock lock{m_lock};
    return getServer(m_baseUrl);
}

int rest::QueryExecutor::GetPort()
{
    std::unique_lock lock{m_lock};
    return getPort(m_baseUrl);
}

void rest::QueryExecutor::SetAuthToken(QString token, QString refreshToken, QDateTime expire)
{
    std::unique_lock lock{m_lock};
    m_authToken = token;
    m_refreshToken = refreshToken;
    m_expireTime = expire;
}

void rest::QueryExecutor::SetCookieJar(QNetworkCookieJar* cookieJar)
{
    auto accessManager = m_accessManager;
    QMetaObject::invokeMethod(accessManager, [accessManager, cookieJar](){
        
        accessManager->setCookieJar(cookieJar);

    }, Qt::QueuedConnection);
}

void rest::QueryExecutor::SetLogCallback(std::function<void (const Query&, const Response&)> func)
{
    std::unique_lock lock{m_lock};
    m_logCallback = func;
}

void rest::QueryExecutor::Log(const Query& q, const Response& r)
{
    std::unique_lock lock{m_lock};
    if(m_logCallback)
        m_logCallback(q, r);
}

void rest::QueryExecutor::SetSubsystemCommunicationKey(const QString &key)
{
    std::unique_lock lock{m_lock};
    m_subsystemCommunicationKey = key;
}

QString rest::QueryExecutor::GetSubsystemCommunicationKey(void)
{
    std::unique_lock lock{m_lock};
    return m_subsystemCommunicationKey;
}

rest::QueryExecutor::AuthToken rest::QueryExecutor::GetAuthToken()
{
    std::unique_lock lock{m_lock};
    return AuthToken{m_authToken, m_refreshToken, m_expireTime};
}

QNetworkReply* rest::QueryExecutor::GetReply(QNetworkAccessManager* accessManager, const Query& q, const QString& bearerToken, const QString& subsystemToken, const QString& host, int port)
{
    QNetworkRequest request;
    // Формируем параметры запроса
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QUrl finalUrl;
    finalUrl.setScheme("http");
    finalUrl.setHost(host);
    finalUrl.setPort(port);
    finalUrl.setPath(q.url());
    
    auto queryParams = q.queryParams();
    auto arrayQueryParams = q.arrayQueryParams();
    
    QUrlQuery urlQuery;
    if(!queryParams.empty())
    {
        for(const auto& key : queryParams.keys())
        {
           auto value = queryParams[key];
           urlQuery.addQueryItem(key, value);
        }
    }
    
    if(!arrayQueryParams.empty())
    {
        for(const auto& key : arrayQueryParams.keys())
           for(const auto& val : arrayQueryParams[key])
               urlQuery.addQueryItem(key, val);
    }
    
    finalUrl.setQuery(urlQuery);

    if(!bearerToken.isEmpty()) {
        const QString authToken = "Bearer " + bearerToken;
        request.setRawHeader("Authorization", authToken.toUtf8());
    } else if(!subsystemToken.isEmpty()) {
        request.setRawHeader("SubToken", subsystemToken.toUtf8());
    }
    request.setUrl(finalUrl);
    
    QNetworkReply* reply;
    switch (q.method()) {
    case Method::GET:
        reply = accessManager->get(request);
        break;
    case Method::POST:
        reply = accessManager->post(request, q.data().toUtf8());
        break;
    case Method::PUT:
        reply = accessManager->put(request, q.data().toUtf8());
        break;
    case Method::DEL:
        reply = accessManager->sendCustomRequest(request, "DELETE", q.data().toUtf8());
        break;
    case Method::PATCH:
        reply = accessManager->sendCustomRequest(request, "PATCH", q.data().toUtf8());
        break;
    }
    
    return reply;
}

QString rest::QueryExecutor::getServer(const QString& url)
{
    auto serverName = url;
    auto pos = serverName.indexOf(':');
    if (pos != std::string::npos)
        return serverName.left(pos);
    else
        return serverName;
}

int rest::QueryExecutor::getPort(const QString& url)
{
    auto serverName = url;
    auto pos = serverName.indexOf(':');
    if (pos != std::string::npos)
        return serverName.mid(pos + 1, serverName.size()).toInt();
    else
        return 8100;
}

rest::Response rest::QueryExecutor::Execute(Query q, CancellationToken ct, bool cancellableByCancelAll)
{
    if(cancellableByCancelAll && RequestCancellations.AddCancellationToken(QThread::currentThread(), ct))
        return Response()
            .setAnswerString(QString())
            .setCode(rest::ClientRequestTimeout);
    
    std::unique_lock queryExecutorLock{m_lock};
    
    auto accessManager = m_accessManager;
    const auto bearerToken = m_authToken;
    const auto subSystemToken = m_subsystemCommunicationKey;
    auto host = getServer(m_baseUrl);
    auto port = getPort(m_baseUrl);
    
    queryExecutorLock.unlock();
    
    auto mtx = QSharedPointer<std::mutex>::create();
    std::unique_lock lock{*mtx};
    auto cv = QSharedPointer<std::condition_variable>::create();
    auto cvLifetime = new QObject();
    auto isReady = QSharedPointer<bool>::create();
    
    auto responseString = QSharedPointer<QString>::create();
    auto responseCode = QSharedPointer<StatusCode>::create();
    *responseCode = (StatusCode)0;
    
    auto appFinishMtx = QSharedPointer<std::mutex>::create();
    auto appQuitFlag = m_isAppClosing;
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, cvLifetime, [appQuitFlag, cv, mtx, isReady, appFinishMtx]()
        {
            appQuitFlag->store(true);
            std::unique_lock innerLock{*mtx};
            std::unique_lock appFinishLock{*appFinishMtx};
            if(!(*isReady))
            {
                *isReady = true;
                cv->notify_all();
            }
        }, Qt::DirectConnection);
    
    if(appQuitFlag->load() || QCoreApplication::closingDown())
    {
        delete cvLifetime;
        return Response().setAnswerString("").setCode(StatusCode::RequestTimeout);
    }
    
    ct.m_impl->Increase();
    QMetaObject::invokeMethod(m_accessManager, [ct, accessManager, mtx, cv, appFinishMtx, isReady, bearerToken, subSystemToken, q, host, port, responseString, responseCode]()
        {
            std::unique_lock innerLock{*mtx};
            
            auto reply = GetReply(accessManager, q, bearerToken, subSystemToken, host, port);
            QObject::connect(qApp, &QCoreApplication::aboutToQuit, reply, &QObject::deleteLater);
            
            QPointer<QTimer> transferTimer = nullptr;
            if(q.transferTimeout() > 0)
            {
                transferTimer = new QTimer();
                QSharedPointer<QMetaObject::Connection> conn = QSharedPointer<QMetaObject::Connection>::create();
                *conn = QObject::connect(reply, &QIODevice::readyRead, transferTimer, [transferTimer, conn]()
                                         {
                                             transferTimer->stop();
                                             QObject::disconnect(*conn);
                                         });
                QObject::connect(transferTimer, &QTimer::timeout, reply, &QNetworkReply::abort);
            }
            
            QPointer<QTimer> wholeTimer = nullptr;
            if(q.wholeTimeout() > 0)
            {
                wholeTimer = new QTimer();
                QObject::connect(wholeTimer, &QTimer::timeout, reply, &QNetworkReply::abort);
            }
            
            QObject::connect(reply, &QNetworkReply::finished, reply, [ct, isReady, wholeTimer, transferTimer, cv, appFinishMtx, reply, responseString, responseCode]()
                             {
                                 std::unique_lock<std::mutex> appFinishLock{*appFinishMtx};
                                 if(reply->isFinished() && reply->isReadable())
                                 {
                                     *responseCode = (StatusCode)reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                                     *responseString = reply->readAll();
                                 }
                                 else
                                 {
                                     *responseCode = StatusCode::ClientRequestTimeout;
                                 }
                                 
                                 ct.m_impl->RemoveReply(reply);
                                 ct.m_impl->m_cv.notify_all();
                                 reply->deleteLater();
                                 if(transferTimer)
                                     transferTimer->deleteLater();
                                 if(wholeTimer)
                                     wholeTimer->deleteLater();
                                 
                                 if(!(*isReady))
                                 {
                                     *isReady = true;
                                     cv->notify_all();
                                 }
                             });
            
            ct.m_impl->ConnectToReply(reply);
            
            if(wholeTimer)
                wholeTimer->start(std::chrono::milliseconds(q.wholeTimeout()));
            if(transferTimer)
                transferTimer->start(std::chrono::milliseconds(q.transferTimeout()));
        }, Qt::QueuedConnection);
    
    cv->wait(lock, [isReady](){return *isReady;});
    delete cvLifetime;
    
    if(appQuitFlag->load() || QCoreApplication::closingDown())
        return Response().setAnswerString("").setCode(StatusCode::RequestTimeout);
    
    auto resultResponse = Response()
                              .setAnswerString(*responseString)
                              .setCode(*responseCode);
    
    queryExecutorLock.lock();
    
    if(cancellableByCancelAll)
        RequestCancellations.RemoveCancellationToken(QThread::currentThread(), ct);
    
    if(m_logCallback)
        m_logCallback(q, resultResponse);
    
    return resultResponse;
}

void rest::QueryExecutor::ExecuteAsync(Query q, std::function<void (Response)> onFinished, CancellationToken ct, std::function<void ()> onCancel)
{
    if(RequestCancellations.AddCancellationToken(QThread::currentThread(), ct))
        return;
    
    std::unique_lock lock{m_lock};
    auto accessManager = m_accessManager;
    const auto bearerToken = m_authToken;
    const auto subSystemToken = m_subsystemCommunicationKey;
    auto host = getServer(m_baseUrl);
    auto port = getPort(m_baseUrl);
    ct.m_impl->Increase();
    
    auto logCallback = m_logCallback;
    
    QMetaObject::invokeMethod(m_accessManager, [logCallback, ct, onFinished, onCancel, accessManager, bearerToken, subSystemToken, q, host, port]()
        {
            auto reply = GetReply(accessManager, q, bearerToken, subSystemToken, host, port);
            
            QPointer<QTimer> transferTimer = nullptr;
            if(q.transferTimeout() > 0)
            {
                transferTimer = new QTimer();
                QSharedPointer<QMetaObject::Connection> conn = QSharedPointer<QMetaObject::Connection>::create();
                *conn = QObject::connect(reply, &QIODevice::readyRead, transferTimer, [transferTimer, conn]()
                                         {
                                             transferTimer->stop();
                                             QObject::disconnect(*conn);
                                         });
                QObject::connect(transferTimer, &QTimer::timeout, reply, &QNetworkReply::abort);
            }
            
            QPointer<QTimer> wholeTimer = nullptr;
            if(q.wholeTimeout() > 0)
            {
                wholeTimer = new QTimer();
                QObject::connect(wholeTimer, &QTimer::timeout, reply, &QNetworkReply::abort);
            }
            
            
            QObject::connect(reply, &QNetworkReply::finished, reply, [logCallback, q, ct, onFinished, onCancel, wholeTimer, transferTimer, reply]()
                             {
                                 RequestCancellations.RemoveAllCancellationTokens(ct);
                
                                 QString responseString = "";
                                 StatusCode responseCode = (StatusCode)0;
                                 
                                 if(reply->isFinished() && reply->isReadable())
                                 {
                                     responseCode = (StatusCode)reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                                     responseString = reply->readAll();
                                     if(onFinished)
                                        onFinished(Response()
                                                       .setCode(responseCode)
                                                       .setAnswerString(responseString)
                                                   );
                                 }
                                 else
                                 {
                                     responseCode = StatusCode::ClientRequestTimeout;
                                     if(onCancel)
                                        onCancel();
                                 }
                                 
                                 ct.m_impl->RemoveReply(reply);
                                 ct.m_impl->m_cv.notify_all();
                                 reply->deleteLater();
                                 if(transferTimer)
                                     transferTimer->deleteLater();
                                 if(wholeTimer)
                                     wholeTimer->deleteLater();
                                 
                                 if(logCallback)
                                     logCallback(q, Response()
                                                        .setCode(responseCode)
                                                        .setAnswerString(responseString));
                             });
            
            ct.m_impl->ConnectToReply(reply);
            
            if(wholeTimer)
                wholeTimer->start(std::chrono::milliseconds(q.wholeTimeout()));
            if(transferTimer)
                transferTimer->start(std::chrono::milliseconds(q.transferTimeout()));
        }, Qt::QueuedConnection);
}

void rest::QueryExecutor::ExecuteAsync(Query q, QObject* context, std::function<void (Response)> onFinishedSlot, CancellationToken ct, std::function<void ()> onCancelSlot)
{
    if(RequestCancellations.AddCancellationToken(QThread::currentThread(), ct))
        return;
    
    std::unique_lock lock{m_lock};
    auto accessManager = m_accessManager;
    const auto bearerToken = m_authToken;
    const auto subSystemToken = m_subsystemCommunicationKey;
    auto host = getServer(m_baseUrl);
    auto port = getPort(m_baseUrl);
    ct.m_impl->Increase();
    auto logCallback = m_logCallback;
    auto completeNotifier = QSharedPointer<AsyncRequestCompleteNotifier>::create();
    if(onFinishedSlot)
        QObject::connect(completeNotifier.data(), &AsyncRequestCompleteNotifier::requestFinished, context, onFinishedSlot);
    if(onCancelSlot)
        QObject::connect(completeNotifier.data(), &AsyncRequestCompleteNotifier::requestCanceled, context, onCancelSlot);
    QMetaObject::invokeMethod(m_accessManager, [logCallback, completeNotifier, ct, onFinishedSlot, onCancelSlot, accessManager, bearerToken, subSystemToken, q, host, port]()
        {
            auto reply = GetReply(accessManager, q, bearerToken, subSystemToken, host, port);
            
            QPointer<QTimer> transferTimer = nullptr;
            if(q.transferTimeout() > 0)
            {
                transferTimer = new QTimer();
                QSharedPointer<QMetaObject::Connection> conn = QSharedPointer<QMetaObject::Connection>::create();
                *conn = QObject::connect(reply, &QIODevice::readyRead, transferTimer, [transferTimer, conn]()
                                         {
                                             transferTimer->stop();
                                             QObject::disconnect(*conn);
                                         });
                QObject::connect(transferTimer, &QTimer::timeout, reply, &QNetworkReply::abort);
            }
            
            QPointer<QTimer> wholeTimer = nullptr;
            if(q.wholeTimeout() > 0)
            {
                wholeTimer = new QTimer();
                QObject::connect(wholeTimer, &QTimer::timeout, reply, &QNetworkReply::abort);
            }
            
            QObject::connect(reply, &QNetworkReply::finished, reply, [q, logCallback, completeNotifier, ct, onFinishedSlot, onCancelSlot, wholeTimer, transferTimer, reply]()
                             {
                                 RequestCancellations.RemoveAllCancellationTokens(ct);
                                 
                                 QString responseString = "";
                                 StatusCode responseCode = (StatusCode)0;
                                 
                                 if(reply->isFinished() && reply->isReadable())
                                 {
                                     responseCode = (StatusCode)reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                                     responseString = reply->readAll();
                                     emit completeNotifier->requestFinished(Response()
                                                                                .setCode(responseCode)
                                                                                .setAnswerString(responseString));
                                 }
                                 else
                                 {
                                     responseCode = StatusCode::ClientRequestTimeout;
                                     emit completeNotifier->requestCanceled();
                                 }
                                 
                                 ct.m_impl->RemoveReply(reply);
                                 ct.m_impl->m_cv.notify_all();
                                 reply->deleteLater();
                                 if(transferTimer)
                                     transferTimer->deleteLater();
                                 if(wholeTimer)
                                     wholeTimer->deleteLater();
                                 
                                 if(logCallback)
                                     logCallback(q, Response()
                                                        .setCode(responseCode)
                                                        .setAnswerString(responseString));
                             });
            
            ct.m_impl->ConnectToReply(reply);
            
            if(wholeTimer)
                wholeTimer->start(std::chrono::milliseconds(q.wholeTimeout()));
            if(transferTimer)
                transferTimer->start(std::chrono::milliseconds(q.transferTimeout()));
            
        }, Qt::QueuedConnection);
}



rest::CancellationToken::CancellationToken()
{
    m_impl = QSharedPointer<CancellationTokenImpl>(new CancellationTokenImpl());
}

void rest::CancellationToken::Cancel()
{
    m_impl->Abort();
}

void rest::CancellationToken::CancelAndWait()
{
    m_impl->Abort();
    m_impl->Wait();
}

void rest::CancellationToken::Wait()
{
    m_impl->Wait();
}

rest::QueryExecutor* rest::Executor()
{
    static QueryExecutor instance;
    return &instance;
}

void rest::CancelAllBlockingRequestsInThread(QThread* thread)
{
    RequestCancellations.Cancel(thread);
}

#include "QueryExecutor.moc"
