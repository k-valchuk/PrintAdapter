#include "QFCategoryBox.h"
#include "qscrollbar.h"
#include "qstandarditemmodel.h"
#include "ui_QFCategoryBox.h"

#include "QFSpoiler.h"

int QFCategoryBox::treeViewContentHeight()
{
    int height = 0;
    auto index = m_model->index(0, 0);
    while(index.isValid())
    {
        height += ui->treeView->visualRect(index).height();
        index = ui->treeView->indexBelow(index);
    }
    
    return height + ui->treeView->contentsMargins().bottom() + ui->treeView->contentsMargins().top();
}

bool QFCategoryBox::Filter(QString text)
{
    auto filter = ui->lineEdit->text().trimmed().toLower();
    if(text.trimmed().toLower().contains(filter))
        return true;
    
    return false;
}

void QFCategoryBox::updateList()
{
    auto vbar = ui->treeView->verticalScrollBar();
    auto scrollVal = vbar->value();
    
    m_model->clear();
    m_model->setColumnCount(2);
    for(auto item : m_items)
    {
        auto stItems = item->GetItems();
        if(Filter(stItems.first()->text()))
            m_model->appendRow(stItems);
    }
    
    ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeView->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->treeView->header()->setStretchLastSection(false);
    ui->treeView->resizeColumnToContents(1);
    
    ui->treeView->setFixedHeight(qMin(treeViewContentHeight(), 240));
    m_spoiler->RecalcSize();
    
    scrollVal = qMin(scrollVal, vbar->maximum());
    vbar->setValue(scrollVal);
}

void QFCategoryBox::updateName()
{
    int counter = 0;
    for(auto child : m_cat->Children())
        if(auto other = dynamic_cast<QuickFilterPreset*>(child))
            if(other->IsChecked())
                counter++;
    
    if(counter > 0)
        m_spoiler->setTitle(QString("%1 %2").arg(m_cat->GetName()).arg(tr("(%1 selected)").arg(counter)));
    else
        m_spoiler->setTitle(m_cat->GetName());
}

QFCategoryBox::QFCategoryBox(bool draggingEnabled, QWidget *parent) :
    DraggableFrame(draggingEnabled, parent),
    ui(new Ui::QFCategoryBox)
{
    ui->setupUi(this);
    m_spoiler = new QFSpoiler("", 150, this);
    m_spoiler->HideSomeLayoutUnderSpoiler(ui->verticalLayout);
    this->layout()->setContentsMargins(0, 0, 0, 5);
    
    m_model = new QStandardItemModel();
    m_model->setColumnCount(2);
    ui->treeView->header()->hide();
    ui->treeView->setModel(m_model);
    ui->treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(m_spoiler, &QFSpoiler::aboutToBeExpanded, this, [this](){
            ui->treeView->setFixedHeight(qMin(treeViewContentHeight(), 240));
    }, Qt::DirectConnection);
    
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this](const QString& str)
            {
                updateList();
            });
    
    connect(ui->treeView, &QTreeView::clicked, this, [this](const QModelIndex& index)
            {
                if(!index.isValid())
                    return;
                
                auto item = m_model->itemFromIndex(index);
                if(!item)
                    return;
                
                auto node = item->data().value<IQuickFilterItem*>();
                if(!node)
                    return;
                
                if(auto filterNode = dynamic_cast<QuickFilterPreset*>(node))
                {
                    {//block signals to update only once
                    auto blocker = m_cat->blockTreeChangedSignal();
                    auto checked = !filterNode->IsChecked();
                    filterNode->SetChecked(checked);
                    
                    if(m_cat->IsSingleSelectionOnly() && checked)
                        for(auto child : m_cat->Children())
                            if(auto other = dynamic_cast<QuickFilterPreset*>(child))
                                if(other != filterNode)
                                    other->SetChecked(false);
                    }
                    m_cat->emitTreeChanged();
                    
                    updateName();
                }
            });
    
    connect(ui->showBtn, &QPushButton::clicked, this, [this](){
        emit this->ShowMoreOrHideBtnClicked();
    });
}

QFCategoryBox::~QFCategoryBox()
{
    delete ui;
    delete m_spoiler;
}

void QFCategoryBox::SetNode(QuickFilterCategory* cat)
{
    m_cat = cat;
    
    //m_spoiler->setTitle(cat->GetName());
    updateName();
    
    m_items.clear();
    
    auto children = cat->Children();
    for(auto child : children)
    {
        if(auto node = dynamic_cast<QuickFilterPreset*>(child))
            m_items.append(node);
    }
    
    if(cat->SearchBarNeeded())
        ui->lineEdit->setVisible(true);
    else
    {
        ui->lineEdit->setText(QString());
        ui->lineEdit->setVisible(false);
    }
    
    if(cat->ShowMoreButtonNeeded())
        ui->showBtn->setVisible(true);
    else
        ui->showBtn->setVisible(false);
    
    if(!cat->AllLoaded())
        ui->showBtn->setText(tr("Show all"));
    else
        ui->showBtn->setText(tr("Hide"));
    
    updateList();
}

void QFCategoryBox::ClearSelection()
{
    if(!m_cat)
        return;
    
    for(auto child : m_cat->Children())
        if(auto node = dynamic_cast<QuickFilterPreset*>(child))
            node->SetChecked(false);
}

void QFCategoryBox::Expand(bool expand)
{
    m_spoiler->expand(expand, false);
}
