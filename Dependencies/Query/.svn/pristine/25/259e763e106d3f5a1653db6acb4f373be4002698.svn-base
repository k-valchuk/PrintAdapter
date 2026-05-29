
#include "Query.h"
#include "version.h"
#include "QUrl"
#include "QUrlQuery"

rest::Query::Query()
{
    m_transferTimeout = 3000;
    m_wholeTimeout = 0;
}

QString rest::Query::data() const
{
    return m_data;
}

QString rest::Query::url() const
{
    return m_url;
}

rest::Method rest::Query::method() const
{
    return m_method;
}

int rest::Query::transferTimeout() const
{
    return m_transferTimeout;
}

int rest::Query::wholeTimeout() const
{
    return m_wholeTimeout;
}

QMap<QString, QString> rest::Query::queryParams() const
{
    return m_queryParams;
}

QMap<QString, QList<QString> > rest::Query::arrayQueryParams() const
{
    return m_arrayQueryParams;
}

QString rest::Query::getResultUrl() const
{
    QUrl finalUrl;
    //finalUrl.setScheme("http");
    finalUrl.setPath(url());
    
    auto qp = queryParams();
    auto aqp = arrayQueryParams();
    
    QUrlQuery urlQuery;
    if(!qp.empty())
    {
        for(const auto& key : qp.keys())
        {
            auto value = qp[key];
            urlQuery.addQueryItem(key, value);
        }
    }
    
    if(!aqp.empty())
    {
        for(const auto& key : aqp.keys())
            for(const auto& val : aqp[key])
                urlQuery.addQueryItem(key, val);
    }
    
    finalUrl.setQuery(urlQuery);
    return finalUrl.toString(QUrl::PrettyDecoded);
}

rest::Query& rest::Query::setData(const QString& data)
{
    m_data = data;
    return *this;
}

rest::Query& rest::Query::setUrl(const QString& url)
{
    m_url = url;
    return *this;
}

rest::Query& rest::Query::setMethod(Method method)
{
    m_method = method;
    return *this;
}

rest::Query& rest::Query::setQuery(Query q)
{
    m_data = q.m_data;
    m_url = q.m_url;
    m_method = q.m_method;
    m_transferTimeout = q.m_transferTimeout;
    m_wholeTimeout = q.m_wholeTimeout;
    return *this;
}

rest::Query& rest::Query::setTransferTimeout(int msecs)
{
    m_transferTimeout = msecs;
    return *this;
}

rest::Query& rest::Query::setWholeTimeout(int msecs)
{
    m_wholeTimeout = msecs;
    return *this;
}

rest::Query& rest::Query::setQueryParameter(const QString& key, const QString& value)
{
    if(value.isEmpty())
        m_queryParams.remove(key);
    else
        m_queryParams[key] = value;
    return *this;
}

rest::Query& rest::Query::setArrayQueryParam(const QString& key, const QStringList& values)
{
    if(values.isEmpty())
        m_arrayQueryParams.remove(key);
    else
        m_arrayQueryParams[key] = values;
    return *this;
}

rest::Query& rest::Query::addArrayQueryParam(const QString& key, const QString& value)
{
    m_arrayQueryParams[key].append(value);
    return *this;
}

rest::Query& rest::Query::removeArrayQueryParam(const QString& key, const QString& value)
{
    m_arrayQueryParams[key].removeAll(value);
    if(m_arrayQueryParams[key].isEmpty())
        m_arrayQueryParams.remove(key);
    return *this;
}

rest::Query& rest::Query::clearArrayQueryParam(const QString& key)
{
    m_arrayQueryParams.remove(key);
    return *this;
}

