#include "ServiceRequests.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace Config {
namespace Request {
namespace Services {

//-----------------------------------------------------------------------
ModifyService::ModifyService(Service service, const QString module)
{
    query()
        .setUrl(QString("/api/%1/service/%2").arg(module).arg(service.id))
        .setData(QJsonDocument(service.construct()).toJson())
        .setMethod(rest::Method::PUT);
}

//-----------------------------------------------------------------------
GetServiceTypes::GetServiceTypes(const QString module)
{
    query()
        .setUrl(QString("/api/%1/service/types").arg(module));
}

//---------------------------
QVector<Config::ServiceType> GetServiceTypes::data() const
{
    QVector<Config::ServiceType> data;
    QJsonArray jsArr = QJsonDocument::fromJson(response().answerString().toUtf8()).array();
    for (auto jsTypes : jsArr) {
        Config::ServiceType type;
        auto jsTypesObj = jsTypes.toObject();
        type.name = jsTypesObj.value("name").toString();
        type.id = jsTypesObj.value("id").toInt();
        data.push_back(type);
    }
    return data;
}

//-----------------------------------------------------------------------
AddService::AddService(Service service, const QString module)
{
    query()
        .setUrl(QString("/api/%1/service").arg(module))
        .setData(QJsonDocument(service.construct()).toJson())
        .setMethod(rest::Method::POST);
}

//--------------------
int AddService::getId(void) const
{
    QJsonDocument doc = QJsonDocument::fromJson(response().answerString().toUtf8());
    return doc["id"].toInt();
}

//-----------------------------------------------------------------------
RemoveService::RemoveService(int serviceID, const QString module)
{
    query()
        .setUrl(QString("/api/%1/service/%2").arg(module).arg(serviceID))
        .setMethod(rest::Method::DEL);
}

}
}
}

