#ifndef IMODULEINFO_H
#define IMODULEINFO_H

#include "ISimpleModule.h"
#include "IMenuManager.h"
#include "IWindowManager.h"
#include "IModuleWidget.h"
#include "ICommandBuilders.h"
#include "ICommandContext.h"

#include <functional>
#include <QMenu>

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
class IModule : public ISimpleModule
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

    virtual int GetWidgetType(IModuleWidget* wgt) = 0;
    virtual IModuleWidget* CreateWidget(int typeId) = 0;
    
    virtual void BeforeModuleWidgetDeletion(IModuleWidget* wgt) {}
    
    virtual void FillPanelsMenu(IMenuManager<IWindowManager*>* mm) {}
    virtual void FillSettingsMenu(IMenuManager<IWindowManager*>* mm) {}
    
    using Version = ISimpleModule::Version;
    
    virtual void ReleaseModuleWidget(IModuleWidget* panel)
    {
        delete panel;
    }
    
    virtual void OnThemeChanged(IThemeManager* tm) {}
    
    virtual void DescribeCommands(ICommandCatalogBuilder& builder) {}
    virtual void RouteCommand(ICommandRoutingContext* context) {}
    virtual void QueryCommandState(ICommandStateContext* context) {}
    virtual void ExecuteCommand(ICommandExecutionContext* context) {}
    
    virtual ~IModule() = default;
};


#endif // IMODULEINFO_H
