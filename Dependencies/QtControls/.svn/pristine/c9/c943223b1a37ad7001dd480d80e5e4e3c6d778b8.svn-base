#pragma once

#include <QObject>
#include <QTcpSocket>

class SocketConnection: public QObject
{
	Q_OBJECT
public:
    explicit SocketConnection(QTcpSocket *soc = NULL);
    ~SocketConnection();

	int getDescriptorID() { return descriptorID; };
    QString getIP();

	QByteArray readAll(quint32 waitFirstTime = 0, quint32 waitNextTime = -1);

	static QString getSocketErrorString(QAbstractSocket::SocketError error);
	static QString getSocketStateString(QAbstractSocket::SocketState state);

private:
    QTcpSocket *socket = nullptr;
	int descriptorID = -2;

	void initSocket();

	qint32 bytesAvailable();

	void setSocketKeepAliveOption(void);

signals:
	//to main
    void signalSocketConnected();
	void signalSocketConnectionError(QAbstractSocket::SocketError error);
    void signalSocketDisconnected(QAbstractSocket::SocketError error);

	void error(QAbstractSocket::SocketError);
	void stateChanged(QAbstractSocket::SocketState socketState);
	
	//to ProtocolBase
    void signalGotData();

public slots:
	//from main
	void connectToServer(QString hostName, quint16 port);
	void connectToServer(QString hostName, quint16 port, quint16 waitTime);
	void disconnectFromServer();

	//from ProtocolBase
	void slotSendByteArray(QByteArray &byteArray);

private slots:
	//from QTcpSocket
	void slotSocketRead();
    void slotSocketConnected();
    void slotSocketDisconnected();
	void errorSlot(QAbstractSocket::SocketError);
	void stateChangedSlot(QAbstractSocket::SocketState socketState);
};
