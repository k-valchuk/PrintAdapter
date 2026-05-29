#include "QueryExternal.h"

#include "QueryExecutor.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>


QueryExecutorData _QueryExecutorData;

bool _QueryExecutorFirst = true;
QTimer _QueryExecutorTimer;
QObject* _QueryExecutorSlotReloginReceiver = nullptr;
std::function<void()> _QueryExecutorReloginSlot = nullptr;
std::function<void (QueryExecutorData)> _QueryExecutorUpdateForAllModules = nullptr;


#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QReadWriteLock>

/*!
 * Cookie manager, which allows thread safe sharing of cookies
 */
class ThreadSafeCookieJar : public QNetworkCookieJar
{
    
public:
    ThreadSafeCookieJar(QObject *parent = nullptr)
        : QNetworkCookieJar(parent)
    {
        // code
    }
    
    //! \copydoc QNetworkCookieJar::setCookiesFromUrl
    //! \threadsafe
    virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookies, const QUrl &url) override
    {
        QWriteLocker l(&m_lock);
        return QNetworkCookieJar::setCookiesFromUrl(cookies, url);
    }
    
    //! \copydoc QNetworkCookieJar::cookiesForUrl
    //! \threadsafe
    virtual QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const override
    {
        QReadLocker l(&m_lock);
        const QList<QNetworkCookie> cookies(QNetworkCookieJar::cookiesForUrl(url));
        return cookies;
    }
    
    //! \copydoc QNetworkCookieJar::deleteCookie
    //! \threadsafe
    virtual bool deleteCookie(const QNetworkCookie &cookie) override
    {
        QWriteLocker l(&m_lock);
        return QNetworkCookieJar::deleteCookie(cookie);
    }
    
    //! \copydoc QNetworkCookieJar::insertCookie
    //! \threadsafe
    virtual bool insertCookie(const QNetworkCookie &cookie) override
    {
        QWriteLocker l(&m_lock);
        return QNetworkCookieJar::insertCookie(cookie);
    }
    
    //! \copydoc QNetworkCookieJar::updateCookie
    //! \threadsafe
    virtual bool updateCookie(const QNetworkCookie &cookie) override
    {
        QWriteLocker l(&m_lock);
        return QNetworkCookieJar::updateCookie(cookie);
    }
    
private:
    mutable QReadWriteLock m_lock { QReadWriteLock::Recursive };
};

void _QueryExecutorUpdateAuthToken()
{
    using namespace rest;
    bool hasRefreshToken = !_QueryExecutorData.RefreshToken.isEmpty();
    Executor()->ExecuteAsync(Query()
                           .setUrl("/api/core/token/refresh")
                           .setMethod(Method::POST)
                           .setData(hasRefreshToken ? QString("{\"refreshToken\": \"%1\"}").arg(_QueryExecutorData.RefreshToken) : QString())
                           .setWholeTimeout(5000),
                       [&](Response resp)
                       {
                           if(resp.isSuccess())
                           {
                               auto doc = QJsonDocument::fromJson(resp.answerString().toUtf8());
                               auto obj = doc.object();
                               auto authToken = obj["accessToken"].toString();
                               auto refreshToken = obj["refreshToken"].toString();
                               auto exp = QDateTime::fromString(obj["exp"].toString(), Qt::DateFormat::RFC2822Date);
                               
                               SetQueryExecutorData(_QueryExecutorData.ServerUrl, authToken, refreshToken, exp);
                           }
                           else //колбек устанавливается снаружи, в нем например
                           {    //может быть попытка перелогина и блок приложения
                               if(_QueryExecutorSlotReloginReceiver && _QueryExecutorReloginSlot)
                                   QMetaObject::invokeMethod(_QueryExecutorSlotReloginReceiver, _QueryExecutorReloginSlot, Qt::QueuedConnection);
                           }
                        }, CancellationToken(),
                        [&](){
                            if(_QueryExecutorSlotReloginReceiver && _QueryExecutorReloginSlot)
                                QMetaObject::invokeMethod(_QueryExecutorSlotReloginReceiver, _QueryExecutorReloginSlot, Qt::QueuedConnection);
                        });
}

void SetQueryExecutorData(QString serverUrl, QString authToken,
                          QString refreshToken, QDateTime exp)
{
    if(_QueryExecutorFirst)
    {
        _QueryExecutorFirst = false;
        
        _QueryExecutorTimer.setSingleShot(true);
        QObject::connect(&_QueryExecutorTimer, &QTimer::timeout, &_QueryExecutorTimer, []()
                         {
                            _QueryExecutorUpdateAuthToken();
                         });
    }
    
    auto toExpire = QDateTime::currentDateTime().msecsTo(exp);
    auto timeoutToExpire = (int)(toExpire*0.8);
    if(toExpire - timeoutToExpire < 10'000)
            timeoutToExpire  = qMax(1'000ll, toExpire - 10'000);

    //timeoutToExpire = 2000; //разкоментить если надо протестировать обновление токена побыстрее
    //qDebug() << token;
    
    if(QThread::currentThread() == _QueryExecutorTimer.thread())
    _QueryExecutorTimer.start(timeoutToExpire);
    else
    QMetaObject::invokeMethod(&_QueryExecutorTimer, [timeoutToExpire]() {
            _QueryExecutorTimer.start(timeoutToExpire);
        }, Qt::BlockingQueuedConnection);

    
    _QueryExecutorData.ServerUrl = serverUrl;
    _QueryExecutorData.AuthToken = authToken;
    _QueryExecutorData.RefreshToken = refreshToken;
    _QueryExecutorData.DateTimeOfExpire = exp;
    
    UpdateQueryExecurorData();
}

void UpdateQueryExecurorData()
{
    rest::Executor()->SetBaseUrl(_QueryExecutorData.ServerUrl);
    rest::Executor()->SetAuthToken(_QueryExecutorData.AuthToken, _QueryExecutorData.RefreshToken, _QueryExecutorData.DateTimeOfExpire);
    
    //update for all modules
    if(_QueryExecutorUpdateForAllModules)
        _QueryExecutorUpdateForAllModules(_QueryExecutorData);
}

void SetReloginCallback(QObject* context, std::function<void ()> slot)
{
    _QueryExecutorSlotReloginReceiver = context;
    _QueryExecutorReloginSlot = slot;
}

void SetOnQueryExecutorDataChanged(std::function<void (QueryExecutorData)> updateForAllModules)
{
    _QueryExecutorUpdateForAllModules = updateForAllModules;
}

QNetworkCookieJar* GetApplicationSharedCookieJar()
{
    static ThreadSafeCookieJar cookieJarInstance;
    return &cookieJarInstance;
}
