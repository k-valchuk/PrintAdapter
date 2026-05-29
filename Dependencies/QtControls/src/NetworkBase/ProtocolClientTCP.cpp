#include "ProtocolClientTCP.h"

//-------------------------------------------------------------------------------------------------
ProtocolClientTCP::ProtocolClientTCP(ProtocolClientBase *client, QObject *parent)
{
    Q_UNUSED(parent);
    clientParent = client;
}

//-------------------------------------------------------------------------------------------------
ProtocolClientTCP::~ProtocolClientTCP()
{

}

//-------------------------------------------------------------------------------------------------
void ProtocolClientTCP::slotConnectedToServer()
{
	emit signalConnectedToServer();
}

//-------------------------------------------------------------------------------------------------
void ProtocolClientTCP::slotDisconnectedFromServer(QAbstractSocket::SocketError error)
{
	clientParent->disconnect_silence();

    emit signalDisconnectedFromServer(error);
}

//-------------------------------------------------------------------------------------------------
void ProtocolClientTCP::slotConnectionErrorToServer(QAbstractSocket::SocketError error)
{
	clientParent->disconnect_silence();

	emit signalConnectionErrorToServer(error);
}
