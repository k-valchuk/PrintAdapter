#ifndef CONFIGSERVERREQUESTS_H
#define CONFIGSERVERREQUESTS_H

#include <QJsonDocument>
#include "ConfigStructs.h"
#include "Request.h"

namespace Config {
namespace Request {
namespace Servers {

// --- Сохранить изменения в настройках сервера
struct ModifyServer : public rest::Request
{
    ModifyServer(Server server, const QString module);
};

// --- Добавить новый сервер
struct AddServer : public rest::Request
{
    AddServer(Server server, const QString module);
    int getId(void) const;
};

// --- Удалить сервер
struct RemoveServer : public rest::Request
{
    RemoveServer(int serverID, const QString module);
};

}
}
}

#endif // CONFIGSERVERREQUESTS_H
