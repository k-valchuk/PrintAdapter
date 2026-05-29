#include <QSqlQuery>
#include <QSqlError>

#include "DBConnector.h"

//---------------------------------------------------------------------
DBConnector::DBConnector(Logger *_logger, DBType db_type, const QString &_db_name, const QString &db_ip1, const QString &db_ip2, int db_port,
						 const QString &db_user, const QString &db_pass, int db_timeout, int reconnect_timeout, QObject *parent)
			:QObject(parent)
{
	logger = _logger;
	codec = QTextCodec::codecForName("CP1251");

	id_timer_db = 0;
	timeout_to_connect = reconnect_timeout;
	connected = false;

	db_server1 = db_ip1;
	db_server2 = db_ip2;
	db_name = _db_name;

    DBConnection::initDB(db, db_type,  "Connector_1" + db_name, db_name, db_server1, db_port, db_user, db_pass, db_timeout);
}

//---------------------------------------------------------------------
DBConnector::~DBConnector()
{
    stopConnecting();
    MessageDebug("~DBConnector!");
}

//---------------------------------------------------------------------------
void DBConnector::MessageDebug(const QString &str, unsigned int code)
{
	if (logger)
		logger->SaveMessage(tr("DBConnector. ")+str,code);
}

//-----------------------------------------------------------------------------------
void DBConnector::startConnecting()			//запускает таймер на соединение с БД
{
	if (id_timer_db==0)
	{
		id_timer_db = startTimer(timeout_to_connect);

QTimerEvent *e = new QTimerEvent(id_timer_db);
		timerEvent(e);
		delete e;
	}
}

//-----------------------------------------------------------------------------------
void DBConnector::changeDatabaseName(QString db_name) //изменяет имя БД
{
	db.setDatabaseName(db_name);
}

//-----------------------------------------------------------------------------------
void DBConnector::stopConnecting()			//останавливает таймер
{
	if (id_timer_db>0)
	{
		killTimer(id_timer_db);
		id_timer_db = 0;
	}
}

//-----------------------------------------------------------------------------------
void DBConnector::timerEvent(QTimerEvent *event)
{
	if (event->timerId()==id_timer_db)
	{
		if (!connected) //есть ли уже соединение? если нет, то -
		{
			bool con = db.open(); //открываем

			if (con) //если открылась
			{
				connected = true;
                emit connectionDBOpen(db.hostName(), db.connectionName());
				MessageDebug(QString::fromUtf8("Соединение с БД установлено! (connectionname=%1, hostName=%2)").arg(db.connectionName()).arg(db.hostName()));
			}
			else //не открылась
			{
				MessageDebug(QString::fromUtf8("Ошибка подсоединения к БД (%1) '%2' (host: %3)!").arg(db.databaseName()).arg(db.lastError().text()).arg(db.hostName()));

				//попробуем к другому подсоединиться
				changeActiveDBHost();
			}
		}
		else //уже есть - проверяем
		{
			QSqlQuery query(db); //для подтверждения соединения

			if (!query.exec("select 1")) //потеряли соединение
			{
				connected = false;
				emit connectionDBClosed();
				MessageDebug(QString::fromUtf8("Соединение с БД разорвано! Error: %1").arg(db.lastError().text()));

				//попробуем к другому подсоединиться
				changeActiveDBHost();
			}
		}
	}
}

//-----------------------------------------------------------------------------------
void DBConnector::changeActiveDBHost()
{
	if (!db_server2.isEmpty()) //если задано второе имя DB сервера, то переключаемся
	{
		QString db_server_active = db.hostName(); //текущий сервер
		if (db_server_active == db_server1) //подключались к первому
			db_server_active = db_server2;
		else
			db_server_active = db_server1;

		//для следующей попытки - меняем host
		db.setDatabaseName(DBConnection::getConnectionString(db_server_active, db_name));
		db.setHostName(db_server_active);
	}
}
