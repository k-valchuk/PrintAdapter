#pragma once

#include "SocketConnection.h"

#include <QObject>

class ProtocolClientBase : public QObject
{
	Q_OBJECT

public:
    virtual void connectTo(QString ip = QString(), quint16 port = 10001) = 0; //emit signalConnectToServer
	virtual void connectTo(QString ip, quint16 port, quint16 waittime) = 0; //emit signalConnectToServer
    virtual void disconnect_silence() = 0;

signals:
	void signalConnectToServer(QString hostName, quint16 port);
	void signalConnectToServer(QString hostName, quint16 port, quint16 waitTime);
	void signalDisconnectFromServer();
};
