#include "AutoHideDockWidget.h"
#include "AutoHideDockWidgetTitle.h"
#include "QEvent"
#include "QMouseEvent"
#include "../AdvancedDockingSystem/DockWidget.h"
#include "QApplication"

constexpr int BORDER_SIZE = 5;//размер рамки у hideDockWidget в закладке

AutoHideDockWidget::AutoHideDockWidget(const QString& title, Qt::DockWidgetArea defArea)
	: QDockWidget(nullptr)
    , area(defArea), userLength(-1), bDragMode(false), ptr_adsDockWidget(nullptr)
{
    setObjectName("AutoHideDockWidget");
    setAllowedAreas(Qt::NoDockWidgetArea);
    QApplication::instance()->installEventFilter(this);

    //заголовок
    titleWidget = new AutoHideDockWidgetTitle();
	setWindowTitle(title);
    setTitleBarWidget(titleWidget);

    //кнопки заголовка
    connect(titleWidget, &AutoHideDockWidgetTitle::sig_autoHideButton_pressed, this, &AutoHideDockWidget::slot_autoHideStateToggled);
    connect(titleWidget, &AutoHideDockWidgetTitle::sig_closeButton_pressed, this, &AutoHideDockWidget::slot_closeDockWidget);
}

AutoHideDockWidget::~AutoHideDockWidget()
{
}

void AutoHideDockWidget::setWindowTitle(const QString& text)
{
    titleWidget->setText(text);
    QDockWidget::setWindowTitle(text);
}

void AutoHideDockWidget::slot_closeDockWidget()
{
    //удаляем из закладки с закрытием
    emit sig_pinned(this, true);
}

bool AutoHideDockWidget::mouseInResizePosition(const QPoint &pos)
{
    if (area == Qt::LeftDockWidgetArea)
    {
        if (pos.x() > width() - BORDER_SIZE)
            return true;
    }
    else if (area == Qt::RightDockWidgetArea)
    {
        if (pos.x() <= BORDER_SIZE)
            return true;
    }
    else if (area == Qt::TopDockWidgetArea)
    {
        if (pos.y() > height() - BORDER_SIZE)
            return true;
    }
    else if (area == Qt::BottomDockWidgetArea)
    {
        if (pos.y() <= BORDER_SIZE)
            return true;
    }

    return false;
}

void AutoHideDockWidget::slot_autoHideStateToggled()
{
    //убрать из панели закладок
    emit sig_pinned(this, false);
}

void AutoHideDockWidget::slide(int maxLength)
{
    //максимальная длина окна из закладки
    if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea)
        setMaximumWidth(maxLength);
    else
        setMaximumHeight(maxLength);

    show();
    raise();
    activateWindow();
}

int AutoHideDockWidget::getActualWidth() const
{
    if ((area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) && userLength > 0)
        return userLength;

    return width();
}

int AutoHideDockWidget::getActualHeight() const
{
    if ((area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) && userLength > 0)
        return userLength;

    return height();
}

int AutoHideDockWidget::getUserLengthInTab(ads::CDockWidget *doc)
{
    if (doc)
    {
        QVariant varUserLength = doc->property("userLengthInTab");
        if (varUserLength.isValid())
            return varUserLength.toInt();
    }

    return -1;
}

void AutoHideDockWidget::setUserLengthInTab(ads::CDockWidget *doc, int value)
{
    if (doc)
        doc->setProperty("userLengthInTab", value);
}

bool AutoHideDockWidget::eventFilter(QObject *target, QEvent *event)
{
    if (target == this)//фильтр по текущему виджету
    {

        if (event->type() == QEvent::MouseMove)//перемещение мыши
        {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            if (!bDragMode)
            {//если не изменяем размер
                if (!mouseInResizePosition(me->pos()))
                    return true;//игнорировать перемещение вне зоны "resize"
            }
        }
        else if (event->type() == QEvent::MouseButtonPress)
        {//нажатие мыши
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            if (me->buttons() == Qt::LeftButton && mouseInResizePosition(me->pos()))
                bDragMode = true;//режим перемещения вкл
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {//отпустили клавишу мыши
            //выкл режим перемещения и сохранить размер окна
            bDragMode = false;
            userLength = width();
            setUserLengthInTab(ptr_adsDockWidget, userLength);
        }
        else if (event->type() == QEvent::ActivationChange)
        {//скрыть окно при потере фокуса
            if (!isActiveWindow())
                emit sig_close(this);
        }
    }
    return false;
}
