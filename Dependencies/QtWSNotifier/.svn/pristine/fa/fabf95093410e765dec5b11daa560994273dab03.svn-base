#include "Notifier.h"

#include "WSConnectionImpl.h"

#include <QMutex>
#include <QSharedPointer>
#include <QWaitCondition>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

//---------------------------------------------------------------------------------------
Notifier::Notifier(QObject* parent)
    : QObject(parent)
{
    // Запускаем клиента в отдельном потоке
    impl = new WSConnectionImpl;
    impl->moveToThread(&thread);

    connect(&thread, &QThread::started, impl, &WSConnectionImpl::init);
    connect(&thread, &QThread::finished, impl, &WSConnectionImpl::release);

    thread.start();

    // сообщаем в основной поток
    connect(impl, &WSConnectionImpl::connected, this, &Notifier::onConnected);
    connect(impl, &WSConnectionImpl::disconnected, this, &Notifier::onDisconnected);
    connect(impl, &WSConnectionImpl::error, this, &Notifier::onError);
    connect(impl, &WSConnectionImpl::messageReceived, this, &Notifier::onMessageReceived);
    // connect(impl, &WSConnectionImpl::stateChanged, this, &Notifier::onSocketStateChanged);
}

//---------------------------------------------------------------------------------------
Notifier::~Notifier()
{
    QMetaObject::invokeMethod(impl, "abort", Qt::QueuedConnection);
    thread.exit();
    thread.wait();

    delete impl;
}

//---------------------------------------------------------------------------------------
void Notifier::connectTo(const char* url)
{
    QMetaObject::invokeMethod(impl, "connectTo", Qt::QueuedConnection, Q_ARG(const QString&, QString(url)));
}

//---------------------------------------------------------------------------------------
void Notifier::disconnectNotifier()
{
    QMetaObject::invokeMethod(impl, "disconnect", Qt::QueuedConnection);
}

//---------------------------------------------------------------------------------------
void Notifier::abort()
{
    QMetaObject::invokeMethod(impl, "abort", Qt::QueuedConnection);
}

//---------------------------------------------------------------------------------------
bool Notifier::isConnected(void)
{
    return impl->isConnected();
}

//---------------------------------------------------------------------------------------
bool Notifier::setSubscriptions(const QString& settings, QString& error)
{
    if (!impl->isConnected())
        return false;

    // Куда мы получим данные
    QSharedPointer<QString> revcMsg(new QString);

    QMutex m;
    QSharedPointer<QWaitCondition> wc(new QWaitCondition);

    m.lock();
    // Делаем запрос
    impl->makeRequest(WSConnectionImpl::ECommand::eCSetSubscriptions, settings, wc, revcMsg);
    // Ожидаем ответ
    if (wc->wait(&m, msgTimeoutMsec)) {
        m.unlock();

        QJsonDocument doc;
        QJsonParseError parseError;
        doc = QJsonDocument::fromJson(QByteArray(revcMsg->toUtf8().data()), &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            QJsonObject o = doc.object();

            const QString result = o.value("result").toString();
            auto res = result == "OK";

            if (!res)
                error = result;

            return res;
        }

        return false;
    } else
        m.unlock();

    return false;
}

//---------------------------------------------------------------------------------------
bool Notifier::getSubscriptions(QString& settings)
{
    // Куда мы получим данные
    QSharedPointer<QString> revcMsg(new QString);

    QMutex m;
    QSharedPointer<QWaitCondition> wc(new QWaitCondition);

    m.lock();
    // Делаем запрос
    impl->makeRequest(WSConnectionImpl::ECommand::eCGetSubscriptions, "{ \"msgType\": 2 }", wc, revcMsg);

    // Ожидаем ответ
    bool ret = wc->wait(&m, msgTimeoutMsec);

    if (ret)
        settings = *revcMsg;

    m.unlock();

    return ret;
}

QString Notifier::clientRequestedDisconnectReason()
{
    return WSConnectionImpl::clientRequestedDisconnectReason();
}

//---------------------------------------------------------------------------------------
void Notifier::onConnected()
{
    emit connected();
}

//---------------------------------------------------------------------------------------
void Notifier::onDisconnected(const QString& reason)
{
    emit disconnected(reason);
}

//---------------------------------------------------------------------------------------
void Notifier::onError(QAbstractSocket::SocketError err)
{
    emit error(err);
}

//---------------------------------------------------------------------------------------
void Notifier::onMessageReceived(const QString& msg)
{
    emit messageReceived(msg);
}
//---------------------------------------------------------------------------------------

void Notifier::onSocketStateChanged(QAbstractSocket::SocketState state)
{
    emit socketStateChanged(state);
}
//---------------------------------------------------------------------------------------
