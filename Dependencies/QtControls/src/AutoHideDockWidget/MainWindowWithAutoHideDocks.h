//-------------------------------------------------------------------------------------------------
// Главное окно с доп. опциями:
// - автоскрытие Dock Widget в закладки
// - новая система dock окон (ADS)
//
//-----------------------------------------
// - createDockManager() необходимо вызвать после инициализации формы главного окна (ui->setupUi)
//-------------------------------------------------------------------------------------------------
#pragma once

#include <QMainWindow>
#include "QMap"
#include "../AdvancedDockingSystem/DockManager.h"

class DockWidgetTabBar;
class AutoHideDockWidget;
class QSettings;

class MainWindowWithAutoHideDocks : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindowWithAutoHideDocks(QWidget *parent = 0);

    //добавить dockWidget
    ads::CDockAreaWidget *addDockWidget(ads::DockWidgetArea area, ads::CDockWidget *dockWidget, const QString objName = QString(), ads::CDockAreaWidget *dockArea = nullptr);

    //созать панели закладок
    void createDockTabBars();

    //сохранение/загрузка настроек панелей закладки
    void loadDockTabBarsSettings(QSettings &set);
    void saveDockTabBarsSettings(QSettings &set);

    //создать dock компоновщик
    void createDockManager();

protected:
    void closeEvent(QCloseEvent*) override;

    QList<ads::CDockWidget*> listDockWidget;                          //список doc виджетов
    ads::CDockManager *dockManager;                                   //компоновщик dok окон
    
    virtual bool onDockWidgetPinned(AutoHideDockWidget* dockWidget, bool bCLose);
private slots:
    //dockWidget прикреплен (убрать из панели закладок)
    void slot_dockWidgetPinned(AutoHideDockWidget* dockWidget, bool bCLose);

    //изменился  статус autoHide у dockWidget (скрыть/показать из панели закладок)
    void slot_autoHideDockWidgetChanged(AutoHideDockWidget* dockWidget);

    //скрыть dock widget в закладку
    void slot_dockWidgetHidden(ads::CDockWidget *dockWidget);

    //закрыть dock widget
    void slot_dockWidgetClose(AutoHideDockWidget* dockWidget);

private:
    QMap<Qt::DockWidgetArea, DockWidgetTabBar *> tabBars;                   //список панелей закладок


    void createDockWidgetBar(Qt::DockWidgetArea area);                      //создать панель закладок
    void hideDockWidget(AutoHideDockWidget* dockWidget);                    //скрыть dockWidget

    void adjustDockWidget(AutoHideDockWidget* dockWidget, int &maxLength);  //выровнить dockWidget при отображении из закладки

    DockWidgetTabBar* getDockWidgetBar(Qt::DockWidgetArea area);            //получить боковую панель по области
    ads::CDockWidget *getDockWidWithObjName(const QString &objName) const;  //получить dock по objName
};
