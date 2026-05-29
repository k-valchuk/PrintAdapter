#include <QFile>
#include <QSqlDatabase>
#include <QThread>

#include "DBConnection.h"

//----------------------------------------------------------------------------
void DBConnection::initDB(QSqlDatabase& db, DBType db_type, const QString& db_conn_name, const QString& db_name,
    const QString& db_ip, int db_port, const QString& db_user, const QString& db_pass, int db_timeout)
{
    QString db_type_name, db_timeout_text;

    switch (db_type) {
    case eDBPostgres:
        db_type_name = "QPSQL";
        db_timeout_text = "connect_timeout";
        break;

    case eDBOracle:
    case eDBMSSQL:
        db_type_name = "QODBC";
        db_timeout_text = "SQL_ATTR_CONNECTION_TIMEOUT";
        break;

    case eDBSQLite:
        db_type_name = "QSQLITE";
        db_timeout_text = "QSQLITE_BUSY_TIMEOUT";
        break;
    }

    db = QSqlDatabase::addDatabase(db_type_name, db_conn_name);

    db.setConnectOptions(QString("%1=%2").arg(db_timeout_text).arg(db_timeout));

    db.setHostName(db_ip);
    db.setPort(db_port);

    if (db_type == eDBMSSQL || db_type == eDBOracle)
        db.setDatabaseName(getConnectionString(db_ip, db_name));
    else
        db.setDatabaseName(db_name);
    db.setUserName(db_user);
    db.setPassword(db_pass);
}

//----------------------------------------------------------------------------
QSqlDatabase DBConnection::initDB(DBType db_type, const QString& db_conn_name, const QString& db_name,
    const QString& db_ip, int db_port, const QString& db_user, const QString& db_pass, int db_timeout)
{
    QString db_type_name, db_timeout_text;

    switch (db_type) {
    case eDBPostgres:
        db_type_name = "QPSQL";
        db_timeout_text = "connect_timeout";
        break;

    case eDBOracle:
    case eDBMSSQL:
        db_type_name = "QODBC";
        db_timeout_text = "SQL_ATTR_CONNECTION_TIMEOUT";
        break;

    case eDBSQLite:
        db_type_name = "QSQLITE";
        db_timeout_text = "QSQLITE_BUSY_TIMEOUT";
        break;
    }

    auto db = QSqlDatabase::addDatabase(db_type_name, db_conn_name);

    db.setConnectOptions(QString("%1=%2").arg(db_timeout_text).arg(db_timeout));

    db.setHostName(db_ip);
    db.setPort(db_port);

    if (db_type == eDBMSSQL || db_type == eDBOracle)
        db.setDatabaseName(getConnectionString(db_ip, db_name));
    else
        db.setDatabaseName(db_name);
    db.setUserName(db_user);
    db.setPassword(db_pass);

    return db;
}

//---------------------------------------------------------------------------
QString DBConnection::getConnectionString(const QString& db_server, const QString& db_name)
{
#ifdef WIN32
    return QString("DRIVER={SQL Server};Server=%1;Database=%2").arg(db_server).arg(db_name);
#else //под CentOS
    return QString("DRIVER={ODBC13};Server=%1;Database=%2").arg(db_server).arg(db_name);
#endif
}

//---------------------------------------------------------------------------------------
bool DBConnection::prepareQuery(QSqlQuery& query, const QString& str, Logger* logger)
{
    query.prepare(str);
    if (query.lastError().type() != QSqlError::NoError) {
        if (logger)
            logger->SaveMessage(QString::fromUtf8("DBConnection. Ошибка при выполнении prepare запроса ('%1'): %2").arg(str).arg(query.lastError().text()));
#ifdef _DEBUG
        else
            qDebug("%s", query.lastError().text().toStdString().c_str());
#endif // _DEBUG
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------------------
bool DBConnection::executeQuery(QSqlQuery& query, Logger* logger)
{
    query.exec();
    if (query.lastError().type() != QSqlError::NoError) {
        if (logger)
            logger->SaveMessage(QString::fromUtf8("DBConnection. Ошибка при выполнении execute запроса ('%1'): %2").arg(query.executedQuery()).arg(query.lastError().text()));
#ifdef _DEBUG
        else
            qDebug("%s", query.lastError().text().toStdString().c_str());
#endif // _DEBUG

        return false;
    }

    return true;
}
