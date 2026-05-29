#include "PrintModuleExport.h"

#include "PrintModule.h"

IModule** CreateModules()
{
    return new IModule*[GetModulesCount()]{new PrintModule()};
}

void ReleaseModules(IModule** modules)
{
    for(int i = 0; i < GetModulesCount(); i++)
    {
        auto module = modules[i];
        delete module;
    }
    delete[] modules;
}

int GetModulesCount()
{
    return 1;
}
