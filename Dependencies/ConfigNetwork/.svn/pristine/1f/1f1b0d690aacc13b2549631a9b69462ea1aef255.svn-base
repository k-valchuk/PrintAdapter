#ifndef CONFIGSUBSCRIBER_H
#define CONFIGSUBSCRIBER_H

#include "ConfigStructs.h"
#include "SubscribersNotifier.h"

namespace Config {
class ConfigSubscriber : public ISubscriber {
    Q_OBJECT

public:
    ConfigSubscriber();

    enum Type {
        ConfigServers = 7,
        ConfigServices = 8,
        ConfigRights = 9,
        ConfigKeyValue = 10
    };

    struct Event {
        int currentItemId;
        int actionNumber;
        int configType;
    };
    QVector<Subscription> GetNeededSubscription() override;

private:
    QJsonObject getConfigEventTitle(const QString& response, Config::ConfigSubscriber::Event& title);

    QString getKeyFromMessage(const QString& message);

    QJsonObject prepareObjToParse(int objID, QJsonObject obj);

    Config::UpdateService parseServiceObj(int objId, QJsonObject data);
    Config::UpdateServer parseServerObj(int objId, QJsonObject data);
    Config::UpdateRightGroup parseRightGroupObj(int objId, QJsonObject data);
    QJsonValue parseKVObj(int objId, QJsonObject data);

    void emitSignalService(int oId, int action, Config::UpdateService service);
    void emitSignalServer(int oId, int action, Config::UpdateServer server);
    void emitSignalRightGroup(int oId, int action, Config::UpdateRightGroup group);
    void emitSignaKV(int oId, int action, QPair <QJsonValue, QString> rawJS);

    static QJsonObject parseResponseToObj(const QString& response)
    {
        QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
        return doc.object();
    }

    static Config::NotifyMsgType getNotifyMsgType(const QString& response)
    {
        QJsonObject jsObj = parseResponseToObj(response);

        return (Config::NotifyMsgType)jsObj.value("msgType").toInt();
    }

public slots:
    void OnMessageReceived(const QString& message) override;

signals:
    void sig_add_server(int oId, Config::UpdateServer data);
    void sig_update_server(int oId, Config::UpdateServer data);
    void sig_remove_server(int oId);

    void sig_add_service(int oId, Config::UpdateService data);
    void sig_update_service(int oId, Config::UpdateService data);
    void sig_remove_service(int oId);

    void sig_add_rightGroup(int oId, Config::UpdateRightGroup data);
    void sig_update_rightGroup(int oId, Config::UpdateRightGroup data);
    void sig_remove_rightGroup(int oId);

    void sig_update_keyValue(int oId, QJsonValue rawJS, QString key);
};
}

Q_DECLARE_METATYPE(Config::ConfigSubscriber::Event)

#endif // CONFIGSUBSCRIBER_H
