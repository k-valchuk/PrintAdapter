#ifndef QUERYEXTERNAL_H
#define QUERYEXTERNAL_H

#include <functional>
#include <QString>
#include <QDateTime>

class QObject;
class QNetworkCookieJar;

struct QueryExecutorData
{
    QString ServerUrl;
    QString AuthToken;
    QString RefreshToken;
    QDateTime DateTimeOfExpire;
};

void SetQueryExecutorData(QString serverUrl, QString authToken,
                          QString refreshToken, QDateTime exp);

void UpdateQueryExecurorData();

void SetReloginCallback(QObject* context, std::function<void()> slot);

void SetOnQueryExecutorDataChanged(std::function<void(QueryExecutorData)> updateForAllModules);

QNetworkCookieJar* GetApplicationSharedCookieJar();

#endif
