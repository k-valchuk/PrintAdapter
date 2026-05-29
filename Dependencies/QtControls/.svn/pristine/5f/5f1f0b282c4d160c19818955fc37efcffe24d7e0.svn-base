#pragma once

#include <QTcpServer>
#include <QHostInfo>

#include "ProtocolServerBase.h"

//Класс обеспечивает основному классу работу со слотами и сигналами
class ProtocolServerTCP : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolServerTCP(ProtocolServerBase *server, QObject *parent = 0);
    ~ProtocolServerTCP();

    bool start(qint16 port = 10001);
    void stop();

    void setMaxConnections(int count);

private:
    ProtocolServerBase *serverParent;
    QTcpServer *tcpServer;

	QMap<int, QHostAddress>	lookupHostMap;

signals:
    void signalClientConnected(int descriptor, const QHostAddress &clientAddress);
	void signalClientLookupped(int descriptor, const QString &clientHostName);
	void signalClientDisconnected(int descriptor, QAbstractSocket::SocketError error);

private slots:
    void slotNewConnection();
	void slotLookupHostConnected(const QHostInfo &host);
	void slotClientDisconnected(QAbstractSocket::SocketError error);
};
