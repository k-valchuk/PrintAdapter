#pragma once

#include <QObject>
#include <QString>
#include "ProtocolClientBase.h"

//Класс обеспечивает основному классу работу со слотами и сигналами
class ProtocolClientTCP : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolClientTCP(ProtocolClientBase *client, QObject *parent = 0);
    ~ProtocolClientTCP();

private:
    ProtocolClientBase *clientParent;

signals:
    void signalConnectedToServer();
    void signalDisconnectedFromServer(QAbstractSocket::SocketError error);
	void signalConnectionErrorToServer(QAbstractSocket::SocketError error);

private slots:
    void slotConnectedToServer();
    void slotDisconnectedFromServer(QAbstractSocket::SocketError error);
	void slotConnectionErrorToServer(QAbstractSocket::SocketError error);
};
