#include "ConfigSubscriber.h"
#include <QJsonArray>

namespace Config
{
//------------------------------------------------------------------------------
QJsonObject ConfigSubscriber::getConfigEventTitle(const QString& response, Config::ConfigSubscriber::Event& title)
{
    QJsonObject jsObj = SubscribersNotifier::parseResponseToObj(response);

    QJsonObject jsEvent = jsObj.value("event").toObject();
    title.actionNumber = jsEvent.value("data").toObject().value("action").toInt();
    title.currentItemId = jsEvent.value("object").toObject().value("id").toInt();
    title.configType = jsEvent.value("type").toInt();

    return jsEvent["data"].toObject();
}

//------------------------------------------------------------------------------
QString ConfigSubscriber::getKeyFromMessage(const QString &message)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
    if(jsonDoc.object().contains("event")){
        QJsonObject event = jsonDoc.object().value("event").toObject();
        if(event.contains("object")){
            QJsonObject object = event.value("object").toObject();
            if(object.contains("key")){
                QString key = object.value("key").toString();
                if(!key.isEmpty())
                    return key;
            }
        }
    }
    return {};
}

//------------------------------------------------------------------------------
QJsonObject ConfigSubscriber::prepareObjToParse(int objID, QJsonObject obj)
{
    QJsonObject parseDat = obj.value("object").toObject();
    parseDat.insert("id",objID);
    return parseDat;
}

//------------------------------------------------------------------------------
Config::UpdateService ConfigSubscriber::parseServiceObj(int objId, QJsonObject data)
{
    Config::UpdateService service{};
    service.parse(prepareObjToParse(objId, data));
    return service;
}

//------------------------------------------------------------------------------
Config::UpdateServer ConfigSubscriber::parseServerObj(int objId, QJsonObject data)
{
    Config::UpdateServer server{};
    server.parse(prepareObjToParse(objId, data));
    return server;
}

//------------------------------------------------------------------------------
Config::UpdateRightGroup ConfigSubscriber::parseRightGroupObj(int objId, QJsonObject data)
{
    Config::UpdateRightGroup rightGroup{};
    rightGroup.parse(prepareObjToParse(objId, data));
    return rightGroup;
}

//------------------------------------------------------------------------------
QJsonValue ConfigSubscriber::parseKVObj(int objId, QJsonObject data)
{
    QString object = data.value("object").toString();
    QJsonDocument valuesJS = QJsonDocument::fromJson(object.toUtf8());

    if (valuesJS.isObject()) {
        return QJsonValue(valuesJS.object());
    } else if (valuesJS.isArray()) {
        return QJsonValue(valuesJS.array());
    }
    return QJsonValue(data);
}

//------------------------------------------------------------------------------
ConfigSubscriber::ConfigSubscriber()
{
    qRegisterMetaType<Config::ConfigSubscriber::Event>();
}

//------------------------------------------------------------------------------
QVector<ISubscriber::Subscription> ConfigSubscriber::GetNeededSubscription()
{
    return {
        {Type::ConfigServices},
        {Type::ConfigServers},
        {Type::ConfigRights},
        {Type::ConfigKeyValue}
    };
}

//------------------------------------------------------------------------------
void ConfigSubscriber::OnMessageReceived(const QString& message)
{
    Config::NotifyMsgType msgType = ConfigSubscriber::getNotifyMsgType(message);
    if (msgType == Config::NotifyMsgType::Event) {
        auto eventType = SubscribersNotifier::getEventType(message);
        Config::ConfigSubscriber::Event configEventTitle;
        const QJsonObject jObjConfigData(getConfigEventTitle(message, configEventTitle));

        switch (eventType)
        {
        case Type::ConfigServers:
        {
            Config::UpdateServer server = parseServerObj(configEventTitle.currentItemId, jObjConfigData);
            emitSignalServer(configEventTitle.currentItemId, configEventTitle.actionNumber, server);
        }
            break;
        case Type::ConfigServices:
        {
            Config::UpdateService service = parseServiceObj(configEventTitle.currentItemId, jObjConfigData);
            emitSignalService(configEventTitle.currentItemId, configEventTitle.actionNumber, service);
        }
            break;
        case Type::ConfigRights:
        {
            Config::UpdateRightGroup group = parseRightGroupObj(configEventTitle.currentItemId, jObjConfigData);
            emitSignalRightGroup(configEventTitle.currentItemId, configEventTitle.actionNumber, group);
        }
        break;
        case Type::ConfigKeyValue:
        {
            const QString key = getKeyFromMessage(message);
            auto rawJS = parseKVObj(configEventTitle.currentItemId, jObjConfigData);
            emitSignaKV(configEventTitle.currentItemId, configEventTitle.actionNumber, {rawJS, key});
        }
        break;
        default:
            break;
        }
    }
}

//------------------------------------------------------------------------------------------------
void ConfigSubscriber::emitSignalService(int oId, int action, Config::UpdateService service)
{
    switch(action){
    case(Config::ConfigEventType::configAdd):
        emit sig_add_service(oId, service);
        break;
    case(Config::ConfigEventType::configEdit):
        emit sig_update_service(oId, service);
        break;
    case(Config::ConfigEventType::configDel):
        emit sig_remove_service(oId);
        break;
    }
}

//------------------------------------------------------------------------------
void ConfigSubscriber::emitSignalServer(int oId, int action, Config::UpdateServer server)
{
    switch(action){
    case(Config::ConfigEventType::configAdd):
        emit sig_add_server(oId, server);
        break;
    case(Config::ConfigEventType::configEdit):
        emit sig_update_server(oId, server);
        break;
    case(Config::ConfigEventType::configDel):
        emit sig_remove_server(oId);
        break;
    }
}

//------------------------------------------------------------------------------
void ConfigSubscriber::emitSignalRightGroup(int oId, int action, Config::UpdateRightGroup group)
{
    switch(action){
    case(Config::ConfigEventType::configAdd):
        emit sig_add_rightGroup(oId, group);
        break;
    case(Config::ConfigEventType::configEdit):
        emit sig_update_rightGroup(oId, group);
        break;
    case(Config::ConfigEventType::configDel):
        emit sig_remove_rightGroup(oId);
        break;
    }
}

//------------------------------------------------------------------------------
void ConfigSubscriber::emitSignaKV(int oId, int action, QPair<QJsonValue, QString> data)
{
    switch(action){
    case(Config::ConfigEventType::configEdit):
        emit sig_update_keyValue(oId, data.first, data.second);
        break;
    }
}
}
