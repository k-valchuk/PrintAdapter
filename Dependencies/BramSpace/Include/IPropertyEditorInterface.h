#ifndef IPROPERTYEDITORINTERFACE_H
#define IPROPERTYEDITORINTERFACE_H

#include "IModule.h"
#include "IModuleWidget.h"
#include "PropertiesList.h"

class IPropertyEditorPanel : public IModuleWidget
{
public:
    IPropertyEditorPanel(IModule* parent) : IModuleWidget(parent) {}
    
    virtual void ShowProperties(QSharedPointer<PropertiesList> vm) = 0;
};

//"10a177cc-ad9a-42f3-b1cb-4351f05c05bf"
class IPropertyEditorInterface : public IBaseInterface
{
public:
    static QUuid Id()
    {
        return QUuid::fromString(QString("10a177cc-ad9a-42f3-b1cb-4351f05c05bf"));
    }
    
    QUuid GetInterfaceId() override final
    {
        return Id();
    }
    
    static InterfaceVersion Version()
    {
        return {1, 0};
    }
    
    InterfaceVersion GetVersion() override final
    {
        return Version();
    }
    
    virtual void ShowProperties(QSharedPointer<PropertiesList> vm) = 0;
    
    virtual IPropertyEditorPanel* CreateOwnPropertyPanel() = 0;
};

#endif // IPROPERTYEDITORINTERFACE_H
