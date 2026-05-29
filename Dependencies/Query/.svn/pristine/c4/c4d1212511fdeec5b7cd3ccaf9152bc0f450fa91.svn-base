#ifndef QUERYEXECUTOR_H
#define QUERYEXECUTOR_H

#include "Query.h"
#include "Response.h"
#include <functional>
#include <QNetworkAccessManager>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <QSharedPointer>
#include <QHash>
#include <QSet>

class QNetworkCookieJar;

namespace rest{

class QueryExecutor;
QueryExecutor* Executor();
void CancelAllBlockingRequestsInThread(QThread* thread);

class CancellationTokenImpl;
class CancellationToken
{
    QSharedPointer<CancellationTokenImpl> m_impl;

    friend class QueryExecutor;
    friend uint qHash(const CancellationToken&, uint) noexcept;
public:
    CancellationToken();
    void Cancel();
    void CancelAndWait();
    void Wait();
    
    bool operator==(const CancellationToken& other) const
    {
        return m_impl.data() == other.m_impl.data();
    }
};

inline uint qHash(const CancellationToken& ct, uint seed = 0) noexcept
{
    return ::qHash(ct.m_impl.data(), seed);
}

class QueryExecutor
{
    QString m_authToken;
    QString m_baseUrl;
    QString m_refreshToken;
    QDateTime m_expireTime;
    QString m_subsystemCommunicationKey; // Ключ для межсервисного взаимодейтсвия. SubToken

    QNetworkAccessManager* m_accessManager;
    QThread* m_thread;

    std::mutex m_lock;
    QSharedPointer<std::atomic_bool> m_isAppClosing = QSharedPointer<std::atomic_bool>::create(false);

    static QNetworkReply* GetReply(QNetworkAccessManager* accessManager,
                                   const Query& q,
                                   const QString& bearerToken,
                                   const QString& subsystemToken,
                                   const QString& host,
                                   int port);
    
    std::function<void(const Query&, const Response&)> m_logCallback = nullptr;
    static QString getServer(const QString& url);
    static int getPort(const QString& url);
public:
    QueryExecutor();

    QueryExecutor(const QueryExecutor& other) = delete;
    QueryExecutor& operator=(const QueryExecutor& other) = delete;

    void SetBaseUrl(QString baseUrl);
    QString GetBaseUrl();
    QString GetServer();
    int GetPort();
    void SetAuthToken(QString token, QString refreshToken, QDateTime expire);
    
    void SetCookieJar(QNetworkCookieJar* cookieJar);
    
    void SetLogCallback(std::function<void(const Query&, const Response&)> func);
    void Log(const Query& q, const Response& r);

    // Установка/получения ключа межсервисного взаимодействия
    void SetSubsystemCommunicationKey(const QString &key);
    QString GetSubsystemCommunicationKey(void);

    struct AuthToken {
        QString authToken;
        QString refreshToken; //ключ
        QDateTime dateTime; //время жизни
    };
    AuthToken GetAuthToken();

    template <class SubObj>
    void SetOnNetworkStatus(SubObj* subscriber, void (SubObj::*action)(QNetworkAccessManager::NetworkAccessibility accessible))
    {
        std::unique_lock lock{m_lock};

        //deprecated in Qt6+
        //In Qt6 using QNetworkInformation class with signal "reachabilityChanged(QNetworkInformation::Reachability newReachability)"
        QObject::connect(m_accessManager, &QNetworkAccessManager::networkAccessibleChanged, subscriber, action, Qt::QueuedConnection);
    }

    template <class SubObj>
    void SetOnNetworkStatus(SubObj* subscriber, void (*action)(QNetworkAccessManager::NetworkAccessibility accessible))
    {
        std::unique_lock lock{m_lock};

        //deprecated in Qt6+
        //In Qt6 using QNetworkInformation class with signal "reachabilityChanged(QNetworkInformation::Reachability newReachability)"
        QObject::connect(m_accessManager, &QNetworkAccessManager::networkAccessibleChanged, subscriber, action, Qt::QueuedConnection);
    }

    Response Execute(Query q, CancellationToken ct = CancellationToken(), bool cancellableByCancelAll = true);

    void ExecuteAsync(Query q, std::function<void(Response)> onFinished,
                      CancellationToken ct = CancellationToken(), std::function<void()> onCancel = nullptr);

    void ExecuteAsync(Query q, QObject* context, std::function<void(Response)> onFinishedSlot,
                      CancellationToken ct = CancellationToken(), std::function<void()> onCancelSlot = nullptr);
};


}

Q_DECLARE_METATYPE(rest::Response)

#endif // QUERYEXECUTOR_H
