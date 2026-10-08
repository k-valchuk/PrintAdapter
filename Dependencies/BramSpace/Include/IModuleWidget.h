#ifndef IMODULEWIDGET_H
#define IMODULEWIDGET_H

#include <QWidget>
#include <QString>
#include <QUuid>
#include <QSet>
#include <QList>
#include <functional>
#include "IMenuManager.h"
#include "IWindowManager.h"

#include "ICommandBuilders.h"
#include "ICommandContext.h"

class ICompositionManager
{
public:
    enum SettingType
    {
        Bool = 0,
        Integer = 1,
        //Text,
        //DateTime,
        //Numeric,
        RowColor = 50,
        CustomWidget = 100
    };
    
    virtual void AddTab(std::function<QString()> name, const QString& key) = 0;
    virtual void AddGroupBox(const QString& tabKey, std::function<QString()> name, const QString& key) = 0;
    virtual void AddSetting(const QString& tabKey, const QVariant& key, SettingType type,
                            std::function<QString()> name = nullptr,
                            const QVariant& value = QVariant(),
                            const QString& groupBoxKey = QString()) = 0;
    
    virtual void SetSettingValue(const QString& tabKey, const QVariant& key, const QVariant& value) = 0;
    virtual QVariant GetSettingValue(const QString& tabKey, const QVariant& key) = 0;
    
    virtual void SubscribeOnValueChange(QObject* context, std::function<void(const QString&, const QVariant&)> action) = 0;
};

struct CompositionManagerIntegerValue
{
    int Value = 0;
    int MinValue;
    int MaxValue;
};

struct CompositionManagerRowColorValue
{
    enum Type {
        BackgroundOnly,
        BackroundAndForeground,
        All
    } Mode;

    QColor Background;
    QColor Foreground;
    QFont  Font;
};

Q_DECLARE_METATYPE(CompositionManagerIntegerValue);
Q_DECLARE_METATYPE(CompositionManagerRowColorValue);

class IModule;
class IModuleWidget
{

    IModule* m_parent = nullptr;
    IModuleWidget* m_parentWidget = nullptr;
    IWindowManager* m_wm = nullptr;
public:
    IModuleWidget(IModule* parent) { m_parent = parent; }
    virtual ~IModuleWidget() {}

    IModule* Parent() {return m_parent;}

    void SetParentWidget(IModuleWidget* parent) { m_parentWidget = parent; }
    IModuleWidget* ParentWidget() {return m_parentWidget;}

    virtual void FillDynamicMenu(IOrderedMenuManager<IWindowManager*>* menu)
    {
        if(m_parentWidget)
            m_parentWidget->FillDynamicMenu(menu);
    }

    virtual void SaveState(QUuid moduleId, int widgetType, QUuid instanceId){ };
    virtual void LoadState(QUuid moduleId, int widgetType, QUuid instanceId){ };

    virtual QByteArray SaveStateToView() {return QByteArray();}
    virtual void RestoreStateFromView(const QByteArray& data) {}
    
    
    virtual QByteArray SaveStateToComposition(ICompositionManager* mgr) {return QByteArray();}
    virtual void RestoreStateFromComposition(IWindowManager* wm, const QByteArray& data) {}
    
    virtual void FillCompositionSettings(IWindowManager* wm, ICompositionManager* mgr, const QByteArray& savedSettings) {}
    
    virtual void ReleaseWidget(QWidget* widget) { delete widget; }
    
    virtual void OnWidgetHide(bool hidden) {}

    enum PanelFocusedBy
    {
        Itself,
        ChildSubpanel
    };

    virtual void OnFocusIn(PanelFocusedBy reason){}
    virtual void OnFocusOut(PanelFocusedBy reason){}

    virtual QIcon GetIcon() { return QIcon(); }

    // При удалении
    virtual void OnModuleWidgetDeletion() {}
    // При закрытии
    virtual bool OnModuleWidgetClosing() { return true; }
    // При изменении языка
    virtual void OnLanguageChanged(QString language) {}

    virtual QString GetName() = 0;

    void SetWindowManager(IWindowManager* wm) {m_wm = wm;}
    void nameChanged(QString name)
    {
        if(m_wm)
            m_wm->panelNameChanged(this, name);
    }
    void iconChanged(const QIcon& icon)
    {
        if(m_wm)
            m_wm->panelIconChanged(this, icon);
    }
    
    virtual void PlaceContent(ISubpanelManager* panel) {}
    virtual void PlaceContent(ISubpanelManager* panel, const QByteArray& compositionData)
    {
        PlaceContent(panel);
    }
    
    virtual void RouteCommand(ICommandRoutingContext* context) {}
    virtual void QueryCommandState(ICommandStateContext* context) {}
    virtual void ExecuteCommand(ICommandExecutionContext* context) {}
};


#endif // IMODULEWIDGET_H
