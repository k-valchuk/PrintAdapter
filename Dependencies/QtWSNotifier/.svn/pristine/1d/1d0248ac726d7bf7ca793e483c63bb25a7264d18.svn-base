#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QAbstractSocket>
#include <QObject>
#include <QThread>

class WSConnectionImpl;

// Реализация уведомителя на события через подписки
class Notifier : public QObject {
    Q_OBJECT
public:
    explicit Notifier(QObject* parent = nullptr);
    ~Notifier();

    // Подключение/отключение
    void connectTo(const char* url);
    void disconnectNotifier();

    // Прерывание работы
    void abort();

    // Подключены ли к серверу
    bool isConnected(void);

    // Установить все подписки (новое значение подписок)
    // settings - json с настройками подписок
    // error - текст ошибки если есть
    bool setSubscriptions(const QString& settingsJson, QString& error);
    // Получить текущие подписки
    // settings - получить текущие подписки в json
    bool getSubscriptions(QString& settingsJson);

    static QString clientRequestedDisconnectReason();

private:
    // Реализация WS клиента
    WSConnectionImpl* impl = nullptr;
    // Поток в котором выполняется WS клиент
    QThread thread;

    // Таймаут в мсек на ожидание синхронной команды
    const unsigned int msgTimeoutMsec = 2000;

private slots:
    void onConnected();
    void onDisconnected(const QString& reason);
    void onError(QAbstractSocket::SocketError err);
    void onMessageReceived(const QString& msg);
    void onSocketStateChanged(QAbstractSocket::SocketState state);

signals:
    // Произошло подключение к серверу
    void connected();
    // Отключение от сервера
    void disconnected(const QString& reason /*Причина отключения*/);
    // Неудачная попытка подключения к сервера
    void error(QAbstractSocket::SocketError err /*Уточнение неудачной попытки подключения*/);
    // Было получено сообщение
    void messageReceived(const QString& msg);
    // Состояние сокета изменилось
    void socketStateChanged(QAbstractSocket::SocketState state);
};

#endif // !NOTIFIER_H
