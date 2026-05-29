#include "OnlyLongerContentToolTipper.h"

#include "QToolTip"
#include "QEvent"
#include "QHelpEvent"
#include "QAbstractItemView"
#include "QHeaderView"


OnlyLongerContentToolTipper::OnlyLongerContentToolTipper(QObject* parent)
    : QObject(parent)
{
}

bool OnlyLongerContentToolTipper::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QAbstractItemView* view = qobject_cast<QAbstractItemView*>(obj->parent());
        if (!view)
        {
            return false;
        }
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        
        QPoint pos = helpEvent->pos();
        QModelIndex index = view->indexAt(pos);
        if (!index.isValid())
            return false;
        
        auto rect = view->visualRect(index);
        
        auto sizeHint = view->sizeHintForIndex(index);
        
        auto toolTipData = index.data(Qt::ToolTipRole).toString();
        
        if(rect.width() < sizeHint.width() && !toolTipData.isEmpty())
        {
            QToolTip::showText(helpEvent->globalPos(), toolTipData, view, rect);
        }
        else
            QToolTip::hideText();
        return true;
    }
    return false;
}

HeaderViewOnlyLongerContentToolTipper::HeaderViewOnlyLongerContentToolTipper(QObject* parent)
    : QObject(parent)
{
}

bool HeaderViewOnlyLongerContentToolTipper::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHeaderView* view = qobject_cast<QHeaderView*>(obj->parent());
        if (!view)
        {
            return false;
        }
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        
        QPoint pos = helpEvent->pos();
        auto index = view->logicalIndexAt(pos);
        if (index < 0 || index >= view->count())
            return false;
        
        auto width = view->sectionSize(index);
        
        auto sizeHint = view->sectionSizeHint(index);
        
        auto toolTipData = view->model()->headerData(index, view->orientation()).toString();
        
        if(width < sizeHint && !toolTipData.isEmpty())
        {
            QToolTip::showText(helpEvent->globalPos(), toolTipData, view);
        }
        else
            QToolTip::hideText();
        return true;
    }
    return false;
}
