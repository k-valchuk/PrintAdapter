#include "DockWidgetTabBar.h"
#include "DockWidgetTabButton.h"
#include "AutoHideDockWidget.h"

//область -> ориентация
static Qt::Orientation areaToOrientation(Qt::DockWidgetArea area)
{
    switch(area)
    {
        case Qt::LeftDockWidgetArea:
        case Qt::RightDockWidgetArea:
            return Qt::Vertical;
        case Qt::TopDockWidgetArea:
        case Qt::BottomDockWidgetArea:
            return Qt::Horizontal;
        default:
            return Qt::Orientation(0);
    }
}

DockWidgetTabBar::DockWidgetTabBar(Qt::DockWidgetArea area) : tabArea(area)
{
	setObjectName("DockWidgetBar");
    setWindowTitle(getTabBarTitle(tabArea));
	setFloatable(false);
	setMovable(false);
	setContextMenuPolicy(Qt::PreventContextMenu);
    setOrientation(areaToOrientation(tabArea));

    hide();
}

DockWidgetTabBar::~DockWidgetTabBar()
{
}

void DockWidgetTabBar::addDockWidget(AutoHideDockWidget* dockWidget)
{
    if(dockWidget == nullptr)    
		return;	

    //добавить кнопку
    DockWidgetTabButton* dockWidgetTabButton = new DockWidgetTabButton(dockWidget->windowTitle(), orientation());
    connect(dockWidgetTabButton, &DockWidgetTabButton::clicked, this, &DockWidgetTabBar::slot_dockWidgetButton_clicked);
    tabs[dockWidgetTabButton] = dockWidget;

    //закладка
	QAction* action = addWidget(dockWidgetTabButton);
	dockWidgetTabButton->setAction(action);

    //показать панель закладок, если была скрыта
    if(tabs.size() == 1)    
	    show();
}

bool DockWidgetTabBar::removeDockWidget(AutoHideDockWidget *dockWidget)
{
    if(dockWidget == nullptr)    
		return false;	

    //поиск закладки
    auto it = std::find_if(std::begin(tabs), std::end(tabs), [dockWidget](const std::pair<DockWidgetTabButton*, AutoHideDockWidget*> v)
    {
		return v.second == dockWidget;
    });

    if(it == tabs.end())    
		return false;	

    //удалить закладку
    DockWidgetTabButton* dockWidgetTabButton = it->first;		
    tabs.erase(it);
	removeAction(dockWidgetTabButton->getAction());

    //скрыть панель закладок, если опустела
    if(tabs.empty())    
		hide();	

    return true;
}

QVector<AutoHideDockWidget *> DockWidgetTabBar::getHideDockWidgetList() const
{
    QVector<AutoHideDockWidget *> dwList;
    for (const auto& dw : tabs)
        dwList.append(dw.second);

    return dwList;
}

void DockWidgetTabBar::slot_dockWidgetButton_clicked()
{
    //определить нажатую закладку
    DockWidgetTabButton* dockWidgetTabButton = qobject_cast<DockWidgetTabButton*>(sender());
    if(dockWidgetTabButton == nullptr)    
		return;	

    auto it = tabs.find(dockWidgetTabButton);
    if(it == tabs.end())    
		return;	

    emit sig_dockWidgetButton_clicked(it->second);
}

QString DockWidgetTabBar::getTabBarTitle(Qt::DockWidgetArea area)
{
    switch (area)
    {
        case Qt::LeftDockWidgetArea: return tr("LeftTabBar");
        case Qt::RightDockWidgetArea: return tr("RightTabBar");
        case Qt::TopDockWidgetArea: return tr("TopTabBar");
        case Qt::BottomDockWidgetArea: return tr("BottomTabBar");
        default: break;
    }
    return QString();
}
