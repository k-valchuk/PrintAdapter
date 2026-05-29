#include "MainWindowWithAutoHideDocks.h"
#include "DockWidgetTabBar.h"
#include "AutoHideDockWidget.h"
#include "QEvent"
#include "QSettings"

using namespace ads;

MainWindowWithAutoHideDocks::MainWindowWithAutoHideDocks(QWidget *parent) : QMainWindow(parent), dockManager(nullptr)
{    
    //флаги ADS
    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, true);
    CDockManager::setConfigFlag(CDockManager::XmlCompressionEnabled, false);
    CDockManager::setConfigFlag(CDockManager::FocusHighlighting, true);
}

void MainWindowWithAutoHideDocks::createDockWidgetBar(Qt::DockWidgetArea area)
{
    if (tabBars.contains(area))//уже есть
        return;

    DockWidgetTabBar* dockWidgetBar = new DockWidgetTabBar(area);
    tabBars[area] = dockWidgetBar;
    //нажатие на закладку
    connect(dockWidgetBar, &DockWidgetTabBar::sig_dockWidgetButton_clicked, this , &MainWindowWithAutoHideDocks::slot_autoHideDockWidgetChanged);

    addToolBar((Qt::ToolBarArea)area, dockWidgetBar);
}

void MainWindowWithAutoHideDocks::slot_autoHideDockWidgetChanged(AutoHideDockWidget *dockWidget)
{
    if(dockWidget == nullptr)
        return;

    if(dockWidget->isHidden())//если dockWidget скрыт
    {
        //расчет размера dockWidget
        int maxLength;
        adjustDockWidget(dockWidget, maxLength);
        dockWidget->slide(maxLength);//показать
    }
}

void MainWindowWithAutoHideDocks::slot_dockWidgetHidden(CDockWidget *dockWidget)
{
    //панель закладок
    DockWidgetTabBar* dockWidgetBar = getDockWidgetBar(dockWidget->getTabArea());
    if(dockWidgetBar == nullptr)
        return;

    //берем виджет из dock окна
    auto *wid = dockWidget->takeWidget();
    AutoHideDockWidget *ahdWid = new AutoHideDockWidget(dockWidget->windowTitle(), dockWidget->getTabArea());
    ahdWid->setWidget(wid);

    //добавить в панель закладок
    dockWidgetBar->addDockWidget(ahdWid);
    ahdWid->setDockWidgetADS(dockWidget);
    dockWidget->toggleViewAction()->setEnabled(false);//засерить пункт меню
    ahdWid->setUserLength(AutoHideDockWidget::getUserLengthInTab(dockWidget));//установить пользовательскую длину

    //убрать из закладки
    connect(ahdWid, &AutoHideDockWidget::sig_pinned, this, &MainWindowWithAutoHideDocks::slot_dockWidgetPinned);
    //закрыть
    connect(ahdWid, &AutoHideDockWidget::sig_close, this, &MainWindowWithAutoHideDocks::slot_dockWidgetClose);
}

void MainWindowWithAutoHideDocks::slot_dockWidgetClose(AutoHideDockWidget *dockWidget)
{
    hideDockWidget(dockWidget);//скрыть
}

void MainWindowWithAutoHideDocks::hideDockWidget(AutoHideDockWidget *dockWidget)
{
    if((dockWidget == nullptr) || (dockWidget->isHidden()))
        return;

    dockWidget->hide();
}

void MainWindowWithAutoHideDocks::adjustDockWidget(AutoHideDockWidget *dockWidget, int &maxLength)
{
    if(dockWidget == nullptr)
        return;

    static const int MIN_VISIBLE_LENGTH = 100;//минимальная видимая область главного окна при показе doca из закладки

    auto *tabBarLeft = getDockWidgetBar(Qt::LeftDockWidgetArea);
    auto *tabBarRight = getDockWidgetBar(Qt::RightDockWidgetArea);
    auto *tabBarTop = getDockWidgetBar(Qt::TopDockWidgetArea);
    auto *tabBarBottom = getDockWidgetBar(Qt::BottomDockWidgetArea);

    //*** задать область показа dockWidget ***//

    int left = (tabBarLeft && tabBarLeft->isVisible()) ? tabBarLeft->width() : 0;//левый край области
    int right = (tabBarRight && tabBarRight->isVisible()) ? tabBarRight->x() : width();//правый край области

    //расчет допустимой ширины и высоты
    int resWidth = dockWidget->getActualWidth();
    int resHeight = dockWidget->getActualHeight();

    if (width() - resWidth < MIN_VISIBLE_LENGTH)
        resWidth = width() - MIN_VISIBLE_LENGTH;

    if (height() - resHeight < MIN_VISIBLE_LENGTH)
        resHeight = height() - MIN_VISIBLE_LENGTH;

    QPoint globalPointXY;

    //задать геометрию dockWidget и указать максимальный размер растяжения (maxLength)
    switch(dockWidget->getArea())
    {
        case Qt::LeftDockWidgetArea:
            globalPointXY = this->mapToGlobal(QPoint(left, tabBarLeft->y()));
            dockWidget->setGeometry(globalPointXY.x(), globalPointXY.y(), resWidth, tabBarLeft->height());
            maxLength = width() - MIN_VISIBLE_LENGTH;
        break;

        case Qt::TopDockWidgetArea:
            globalPointXY = this->mapToGlobal(QPoint(left, tabBarTop->height() + tabBarTop->y()));
            dockWidget->setGeometry(globalPointXY.x(), globalPointXY.y(), right - left, resHeight);
            maxLength = height() - MIN_VISIBLE_LENGTH;
        break;

        case Qt::RightDockWidgetArea:
            globalPointXY = this->mapToGlobal(QPoint(tabBarRight->x() - resWidth, tabBarRight->y()));
            dockWidget->setGeometry(globalPointXY.x(), globalPointXY.y(), resWidth, tabBarRight->height());
            maxLength = width() - MIN_VISIBLE_LENGTH;
        break;

        case Qt::BottomDockWidgetArea:
            globalPointXY = this->mapToGlobal(QPoint(left, tabBarBottom->y() - resHeight));
            dockWidget->setGeometry(globalPointXY.x(), globalPointXY.y(), right - left, resHeight);
            maxLength = height() - MIN_VISIBLE_LENGTH;
        break;

        default:break;
    }
}

CDockAreaWidget * MainWindowWithAutoHideDocks::addDockWidget(ads::DockWidgetArea area, ads::CDockWidget *dockWidget, const QString objName, CDockAreaWidget *dockArea)
{
    if(dockManager == nullptr || dockWidget == nullptr)
        return nullptr;

    listDockWidget.append(dockWidget);
    if (!objName.isEmpty())
        dockWidget->setObjectName(objName);

    return dockManager->addDockWidget(area, dockWidget, dockArea);
}

void MainWindowWithAutoHideDocks::createDockTabBars()
{
    //создать панели закладок
    createDockWidgetBar(Qt::LeftDockWidgetArea);
    createDockWidgetBar(Qt::RightDockWidgetArea);
    createDockWidgetBar(Qt::TopDockWidgetArea);
    createDockWidgetBar(Qt::BottomDockWidgetArea);
}

void MainWindowWithAutoHideDocks::loadDockTabBarsSettings(QSettings &set)
{
    //приводим к состоянию главного окна без закладок
    foreach (auto *tabB, tabBars)
    {
        if (tabB)
        {
            for (auto *dock : tabB->getHideDockWidgetList())
            {
                if (dock)
                {
                    slot_dockWidgetPinned(dock, dock->getDockWidgetADS()->isClosed());
                }
            }
        }
    }

    set.beginGroup("DockTabBars");
    int dwSize = set.beginReadArray("DockWidgets");
    for (int i = 0; i < dwSize; ++i)
    {
        set.setArrayIndex(i);
        auto *dw = getDockWidWithObjName(set.value("ObjName").toString());
        if (dw)
        {
            Qt::DockWidgetArea area = (Qt::DockWidgetArea)set.value("Area", 0).toInt();
            if (area != Qt::NoDockWidgetArea)
            {
                dw->setTabArea(area);
                AutoHideDockWidget::setUserLengthInTab(dw, set.value("UserLength").toInt());
                if (set.value("isHide").toBool())
                    slot_dockWidgetHidden(dw);
            }
        }
    }

    set.endArray();
    set.endGroup();
}

void MainWindowWithAutoHideDocks::saveDockTabBarsSettings(QSettings &set)
{
    set.beginGroup("DockTabBars");
    set.remove("");//очистить группу

    set.beginWriteArray("DockWidgets");
    int i = 0;

    for (auto *doc : listDockWidget)
    {
        set.setArrayIndex(i++);
        set.setValue("ObjName", doc->objectName());

        set.setValue("Area", doc->getTabArea());
        set.setValue("UserLength", AutoHideDockWidget::getUserLengthInTab(doc));
        set.setValue("isHide", !doc->toggleViewAction()->isEnabled());
    }
    set.endArray();

    set.endGroup();
}

void MainWindowWithAutoHideDocks::createDockManager()
{
    dockManager = new CDockManager(this);
    //убрать dock окно в закладку
    connect(dockManager, &CDockManager::dockWidgetHidden, this, &MainWindowWithAutoHideDocks::slot_dockWidgetHidden);
}

void MainWindowWithAutoHideDocks::closeEvent(QCloseEvent *)
{
    if (dockManager)
        dockManager->deleteLater();
}

bool MainWindowWithAutoHideDocks::onDockWidgetPinned(AutoHideDockWidget* dockWidget, bool bCLose)
{
    return true;
}

void MainWindowWithAutoHideDocks::slot_dockWidgetPinned(AutoHideDockWidget *dockWidget, bool bCLose)
{
    if(dockWidget == nullptr)
        return;

    //панель закладок
    DockWidgetTabBar* dockWidgetBar = getDockWidgetBar(dockWidget->getArea());
    if(dockWidgetBar == nullptr)
        return;
    
    if(!onDockWidgetPinned(dockWidget, bCLose))
        return;
    
    //удалить из панели закладок
    if(dockWidgetBar->removeDockWidget(dockWidget))
    {//добавить в главное окно
        CDockWidget *dockWidgetADS = qobject_cast<CDockWidget *>(dockWidget->getDockWidgetADS());
        dockWidgetADS->setWidget(dockWidget->widget());
        dockWidgetADS->toggleViewAction()->setEnabled(true);//пункт меню доступен
        dockWidget->deleteLater();

        if (!bCLose)//показать
            dockWidgetADS->toggleView();
    }
}

DockWidgetTabBar *MainWindowWithAutoHideDocks::getDockWidgetBar(Qt::DockWidgetArea area)
{
    auto it = tabBars.find(area);
    if(it != std::end(tabBars))
        return it.value();

    return nullptr;
}

CDockWidget *MainWindowWithAutoHideDocks::getDockWidWithObjName(const QString &objName) const
{
    for (auto * dw: listDockWidget)
    {
        if (dw->objectName() == objName)
            return dw;
    }
    return nullptr;
}
