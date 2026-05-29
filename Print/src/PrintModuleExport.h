#ifndef PRINTMODULEEXPORT_H
#define PRINTMODULEEXPORT_H

#include "IModule.h"

extern "C" {
Q_DECL_EXPORT int GetModulesCount();
Q_DECL_EXPORT IModule** CreateModules();
Q_DECL_EXPORT void ReleaseModules(IModule** modules);
}

#endif // PRINTMODULEEXPORT_H
