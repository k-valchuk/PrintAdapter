#ifndef IWINDOWMANAGER_H
#define IWINDOWMANAGER_H

#include <QObject>
#include <QUuid>
#include <QHash>
#include <QSet>
#include <functional>

namespace ads
{
class CDockAreaWidget;
}

class IModuleLoader;
class IModuleWidget;
class IModulePanel;
class IModule;
class ISubpanelManager;

class IWindowManager
{
public:
    enum DockWidgetArea
    {
        NoDockWidgetArea = 0x00,
        LeftDockWidgetArea = 0x01,
        RightDockWidgetArea = 0x02,
        TopDockWidgetArea = 0x04,
        BottomDockWidgetArea = 0x08,
        CenterDockWidgetArea = 0x10,
    
        InvalidDockWidgetArea = NoDockWidgetArea,
        OuterDockAreas = TopDockWidgetArea | LeftDockWidgetArea | RightDockWidgetArea | BottomDockWidgetArea,
        AllDockAreas = OuterDockAreas | CenterDockWidgetArea
    };
    
    enum ColorTheme
    {
        Dark = 1,
        Light = 2
    };
    
    virtual void PlaceNewPanel(IModuleWidget* panel,
                       DockWidgetArea area = DockWidgetArea::CenterDockWidgetArea,
                       ads::CDockAreaWidget* dockAreaWidget = nullptr,
                       bool applyComposition = true) = 0;
    virtual void PlaceNewExclusivePanel(IModuleWidget* panel, bool applyComposition = true) = 0;
    virtual void PlaceNewExclusivePanel(IModuleWidget* panel, DockWidgetArea area,
                                        ads::CDockAreaWidget* dockAreaWidget,
                                        bool applyComposition = true) = 0;
    virtual void ApplyDefaultComposition(IModuleWidget* panel) = 0;
    
    virtual void RemovePanel(IModuleWidget* panel) = 0;
    
    virtual void RaisePanel(IModuleWidget* panel, bool setFocus = true) = 0;
    
    virtual int CountWidgetsWithType(IModule* module, int widgetType) = 0;
    virtual int CountVisibleWidgetsWithType(IModule* module, int widgetType) = 0;
    
    virtual ads::CDockAreaWidget* GetDockAreaForOwnedWidget(IModuleWidget* panel) = 0;
    
    virtual IModuleLoader* ModuleLoader() = 0;
    
    virtual void ExecuteInUiThreadBlocking(std::function<void()>) = 0;
    virtual void ExecuteInUiThread(std::function<void()>) = 0;
    
    
    virtual void panelNameChanged(IModuleWidget* panel, QString name) = 0;
    virtual void panelIconChanged(IModuleWidget* panel, const QIcon& icon) = 0;
    
    virtual void SubscribeToFocusedPanelChanged(QObject* receiver, std::function<void(QWidget*, IModuleWidget*,
                                                           QWidget*, IModuleWidget*)> functor) = 0;
    
    virtual void SubscribeToPanelDeletion(QObject* receiver, std::function<void(IModule*, IModuleWidget*)>) = 0;
    virtual void SubscribeToPanelHide(QObject* receiver, std::function<void(QWidget*, IModuleWidget*, bool)>) = 0;
    
    virtual bool IsPanelExists(IModuleWidget* panel) = 0;
    
    virtual QWidget* GetMainWindow() = 0;
    
    virtual void BlockPanel(IModuleWidget* panel, QString message) = 0;
    virtual void UnblockPanel(IModuleWidget* panel) = 0;
    
    virtual void ChangeSubpanelVisiblity(IModuleWidget* subpanel, bool visible) = 0;
    virtual bool IsSubpanelVisible(IModuleWidget* subpanel) = 0;
    
    virtual void EditSubpanels(IModuleWidget* parent, std::function<void (ISubpanelManager*)> action,
                               bool reapplyComposition = false) = 0;
    
    virtual QSet<IModuleWidget*> GetSubpanels(IModuleWidget* parent) = 0;
    virtual QWidget* GetDockWidget(IModuleWidget* panel) = 0;
    
    virtual QObject* UiContextObject() = 0;
    
    virtual void UpdateDynamicMenu() = 0;
};

template<class ...Args>
class IMenuManager;

class ISubpanelManager
{
public:
    virtual void SetWidget(QWidget* wgt) = 0;
    virtual void PlaceSubpanel(IModuleWidget* wgt,
                               IWindowManager::DockWidgetArea area = IWindowManager::CenterDockWidgetArea,
                               ads::CDockAreaWidget* dockAreaWidget = nullptr, bool noTab = false, bool isHidden = false) = 0;
    //virtual IMenuManager<IWindowManager*>* GetPanelToolbarMenuManager() = 0;
    virtual ads::CDockAreaWidget* GetDockArea(IModuleWidget* widget) = 0;
    virtual QWidget* GetNativeParent() = 0;
};

class IThemeManager
{
public:
    virtual IWindowManager::ColorTheme CurrentTheme() = 0;
    virtual void LoadStyleSheet(const QString& resource) = 0;
};

#endif // IWINDOWMANAGER_H
