#ifndef QUERY1_H
#define QUERY1_H

#include <QtGlobal>
#include <QString>
#include <QMap>

namespace rest{


enum class Method {
    GET = 1,
    POST = 2,
    PUT = 3,
    DEL = 4,
    PATCH = 5
};


class Query
{
    QString m_data;
    QString m_url;
    Method m_method = Method::GET;
    QMap<QString, QString> m_queryParams;
    QMap<QString, QList<QString>> m_arrayQueryParams;
    
    int m_transferTimeout;
    int m_wholeTimeout;
public:
    Query();
    
    QString data() const;
    QString url() const;
    Method method() const;
    int transferTimeout() const;
    int wholeTimeout() const;
    QMap<QString, QString> queryParams() const;
    QMap<QString, QList<QString>> arrayQueryParams() const;
    
    QString getResultUrl() const;
    
    Query& setData(const QString& data);
    Query& setUrl(const QString& url);
    Query& setMethod(Method method);
    Query& setQuery(Query q);
    Query& setTransferTimeout(int msces);
    Query& setWholeTimeout(int msecs);
    Query& setQueryParameter(const QString& key, const QString& value);
    Query& setArrayQueryParam(const QString& key, const QStringList& values);
    Query& addArrayQueryParam(const QString& key, const QString& value);
    Query& removeArrayQueryParam(const QString& key, const QString& value);
    Query& clearArrayQueryParam(const QString& key);
};





}
#endif // QUERY_H
