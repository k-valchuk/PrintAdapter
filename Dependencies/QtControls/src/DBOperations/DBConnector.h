#pragma once

#include <QtCore>
#include <QSqlDatabase>

#include "Logger.h"
#include "DBConnection.h"

class DBConnector : public QObject
{
	Q_OBJECT

public:
	DBConnector(Logger *_logger, DBType db_type, const QString &_db_name, const QString &db_ip1, const QString &db_ip2, int db_port,
				const QString &db_user, const QString &db_pass, int db_timeout,	int reconnect_timeout, QObject *parent = NULL);
	~DBConnector();

public slots:
	void startConnecting();			//запускает таймер на соединение с БД
	void changeDatabaseName(QString db_name); //изменяет имя БД
	void stopConnecting();			//останавливает таймер

signals:
	void finished(); 	// сигнал о завершении работы
    void connectionDBOpen(const QString &hostName, const QString &conName);	// сигнал о соединении с БД
	void connectionDBClosed();	// сигнал о отсоединении от БД

protected:
	void timerEvent(QTimerEvent *event);

private:
	QTextCodec	*codec;
	Logger		*logger;
	void		MessageDebug(const QString &s, unsigned int code=0); //для Logger

	QSqlDatabase db;
	QString		db_server1, db_server2, db_name;

	int			id_timer_db; //таймер на соединение с БД
	int			timeout_to_connect; //таймаут на соединение с БД (в мс)
	bool		connected; //текущее состояние соединения с БД

	void		changeActiveDBHost(); //изменение текущего имени машины DB server
};
