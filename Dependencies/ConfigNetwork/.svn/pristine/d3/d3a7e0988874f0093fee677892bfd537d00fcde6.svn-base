#include "ConfigCommonRequests.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <optional>

namespace Config {
namespace Request {
//------------------------------------------------------------------------------
GetConfigVersion::GetConfigVersion(const QString module)
{
    query()
        .setUrl(QString("/api/%1/version").arg(module))
        .setMethod(rest::Method::GET);
}

//------------------------------------------------------------------------------
GetServers::GetServers(const QString& module)
{
    query()
        .setUrl(QString("/api/%1/server").arg(module))
        .setMethod(rest::Method::GET);
}

//-------------------------
QVector<Server> GetServers::data() const
{
    QVector<Server> data;
    auto root = QJsonDocument::fromJson(response().answerString().toUtf8());
    QJsonArray jsArr = root["servers"].toArray();
    for(auto jsServ : jsArr) {
        Server server;
        server.parse(jsServ.toObject());
        data.push_back(server);
    }
    return data;
}

//------------------------------------------------------------------------------
GetServices::GetServices(const QString& module)
{
    query().setUrl(QString("/api/%1/service").arg(module));
}

//-------------------------
QVector<Config::Service> GetServices::data() const
{
    QVector<Config::Service> data;
    auto root = QJsonDocument::fromJson(response().answerString().toUtf8());
    const QJsonArray jsArr = root["services"].toArray();

    if (!jsArr.isEmpty()) {
        for(auto jsService : jsArr) {
            Service service;
            if(service.parse(jsService.toObject()))
                data.push_back(service);
        }
    }
    return data;
}

//------------------------------------------------------------------------------
GetService::GetService(int vsId, const QString module)
{
    query().setUrl(QString("/api/%1/service/%2").arg(module).arg(vsId));
}

//-------------------------
Service GetService::data() const
{
    Service service;
    QJsonDocument doc = QJsonDocument::fromJson(response().answerString().toUtf8());
    service.parse(doc.object());
    return service;
}

//------------------------------------------------------------------------------
GetRightGroups::GetRightGroups(const QString& module) : m_module(module)
{
    query()
        .setUrl(QString("/api/%1/user/right/group").arg(module))
        .setMethod(rest::Method::GET);
}


//-------------------------
QVector<Config::RightGroup> GetRightGroups::data() const
{
    QVector<RightGroup> rightGroupsVec;
    QJsonDocument doc = QJsonDocument::fromJson(response().answerString().toUtf8());
    QJsonArray jsArr = doc["rightGroups"].toArray();
    if (!jsArr.isEmpty()) {
        for (auto jsUser : jsArr) {
            RightGroup rightGroup;
            rightGroup.parse(jsUser.toObject());
            rightGroupsVec.push_back(rightGroup);
        }
    }
    return rightGroupsVec;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
GetNotificationScheme::GetNotificationScheme(const QString& module)
{
    query()
        .setUrl(QString("/api/%1/user/notifications/schema").arg(module))
        .setMethod(rest::Method::GET);
}

//-----
NotificationScheme GetNotificationScheme::data() const
{
    NotificationScheme scheme;
    QJsonDocument doc = QJsonDocument::fromJson(response().answerString().toUtf8());
    scheme.parse(doc.object());
    return scheme;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
QJsonValue GetConfigKV::getData() const
{
    return GetConfigKV::responseGetData(response().answerString());
}

QJsonValue GetConfigKV::responseGetData(const QString &response)
{
    auto root = QJsonDocument::fromJson(response.toUtf8());
    QJsonObject mainJs = root.object();
    QString stringEscapedJS = mainJs.value("value").toString();//Экранированный json
    QJsonDocument valuesJS = QJsonDocument::fromJson(stringEscapedJS.toUtf8());
    if(valuesJS.isArray()){
        return QJsonValue(valuesJS.array());
    }
    else if(valuesJS.isObject()){
        return QJsonValue(valuesJS.object());
    }
    return QJsonValue();
}

QJsonValue GetConfigKV::getJsValData(void) const
{
    return GetConfigKV::responseGetJsValData(response().answerString());
}

QJsonValue GetConfigKV::responseGetJsValData(const QString& response)
{
    auto root = QJsonDocument::fromJson(response.toUtf8());
    auto answerObj = root.object();
    return answerObj;
}

QString EditConfigKV::requestEditKV(QJsonValue data)
{
    QJsonDocument doc;
    if(data.isObject())
        doc.setObject(data.toObject());
    else
        doc.setArray(data.toArray());
    return doc.toJson();
}

QStringList GetConfigurationKeys::data() const
{
    QStringList ret;
    auto root = QJsonDocument::fromJson(response().answerString().toUtf8());
    QJsonObject mainJs = root.object();
    if(mainJs.contains("keys")) {
        auto array = mainJs.value("keys").toArray();
        for(auto val: array) {
            ret.push_back(val.toString());
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
