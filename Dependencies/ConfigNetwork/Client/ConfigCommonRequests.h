#ifndef CONFIG_COMMON_REQUESTS_H
#define CONFIG_COMMON_REQUESTS_H

#include "ConfigStructs.h"
#include "Request.h"
#include <optional>

namespace Config {
namespace Request {
    //------------------------------------------------------------------------------
    // Получить версию конфигурации
    struct GetConfigVersion: public rest::Request {
        GetConfigVersion(const QString module);
    };

    //------------------------------------------------------------------------------
    // --- Получить список Серверов и их служб
    struct GetServers : public rest::Request {
        GetServers(const QString& module);
        QVector<Server> data() const;
    };

    // --- Получить список всех служб
    struct GetServices : public rest::Request {
        GetServices(const QString& module);
        QVector<Config::Service> data() const;
    };

    // --- Получить данных о сервисе
    struct GetService : public rest::Request {
        GetService(int vsId, const QString module);
        Service data() const;
    };

    //------------------------------------------------------------------------------
    // Получить все группы прав Config
    struct GetRightGroups : public rest::Request {
        GetRightGroups(const QString& module);
        QVector<Config::RightGroup> data() const;
        QString getModule(){return m_module;};
    private:
        QString m_module;
    };

    //------------------------------------------------------------------------------
    // Получить схему уведомлений
    struct GetNotificationScheme : public rest::Request{
        GetNotificationScheme(const QString& module);
        NotificationScheme data() const;
    };

    //------------------------------------------------------------------------------
    // Получить данные по модулю и key
    struct GetConfigKV : public rest::Request{
        GetConfigKV(const QString module, const QString key) : m_module(module),m_key(key)
        {
            query()
                .setUrl(QString("/api/%1/config/kv/%2").arg(module).arg(key))
                .setMethod(rest::Method::GET);
        }
        QJsonValue getData(void) const;
        static QJsonValue responseGetData(const QString& response);

        QJsonValue getJsValData(void) const;
        static QJsonValue responseGetJsValData(const QString& response);

        QString getModule(){return m_module;};
        QString getKey(){return m_key;};
    private:
        QString m_module;
        QString m_key;
    };

    // --- Редактировать данные по модулю и key
    struct EditConfigKV : public rest::Request{
        EditConfigKV(const QString module, const QString key, const QJsonValue data) : m_module(module),m_key(key), m_data(data)
        {
            query()
                .setUrl(QString("/api/%1/config/kv/%2").arg(module).arg(key))
                .setData(requestEditKV(data))
                .setMethod(rest::Method::POST);
        }
        static QString requestEditKV(QJsonValue data);
    private:
        QString m_module;
        QString m_key;
        QJsonValue m_data;
    };


    // --- Обновить данные по модулю и key
    struct UpdateConfigKV : public rest::Request{
        UpdateConfigKV(const QString module, const QString key, const QJsonValue data) : m_module(module),m_key(key), m_data(data)
        {
            query()
                .setUrl(QString("/api/%1/config/kv/%2").arg(module).arg(key))
                .setData(data.toString())
                .setMethod(rest::Method::POST);
        }
    private:
        QString m_module;
        QString m_key;
        QJsonValue m_data;
    };

    //------------------------------------------------------------------------------
    // Получить данные по ключу
    struct GetConfigurationKeys : public rest::Request{
        GetConfigurationKeys(const QString module) : m_module(module)
        {
            query()
                .setUrl(QString("/api/%1/config/keys").arg(module))
                .setMethod(rest::Method::GET);
        }
        QStringList data() const;

        QString getModule(){return m_module;};

    private:
        QString m_module;

    };


    // --- Подсистемы (доп возврат в GetConfigKV)
    struct GetSubsytems : public GetConfigKV
    {
        GetSubsytems() : GetConfigKV("core", "subsystems") {}
        
        SubsytemsList GetList() {
            auto data = getData();
            
            QStringList subsystems;
            for(const auto& jsKey : data.toArray())
                subsystems.append(jsKey.toString().toLower().trimmed());
            
            return SubsytemsList(subsystems);
        }
    };
    
    struct SetSubsystems : public EditConfigKV
    {
        SetSubsystems(const SubsytemsList& list) : EditConfigKV("core", "subsystems", buildData(list)) {}
        
    protected:
        static QJsonValue buildData(const SubsytemsList& list)
        {
            auto keys = list.GetKeys();
            
            QJsonArray arr;
            for(const auto& key : keys)
                arr.append(key);
            return arr;
        }
    };
}
}

#endif // CONFIG_COMMON_REQUESTS_H
