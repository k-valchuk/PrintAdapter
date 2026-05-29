#pragma once

#include "ProtocolServerBase.h"
#include "ProtocolServerTCP.h"

#include <QThread>

//От пользователя только требуется создать экземпляр класса ProtocolServer с указанием протокола, по которому будет вестись передача данных
//Пример: ProtocolServer<ProtocolProxyCP> server;
template <typename T>
class ProtocolServer : public ProtocolServerBase
{
public:
    explicit ProtocolServer(); //Конструктор
    ~ProtocolServer(); //Деструктор

    void startServer() override;

    T* getProtocol(int descriptor); //Получение протокола  по дескриптору соединения чтобы впоследствии отправлять и принимать данные от него
    QMap<int, T*> getProtocols(); //Список протоколов, тоесть соединений с клиентами
    ProtocolServerTCP* protocolServerTCP();

    bool isWorking(); //Сервер работает

    void newConnection(SocketConnection *socketConnection) override; //Новое соединение
    void clientDisconnected(int descriptor) override; //Клиент отсоединился

    bool isClientConnected(int descriptor); //клиент подсоединен
    inline bool isAnyClientsConnected(void) const { return !protocols.isEmpty(); }

    void setMaxConnections(int count); //Ограничить максимальное количество клиентов у сервера

    // Разоравать соединение клиента с clientId
    bool dropConnection(int clientId) {
        auto it = protocols.find(clientId);
        if (it != protocols.end()) {
            auto socket = it.value()->getSocketConnection();
            if(socket)
                socket->disconnectFromServer();
            return true;
        }

        return false;
    }

public slots:
    void Start(qint16 port = 12345) override; //Запуск поиска клиентов
    void Stop() override; //Остановка поиска клиентов

private:
    ProtocolServerTCP *tcpServer;
    QMap<int, T*> protocols;
    bool serverIsWork;
};

//-------------------------------------------------------------------------------------------------
template <typename T>
ProtocolServer<T>::ProtocolServer()
:serverIsWork(false), tcpServer(nullptr)
{
}

//-------------------------------------------------------------------------------------------------
template <typename T>
ProtocolServer<T>::~ProtocolServer()
{
    Stop();

    delete tcpServer;
    tcpServer = nullptr;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void ProtocolServer<T>::startServer()
{
    tcpServer = new ProtocolServerTCP(this);

    emit serverStarted();
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void ProtocolServer<T>::Start(qint16 port)
{
    if (!serverIsWork) //если не работал
    {
        if (tcpServer->start(port)) //и стартанул, то
        {
            serverIsWork = true; //работает
            //return true;
        }

        //return false; //не стартанул
    }

    //return true; //уже работает
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void ProtocolServer<T>::Stop()
{
    if (serverIsWork)
    {
        foreach(T *prot, protocols)
            delete prot;

        protocols.clear();
        tcpServer->stop();
        serverIsWork = false;
    }
}

//-------------------------------------------------------------------------------------------------
template <typename T>
T* ProtocolServer<T>::getProtocol(int descriptor) { return protocols[descriptor]; }

//-------------------------------------------------------------------------------------------------
template <typename T>
QMap<int, T*> ProtocolServer<T>::getProtocols() { return protocols; }

//-------------------------------------------------------------------------------------------------
template <typename T>
ProtocolServerTCP* ProtocolServer<T>::protocolServerTCP() { return tcpServer; }

//-------------------------------------------------------------------------------------------------
template <typename T>
bool ProtocolServer<T>::isWorking() { return serverIsWork; }

template <typename T>
bool ProtocolServer<T>::isClientConnected(int descriptor) { return protocols.keys().contains(descriptor); }

//-------------------------------------------------------------------------------------------------
template <typename T>
void ProtocolServer<T>::newConnection(SocketConnection *socketConnection)
{
    T* newProtocol = new T;
    int descriptor = socketConnection->getDescriptorID();

    QObject::connect(socketConnection, SIGNAL(signalSocketDisconnected(QAbstractSocket::SocketError)), tcpServer, SLOT(slotClientDisconnected(QAbstractSocket::SocketError)));

    protocols[descriptor] = newProtocol;

    newProtocol->setSocketConnection(socketConnection);
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void ProtocolServer<T>::clientDisconnected(int descriptor)
{
    (protocols[descriptor])->deleteLater();
    protocols.remove(descriptor);
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void ProtocolServer<T>::setMaxConnections(int count) { tcpServer->setMaxConnections(count); }
//-------------------------------------------------------------------------------------------------
