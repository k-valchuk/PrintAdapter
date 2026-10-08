#include "SubscribersNotifier.h"
#include <QJsonArray>

//#define networkInfo

//-----------------------------------------------------------------------------
SubscribersNotifier::SubscribersNotifier()
{
    qRegisterMetaType<ISubscriber::Type>();
    QObject::connect(this, &Notifier::connected, this, [this]() {
        if(m_wasDisconnectedByError == false)
            emit sig_firstlyConnected();
        emit sig_connected();
        UpdateSubscriptionsList();
        if(reconnectionPeriodMsec && m_wasDisconnectedByError) {
            m_wasDisconnectedByError = false;
            emit sig_recovered();
        }
        if(reconnectTimer.isActive())
            reconnectTimer.stop();

#ifdef networkInfo
        qDebug() << "Notifire "<< " (" << this << ") " << "connected "<< this->getPath() << " : " << m_subscribers.keys();
#endif
    });
    QObject::connect(this, &Notifier::disconnected, this, [this](const QString& reason) {
        emit sig_disconnected(reason);
        
        m_wasDisconnectedByError = reason != SubscribersNotifier::clientRequestedDisconnectReason();
        
        if(m_wasDisconnectedByError && reconnectionPeriodMsec){
            //первый раз пробуем сразу
            if(!isConnected())
                this->Connect();
            
            //затем повторяем по интервалу с таймером
            if(!isConnected())
                reconnectTimer.start(reconnectionPeriodMsec);
        }

#ifdef networkInfo
        qDebug() << "Notifire "<< " (" << this << ") " << "disconnected "<< this->getPath() << m_subscribers.keys();
#endif
    });

    // При неудачной попытке подключения к серверу
    QObject::connect(this, &Notifier::error, this, [this](QAbstractSocket::SocketError err) {
        emit sig_error(err);
        
        m_wasDisconnectedByError = true;
        
        if(reconnectionPeriodMsec){
            reconnectTimer.start(reconnectionPeriodMsec);
        }

#ifdef networkInfo
        qDebug() << "Notifire "<< " (" << this << ") " << "error try connect " << err << this->getPath() << m_subscribers.keys();
#endif
    });

    //reconnectTimer.setSingleShot(true);
    connect(&reconnectTimer, &QTimer::timeout, [this](){
        if(!isConnected()){
            this->Connect();
#ifdef networkInfo
            qDebug() << "Try notifire"<< " (" << this << ") " << " connect by internal timer "<< this->getPath();
#endif
        }
        else
            reconnectTimer.stop();
    });
}

SubscribersNotifier::~SubscribersNotifier()
{
    QObject::disconnect(this, nullptr, this, nullptr);
}

//-----------------------------------------------------------------------------
void SubscribersNotifier::Connect()
{
    const QString strUrl = QString("ws://%1:%2%3").arg(server).arg(port).arg(getPath());
    connectTo(strUrl.toStdString().c_str());

#ifdef networkInfo
    qDebug() << "Try notifire"<< " (" << this << ") " << "  connect "<< this->getPath();
#endif
}

//-----------------------------------------------------------------------------
void SubscribersNotifier::Connect(const QString& s, int p)
{
    setServerPath(s, p);
    Connect();

#ifdef networkInfo
    qDebug() << "Try notifire"<< " (" << this << ") " << "  connect "<< this->getPath();
#endif
}

//-----------------------------------------------------------------------------
void SubscribersNotifier::Disconnect()
{
    reconnectTimer.stop();
    Notifier::disconnectNotifier();

#ifdef networkInfo
    qDebug() << "Try notifire"<< " (" << this << ") " << "  disconect "<< this->getPath();
#endif
}

//-----------------------------------------------------------------------------
bool SubscribersNotifier::UpdateSubscriptionsList()
{
    QJsonObject jsRoot;
    QJsonArray jsSubs;
    bool isEmpty = true;
    for (auto subscriber : m_subscribers.keys()) {
        for (auto subscription : subscriber->GetNeededSubscription()) {
            isEmpty = false;
            
            QJsonObject jsSub;
            jsSub["type"] = subscription.type;

            QJsonArray jsObjects;
            for (auto obj : subscription.objects)
                jsObjects.append(obj);
            jsSub["objects"] = jsObjects;

            jsSubs.append(jsSub);
        }
    }

    jsRoot["msgType"] = ISubscriber::SetSubs;
    jsRoot["subscriptions"] = jsSubs;

    QJsonDocument doc(jsRoot);
    QString error;
    if(isEmpty)
        return true;
    else
        return Notifier::setSubscriptions(doc.toJson(), error);
}

//-----------------------------------------------------------------------------
#include "QMetaMethod"
void SubscribersNotifier::SubscribeWith(ISubscriber* sub, Qt::ConnectionType connectionType)
{
    if(m_subscribers.contains(sub))
    {
        Notifier::disconnect(this, nullptr, sub, nullptr);
    }
    m_subscribers.insert(sub, connectionType);
    QObject::connect(this, &SubscribersNotifier::messageReceived, sub,
        [sub](const QString& message){
            if(sub->ShouldBeCalledWhenNothingConnected())
                return sub->OnMessageReceived(message);
            
            auto metaObj = sub->metaObject();
            for(int i = 0; i < metaObj->methodCount(); i++)
            {
                auto method = metaObj->method(i);
                if(method.methodType() == QMetaMethod::MethodType::Signal)
                    if(sub->IsSignalConnected(method))
                        return sub->OnMessageReceived(message);
            }
        }, connectionType);
    QObject::connect(this, &SubscribersNotifier::sig_connected, sub, &ISubscriber::OnConnected, connectionType);
    QObject::connect(this, &SubscribersNotifier::sig_disconnected, sub, &ISubscriber::OnDisconnected, connectionType);
    
    UpdateSubscriptionsList();

    if (Notifier::isConnected())
        QMetaObject::invokeMethod(
            sub, [sub]() { sub->OnConnected(); }, connectionType);
}

//-----------------------------------------------------------------------------
void SubscribersNotifier::Unsubscribe(ISubscriber* sub)
{
    auto connectionType = m_subscribers[sub];
    m_subscribers.remove(sub);
    Notifier::disconnect(this, nullptr, sub, nullptr);

    UpdateSubscriptionsList();

    QMetaObject::invokeMethod(
        sub, [sub]() { sub->OnDisconnected(); }, connectionType);
}

//-----------------------------------------------------------------------------
QList<ISubscriber*> SubscribersNotifier::subscribers(void)
{
    return m_subscribers.keys();
}

//-----------------------------------------------------------------------------
ISubscriber* SubscribersNotifier::getSubscriber(ISubscriber::Type t)
{
    for (auto& s : m_subscribers.keys()) {
        auto nsubs = s->GetNeededSubscription();
        for (auto& ns : nsubs) {
            if (ns.type == t)
                return s;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------
void SubscribersNotifier::setReconnection(int msec)
{
    reconnectTimer.setInterval(reconnectionPeriodMsec = msec);
}
//-----------------------------------------------------------------------------
void SubscribersNotifier::stopReconnection(void)
{
    reconnectTimer.stop();
    reconnectionPeriodMsec = 0;
    this->abort();
}

void SubscribersNotifier::startReconnection(int periodMsec)
{
    if(periodMsec)
        setReconnection(periodMsec);

    if(reconnectionPeriodMsec)
    {
        if(!isConnected())
            Connect();
        reconnectTimer.start(reconnectionPeriodMsec);
    }
}
