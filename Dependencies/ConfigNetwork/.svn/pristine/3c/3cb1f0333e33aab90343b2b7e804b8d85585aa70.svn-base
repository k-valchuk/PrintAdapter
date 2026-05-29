#include "ServerRequests.h"
#include <QJsonArray>
#include <QJsonObject>

namespace Config {
namespace Request {
namespace Servers {

//----------------------------------------
ModifyServer::ModifyServer(Server server, const QString module)
{
    query()
        .setUrl(QString("/api/%1/server/%2").arg(module).arg(server.id))
        .setMethod(rest::Method::PUT)
        .setData(QJsonDocument(server.construct()).toJson());
}

//----------------------------------------
AddServer::AddServer(Server server, const QString module)
{
    query()
        .setUrl(QString("/api/%1/server").arg(module))
        .setMethod(rest::Method::POST)
        .setData(QJsonDocument(server.construct()).toJson());
}

//---------------
int AddServer::getId(void) const
{
    QJsonDocument doc = QJsonDocument::fromJson(response().answerString().toUtf8());
    return doc["id"].toInt();
}

//----------------------------------------
RemoveServer::RemoveServer(int serverID, const QString module)
{
    query()
        .setUrl(QString("/api/%1/server/%2").arg(module).arg(serverID))
        .setMethod(rest::Method::DEL);
}

}
}
}
