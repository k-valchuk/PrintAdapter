#include "ProtocolServerTCP.h"

#include  <QThread>

//-------------------------------------------------------------------------------------------------
ProtocolServerTCP::ProtocolServerTCP(ProtocolServerBase *server, QObject *parent)
{
    Q_UNUSED(parent);
    serverParent = server;
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

//-------------------------------------------------------------------------------------------------
ProtocolServerTCP::~ProtocolServerTCP()
{
    stop();
    delete tcpServer;
}

//-------------------------------------------------------------------------------------------------
void ProtocolServerTCP::slotNewConnection()
{
	QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    SocketConnection *newConnection = new SocketConnection(clientSocket);

    serverParent->newConnection(newConnection);
	
	lookupHostMap[newConnection->getDescriptorID()] = clientSocket->peerAddress();

	QHostInfo::lookupHost(clientSocket->peerAddress().toString(), this, SLOT(slotLookupHostConnected(QHostInfo)));

	emit signalClientConnected(newConnection->getDescriptorID(), clientSocket->peerAddress());
}

//-------------------------------------------------------------------------------------------------
void ProtocolServerTCP::slotLookupHostConnected(const QHostInfo &host)
{
	QString clientHostName;
	int descriptor = 0;

	QList<QHostAddress> addresses = host.addresses();

	QHostAddress addr;

	if (addresses.isEmpty())
		addr = QHostAddress(host.hostName());
	else
		addr = addresses.first();

	for (int i = 0; i < lookupHostMap.values().count(); i++)
		if (lookupHostMap.values()[i] == addr)
		{
			descriptor = lookupHostMap.keys()[i];
			lookupHostMap.remove(descriptor);
			break;
		}

	if (addr.isLoopback())
		clientHostName = "localhost";
	else
	{
		if (host.error() == QHostInfo::NoError) //все нашли
			clientHostName = host.hostName(); //берем имя машины
		else //не нашли - берем ipV4
			clientHostName = QHostAddress(addr.toIPv4Address()).toString();
	}

	if(tcpServer->isListening())
		emit signalClientLookupped(descriptor, clientHostName);
}

//-------------------------------------------------------------------------------------------------
void ProtocolServerTCP::slotClientDisconnected(QAbstractSocket::SocketError error)
{
    SocketConnection *connection = (SocketConnection*)sender();
    int descriptor = connection->getDescriptorID();

    serverParent->clientDisconnected(descriptor);

    emit signalClientDisconnected(descriptor, error);
}

//-------------------------------------------------------------------------------------------------
bool ProtocolServerTCP::start(qint16 port)
{
	if (tcpServer->listen(QHostAddress::AnyIPv4, port))
        return true;

    return false;
}

//-------------------------------------------------------------------------------------------------
void ProtocolServerTCP::stop()
{
    tcpServer->close();
}

//-------------------------------------------------------------------------------------------------
void ProtocolServerTCP::setMaxConnections(int count)
{
    tcpServer->setMaxPendingConnections(count);
}
//-------------------------------------------------------------------------------------------------
