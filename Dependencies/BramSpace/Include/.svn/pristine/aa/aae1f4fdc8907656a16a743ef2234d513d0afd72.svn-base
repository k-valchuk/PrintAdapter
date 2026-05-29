#ifndef IMODULELOADER_H
#define IMODULELOADER_H

#include <QList>
#include "IModule.h"
#include <QVariantMap>

class IModuleLoader
{
public:
    IModule* GetModuleByIdentifier(QUuid id)
    {
        auto modules = GetModules();
        for(auto module : modules)
            if(module->GetModuleTypeIdentifier() == id)
                return module;
        
        return nullptr;
    }
    
    template<class M> //where M : IModule
    M* QueryModule(QUuid id)
    {
        auto module = GetModuleByIdentifier(id);
        return dynamic_cast<M*>(module);
    }
    
    template<class I> //where I : IBaseInterface
    I* QueryInterface(QUuid interfaceId, IBaseInterface::InterfaceVersion requestedVersion)
    {
        auto modules = GetModules();
        for(auto module : modules)
        {
            auto interfaces = module->GetSupportedInterfaces();
            for(auto i : interfaces)
                if(i->GetInterfaceId() == interfaceId)
                {
                    auto ver = i->GetVersion();
                    if(ver.Major == requestedVersion.Major && ver.Minor >= requestedVersion.Minor)
                        return dynamic_cast<I*>(i);
                    else
                        return nullptr;
                }
        }
        return nullptr;
    }
    
    void OnLanguageChanged(QString language)
    {
        auto modules = GetModules();
        for(auto module : modules)
            module->OnLanguageChanged(language);        
    }
    
    virtual void LoadModules() = 0;
    virtual void SubscribeOnModulesLoaded(QObject* context, std::function<void(bool)> callback) = 0;
    
    virtual QList<IModule*> GetModules() = 0;
    virtual bool IsExternalModule(IModule* module) = 0;
    
    virtual QVariantMap GetData()
    {
        return {};   
    }
    
    virtual void ReleaseModules() = 0;
};

#endif // IMODULELOADER_H
