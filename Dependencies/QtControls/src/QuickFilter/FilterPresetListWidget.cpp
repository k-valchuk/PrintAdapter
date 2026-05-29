#include "FilterPresetListWidget.h"

FilterPresetListWidget::FilterPresetListWidget(QWidget* parent)
    : QFrame(parent)
{
    m_layout = new ReordarableVLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    this->setLayout(m_layout);
    
    connect(m_layout, &ReordarableVLayout::startedDragging, this, [this](DraggableFrame* wgt)
            {
                m_currentDraggable = wgt;
                for(auto wgt : m_layout->GetDraggableWidgets())
                    if(auto row = qobject_cast<FilterPresetRow*>(wgt))
                        row->setMyHover(false);
            });
    connect(m_layout, &ReordarableVLayout::endedDragging, this, [this](DraggableFrame* wgt, bool canceled)
            {
                m_currentDraggable = nullptr;
            });
    connect(m_layout, &ReordarableVLayout::orderChanged, this, [this]()
            {
                auto wgts = m_layout->GetDraggableWidgets();
                m_itemsOrdered.clear();
                int num = 1;
                for(auto wgt : wgts)
                {
                    if(auto frow = qobject_cast<FilterPresetRow*>(wgt))
                    {
                        m_itemsOrdered.push_back({m_Wgts[frow], frow});
                        frow->SetPrefix(QString("%1. ").arg(num++));
                    }
                }
                
                QSignalBlocker blocker{m_root};
                if(m_root)
                m_root->Tree([this](IFilterPresetItem* item)
                             {
                                 if(auto group = dynamic_cast<FilterPresetGroup*>(item))
                                 {
                                     auto children = group->Children().toSet();
                                     group->Clear(false);
                                     for(auto item : m_itemsOrdered)
                                     {
                                         if(children.contains(item.first))
                                             group->AddPreset(item.first);
                                     }
                                 }
                             });
                emit FilterPresetsOrderChanged();
            });
}

void FilterPresetListWidget::SetModel(IFilterPresetItem* rootItem)
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
                                   auto rowWidget = new FilterPresetRow(this);
                                   rowWidget->SetPreviewOpacity(0.9);
                                   rowWidget->SetPreviewBorder(false);
                                   m_layout->AddDraggableWidget(rowWidget);
                                   rowWidget->SetPrefix(QString("%1. ").arg(num++));
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
                                   
                                   connect(rowWidget, &FilterPresetRow::nameEditRequested, this, [this, preset](QString oldName, QString newName)
                                           {
                                               emit FilterPresetEditRequested(preset, oldName, newName);
                                           });
                                   connect(rowWidget, &FilterPresetRow::deleteClicked, this, [this, preset]()
                                           {
                                               emit FilterPresetDeleteRequested(preset);
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
