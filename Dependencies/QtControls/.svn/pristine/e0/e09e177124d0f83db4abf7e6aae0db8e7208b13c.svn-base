#include "ProtocolMain.h"

//-------------------------------------------------------------------------------------------------
ProtocolMain::~ProtocolMain()
{
    if (connection != nullptr)
    {
        disconnect(this, SIGNAL(signalSendByteArray(QByteArray&)), connection, SLOT(slotSendByteArray(QByteArray&))); //отправка

        delete connection;
    }

    connection = nullptr;
}

//-------------------------------------------------------------------------------------------------
SocketConnection* ProtocolMain::getSocketConnection()
{
    return connection;
}

//-------------------------------------------------------------------------------------------------
void ProtocolMain::setSocketConnection(SocketConnection* socketConnection)
{
    if (connection != nullptr)
        delete connection;

    Q_ASSERT(socketConnection);
    if (socketConnection)
    {
        connection = socketConnection;

        qRegisterMetaType<QByteArray>("QByteArray &");

        connect(this, SIGNAL(signalSendByteArray(QByteArray&)), connection, SLOT(slotSendByteArray(QByteArray&))); //отправка
        connectSignalsToSocket(); //прием
    }
}
//-------------------------------------------------------------------------------------------------
