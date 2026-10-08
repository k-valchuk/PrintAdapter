#ifndef IMODULELOADER_H
#define IMODULELOADER_H

#include <QList>
#include "IModule.h"
#include "ISimpleModuleProvider.h"
#include <QVariantMap>

class IModuleLoader : public ISimpleModuleProvider
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
    
    virtual void LoadModules() = 0;
    virtual void SubscribeOnModulesLoaded(QObject* context, std::function<void(bool)> callback) = 0;
    
    virtual QList<IModule*> GetModules() = 0;
    virtual bool IsExternalModule(IModule* module) = 0;
    
    virtual void ReleaseModules() = 0;
};

#endif // IMODULELOADER_H
