#pragma once
#include <QObject>
#include "SocketConnection.h"

class ProtocolMain : public QObject
{
    Q_OBJECT
public:
    ~ProtocolMain();

    SocketConnection* getSocketConnection();
    void setSocketConnection(SocketConnection* socketConnection);

    virtual void clear() = 0;

protected:
    SocketConnection* connection = nullptr;
    virtual void connectSignalsToSocket() = 0;

signals:
    void signalSendByteArray(QByteArray &byteArray);
};
