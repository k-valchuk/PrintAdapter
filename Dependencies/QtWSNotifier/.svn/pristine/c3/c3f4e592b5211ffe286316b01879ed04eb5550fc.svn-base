#ifndef SUBSCRIBERSNOTIFIER_H
#define SUBSCRIBERSNOTIFIER_H

#include "Notifier.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QSet>
#include <QSharedPointer>
#include <QTimer>
#include <QVector>

class ISubscriber : public QObject {
    Q_OBJECT

public:
    typedef int Type;

    struct Subscription {
        Type type;
        QVector<QJsonValue> objects;
    };

    virtual QVector<Subscription> GetNeededSubscription() = 0;
    virtual bool ShouldBeCalledWhenNothingConnected() {return false;}

    enum NotifyMsgType {
        Unknown = 0,
        SetSubs = 1, // Установка подписок
        GetSubs = 2, // Получение подписок
        Event = 3 // Получение уведомления
    };

    static NotifyMsgType getNotifyMsgType(const QString& response)
    {
        const QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
        return (NotifyMsgType)doc.object().value("msgType").toInt();
    }
    
    bool IsSignalConnected(const QMetaMethod& sig){
        return isSignalConnected(sig);
    }
    
public slots:
    virtual void OnMessageReceived(const QString& message) = 0;
    virtual void OnConnected() { }
    virtual void OnDisconnected() { }
};

class SubscribersNotifier : public Notifier {
    Q_OBJECT

    QHash<ISubscriber*, Qt::ConnectionType> m_subscribers;
    // Текущие значения имени сервера и порта к которым нужно производить подключение при вызове connect
    QString server;
    int port;

    // Период пересоединение в мсек
    // Если установлен - значит включен процесс пересоединения с переподпиской
    int reconnectionPeriodMsec = 0;
    QTimer reconnectTimer;
    bool m_wasDisconnectedByError = false;
protected:
    // Путь подключения который используется как: ws://<server>:<port><path>
    virtual const QString getPath(void) const = 0;

public:
    SubscribersNotifier();
    ~SubscribersNotifier();

    // Установить процесс пересоединения (период попыток в мсек)
    // Пересоединение - это процесс который запускается при разрыве соединения, попытки происходят
    //  каждые periodMsec. После пересоединения вызывается сигнал sig_recovered и происходит восстановление подписок.
    static const int defaultReconnectPeriodMsec = 1000;
    void setReconnection(int periodMsec = defaultReconnectPeriodMsec);
    void stopReconnection(void);
    void startReconnection(int periodMsec = 0);

    // Получить список объектов подписок
    QList<ISubscriber*> subscribers(void);
    // Получить подписчик по типу
    ISubscriber* getSubscriber(ISubscriber::Type t);

    // Установить параметры доступа по websocket
    inline void setServerPath(const QString& s, int p)
    {
        server = s;
        port = p;
    }

    // Подключиться с текущими параметрами
    void Connect();
    // Подключиться c указанными параметрами
    // Примерение парамеров: ws://<server>:<port><path>
    void Connect(const QString& server, int port);
    void Disconnect();

    // Подписаться
    void SubscribeWith(ISubscriber* sub, Qt::ConnectionType connectionType = Qt::QueuedConnection);
    // Отписаться
    void Unsubscribe(ISubscriber* sub);
    // Отправить список текущих подписок (нужно при переподключении)
    bool UpdateSubscriptionsList();

    static QJsonObject parseResponseToObj(const QString& response)
    {
        QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
        return doc.object();
    }

    static ISubscriber::Type getEventType(const QString& response)
    {
        QJsonObject jsObj = parseResponseToObj(response);

        return (ISubscriber::Type)jsObj.value("event").toObject().value("type").toInt();
    }
signals:
    void sig_connected();
    void sig_firstlyConnected();
    void sig_disconnected(const QString& reason);
    void sig_error(QAbstractSocket::SocketError err);
    void sig_recovered();
};

Q_DECLARE_METATYPE(ISubscriber::Type)

#endif // SUBSCRIBERSNOTIFIER_H
