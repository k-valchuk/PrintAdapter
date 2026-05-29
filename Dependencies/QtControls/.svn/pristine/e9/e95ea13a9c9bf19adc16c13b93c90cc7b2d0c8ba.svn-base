#pragma once

#include "SocketConnection.h"

#include <QObject>

class ProtocolServerBase : public QObject
{
	Q_OBJECT

public:
    virtual void newConnection(SocketConnection *socketConnection) = 0;
    virtual void clientDisconnected(int descriptor) = 0;

signals:
	void serverStarted();

public slots:
	virtual void startServer() = 0;

	virtual void Start(qint16 port = 12345) = 0;
	virtual void Stop() = 0;
};
