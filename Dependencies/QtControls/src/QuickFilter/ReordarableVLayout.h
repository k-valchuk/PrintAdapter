#ifndef REORDARABLEVLAYOUT_H
#define REORDARABLEVLAYOUT_H

#include <QVBoxLayout>
#include "DraggableFrame.h"

class ReordarableVLayout : public QVBoxLayout
{
    Q_OBJECT
    
    QFrame* m_emptyFrame;
    DraggableFrame* m_currentWgt;
    int m_originalPos;
    
    QWidget* closestWidgetAt(QPoint point);
public:
    ReordarableVLayout(QWidget* parent = nullptr);
    
    void AddDraggableWidget(DraggableFrame* wgt);
    void InsertDraggableWidget(int index, DraggableFrame* wgt);
    void RemoveDraggableWidget(DraggableFrame* wgt, bool deleteWidget = true);
    
    QList<DraggableFrame*> GetDraggableWidgets();
    
    void Clear(bool deleteWidgets = true);
    
signals:
    void dragAboutToStart(DraggableFrame* wgt);
    void startedDragging(DraggableFrame* wgt);
    void endedDragging(DraggableFrame* wgt, bool canceled = false);
    void orderChanged();
};

#endif // REORDARABLEVLAYOUT_H
