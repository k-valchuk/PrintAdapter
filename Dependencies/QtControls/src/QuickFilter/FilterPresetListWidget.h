#ifndef FILTERPRESETLISTWIDGET_H
#define FILTERPRESETLISTWIDGET_H

#include <QFrame>
#include "ReordarableVLayout.h"
#include "FilterPresetItem.h"
#include "FilterPresetRow.h"

class FilterPresetListWidget : public QFrame
{
    Q_OBJECT
    ReordarableVLayout* m_layout;
    
    IFilterPresetItem* m_root = nullptr;
    
    DraggableFrame* m_currentDraggable = nullptr;
    friend class FilterPresetRow;
    
    QMap<FilterPreset*, FilterPresetRow*> m_items;
    QMap<FilterPresetRow*, FilterPreset*> m_Wgts;
    QList<QPair<FilterPreset*, FilterPresetRow*>> m_itemsOrdered;
public:
    FilterPresetListWidget(QWidget* parent = nullptr);
    void SetModel(IFilterPresetItem* rootItem);
    
signals:
    void FilterPresetDeleteRequested(FilterPreset* item);
    void FilterPresetEditRequested(FilterPreset* item, QString oldName, QString newName);
    void FilterPresetApplyRequested(FilterPreset* item);
    void FilterPresetsOrderChanged();
};


#endif // FILTERPRESETLISTWIDGET_H
