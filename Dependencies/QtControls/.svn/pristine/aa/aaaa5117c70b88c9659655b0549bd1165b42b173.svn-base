#include <QHostAddress>

#include "SocketConnection.h"

#ifdef WIN32
#include <winsock2.h>

#define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4)

struct tcp_keepalive {
	u_long onoff;
	u_long keepalivetime;
	u_long keepaliveinterval;
};
#else // linux or mac
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#endif //WIN32

//-------------------------------------------------------------------------------------------------
SocketConnection::SocketConnection(QTcpSocket *soc)
{
    if (soc == NULL)
		socket = new QTcpSocket();
	else
	{
		socket = soc;
		descriptorID = socket->socketDescriptor();

		// Установка опций для сокетов сервера
		setSocketKeepAliveOption();
	}

	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

	initSocket();
}

//-------------------------------------------------------------------------------------------------
SocketConnection::~SocketConnection()
{
    if (socket == nullptr)
        return;

    QObject::disconnect(socket, SIGNAL(disconnected()), 0, 0);
    if (socket->socketDescriptor()!=-1)
        socket->disconnectFromHost();

	delete socket;
	socket = nullptr;
}

//-------------------------------------------------------------------------------------------------
QString SocketConnection::getIP()
{
	if (socket == nullptr)
		return "";

	return socket->peerAddress().toString();
}

//-------------------------------------------------------------------------------------------------
void SocketConnection::initSocket()
{
	QObject::connect(socket, SIGNAL(connected()), this, SLOT(slotSocketConnected()));
	QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(slotSocketRead()));
	QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconnected()));
	QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)));
	QObject::connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChangedSlot(QAbstractSocket::SocketState)));
}

//-------------------------------------------------------------------------------------------------
void SocketConnection::connectToServer(QString hostName, quint16 port)
{
	if (getDescriptorID() < 0) //не соединены - соединяемся
		socket->connectToHost(hostName, port);
}

//-------------------------------------------------------------------------------------------------
void SocketConnection::connectToServer(QString hostName, quint16 port, quint16 waitTime)
{
	if (getDescriptorID() < 0) //не соединены - соединяемся
	{
		socket->connectToHost(hostName, port);
		if (!socket->waitForConnected(waitTime))
			emit signalSocketConnectionError(socket->error());
	}
}

//-------------------------------------------------------------------------------------------------
void SocketConnection::slotSocketConnected()
{
	descriptorID = socket->socketDescriptor();

	// Установка опций после подключения клиента
	setSocketKeepAliveOption();

	emit signalSocketConnected();
}

//-------------------------------------------------------------------------------------------------
void SocketConnection::disconnectFromServer()
{
	if (socket == NULL)
        return;

	if (socket->socketDescriptor() != -1)
        socket->disconnectFromHost();
}

//-------------------------------------------------------------------------------------------------
void SocketConnection::slotSocketDisconnected()
{
	emit signalSocketDisconnected(socket->error());

	descriptorID = -1;
}

//-------------------------------------------------------------------------------------------------
qint32 SocketConnection::bytesAvailable()
{
    if (socket == NULL)
        return -1;

    return socket->bytesAvailable();
}

//-------------------------------------------------------------------------------------------------
void SocketConnection::setSocketKeepAliveOption(void)
{
#ifdef WIN32
	DWORD dwBytes;
	tcp_keepalive pClSock_tcpKeepalive = { 0 }, sReturned = { 0 };
	pClSock_tcpKeepalive.onoff = 1;					//включить keepalive
	pClSock_tcpKeepalive.keepalivetime = 1000;		// каждую "1.000" секунду отсылать пакет
	pClSock_tcpKeepalive.keepaliveinterval = 1500;	// Если не пришел ответ выслать через 1.5с повторно
	if (WSAIoctl(descriptorID, SIO_KEEPALIVE_VALS, &pClSock_tcpKeepalive, sizeof(pClSock_tcpKeepalive), &sReturned, 
		sizeof(sReturned), &dwBytes, NULL, NULL) != 0)
	{
		qDebug("WSAIoctl failed: %d", WSAGetLastError());
	}
#else // linux or mac
	int optval = 1;
	socklen_t optlen = sizeof(optval);

   // Включение самой функции KEEP ALIVE
#ifdef MACX
    if (setsockopt(descriptorID, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0)
#else // LINUX
    if (setsockopt(descriptorID, IPPROTO_TCP, TCP_KEEPIDLE, &optval, optlen) < 0)
#endif
    {
		qDebug("setsockopt enable SO_KEEPALIVE failture!");
		return;
	}
	
	/*TCP_KEEPCNT (since Linux 2.4)
	The maximum number of keepalive probes TCP should send before
		dropping the connection.This option should not be used in
		code intended to be portable.*/
	optval = 2;
	if (setsockopt(descriptorID, IPPROTO_TCP, TCP_KEEPCNT, &optval, optlen) < 0)
	{
		qDebug("setsockopt enable TCP_KEEPCNT failture!");
		return;
	}

	/*TCP_KEEPIDLE (since Linux 2.4)
              The time (in seconds) the connection needs to remain idle
              before TCP starts sending keepalive probes, if the socket
              option SO_KEEPALIVE has been set on this socket.  This option
              should not be used in code intended to be portable.*/
	optval = 2;
    if (setsockopt(descriptorID, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0)
	{
		qDebug("setsockopt enable TCP_KEEPIDLE failture!");
		return;
	}

	/*TCP_KEEPINTVL(since Linux 2.4)
		The time(in seconds) between individual keepalive probes.
		This option should not be used in code intended to be
		portable.*/
	optval = 1;
	if (setsockopt(descriptorID, IPPROTO_TCP, TCP_KEEPINTVL, &optval, optlen) < 0)
	{
		qDebug("setsockopt enable TCP_KEEPINTVL failture!");
		return;
	}

#endif // WIN32
}

//-------------------------------------------------------------------------------------------------
void SocketConnection::errorSlot(QAbstractSocket::SocketError err)
{
	if (getDescriptorID() < 0) //нет дескриптора - значит ошибка подсоединения, а не разрыва
		emit signalSocketConnectionError(socket->error());

	emit error(err);
}

void SocketConnection::stateChangedSlot(QAbstractSocket::SocketState socketState)
{
	emit stateChanged(socketState);
}

//-------------------------------------------------------------------------------------------------
QString SocketConnection::getSocketErrorString(QAbstractSocket::SocketError error)
{
	switch (error)
	{
		case QAbstractSocket::ConnectionRefusedError:			return QString("ConnectionRefusedError");
		case QAbstractSocket::RemoteHostClosedError:			return QString("RemoteHostClosedError");
		case QAbstractSocket::HostNotFoundError:				return QString("HostNotFoundError");
		case QAbstractSocket::SocketAccessError:				return QString("SocketAccessError");
		case QAbstractSocket::SocketResourceError:				return QString("SocketResourceError");
		case QAbstractSocket::SocketTimeoutError:				return QString("SocketTimeoutError");
		case QAbstractSocket::DatagramTooLargeError:			return QString("DatagramTooLargeError");
		case QAbstractSocket::NetworkError:						return QString("NetworkError");
		case QAbstractSocket::AddressInUseError:				return QString("AddressInUseError");
		case QAbstractSocket::SocketAddressNotAvailableError:	return QString("SocketAddressNotAvailableError");
		case QAbstractSocket::UnsupportedSocketOperationError:	return QString("UnsupportedSocketOperationError");
		case QAbstractSocket::UnfinishedSocketOperationError:	return QString("UnfinishedSocketOperationError");
		case QAbstractSocket::ProxyAuthenticationRequiredError:	return QString("ProxyAuthenticationRequiredError");
		case QAbstractSocket::SslHandshakeFailedError:			return QString("SslHandshakeFailedError");
		case QAbstractSocket::ProxyConnectionRefusedError:		return QString("ProxyConnectionRefusedError");
		case QAbstractSocket::ProxyConnectionClosedError:		return QString("ProxyConnectionClosedError");
		case QAbstractSocket::ProxyConnectionTimeoutError:		return QString("ProxyConnectionTimeoutError");
		case QAbstractSocket::ProxyNotFoundError:				return QString("ProxyNotFoundError");
		case QAbstractSocket::ProxyProtocolError:				return QString("ProxyProtocolError");
		case QAbstractSocket::OperationError:					return QString("OperationError");
		case QAbstractSocket::SslInternalError:					return QString("SslInternalError");
		case QAbstractSocket::SslInvalidUserDataError:			return QString("SslInvalidUserDataError");
		case QAbstractSocket::TemporaryError:					return QString("TemporaryError");
	}

	return QString("Unknown socket error");
}

//-------------------------------------------------------------------------------------------------
QString SocketConnection::getSocketStateString(QAbstractSocket::SocketState state)
{
	switch (state)
	{
		case QAbstractSocket::UnconnectedState:	return QString("UnconnectedState");
		case QAbstractSocket::HostLookupState:	return QString("HostLookupState");
		case QAbstractSocket::ConnectingState:	return QString("ConnectingState");
		case QAbstractSocket::ConnectedState:	return QString("ConnectedState");
		case QAbstractSocket::BoundState:		return QString("BoundState");
		case QAbstractSocket::ListeningState:	return QString("ListeningState");
		case QAbstractSocket::ClosingState:		return QString("ClosingState");
	}
	return QString("UnknownState");
}

//-------------------------------------------------------------------------------------------------
QByteArray SocketConnection::readAll(quint32 waitFirstTime, quint32 waitNextTime)
{
    QByteArray byteArray;

    if (socket->bytesAvailable())
    {
        while (socket->bytesAvailable())
            byteArray.append(socket->readAll());

    }
    else if (socket->waitForReadyRead(waitFirstTime))
    {
        byteArray.append(socket->readAll());
    }
    else
        return byteArray;

    if (waitNextTime == (quint32)-1)
        waitNextTime = waitFirstTime;

    while (socket->waitForReadyRead(waitNextTime))
        byteArray.append(socket->readAll());

    return byteArray;
}

//-------------------------------------------------------------------------------------------------
void SocketConnection::slotSocketRead() //прием
{
	emit signalGotData();
}

//-------------------------------------------------------------------------------------------------
void SocketConnection::slotSendByteArray(QByteArray &byteArray) //передача
{
    if (socket == NULL)
        return;

	if (socket->isOpen())
		socket->write(byteArray);
}
//-------------------------------------------------------------------------------------------------
