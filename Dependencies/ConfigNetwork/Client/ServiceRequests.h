#ifndef CONFIGSERVICEREQUESTS_H
#define CONFIGSERVICEREQUESTS_H

#include "ConfigStructs.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "Request.h"

namespace Config {
namespace Request {
namespace Services {

//Сохранить изменения в настройках службы
struct ModifyService : public rest::Request
{
    ModifyService(Service service, const QString module);
};

//Получить список типов служб
struct GetServiceTypes : public rest::Request
{
    GetServiceTypes(const QString module);
    QVector<Config::ServiceType> data() const;
};

//Добавить новую службу для сервера
struct AddService : public rest::Request
{
    AddService(Service service, const QString module);
    int getId(void) const;
};

//Удалить службу
struct RemoveService : public rest::Request
{
    RemoveService(int serviceID, const QString module);
};

}
}
}

#endif // CONFIGSERVICEREQUESTS_H
