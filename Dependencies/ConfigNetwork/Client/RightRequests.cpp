#include "RightRequests.h"
#include <QJsonDocument>
#include <QJsonArray>

namespace Config {
namespace Request {
namespace Rights {

//-------------------------------------------------------------------------------
GetRightScheme::GetRightScheme(const QString& module)
{
    query()
        .setUrl(QString("/api/%1/user/right/schema").arg(module));
}

//-----------------------
RightScheme GetRightScheme::data()
{
    RightScheme scheme;
    auto o = QJsonDocument::fromJson(response().answerString().toUtf8()).object();
    scheme.parse(o);
    return scheme;
}

//-------------------------------------------------------------------------------
GetRightObjectsHierarchy::GetRightObjectsHierarchy(const QString& module)
{
    query()
        .setUrl(QString("/api/%1/objects/hierarchy").arg(module));
}

//-----------------------
ObjectsHierarchy GetRightObjectsHierarchy::data()
{
    ObjectsHierarchy hierarchy;
    auto o = QJsonDocument::fromJson(response().answerString().toUtf8()).object();
    hierarchy.parse(o);
    return hierarchy;
}

//-------------------------------------------------------------------------------
AddRightGroup::AddRightGroup(UpdateRightGroup& group, const QString& module)
{
    query()
        .setData(QJsonDocument(group.construct()).toJson())
        .setMethod(rest::Method::POST)
        .setUrl(QString("/api/%1/user/right/group").arg(module));
}

//-----------------------
int AddRightGroup::getId(void) const
{
    QJsonDocument doc = QJsonDocument::fromJson(response().answerString().toUtf8());
    return doc["id"].toInt();
}

//-------------------------------------------------------------------------------
ModifyRightGroup::ModifyRightGroup(UpdateRightGroup& group, const QString& module)
{
    query()
        .setUrl(QString("/api/%1/user/right/group/%2").arg(module).arg(group.id))
        .setData(QJsonDocument(group.construct()).toJson())
        .setMethod(group.isFullUpdate()? rest::Method::PUT : rest::Method::PATCH);
}

//-------------------------------------------------------------------------------
ModifyRightGroupsMembers::ModifyRightGroupsMembers(QMap<int, QVector<int>> updateRightsMap, const QString& module)
{
    query()
        .setUrl(QString("/api/%1/user/right/group/links").arg(module))
        .setData(constructJson(updateRightsMap))
        .setMethod(rest::Method::POST);
}

//-----------------------
QString ModifyRightGroupsMembers::constructJson(QMap<int, QVector<int>> updateRightsMap)
{
    QJsonArray objArr;
    for(auto rightId : updateRightsMap.keys()){
        QJsonObject obj;
        QJsonArray membersArr;
        for(auto member : updateRightsMap.value(rightId)){
            membersArr.push_back(member);
        }
        obj.insert("userGroups", membersArr);
        obj.insert("rightGroup", rightId);
        objArr.push_back(obj);
    }

    QJsonDocument doc(objArr);
    return doc.toJson();
}

//-------------------------------------------------------------------------------
RemoveRightGroup::RemoveRightGroup(int groupId, const QString& module)
{
    query()
        .setMethod(rest::Method::DEL)
        .setUrl(QString("/api/%1/user/right/group/%2").arg(module).arg(groupId));
}

}
}
}

