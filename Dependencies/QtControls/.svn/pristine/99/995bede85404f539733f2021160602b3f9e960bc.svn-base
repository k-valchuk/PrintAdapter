#include "QuickFilterPanel.h"
#include "qdatetimeedit.h"
#include "qstyleditemdelegate.h"
#include "ui_QuickFilterPanel.h"
#include <QMessageBox>

#include "QFCategoryBox.h"
#include "ReordarableVLayout.h"
#include "QPointer"

QuickFilterPanel::QuickFilterPanel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::QuickFilterPanel),
    m_favRoot(),
    m_presetsRoot("", nullptr),
    m_historyRoot("", nullptr)
{
    ui->setupUi(this);
     
    m_quickFilterVLayout = new ReordarableVLayout(ui->frame_4);
    ui->frame_4->setLayout(m_quickFilterVLayout);
    m_quickFilterVLayout->setContentsMargins(0, 0, 0, 0);
    m_quickFilterVLayout->setSpacing(0);
    
    connect(ui->frame, &QuickFilterTabButtons::sig_page1, this, [this]()
            {
                ui->stackedWidget->setCurrentIndex(0);
            });
    connect(ui->frame, &QuickFilterTabButtons::sig_page2, this, [this]()
            {
                ui->stackedWidget->setCurrentIndex(1);
            });
    connect(ui->frame, &QuickFilterTabButtons::sig_page3, this, [this]()
            {
                ui->stackedWidget->setCurrentIndex(2);
            });
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &QuickFilterPanel::onPageChanged);
    onPageChanged(ui->stackedWidget->currentIndex());
    
    connect(ui->hideBtn, &QPushButton::clicked, this, [this]()
            {
                this->hide();
            });
    connect(ui->savePresetBtn, &QPushButton::clicked, this, [this]()
            {
                emit SavePresetClicked(nullptr);
            });
    
    connect(&m_favRoot, &IQuickFilterItem::treeChanged, this, [this](IQuickFilterItem* parent, bool supressFilterUpdate)
            {
                //чтобы не мерцало пока все перестраивается, в конце show()
                //и focusedWgt->setFocus() чтобы не слетал куда-то фокус из-за hide()
                QPointer<QWidget> focusedWgt = qApp->focusWidget();
                ui->scrollAreaWidgetContents_2->hide();
                
                if(parent == nullptr || parent == &m_favRoot)
                {
                    auto cats = m_favRoot.AllChildCategoriesRecursive();
                    //delete non present categories
                    for(auto cat : m_catToExpander.keys())
                    {
                        if(!cats.contains(cat))
                        {
                            (m_quickFilterVLayout)->RemoveDraggableWidget(m_catToExpander[cat]);
                            m_catToExpander.remove(cat);
                        }
                    }
                    for(auto cat : cats)
                    {
                        //if category is present just update QFCatBox
                        if(m_catToExpander.contains(cat))
                        {
                            m_catToExpander[cat]->SetNode(cat);
                        }
                        else //if category is not present insert QFCatBox in pos of cats.indexOf(cat)
                        {
                            auto expander = new QFCategoryBox(false);
                            int lastPosInLayout = m_quickFilterVLayout->count();
                            auto index = qMin(cats.indexOf(cat), lastPosInLayout);
                            if(index < 0) index = lastPosInLayout;
                            m_quickFilterVLayout->InsertDraggableWidget(index, expander);
                            
                            expander->SetNode(cat);
                            connect(expander, &QFCategoryBox::ShowMoreOrHideBtnClicked, this, [this, expander]()
                                    {
                                        emit this->QFCategoryShowOrHideButtonPressed(expander->GetNode());
                                    });
                            m_catToExpander.insert(cat, expander);
                            expander->Expand(true);
                        }
                    }
                }
                else if(auto cat = dynamic_cast<QuickFilterCategory*>(parent))
                {
                    if(m_catToExpander.contains(cat))
                    {
                        auto expander = m_catToExpander[cat];
                        expander->SetNode(cat);
                    }
                }
                else if(auto filter = dynamic_cast<QuickFilterPreset*>(parent))
                {
                    if(auto parent = filter->Parent())
                        if(auto parentCat = dynamic_cast<QuickFilterCategory*>(parent))
                            if(m_catToExpander.contains(parentCat))
                            {
                                auto expander = m_catToExpander[parentCat];
                                expander->SetNode(parentCat);
                            }
                }
                
                onItemChanged(supressFilterUpdate);
                
                ui->scrollAreaWidgetContents_2->show();
                if(focusedWgt) focusedWgt->setFocus();
            });
    connect(ui->clearFavSelection, &QPushButton::clicked, this, [this](){
        auto cats = m_favRoot.AllChildCategoriesRecursive();
        for(auto cat : cats)
        {
            if(!cat) continue;
            
            if(m_catToExpander.contains(cat))
            {
                auto expander = m_catToExpander[cat];
                expander->ClearSelection();
            }
        }
    });
    
    //-------------presets page
    this->ui->presetsFrame->SetModel(&m_presetsRoot);
    
    connect(this->ui->presetsFrame, &FilterPresetListWidget::FilterPresetDeleteRequested, this, [this](FilterPreset* item)
            {
                emit FilterPresetDeleteRequested(item);
            });
    connect(this->ui->presetsFrame, &FilterPresetListWidget::FilterPresetApplyRequested, this, [this](FilterPreset* item)
            {
                emit FilterPresetApplied(item->Expression());
            });
    connect(this->ui->presetsFrame, &FilterPresetListWidget::FilterPresetsOrderChanged, this, [this]()
            {
                emit FilterPresetsOrderChanged();
            });
    connect(this->ui->presetsFrame, &FilterPresetListWidget::FilterPresetEditRequested, this, [this](FilterPreset* item, QString oldName, QString newName)
            {
                emit FilterPresetEdited(item, newName, item->Expression());
            });
    
    
    //----------------history page
    this->ui->historyFrame->SetModel(&m_historyRoot);
    connect(this->ui->historyFrame, &FilterHistoryListWidget::FilterPresetApplyRequested, this, [this](FilterPreset* item)
            {
                emit FilterPresetApplied(item->Expression());
            });
    connect(this->ui->historyFrame, &FilterHistoryListWidget::FilterPresetSaveRequested, this, [this](FilterPreset* item)
            {
                emit SavePresetClicked(item->Expression());
            });
}

QuickFilterPanel::~QuickFilterPanel()
{
    delete ui;
}

void QuickFilterPanel::SetOtherQuickFilterTabWgt(QuickFilterTabButtons* tabWgt)
{
    m_tabWgt = tabWgt;
    m_tabWgt->SetSelected(-1);
    m_tabWgt->SetSeletedFavoutitesCount(m_selectedFavouritesCount);
    if(this->isVisible())
        m_tabWgt->hide();
    else
        m_tabWgt->show();
    
    connect(tabWgt, &QuickFilterTabButtons::sig_page1, this, [this]()
            {
                this->show();
                ui->stackedWidget->setCurrentIndex(0);
                ui->stackedWidget->setFocus();
            });
    connect(tabWgt, &QuickFilterTabButtons::sig_page2, this, [this]()
            {
                this->show();
                ui->stackedWidget->setCurrentIndex(1);
                ui->stackedWidget->setFocus();
            });
    connect(tabWgt, &QuickFilterTabButtons::sig_page3, this, [this]()
            {
                this->show();
                ui->stackedWidget->setCurrentIndex(2);
                ui->stackedWidget->setFocus();
            });
}

void QuickFilterPanel::SetVisibility(bool quick, bool presets, bool history)
{
    ui->stackedWidget->widget(0)->setVisible(quick);
    ui->stackedWidget->widget(1)->setVisible(presets);
    ui->stackedWidget->widget(2)->setVisible(history);
    
    ui->frame->SetVisibility(quick, presets, history);
    m_tabWgt->SetVisibility(quick, presets, history);
}

void QuickFilterPanel::showEvent(QShowEvent* event)
{
    if(m_tabWgt)
        m_tabWgt->hide();
    
    if(m_splitter && m_wasCollapsed)
    {
        m_wasCollapsed = false;
        
        auto indexOfThis = m_splitter->indexOf(this);
        m_splitter->setCollapsible(m_splitter->indexOf(this), false);
        m_splitter->blockSignals(true);
        auto size = m_splitter->sizes().size();
        if(indexOfThis + 1 < size && indexOfThis + 1 >= 0)
            m_splitter->MoveSplitter(m_lastNonZeroSplitterPosR, indexOfThis + 1);
        if(indexOfThis < size && indexOfThis >= 0)
            m_splitter->MoveSplitter(m_lastNonZeroSplitterPosL, indexOfThis);
        
        m_splitter->blockSignals(false);
        m_splitter->setCollapsible(m_splitter->indexOf(this), true);
    }
    
    if(!m_connectedToSplitter)
    {
        m_connectedToSplitter = true;
        
        QWidget* parentWgt = this->parentWidget();
        while(parentWgt)
        {
            if(auto splitter = qobject_cast<QuickFilterPanelSplitter*>(parentWgt))
            {
                m_splitter = splitter;
                splitter->setSizes({1, 25000});
                connect(splitter, &QSplitter::splitterMoved, this, [this, splitter](int pos, int index)
                        {
                            auto thisIdx = splitter->indexOf(this);
                            
                            auto size = splitter->sizes()[thisIdx];
                            
                            if(size == 0)
                            {
                                m_wasCollapsed = true;
                                this->hide();
                            }
                            else
                            {
                                m_lastNonZeroSplitterPosL = splitter->handle(thisIdx)->pos().x();
                                m_lastNonZeroSplitterPosR = splitter->handle(thisIdx + 1)->pos().x();
                            }
                        });
                break;
            }
            parentWgt = parentWgt->parentWidget();
        }
    }
}

QSharedPointer<IExpression> QuickFilterPanel::GetFilterFormedOnlyFromCategories(bool all, const QSet<QuickFilterCategory*>& parentCategories)
{
    QHash<QuickFilterCategory*, QSet<QuickFilterPreset*>> selectedNodes;
    
    auto nodes = m_favRoot.AllFilterNodes();
    for(auto node : nodes)
        if(node->IsChecked())
            if(auto cat = dynamic_cast<QuickFilterCategory*>(node->Parent()))
                if(all || parentCategories.contains(cat))
                    selectedNodes[cat].insert(node);
    
    QList<IExpression*> expressions;
    
    for(auto cat : selectedNodes.keys())
    {
        auto filters = selectedNodes[cat];
        QList<IExpression*> exps;
        for(auto filter : filters)
            exps.append(filter->CreateExpression());
        
        if(exps.size() == 0)
            continue;
        
        bool orOp = cat ? cat->IsOrOperator() : false;
        
        IExpression* exp = exps.first();
        for(int i = 1; i < exps.size(); i++)
            if(orOp)
                exp = Concat<OrOperator>(exp, exps[i]);
            else
                exp = Concat<AndOperator>(exp, exps[i]);
        
        if(orOp) //можно убрать добавление скобок, порядок и так определен
        {        //нужны только если потребуется восстанавливать в облачка
            auto brackets = new BracketsExpression(nullptr);
            brackets->SetChild(exp);
            exp = brackets;
        }
        
        expressions.append(exp);
    }
    
    if(expressions.size() == 0)
        return nullptr;
    else
    {
        IExpression* exp = expressions.first();
        for(int i = 1; i < expressions.size(); i++)
            exp = Concat<AndOperator>(exp, expressions[i]);
        return QSharedPointer<IExpression>(exp);
    }
}

QSharedPointer<IExpression> QuickFilterPanel::GetFilterFormedOnlyFromCategories(const QSet<QuickFilterCategory*>& parentCategories)
{
    return GetFilterFormedOnlyFromCategories(false, parentCategories);
}

void QuickFilterPanel::onItemChanged(bool supressFilterUpdate)
{
    auto counter = 0;
    auto nodes = m_favRoot.AllFilterNodes();
    for(auto node : nodes)
        if(node->IsChecked())
            counter++;
    
    if(!supressFilterUpdate)
    {
        auto filter = GetFilterFormedOnlyFromCategories();
        auto filterJson = filter ? filter->ToJson() : QJsonObject();
        if(filterJson != m_quickFilterCache)
        {
            m_quickFilterCache = filterJson;
            emit this->QuickFilterChanged(filter);
        }
    }
    
    m_selectedFavouritesCount = counter;
    ui->favSelectionCounter->setText(tr("%1 filters selected").arg(counter));
    ui->frame->SetSeletedFavoutitesCount(counter);
    if(m_tabWgt)
        m_tabWgt->SetSeletedFavoutitesCount(counter);
}

void QuickFilterPanel::onPageChanged(int i)
{
    ui->frame->SetSelected(i);
}

void QuickFilterPanel::hideEvent(QHideEvent* event)
{
    if(m_tabWgt)
        m_tabWgt->show();
}
