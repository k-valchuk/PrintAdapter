#include "WSConnectionImpl.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWaitCondition>
#include <QTimerEvent>

//#define DEB_INF

//---------------------------------------------------------------------------------------------
WSConnectionImpl::WSConnectionImpl()
    : QObject(nullptr)
{
    qRegisterMetaType<QAbstractSocket::SocketState>();    
}

//---------------------------------------------------------------------------------------------
WSConnectionImpl::~WSConnectionImpl()
{
    release();
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::release(void)
{
    delete m_socket;
    m_socket = nullptr;
}

//---------------------------------------------------------------------------------------------
bool WSConnectionImpl::isConnected(void)
{
    if (!m_socket)
        return false;

    switch (m_socket->state()) {
    case QAbstractSocket::SocketState::ConnectedState:
        return true;
    default:
        return false;
    }
}

//---------------------------------------------------------------------------------------------
WSConnectionImpl::ECommand WSConnectionImpl::getMessageType(const QString& msg)
{
    QJsonDocument doc;
    QJsonParseError parseError;
    doc = QJsonDocument::fromJson(QByteArray(msg.toUtf8().data()), &parseError);

    if (parseError.error == QJsonParseError::NoError) {
        QJsonObject o = doc.object();

        // Если у нас тип сообщения - ответ на запрос
        if (o.contains("msgType")) {
            const int msgTypeInt = o.value("msgType").toInt();
            // Если присутствует такая команда
            if (msgTypeInt > (int)ECommand::eCNone || msgTypeInt <= (int)ECommand::eCSubscriptionsEvents)
                return static_cast<ECommand>(msgTypeInt);
        }
        // Если тип сообщения - инициативные по подпискам
        else if (o.contains("subType"))
            return ECommand::eCSubscriptionsEvents;
    }

    return ECommand::eCNone;
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::connectTo(const QString& url)
{
    const QString curUrl(url);

    m_socket->open(QUrl(curUrl));
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::disconnect(void)
{
    disconnect(clientRequestedDisconnectReason());
}

void WSConnectionImpl::disconnect(const QString& reason)
{
    m_socket->close(QWebSocketProtocol::CloseCodeNormal, reason);
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::abort(void)
{
    m_socket->abort();
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::makeRequest(ECommand c, const QString& data, QSharedPointer<QWaitCondition> wc, QSharedPointer<QString> recvData)
{
    // Сохраняем данные запроса в этом потоке
    revcBuffer = recvData;
    expectedCommand = c;
    waitCondition = wc;
    // Отправляем запрос на отправку сообщения в поток клиента
    QMetaObject::invokeMethod(this, "sendTextMessage", Qt::QueuedConnection, Q_ARG(const QString&, data));
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::init(void)
{
    if (!m_socket) {
        m_socket = new QWebSocket;

        connect(m_socket, &QWebSocket::connected, this, &WSConnectionImpl::onConnected);
        connect(m_socket, &QWebSocket::disconnected, this, &WSConnectionImpl::onDisconnected);
        connect(m_socket, &QWebSocket::textMessageReceived, this, &WSConnectionImpl::onTextMessageReceived);
        connect(m_socket, &QWebSocket::stateChanged, this, &WSConnectionImpl::onStateChanged);
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &WSConnectionImpl::onError);
        connect(m_socket, &QWebSocket::pong, this, &WSConnectionImpl::onPong);
    }
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::onConnected()
{
    m_serverSupportsPong = false;

    if(m_pingTimerID)
        killTimer(m_pingTimerID);
    m_pingTimerID = startTimer(pingPeriodMsec);

    if(!m_pongTimerTimeout) {
        m_pongTimerTimeout = new QTimer(this);
        m_pongTimerTimeout->setSingleShot(true);
        connect(m_pongTimerTimeout, &QTimer::timeout, [this](){
#ifdef WSTRACE
            qDebug("WS:pong timeout!");
#endif // WSTRACE
            QMetaObject::invokeMethod(this, "abort", Qt::QueuedConnection);
        });
    }

    if(m_socket)
        m_socket->ping();

#ifdef WSTRACE
    qDebug("WS: connected!");
#endif // WSTRACE

    emit connected();
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::onDisconnected()
{
    killTimer(m_pingTimerID);
    m_pingTimerID = 0;
    m_serverSupportsPong = false;
    if(m_pongTimerTimeout) {
        m_pongTimerTimeout->stop();
        m_pongTimerTimeout->deleteLater();
        m_pongTimerTimeout = nullptr;
    }

#ifdef WSTRACE
    qDebug("WS: disconnected!");
#endif // WSTRACE

    emit disconnected(m_socket->closeReason());
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::onTextMessageReceived(const QString& message)
{
#ifdef DEB_INF
#ifdef _DEBUG
    qDebug("WS:Message received! Size is %d", message.size());
#endif // _DEBUG
#endif

    switch (getMessageType(message)) {
    case ECommand::eCGetSubscriptions:
    case ECommand::eCSetSubscriptions:
        // Сохраняем данные в буфер
        if (revcBuffer)
            (*revcBuffer) = message;
        // Будем вызывающий поток
        if (waitCondition)
            waitCondition->wakeAll();

        // Очищаем, это нам больше не нужно
        revcBuffer.clear();
        waitCondition.clear();

        break;

    // Если сообщение другого типа просто отправляем данные
    default:
        emit messageReceived(message);
    }
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::onStateChanged(QAbstractSocket::SocketState state)
{
#ifdef WSTRACE
    qDebug("WS:state changed: %d", state);
#endif // WSTRACE

    emit stateChanged(state);
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::onError(QAbstractSocket::SocketError e)
{
#ifdef _DEBUG
    qDebug("WS:error: %d", e);
#endif // _DEBUG
    emit error(e);
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::sendTextMessage(const QString& data)
{
    m_socket->sendTextMessage(data);
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::onPong(quint64 elapsedTime, const QByteArray& payload)
{
    Q_UNUSED(elapsedTime)
    Q_UNUSED(payload)

    m_serverSupportsPong = true;
    if(m_pongTimerTimeout)
        m_pongTimerTimeout->stop();

#ifdef WSTRACE
    qDebug("WS:pong!");
#endif // WSTRACE
}

//---------------------------------------------------------------------------------------------
void WSConnectionImpl::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_pingTimerID){
        if(isConnected()) {
            m_socket->ping();

            if(m_serverSupportsPong && m_pongTimerTimeout)
                m_pongTimerTimeout->start(pongTimeoutMsec);
#ifdef WSTRACE
            qDebug("WS:ping!");
#endif // WSTRACE
        }
    }
}
