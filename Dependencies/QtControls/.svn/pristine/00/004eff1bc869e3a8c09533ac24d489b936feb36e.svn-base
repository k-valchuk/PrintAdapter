#pragma once

#include <QtCore>
#include <QtSql>

#include "Logger.h"

enum DBType
{
	eDBPostgres = 0,
	eDBOracle = 1,
	eDBMSSQL = 2,
	eDBSQLite = 3
};

class DBConnection : public QObject
{
public:

	static QString getConnectionString(const QString &db_server, const QString &db_name);

	static void initDB(QSqlDatabase &db, DBType db_type, const QString &db_conn_name, const QString &db_name,
					   const QString &db_ip, int db_port, const QString &db_user, const QString &db_pass, int db_timeout = 5);

    static QSqlDatabase initDB(DBType db_type, const QString &db_conn_name, const QString &db_name,
                       const QString &db_ip, int db_port, const QString &db_user, const QString &db_pass, int db_timeout = 5);

	static bool	prepareQuery(QSqlQuery &query, const QString &str, Logger *logger=0);
	static bool	executeQuery(QSqlQuery &query, Logger *logger=0);
};
