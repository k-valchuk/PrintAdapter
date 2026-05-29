//-------------------------------------------------------------------------------------------------
// Панель закладок для DockWidget
//-------------------------------------------------------------------------------------------------
#pragma once

#include <QToolBar>

class DockWidgetTabButton;
class AutoHideDockWidget;

class DockWidgetTabBar : public QToolBar
{
	Q_OBJECT
public:
    explicit DockWidgetTabBar(Qt::DockWidgetArea area);
    ~DockWidgetTabBar();

    void addDockWidget(AutoHideDockWidget* dockWidget);                     //добавить dockWidget в закладки
    bool removeDockWidget(AutoHideDockWidget* dockWidget);                  //удалить dockWidget из закладок
    QVector<AutoHideDockWidget *> getHideDockWidgetList() const;            //получить список dockWidgets из панели закладок
    Qt::DockWidgetArea getTabArea() const {return tabArea;}                 //получить положение панели закладок

private slots:
    void slot_dockWidgetButton_clicked();                                   //нажатие на закладку в панели

signals:
    void sig_dockWidgetButton_clicked(AutoHideDockWidget* dockWidget);      //нажатие на закладку в панели
    void sig_showDockWidget(AutoHideDockWidget* dockWidget);
    void sig_closeDockWidget(AutoHideDockWidget* dockWidget);

private:
    std::map<DockWidgetTabButton*, AutoHideDockWidget*> tabs;               //список закладок
    Qt::DockWidgetArea tabArea;                                             //положение панели

    static QString getTabBarTitle(Qt::DockWidgetArea area);                 //получить имя боковой панели
};
