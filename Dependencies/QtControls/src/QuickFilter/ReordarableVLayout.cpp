
#include "ReordarableVLayout.h"
#include "qdebug.h"

QWidget* ReordarableVLayout::closestWidgetAt(QPoint point)
{
    std::function<int(QPoint, QPoint)> distance = [](QPoint a, QPoint b)
    {
        return (a.x() - b.x())*(a.x() - b.x()) + (a.y() - b.y())*(a.y() - b.y());
    };
    
    bool first = true;
    int minDistance = 0;
    QWidget* wgt = nullptr;
    QPoint last;
    for(int i = 0; i < this->count(); i++)
    {
        auto item = this->itemAt(i);
        if(item && item->widget() && qobject_cast<DraggableFrame*>(item->widget()))
        {
            auto dist = distance(item->widget()->mapToParent(item->widget()->rect().topLeft()), point);
            last = item->widget()->mapToParent(item->widget()->rect().bottomLeft());
            if(first || dist < minDistance)
            {
                minDistance = dist;
                wgt = item->widget();
                first = false;
            }
        }
    }
    
    auto dist = distance(last, point);
    if(first || dist < minDistance)
    {
        minDistance = dist;
        wgt = nullptr;
        first = false;
    }
    
    return wgt;
}

ReordarableVLayout::ReordarableVLayout(QWidget* parent)
    : QVBoxLayout(parent)
{
    m_emptyFrame = new QFrame();
    m_emptyFrame->hide();
}

void ReordarableVLayout::AddDraggableWidget(DraggableFrame* wgt)
{
    InsertDraggableWidget(this->count(), wgt);
}

void ReordarableVLayout::InsertDraggableWidget(int index, DraggableFrame* wgt)
{
    connect(wgt, &DraggableFrame::dragStarted, this, [this, wgt]()
        {
            m_originalPos = this->indexOf(wgt);
            m_emptyFrame->setFixedSize(wgt->size());
            this->replaceWidget(wgt, m_emptyFrame);
            m_currentWgt = wgt;
            wgt->hide();
            m_emptyFrame->show();
            
            emit startedDragging(wgt);
        }, Qt::DirectConnection);
    connect(wgt, &DraggableFrame::dragPositionChanged, this, [this, wgt](QPoint point, QPoint middle)
        {
            auto closest = this->closestWidgetAt(middle);
            if(closest != m_emptyFrame)
            {
                this->removeWidget(m_emptyFrame);
                
                int index = this->indexOf(closest);
                if(!closest)
                {
                    auto draggableWidgets = GetDraggableWidgets();
                    index = draggableWidgets.size() > 0 ? (this->indexOf(draggableWidgets.last()) + 1) : 0;
                }
                
                this->insertWidget(index, m_emptyFrame);
            }
        }, Qt::DirectConnection);
    connect(wgt, &DraggableFrame::dropped, this, [this, wgt](QPoint point)
        {
            this->replaceWidget(m_emptyFrame, wgt);
            m_currentWgt = nullptr;
            wgt->show();
            m_emptyFrame->hide();
            
            emit endedDragging(wgt);
            emit orderChanged();
        }, Qt::DirectConnection);
    connect(wgt, &DraggableFrame::dragCanceled, this, [this, wgt]()
        {
            if(!m_currentWgt)
                return;
            this->removeWidget(m_emptyFrame);
            this->insertWidget(m_originalPos, wgt);
            m_emptyFrame->hide();
            wgt->show();
            
            m_currentWgt = nullptr;
            emit endedDragging(wgt, true);
        }, Qt::DirectConnection);
    connect(wgt, &DraggableFrame::dragAboutToStart, this, [this, wgt]()
        {
            emit dragAboutToStart(wgt);
        }, Qt::DirectConnection);
    
    this->insertWidget(index, wgt);
}

void ReordarableVLayout::RemoveDraggableWidget(DraggableFrame* wgt, bool deleteWidget)
{
    for(int i = 0; i < this->count(); i++)
    {
        auto item = this->itemAt(i);
        if(item && item->widget())
            if(qobject_cast<DraggableFrame*>(item->widget()) == wgt)
            {
                this->takeAt(i);
                auto wgt = item->widget();
                delete item;
                if(wgt && deleteWidget)
                    delete wgt;
                break;
            }
    }
}

QList<DraggableFrame*> ReordarableVLayout::GetDraggableWidgets()
{
     QList<DraggableFrame*> result;
     for(int i = 0; i < this->count(); i++)
     {
        auto item = this->itemAt(i);
        if(item && item->widget())
            if(auto df = qobject_cast<DraggableFrame*>(item->widget()))
                result.append(df);
     }
     return result;
}

void ReordarableVLayout::Clear(bool deleteWidgets)
{
     while(this->count() > 0)
     {
        auto item = takeAt(0);
        auto wgt = item->widget();
        delete item;
        if(wgt && deleteWidgets)
            delete wgt;
     }
}

