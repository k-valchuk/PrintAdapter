#pragma once

#include <QString>
#include <QHostAddress>
#include <QHostInfo>

#include "ProtocolClientBase.h"
#include "ProtocolClientTCP.h"

//От пользователя только требуется создать экземпляр класса ProtocolClient с указанием протокола, по которому будет вестись передача данных
//Пример: ProtocolClient<ProtocolCP> client;
template <typename T>
class ProtocolClient : public ProtocolClientBase
{
public:
    explicit ProtocolClient(); //Конструктор
    virtual ~ProtocolClient(); //Деструктор

    void connectTo(QString ip = QString(), quint16 port = 10001) override; //Соединение с сервером
	void connectTo(QString ip, quint16 port, quint16 waittime) override; //Соединение с сервером

    void disconnectFrom(); //Отсоединение от сервера

    T* getProtocol();  //Получение протокола чтобы впоследствии отправлять и принимать данные от него
	ProtocolClientTCP* protocolClientTCP();
    bool isClientConnected(); //Соединение с сервером есть и работает

private:
    ProtocolClientTCP *tcpClient;
    T *protocol;

    void disconnect_silence() override;
};

//-------------------------------------------------------------------------------------------------
template <typename T>
ProtocolClient<T>::ProtocolClient()
:tcpClient(0), protocol(0)
{
	protocol = new T;
	tcpClient = new ProtocolClientTCP(this);
	SocketConnection* connection = new SocketConnection;

	QObject::connect(this, SIGNAL(signalConnectToServer(QString, quint16)), connection, SLOT(connectToServer(QString, quint16)), Qt::QueuedConnection); //подсоединиться к серверу
	QObject::connect(this, SIGNAL(signalConnectToServer(QString, quint16, quint16)), connection, SLOT(connectToServer(QString, quint16, quint16)), Qt::QueuedConnection); //подсоединиться к серверу
	QObject::connect(this, SIGNAL(signalDisconnectFromServer()), connection, SLOT(disconnectFromServer()), Qt::QueuedConnection); //отсоединиться от сервера

	QObject::connect(connection, SIGNAL(signalSocketConnected()), tcpClient, SLOT(slotConnectedToServer()), Qt::QueuedConnection); //подсоединились
	QObject::connect(connection, SIGNAL(signalSocketConnectionError(QAbstractSocket::SocketError)), tcpClient, SLOT(slotConnectionErrorToServer(QAbstractSocket::SocketError)), Qt::QueuedConnection); //подсоединились
	QObject::connect(connection, SIGNAL(signalSocketDisconnected(QAbstractSocket::SocketError)), tcpClient, SLOT(slotDisconnectedFromServer(QAbstractSocket::SocketError)), Qt::QueuedConnection); //отсоединились

	protocol->setSocketConnection(connection);
}

//-------------------------------------------------------------------------------------------------
template <typename T>
ProtocolClient<T>::~ProtocolClient()
{
    delete tcpClient;
    delete protocol;
	//connection deleting in ProtocolMain(ProtocolBase(ProtocolVSM))
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void ProtocolClient<T>::connectTo(QString ip, quint16 port)
{
	if (!protocol) //ещё не создались
		return;

	SocketConnection *connection = protocol->getSocketConnection();

	if (connection)
    {
		if (connection->getDescriptorID() > 0) //подсоединен
			return;

		emit signalConnectToServer(ip, port);
    }

    //были подсоединены
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void ProtocolClient<T>::connectTo(QString ip, quint16 port, quint16 waittime)
{
	if (!protocol) //ещё не создались
		return;

	SocketConnection *connection = protocol->getSocketConnection();

	if (connection)
	{
		if (connection->getDescriptorID() > 0) //подсоединен
			return;

		emit signalConnectToServer(ip, port, waittime);
	}

	//были подсоединены
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void ProtocolClient<T>::disconnectFrom()
{
	disconnect_silence(); //отсоединяемся
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void ProtocolClient<T>::disconnect_silence()
{
	if (isClientConnected()) //были подсоединены
	{
		emit signalDisconnectFromServer();

		protocol->clear();
	}
}

//-------------------------------------------------------------------------------------------------
template <typename T>
T* ProtocolClient<T>::getProtocol() { return protocol; }

//-------------------------------------------------------------------------------------------------
template <typename T>
ProtocolClientTCP* ProtocolClient<T>::protocolClientTCP() { return tcpClient; }

//-------------------------------------------------------------------------------------------------
template <typename T>
bool ProtocolClient<T>::isClientConnected() //Соединение с сервером есть и работает
{
	if (!protocol) //ещё не создались
		return false;

	SocketConnection *connection = protocol->getSocketConnection();

	if (connection && connection->getDescriptorID() > 0) //подсоединен
		return true;

	return false;
}
//-------------------------------------------------------------------------------------------------
