#ifndef CONFIGRIGHTREQUESTS_H
#define CONFIGRIGHTREQUESTS_H

#include "ConfigStructs.h"
#include "Request.h"
#include <QJsonDocument>

namespace Config {
namespace Request {
namespace Rights {

// --- Получить схему прав
struct GetRightScheme : public rest::Request
{
    GetRightScheme(const QString& module);
    RightScheme data();
};

// --- Получить иерархию объектных прав и их значения
struct GetRightObjectsHierarchy : public rest::Request
{
    GetRightObjectsHierarchy(const QString& module);
    ObjectsHierarchy data();
};

// --- Добавить новую группу прав
struct AddRightGroup : public rest::Request
{
    AddRightGroup(UpdateRightGroup& group, const QString& module);
    int getId(void) const;
};

// --- Редактировать группу прав
struct ModifyRightGroup : public rest::Request
{
    ModifyRightGroup(UpdateRightGroup& group, const QString& module);
};

// --- Редактировать список групп пользователей, на которые назначено данное право
struct ModifyRightGroupsMembers : public rest::Request
{
    ModifyRightGroupsMembers(QMap<int, QVector<int>> updateRightsMap, const QString& module);
private:
    QString constructJson(QMap<int, QVector<int>> updateRightsMap);
};

// --- Удалить группу прав
struct RemoveRightGroup : public rest::Request
{
    RemoveRightGroup(int groupId, const QString& module);
};

}
}
}

#endif // CONFIGRIGHTREQUESTS_H
