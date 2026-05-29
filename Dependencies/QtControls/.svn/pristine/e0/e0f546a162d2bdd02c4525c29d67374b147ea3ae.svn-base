#include "FiltersList.h"
#include "ui_FiltersList.h"
#include <QDebug>
#include "FilterRow.h"

void FiltersList::updateList()
{
    m_list.clear();
    QList<QPair<QString, std::function<void()>>> actions;
    for(auto filter : m_parentRow->GetOrderedFilters())
    {
        if(m_parentRow->IsOnlyFavouritesList() && !m_parentRow->GetFavouriteFilters()[filter->GetElementId()])
            continue;
        
        if(!m_searchStr.isEmpty() && !filter->GetName().toLower().contains(m_searchStr.toLower().trimmed()))
            continue;
        
        auto item = new QStandardItem();
        item->setCheckable(true);
        item->setCheckState(m_parentRow->GetFavouriteFilters()[filter->GetElementId()] ? Qt::Checked : Qt::Unchecked);
        item->setTristate(false);
        item->setText(filter->GetName());
        item->setData(filter->GetElementId());
        item->setEditable(false);
        
        m_list.appendRow(item);
    }
}

FiltersList::FiltersList(FilterRow* parentRow, QWidget *parent) :
    QFrame(nullptr),
    m_list(0, 1),
    ui(new Ui::FiltersList)
{
    ui->setupUi(this);
    
    m_parentRow = parentRow;
    ui->pushButton->setText(m_parentRow->IsOnlyFavouritesList() ? tr("Show all") : tr("Hide"));
    ui->pushButton->setCheckable(true);
    ui->pushButton->setChecked(m_parentRow->IsOnlyFavouritesList());
    
    updateList();
    
    ui->listView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->listView->setModel(&m_list);
    
    connect(&m_list, &QStandardItemModel::itemChanged, this, [this](QStandardItem* item)
            {
                if(item->checkState() == Qt::Checked)
                    m_parentRow->SetFavourite(item->data(), true);
                else
                    m_parentRow->SetFavourite(item->data(), false);
                
                if(m_parentRow->IsOnlyFavouritesList())
                    updateList();
                
                supressClick = true;
            });
    
    connect(ui->listView, &QAbstractItemView::clicked, this, [this](QModelIndex index)
            {
                if(supressClick)
                {
                    supressClick = false;
                    return;
                }
                
                if(!index.isValid())
                    return;
                
                auto item = m_list.itemFromIndex(index);
                if(!item)
                    return;
                
                auto data = item->data();
                this->hide();
                ui->lineEdit->clear(); m_searchStr.clear();
                
                updateList();
                emit this->filterSelected(data);
            });
    
    connect(ui->pushButton, &QPushButton::toggled, this, [this](bool isFavourites)
            {
                ui->pushButton->setText(isFavourites ? tr("Show all") : tr("Hide"));
                m_parentRow->SetFilterListMode(isFavourites);
                updateList();
            });
    
    connect(ui->lineEdit, &QLineEdit::textEdited, this, [this](QString str)
            {
                m_searchStr = str;
                updateList();
            });
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, [this]()
            {
                if(m_list.rowCount() == 1)
                {
                    auto item = m_list.itemFromIndex(m_list.index(0, 0));
                    auto data = item->data();
                    this->hide();
                    ui->lineEdit->clear(); m_searchStr.clear();
                    updateList();
                    emit this->filterSelected(data);
                }
                else if(m_list.rowCount() > 1)
                {
                    QList<QVariant> keys;
                    for(int i = 0; i < m_list.rowCount(); i++)
                    {
                        auto item = m_list.itemFromIndex(m_list.index(i, 0));
                        auto key = item->data();
                        auto name = m_parentRow->GetFilters()[key]->GetName();
                        if(name.toLower().trimmed() == ui->lineEdit->text().toLower().trimmed())
                            keys.append(key);
                    }
                    
                    if(keys.size() == 1)
                    {
                        this->hide();
                        ui->lineEdit->clear(); m_searchStr.clear();
                        updateList();
                        emit this->filterSelected(keys.first());
                    }
                }
            });
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->lineEdit, &FiltersListLineEdit::moveToList, this, [this](bool down)
            {
                if(m_list.rowCount() > 0)
                {
                    auto indexes = ui->listView->selectionModel()->selectedIndexes();
                    if(down || (indexes.size() > 0 && indexes.first().row() > 0))
                        ui->listView->setFocus();
                    if(indexes.isEmpty())
                        ui->listView->selectionModel()->setCurrentIndex(m_list.index(0, 0), QItemSelectionModel::SelectionFlag::ClearAndSelect);
                }
            });
    connect(ui->listView, &FiltersListListView::moveToEdit, this, [this]()
            {
                ui->lineEdit->setFocus();
            });
}

FiltersList::~FiltersList()
{
    delete ui;
}

void FiltersList::SetFocusToSearch()
{
    ui->lineEdit->setFocus();
}

void FiltersList::showEvent(QShowEvent* event)
{
    updateList();
    ui->pushButton->setText(m_parentRow->IsOnlyFavouritesList() ? tr("Show all") : tr("Hide"));
}
