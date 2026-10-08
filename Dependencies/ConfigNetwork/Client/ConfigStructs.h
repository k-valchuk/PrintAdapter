#ifndef CONFIG_STRUCTS_H
#define CONFIG_STRUCTS_H

#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QString>
#include <QVector>

namespace Config {

enum NotifyMsgType {
    Unknown = 0,
    SetSubs = 1, // Установка подписок
    GetSubs = 2, // Получение подписок
    Event = 3 // Получение уведомления
};

//тип конкретного события по типу "Изменение путей/трактов и пр. конфигураций" (type = 6)
enum ConfigEventType {
    configAdd = 1,
    configEdit = 2,
    configDel = 3
};

//--------------------------------------------------------------------------------
// Служба
//--------------------------------------------------------------------------------
// --- Типы сервисов по всем модулям
enum class EServiceType {
    eNone = 0,

    eElasticSearch = 1,
    ePostgreSQL = 2,
    eAccessPoint = 3,
    eQueue = 4,
    eAzimuth = 5,

    //Air
    eAirVSManager = 10,
    eAirApp = 11,
    eTeletextInserter = 12,
    eGraphicsTitleStation = 13,
    eGraphicsCarrot = 14,
    eGraphicsVizRT = 15,
    eAirConfigApp = 16,
    eAirMOSSynchronizer = 17,

    //Storage
    eStorageFileBrowser = 20,
    eStorageApp = 22,

    //Core
    eCoreAuth = 30,
    eCoreData = 31,
    eCoreConfig = 32,

    //Capture
    eCaptureVSManager = 40,
    eCaptureApp = 41,
    eCaptureConfigApp = 42,

    //Plan
    ePlanApp = 51,
    ePlanConfigApp = 52,

    //MAM
    eAssetApp = 61,
    eAssetConfigApp = 62,
    eDAMFileManager = 63,
    eDAMFileAgent = 64,
    eDAMFileWatcher = 65,

    //News
    eNewsApp = 71,
    eNewsConfigApp = 72,
    eGraphicsCarrotPlugin = 73,

    //Agency
    eAgencyApp = 81,
    eAgencyConfigApp = 82,

    //Studio
    eStudioVSManager = 90,
    eStudioApp = 91,
    ePrompterServer = 92,
    eGraphicsServer = 93,
    eStudioConfigApp = 94,

    //GAR (Пока в основном для поиска в Plan)
    eGARSearch = 100,

    //Notifications
    eMessageApp = 110,
    eMessageConfigApp = 111,
    eMessageSenderService = 112,

    // Мониторинг
    eRestService = 120,
    eGuardService = 121,
    eCollectorService = 122,
    eIPMILogService = 123,
    eNotifierService = 124,
    eMonitoringConfigApp = 125,
    eAirMonitorApp = 126,
    eAirMonitor = 127,

    // планирование эфира
    eTrafficApp = 130,
    eTrafficConfigApp = 131,

    // BPM
    eBPMCoreConnection = 200,
    eBPMCoreStorage = 201,
    eBPMRestService = 205,
    eBPMScriptService = 206
};

// --- Тип сервиса - его ИД и имя
struct ServiceType {
    int id = 0;
    QString name;
    // Из json
    void parse(const QJsonObject& o);
};

// --- Функциональный статус канала
enum EChannelFunctionStatus {
    eCFSNone = 0, // Ожидает
    eCFSAir = 1, // Воспроизведение
    eCFSCapture = 2 // Запись
};

// --- for Sender Email
enum EConnectionSecurityOptions {
    eNone = 0,
    eSTARTTLS = 1,
    eSSL_TLS = 2
};

enum EAuthorizationStates {
    eNo = 0,
    eUserPass = 1
};

// --- Свойства служб
struct ServiceProps {
    EServiceType type = EServiceType::eNone;
    int serverId = -1;
    QString name{}, path{}; /*Путь - по серверу*/
};

// --- Порт(канал) конфигурации VSManager
struct VSMChannel {
    int channel = 0; // Номер канала на сервере
    QString name; // Имя сервера канала
    EChannelFunctionStatus type = EChannelFunctionStatus::eCFSNone; // Тип канала
    int id = 0; // Уникальный ИД порта
};

// --- Запись конфигурации VSM для одного сервера
struct VSMServerProps {
    int vsId = 0;
    QVector<VSMChannel> channels;
};

// --- Конфигурация канала Azimuth
struct AzChannelConfig {
    QString rtsp;
    int number = 0;
};

// --- Конфигурация сервиса Азимут
struct AzServiceConfiguration {
    QVector<AzChannelConfig> channels;
};

// --- Объект службы
struct Service {
    // ID
    int id = -1;
    // Свойства службы
    ServiceProps props{};
    // Не разобранные данные конфигурации сервиса
    QJsonValue rawConfiguration{};
    // Разобранные данные конфигурации в зависимости от типа службы:
    QVector<VSMServerProps> vsmServersProp{}; // 1. VSM
    AzServiceConfiguration azServiceConfig{}; // 2. Азимут

    //-----------------------------------------------
    // Из/в json
    bool parse(const QJsonObject& o);
    QJsonObject construct(void) const;
    //-----------------------------------------------
    // Валидная служба или нет
    inline bool isValid() const {return id > 0;}
    // Является ли службы необходимой для страницы каналов
    bool isNecessaryForChannels() const;
    // Является ли служба службой Azimuth
    bool isAzimuth() const;
    // Является ли служба VSM службой
    bool isVSMType() const;
    // Является ли служба графической службой
    bool isGraphicsType() const;
    // Является ли служба той, на которую можно завязать каналы
    bool isManagedService() const;
    // Есть ли тег path или host в конфиге
    bool isServiceHavePath() const;
    // Есть ли собственный путь у службы
    static bool isServiceHavePathByType(Config::EServiceType type);
};

// --- Объект обновленной службы (Notifier)
struct UpdateService : Service {
    bool isTypeUp = false;
    bool isServerIdUp = false;
    bool isNameUp = false;
    bool isPathUp = false;
    bool isConfigUp = false;

    bool parse(const QJsonObject& o);
};

// --- Конфигурация видеосервиса
struct VsmConf {
    int number;
    QString functionType;
};

//--------------------------------------------------------------------------------
// Серверы
//--------------------------------------------------------------------------------
// --- Свойства сервера
struct ServerProps {
    // Свойства
    QString name{};
    QString path{};
    // Из/в Json
    void prase(const QJsonObject& o);
    QJsonObject construct(void) const;
};

// --- Объект сервера
struct Server {
    // Свойства
    int id = -1;
    ServerProps props;
    QVector<Service> services;
    // Из/в Json
    void parse(const QJsonObject& o);
    QJsonObject construct(void) const;
    // Валидноть
    inline bool isValid(){return id > 0;}
};

// --- Объект сервера для получения уведомлений
struct UpdateServer : public Server {
    // Какие поля обновились
    bool isPropsNameUp = false;
    bool isPropPathUp = false;
    bool isServicesUp = false;
    // Из Json
    void parse(const QJsonObject& o);
};

///////////////////////////////////////////////
//----------------- Права --------------------
// Схема прав
struct RightScheme{
    QJsonArray schemeArray{};
    QJsonArray schemeObjectsArray{};

    // Из Json
    void parse(const QJsonObject& o);
};

struct ObjectsHierarchy{
    QJsonArray hierarchy{};
    // Из Json
    void parse(const QJsonObject& o);
};

struct ObjectRightGroup{
    int id;
    int typeId;
    QVector<int> allowedRightsVec;
    QVector<int> restrictedRightsVec;
};

// Группа прав
struct RightGroup{
    int id;
    QString name;
    QVector<int> allowedRightsVec{};
    QVector<int> restrictedRightsVec{};
    QVector<int> userGroupsVec{};

    QVector<ObjectRightGroup> objectsRights{};

    void parse(const QJsonObject& o);

    inline bool isValid(){return id > 0;};
};

// Обновленная группа прав
struct UpdateRightGroup : RightGroup{
    bool isNameUpdate = false;
    bool isAllowedRightsUpdate = false;
    bool isRestrictedRightsUpdate = false;
    bool isUserGroupsUpdate = false;
    bool isObjectRightsUpdate = false;

    void parse(const QJsonObject& o);
    QJsonObject construct(void) const;
    bool isFullUpdate();
    inline bool isRightsMembersUpdate() {return (isAllowedRightsUpdate || isRestrictedRightsUpdate || isObjectRightsUpdate);};
};

////////////////////////////////////////////////////
//----------------- Уведомления --------------------
// Схема уведомлений
struct NotificationScheme{
    QJsonArray schemeArray{};
    QJsonArray configSchemeArray{};
    // Из Json
    void parse(const QJsonObject& o, bool parseForTree = true);

private:
    QJsonObject transformConfigToDefaultScheme(QJsonObject& deepChild);
};

//--------------------------------------------------
// Configuration
//-------------------
// --- Объект Configuration
struct Configuration {
    QString name{};
    QString value{};
        // Из/в Json
    static void parse(const QJsonObject& o);
    QJsonObject construct(void) const;
};

// --- Объект обновленной конфигурации (Notifier)
struct UpdateConfiguration : Configuration {
    QString name{};
    bool isNameUp = false;
    bool isValueUp = false;
    static bool parse(const QJsonObject& o);
};

//список подсистем
struct SubsytemsList
{
    enum Subsystem
    {
        Air,
        Capture,
        Catalogs,
        News,
        Agency,
        Log,
        MAM,
        Studio,
        Plan,
        Monitoring,
        BPM
    };
private:
    
    QStringList m_list;
public:
    SubsytemsList(const QList<Subsystem>& subsystems)
    {
        for(auto s : subsystems)
            m_list.append(SubsystemToString(s));
    }
    
    SubsytemsList(const QStringList& subsystems)
    {
        m_list = subsystems;
    }
    
    bool CheckSubsystem(Subsystem s) const
    {
        return m_list.contains(SubsystemToString(s));
    }
    
    QStringList GetKeys() const {return m_list;}
    
    static QString SubsystemToString(Subsystem s)
    {
        switch(s)
        {
        case Air:
            return "air";
        case Capture:
            return "capture";
        case Catalogs:
            return "catalogs";
        case News:
            return "news";
        case Agency:
            return "newsagency";
        case Log:
            return "log";
        case MAM:
            return "mam";
        case Studio:
            return "studio";
        case Plan:
            return "plan";
        case Monitoring:
            return "monitoring";
        case BPM:
            return "bpm";
        }
        return QString();
    }
};


}

#endif // CONFIG_STRUCTS_H
