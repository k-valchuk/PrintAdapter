#include "FilterHistoryListWidget.h"

FilterHistoryListWidget::FilterHistoryListWidget(QWidget* parent)
    : QFrame(parent)
{
    m_layout = new ReordarableVLayout();
    this->setLayout(m_layout);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    
    connect(m_layout, &ReordarableVLayout::startedDragging, this, [this](DraggableFrame* wgt)
            {
                m_currentDraggable = wgt;
                for(auto wgt : m_layout->GetDraggableWidgets())
                    if(auto row = qobject_cast<FilterHistoryRow*>(wgt))
                        row->setMyHover(false);
            });
    connect(m_layout, &ReordarableVLayout::endedDragging, this, [this](DraggableFrame* wgt, bool canceled)
            {
                m_currentDraggable = nullptr;
            });
}

void FilterHistoryListWidget::SetModel(IFilterPresetItem* rootItem)
{
    m_root = rootItem;
    connect(m_root, &IFilterPresetItem::treeChanged, this, [this]()
            {
                m_layout->Clear();
                m_items.clear();
                m_itemsOrdered.clear();
                m_Wgts.clear();
                int num = 1;
                m_root->Tree([this, &num](IFilterPresetItem* item)
                             {
                                 if(auto preset = dynamic_cast<FilterPreset*>(item))
                                 {
                                     auto rowWidget = new FilterHistoryRow(this);
                                     rowWidget->SetDraggingEnabled(false);
                                     rowWidget->SetPreviewOpacity(0.9);
                                     rowWidget->SetPreviewBorder(false);
                                     m_layout->AddDraggableWidget(rowWidget);
                                     rowWidget->SetName(item->Name());
                                     rowWidget->SetExpGetter([preset](){
                                         return preset->Expression();
                                     },
                                                             [preset](){
                                         return dynamic_cast<FilterPresetGroup*>(preset->Root())->GetAllFieldsFilter();
                                     },
                                                             [preset](){
                                         return dynamic_cast<FilterPresetGroup*>(preset->Root())->GetFilters();
                                     });
                                     
                                     m_Wgts.insert(rowWidget, preset);
                                     m_items.insert(preset, rowWidget);
                                     m_itemsOrdered.push_back({preset, rowWidget});
                                     
                                     connect(rowWidget, &FilterHistoryRow::saveClicked, this, [this, preset]()
                                             {
                                                 emit FilterPresetSaveRequested(preset);
                                             });
                                     connect(rowWidget, &DraggableFrame::clicked, this, [this, preset]()
                                             {
                                                 emit FilterPresetApplyRequested(preset);
                                             });
                                     connect(preset, &IFilterPresetItem::valueChanged, this, [this, preset]()
                                             {
                                                 if(m_items.contains(preset))
                                                 {
                                                     auto wgt = m_items[preset];
                                                     wgt->SetName(preset->Name());
                                                 }
                                             });
                                 }
                             });
            });
}

