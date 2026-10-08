#include "ConfigStructs.h"
#include <QJsonArray>
#include <QJsonDocument>

namespace Config {
//-------------------------------------------------------
void ServiceType::parse(const QJsonObject& o)
{
    name = o.value("name").toString();
    id = o.value("id").toInt();
}

//-------------------------------------------------------
void Server::parse(const QJsonObject& o)
{
    if (!o.isEmpty()) {
        // Разбираем параметры сервера
        if(o.contains("id"))
            id = o.value("id").toInt();
        if(o.contains("props")){
            auto jsServerPropsObj = o.value("props").toObject();
            if(jsServerPropsObj.contains("name"))
                props.name = jsServerPropsObj.value("name").toString();
            if(jsServerPropsObj.contains("path"))
                props.path = jsServerPropsObj.value("path").toString();
        }
        // Разбираем службы, которые завязанны на сервер
        if(o.contains("services")){
            services.clear();
            auto jsServerServicesArr = o.value("services").toArray();
            for (auto jsServerServicesObj : jsServerServicesArr){
                Service service;
                service.parse(jsServerServicesObj.toObject());
                services.append(service);
            }
        }
    }
}

//-----------------
QJsonObject Server::construct(void) const
{
    QJsonObject jsObj;
    QJsonObject jsProps;
    jsProps.insert("path", props.path);
    jsProps.insert("name", props.name);
    jsObj.insert("props", jsProps);
    return jsObj;
}

//-----------------
void UpdateServer::parse(const QJsonObject& o)
{
    if(o.contains("props")){
        auto jsServerPropsObj = o.value("props").toObject();
        if(jsServerPropsObj.contains("name"))
            isPropsNameUp = true;
        if(jsServerPropsObj.contains("path"))
            isPropPathUp = true;
    }

    if(o.contains("services"))
        isServicesUp = true;

    Server::parse(o);
}

//-------------------------------------------------------
bool Service::parse(const QJsonObject& o)
{
    if (!o.isEmpty()) {

        if(o.contains("id"))
            id = o.value("id").toInt();

        if(o.contains("props")){
            auto jsServiceProps = o.value("props").toObject();
            if(jsServiceProps.contains("typeId"))
                props.type = static_cast<EServiceType>(jsServiceProps.value("typeId").toInt());
            if(jsServiceProps.contains("serverId"))
                props.serverId = jsServiceProps.value("serverId").toInt();
            if(jsServiceProps.contains("name"))
                props.name = jsServiceProps.value("name").toString();
            if(jsServiceProps.contains("path"))
                props.path = jsServiceProps.value("path").toString();
        }

        if(o.contains("configuration")){
            // Сохраним конфигурации в сыром виде
            rawConfiguration = o.value("configuration");
            // Разберем дополнительно в случае конкретных служб
            switch (props.type) {
            case EServiceType::eAzimuth: {
                auto cfgObject = o.value("configuration").toObject();
                auto channelsObjects = cfgObject["channels"].toArray();

                for (auto arr : channelsObjects) {
                    auto chObj = arr.toObject();
                    AzChannelConfig v;
                    v.rtsp = chObj["rtsp"].toString();
                    v.number = chObj["number"].toInt();
                    azServiceConfig.channels.push_back(v);
                }
                break;
            }
            case EServiceType::eCaptureVSManager:
            case EServiceType::eAirVSManager:{
                auto jsArray = o.value("configuration").toArray();
                for (auto arr : jsArray) {
                    auto jsjsChanelObj = arr.toObject();

                    VSMServerProps props;

                    props.vsId = jsjsChanelObj.value("id").toInt();

                    auto jsArrayChan = jsjsChanelObj.value("channels").toArray();

                    for (auto channel : jsArrayChan) {
                        auto chan = channel.toObject();

                        VSMChannel sc;

                        sc.channel = chan.value("number").toInt();
                        sc.name = chan.value("name").toString();
                        if (chan.value("channelType").toString() == "capture")
                            sc.type = EChannelFunctionStatus::eCFSCapture;
                        else if (chan.value("channelType").toString() == "air")
                            sc.type = EChannelFunctionStatus::eCFSAir;

                        props.channels.append(sc);
                    }

                    vsmServersProp.append(props);
                }
                break;
            }
            default:
                break;
            }
            return true;
        }
    }
    return false;
}

//---------------
QJsonObject Service::construct(void) const
{
    QJsonObject jsPropsObj;
    QJsonObject jsMainObj;

    jsPropsObj.insert("typeId", static_cast<int>(props.type));
    jsPropsObj.insert("serverId", props.serverId);
    jsPropsObj.insert("name", props.name);

    jsMainObj.insert("props", jsPropsObj);

    if(!rawConfiguration.isNull())
        jsMainObj["configuration"] = rawConfiguration;

    return jsMainObj;
}

//-----------------------------------------------
bool Service::isNecessaryForChannels() const
{
    if(props.type == Config::EServiceType::eCaptureVSManager || props.type == Config::EServiceType::eAirVSManager
        || props.type == Config::EServiceType::eAirMOSSynchronizer
        || props.type == Config::EServiceType::eAzimuth || props.type == Config::EServiceType::eTeletextInserter
        || props.type == Config::EServiceType::eGraphicsCarrot || props.type == Config::EServiceType::eGraphicsTitleStation
        || props.type == Config::EServiceType::eGraphicsVizRT)
        return true;
    return false;
}

//---------------
bool Service::isAzimuth() const
{
    return props.type == Config::EServiceType::eAzimuth;
}

//---------------
bool Service::isVSMType() const
{
    return (props.type == Config::EServiceType::eCaptureVSManager || props.type == Config::EServiceType::eAirVSManager);
}

//---------------
bool Service::isGraphicsType() const
{
    return (props.type == Config::EServiceType::eGraphicsCarrot || props.type == Config::EServiceType::eGraphicsTitleStation
            || props.type == Config::EServiceType::eGraphicsVizRT);
}

//---------------
bool Service::isManagedService() const
{
    return ((props.type == Config::EServiceType::eAzimuth) || (props.type == Config::EServiceType::eTeletextInserter) ||
            (props.type == Config::EServiceType::eGraphicsCarrot) || (props.type == Config::EServiceType::eGraphicsTitleStation) ||
            (props.type == Config::EServiceType::eGraphicsVizRT));
}

//---------------
bool Service::isServiceHavePath() const
{
    return rawConfiguration.toObject().contains("path") || rawConfiguration.toObject().contains("host");
}

//---------------
bool Service::isServiceHavePathByType(Config::EServiceType type)
{
    return (type == Config::EServiceType::eQueue) || (type == Config::EServiceType::ePostgreSQL) ||
           (type == Config::EServiceType::eElasticSearch) || (type == Config::EServiceType::eAccessPoint) ||
           (type == Config::EServiceType::eGARSearch) || (type == Config::EServiceType::eMessageApp) ||
           (type == Config::EServiceType::eBPMCoreConnection) || (type == Config::EServiceType::eBPMCoreStorage) ||
           (type == Config::EServiceType::eBPMRestService) ||
           (type == Config::EServiceType::eBPMScriptService);
}

//---------------
bool UpdateService::parse(const QJsonObject& o)
{
    auto jsServiceProps = o.value("props").toObject();
    if(jsServiceProps.contains("typeId"))
        isTypeUp = true;
    if(jsServiceProps.contains("serverId"))
        isServerIdUp = true;
    if(jsServiceProps.contains("name"))
        isNameUp = true;
    if(jsServiceProps.contains("path"))
        isPathUp = true;
    if(o.contains("configuration"))
        isConfigUp = true;

    return Service::parse(o);
}

//-----------------------------------------------------
void RightScheme::parse(const QJsonObject& o)
{
    if(o.contains("rights"))
        schemeArray = o["rights"].toArray();
    if(o.contains("objectsRights"))
        schemeObjectsArray = o["objectsRights"].toArray();
}

//-----------------------------------------------------
void ObjectsHierarchy::parse(const QJsonObject& o)
{
    if(o.contains("objects"))
        hierarchy = o["objects"].toArray();
}

//-----------------------------------------------------
void RightGroup::parse(const QJsonObject& o)
{
    if (!o.isEmpty()) {

        if(o.contains("id"))
            id = o.value("id").toInt();

        if(o.contains("name"))
            name = o.value("name").toString();

        if(o.contains("allowedRights")){
            allowedRightsVec.clear();
            for(auto obj : o.value("allowedRights").toArray()){
                allowedRightsVec.push_back(obj.toInt());
            }
        }

        if(o.contains("restrictedRights")){
            restrictedRightsVec.clear();
            for(auto obj : o.value("restrictedRights").toArray()){
                restrictedRightsVec.push_back(obj.toInt());
            }
        }

        if(o.contains("userGroups")){
            userGroupsVec.clear();
            for(auto obj : o.value("userGroups").toArray()){
                userGroupsVec.push_back(obj.toInt());
            }
        }

        if(o.contains("objectsRights")){
            objectsRights.clear();
            for(auto objRightGroup : o.value("objectsRights").toArray()){
                auto jsObj = objRightGroup.toObject();
                ObjectRightGroup gr;
                if(jsObj.contains("objectId"))
                    gr.id = jsObj.value("objectId").toInt();
                if(jsObj.contains("objectTypeId"))
                    gr.typeId = jsObj.value("objectTypeId").toInt();

                if(jsObj.contains("allowedRights")){
                    gr.allowedRightsVec.clear();
                    for(auto obj : jsObj.value("allowedRights").toArray()){
                        gr.allowedRightsVec.push_back(obj.toInt());
                    }
                }

                if(jsObj.contains("restrictedRights")){
                    gr.restrictedRightsVec.clear();
                    for(auto obj : jsObj.value("restrictedRights").toArray()){
                        gr.restrictedRightsVec.push_back(obj.toInt());
                    }
                }
                objectsRights.push_back(gr);
            }
        }
    }
}

//----------------------------------
void UpdateRightGroup::parse(const QJsonObject& o)
{
    if (!o.isEmpty()) {
        if(o.contains("id"))
            id = o.value("id").toInt();
        if(o.contains("name")){
            name = o.value("name").toString();
            isNameUpdate = true;
        }

        if(o.contains("allowedRights")){
            allowedRightsVec.clear();
            for(auto obj : o.value("allowedRights").toArray()){
                allowedRightsVec.push_back(obj.toInt());
            }
            isAllowedRightsUpdate = true;
        }

        if(o.contains("restrictedRights")){
            restrictedRightsVec.clear();
            for(auto obj : o.value("restrictedRights").toArray()){
                restrictedRightsVec.push_back(obj.toInt());
            }
            isRestrictedRightsUpdate = true;
        }

        if(o.contains("userGroups")){
            userGroupsVec.clear();
            for(auto obj : o.value("userGroups").toArray()){
                userGroupsVec.push_back(obj.toInt());
            }
            isUserGroupsUpdate = true;
        }

        if(o.contains("objectsRights")){
            objectsRights.clear();
            for(auto objRightGroup : o.value("objectsRights").toArray()){
                auto jsObj = objRightGroup.toObject();
                ObjectRightGroup gr;
                if(jsObj.contains("objectId"))
                    gr.id = jsObj.value("objectId").toInt();
                if(jsObj.contains("objectTypeId"))
                    gr.typeId = jsObj.value("objectTypeId").toInt();

                if(jsObj.contains("allowedRights")){
                    gr.allowedRightsVec.clear();
                    for(auto obj : jsObj.value("allowedRights").toArray()){
                        gr.allowedRightsVec.push_back(obj.toInt());
                    }
                }

                if(jsObj.contains("restrictedRights")){
                    gr.restrictedRightsVec.clear();
                    for(auto obj : jsObj.value("restrictedRights").toArray()){
                        gr.restrictedRightsVec.push_back(obj.toInt());
                    }
                }
                objectsRights.push_back(gr);
            }
            isObjectRightsUpdate = true;
        }
    }
}

//----------------------------------
QJsonObject UpdateRightGroup::construct(void) const
{
    QJsonObject groupObj;

    if(isNameUpdate)
        groupObj.insert("name", name);

    if(isAllowedRightsUpdate){
        QJsonArray allowedArr{};
        for(auto id : allowedRightsVec)
            allowedArr.append(id);
        groupObj.insert("allowedRights", allowedArr);
    }

    if(isRestrictedRightsUpdate){
        QJsonArray restrictedArr{};
        for(auto id : restrictedRightsVec)
            restrictedArr.append(id);
        groupObj.insert("restrictedRights", restrictedArr);
    }

    if(isObjectRightsUpdate){
        QJsonArray ObjectRightsArr{};
        for(auto rightGr : objectsRights){
            QJsonObject rightGrObj;
            rightGrObj.insert("objectId", rightGr.id);
            rightGrObj.insert("objectTypeId", rightGr.typeId);

            QJsonArray objAllowedRights;
            for(auto id : rightGr.allowedRightsVec)
                objAllowedRights.append(id);
            rightGrObj.insert("allowedRights", objAllowedRights);

            QJsonArray objRestrictedRights;
            for(auto id : rightGr.restrictedRightsVec)
                objRestrictedRights.append(id);
            rightGrObj.insert("restrictedRights", objRestrictedRights);

            ObjectRightsArr.append(rightGrObj);
        }
        groupObj.insert("objectsRights", ObjectRightsArr);
    }

    if(isUserGroupsUpdate){
        QJsonArray userGrArr{};
        for(auto id : userGroupsVec)
            userGrArr.append(id);
        groupObj.insert("userGroups", userGrArr);
    }

    return groupObj;
}

//----------------------------------
bool UpdateRightGroup::isFullUpdate()
{
    return isNameUpdate && isRestrictedRightsUpdate && isAllowedRightsUpdate && isUserGroupsUpdate && isObjectRightsUpdate;
}

//---------------------------------- Уведомления -----------------------------------------------------------
void NotificationScheme::parse(const QJsonObject& o, bool parseForTree)
{
    if(o.contains("notificationEvents"))
        schemeArray = o["notificationEvents"].toArray();
    if(o.contains("notificationEventsConfig")){
        configSchemeArray = o["notificationEventsConfig"].toArray();
        if(parseForTree){ // Если парсим для дерева - разбираем и переформируем json и докидываем в основной jsonArray
            for(auto confVal : configSchemeArray){
                auto confObj = confVal.toObject();
                if(confObj.contains("childs")){
                    auto confObjChilds = confObj["childs"].toArray();
                    for(auto childVal : confObjChilds){
                        auto childObj = childVal.toObject();
                        if(childObj.contains("deep_childs")){
                            auto deepChilds = childObj["deep_childs"].toArray();
                            for(auto deepChildVal : deepChilds){
                                auto deepChildObj = deepChildVal.toObject();
                                auto transformedObj = transformConfigToDefaultScheme(deepChildObj);
                                schemeArray.append(transformedObj);
                            }
                        }
                    }
                }
            }
        }
    }
}

//----------
QJsonObject NotificationScheme::transformConfigToDefaultScheme(QJsonObject& deepChild)
{
    if(!deepChild.contains("name") || !deepChild.contains("types"))
        return {};

    QJsonObject retObj;

    auto name = deepChild["name"].toString();
    QJsonObject nameObj;
    nameObj.insert("ru", name);
    nameObj.insert("en", name);
    retObj.insert("name", nameObj);

    QJsonArray childs;
    auto types = deepChild["types"].toArray();
    for(auto type : types){
        auto typeObj = type.toObject();
        if(!typeObj.contains("name") || !typeObj.contains("id"))
            continue;

        QJsonObject child;
        child.insert("id", typeObj["id"]);

        auto name = typeObj["name"].toString();
        QJsonObject nameObj;
        nameObj.insert("ru", name);
        nameObj.insert("en", name);
        child.insert("name", nameObj);

        childs.push_back(child);
    }
    retObj.insert("childs", childs);
    return retObj;
}

//---------------------------------- Key - Val -----------------------------------------------------------
void Configuration::parse(const QJsonObject &o)
{
    if (!o.isEmpty() && o.contains("keys")) {
        auto jsConfigurationObj = o.value("keys").toObject();
        auto value = jsConfigurationObj.value("keys").toString();
    }
}

QJsonObject Configuration::construct() const
{
    QJsonObject jsObj;
    jsObj.insert("keys", name);
    return jsObj;
}

bool UpdateConfiguration::parse(const QJsonObject &o)
{
    if(o.contains("keys")){
        auto jsConfigurationObj = o.value("keys").toObject();
    }
    Configuration::parse(o);
    return true;
}

}
