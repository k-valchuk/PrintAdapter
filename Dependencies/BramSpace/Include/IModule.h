#ifndef IMODULEINFO_H
#define IMODULEINFO_H

#include <QString>
#include <QList>
#include <functional>
#include <QUuid>
#include <QSet>
#include <QMenu>
#include <QDateTime>
#include "IMenuManager.h"
#include "IWindowManager.h"
#include "IModuleWidget.h"

#ifdef _USES_LIB_QUERY
#include "QueryExecutor.h"
#endif

class QNetworkCookieJar;

class IBaseInterface
{
public:
    struct InterfaceVersion
    {
        unsigned Major;
        unsigned Minor;
    };
    
    virtual QUuid GetInterfaceId() = 0;
    virtual InterfaceVersion GetVersion() = 0;
};

class IConsoleLogger
{
public:
    enum LogType
    {
        Info = 1,
        Success = 2,
        Warning = 3,
        Error = 4
    };
    
    struct LogEntry
    {
        LogType Type;
        QString Where;
        QString Action;
        QString Result;
        QString Details;
    };
    
    virtual void Log(const LogEntry& entry) = 0;
};

class IBramSpaceServiceProvider
{
public:
    enum ServiceType
    {
        ConsoleLogger = 1
    };
    
    virtual void* GetService(ServiceType type) = 0;
};

class IModuleLoader;
class IModule
{
public:
    
    virtual void Init(IWindowManager* wm, IModuleLoader* loader) = 0;
    
    virtual void InitServices(IBramSpaceServiceProvider* services)
    {
#ifdef _USES_LIB_QUERY
        auto logger = (IConsoleLogger*)services->GetService(IBramSpaceServiceProvider::ConsoleLogger);
        if(logger)
        {
            //установить функцию логирования для QueryExecutor через логгер BramSpace
            rest::Executor()->SetLogCallback([logger](const rest::Query& q, const rest::Response& r){
                IConsoleLogger::LogEntry log;
                log.Type = r.isSuccess() ? IConsoleLogger::Success : IConsoleLogger::Error;
                log.Where = "http-request";
                log.Action = q.getResultUrl();
                log.Result = QString("%1 (%2)").arg(rest::errorStrMap[r.code()]).arg(r.code());
                log.Details = r.answerString();
                logger->Log(log);
            });
        }
#endif
    }
    
    virtual QUuid GetModuleTypeIdentifier() = 0;

    virtual int GetWidgetType(IModuleWidget* wgt) = 0;
    virtual IModuleWidget* CreateWidget(int typeId) = 0;
    
    virtual QSet<IBaseInterface*> GetSupportedInterfaces() { return {}; };
    
    template <class I> //where I : IBaseInterface
    I* QueryInterface(QUuid interfaceId)
    {
        auto interfaces = GetSupportedInterfaces();
        for(auto i : interfaces)
            if(i->GetInterfaceId() == interfaceId)
                return dynamic_cast<I*>(i);
        return nullptr;
    }
    
    virtual void BeforeModuleWidgetDeletion(IModuleWidget* wgt) {}
    
    virtual void FillPanelsMenu(IMenuManager<IWindowManager*>* mm) {}
    virtual void FillSettingsMenu(IMenuManager<IWindowManager*>* mm) {}
    
    virtual void OnLanguageChanged(QString language) {}
    
    virtual QString GetName() {return "";}
    
    struct Version
    {
        int global   = 1;
        int major    = 0;
        int minor    = 0;
        int revision = 1;
    };
    
    virtual Version GetVersion() = 0;
    
    virtual void OnQueryExecutorDataChanged(QString url, QString aToken,
                                            QString rToken, QDateTime exp)
    {
#ifdef _USES_LIB_QUERY
        rest::Executor()->SetBaseUrl(url);
        rest::Executor()->SetAuthToken(aToken, rToken, exp);
#endif
    }
    
    virtual void SetApplicationCookieJar(QNetworkCookieJar* cookieJar)
    {
#ifdef _USES_LIB_QUERY
        rest::Executor()->SetCookieJar(cookieJar);
#endif
    }
    
    virtual void ReleaseModuleWidget(IModuleWidget* panel)
    {
        delete panel;
    }
    
    virtual void OnThemeChanged(IThemeManager* tm) {}
    
    virtual ~IModule() {
#ifdef _USES_LIB_QUERY
        rest::Executor()->SetLogCallback(nullptr);
#endif
    }
};


#endif // IMODULEINFO_H
