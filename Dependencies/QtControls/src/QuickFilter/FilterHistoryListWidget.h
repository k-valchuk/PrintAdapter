
#ifndef FILTERHISTORYLISTWIDGET_H
#define FILTERHISTORYLISTWIDGET_H

#include <QFrame>

#include "ReordarableVLayout.h"
#include "FilterPresetItem.h"
#include "FilterHistoryRow.h"

class FilterHistoryListWidget : public QFrame
{
    Q_OBJECT
    ReordarableVLayout* m_layout;
    
    IFilterPresetItem* m_root = nullptr;
    
    DraggableFrame* m_currentDraggable = nullptr;
    friend class FilterHistoryRow;
    
    QMap<FilterPreset*, FilterHistoryRow*> m_items;
    QMap<FilterHistoryRow*, FilterPreset*> m_Wgts;
    QList<QPair<FilterPreset*, FilterHistoryRow*>> m_itemsOrdered;
public:
    FilterHistoryListWidget(QWidget* parent = nullptr);
    void SetModel(IFilterPresetItem* rootItem);
    
signals:
    void FilterPresetApplyRequested(FilterPreset* item);
    void FilterPresetSaveRequested(FilterPreset* item);
};

#endif // FILTERHISTORYLISTWIDGET_H
