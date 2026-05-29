#pragma once

#include <QObject>

#include <QSharedPointer>
#include <QWebSocket>
#include <QTimer>

class QWaitCondition;

// Реализация WS клиента
class WSConnectionImpl : public QObject {
    Q_OBJECT
public:
    WSConnectionImpl();
    ~WSConnectionImpl();

    // Подключены ли мы сейчас
    bool isConnected(void);

    enum class ECommand : unsigned {
        eCNone = 0, // Неизвесное сообщение
        eCSetSubscriptions = 1, // Ответ на установку подписок
        eCGetSubscriptions = 2, // Ответ на получение подписок
        eCSubscriptionsEvents = 3 // Инициативные сообщения о событиях
    };
    
    static QString clientRequestedDisconnectReason() {return QString("disconnectRequestedByClient");}
    static QString pingPongTimeoutDisconnectReason() {return QString("disconnectByPingPongTimeout");}
    
private:
    QWebSocket* m_socket = nullptr;

    // Методы синхронного выполнения команды
    QSharedPointer<QWaitCondition> waitCondition;
    QSharedPointer<QString> revcBuffer;
    ECommand expectedCommand = ECommand::eCNone;
    // Получить тип сообщения
    ECommand getMessageType(const QString& msg);

    // Механизм проверки актуальности соединения
    int m_pingTimerID = 0;
    static const int pingPeriodMsec = 3000;
    static const int pongTimeoutMsec = pingPeriodMsec * 1.2;
    virtual void timerEvent(QTimerEvent *event) override;
    QTimer *m_pongTimerTimeout = nullptr;

public slots:
    void init(void);
    void release(void);

    void connectTo(const QString& url);
    void disconnect(void);
    void disconnect(const QString& reason);
    void abort(void);

    // Выполнить синхронный запрос command с данными data.
    void makeRequest(ECommand command, const QString& data, QSharedPointer<QWaitCondition> wc, QSharedPointer<QString> recvData);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onStateChanged(QAbstractSocket::SocketState state);
    void onError(QAbstractSocket::SocketError error);    
    void onPong(quint64 elapsedTime, const QByteArray& payload);

    void sendTextMessage(const QString& msg);

signals:
    void connected();
    void disconnected(const QString& reason);
    void messageReceived(const QString& str);
    void stateChanged(QAbstractSocket::SocketState state);
    void error(QAbstractSocket::SocketError err);
};
