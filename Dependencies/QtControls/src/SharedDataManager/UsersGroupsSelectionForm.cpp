#include "UsersGroupsSelectionForm.h"
#include "TreeGroupingStyleItemDelegate.h"
#include "ui_UsersGroupsSelectionForm.h"

#include <QStandardItemModel>


UsersGroupsSelectionForm::UsersGroupsSelectionForm(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::UsersGroupsSelectionForm),
    m_treeView({tr("ID"), tr("Name")}, false, new TreeGroupingStyleItemDelegate(true))
{
    qRegisterMetaType<QPair<int, quint64>>();
    ui->setupUi(this);
    ui->verticalLayout->insertWidget(1, &m_treeView);
    
    ((QSortFilterProxyModel*)m_treeView.model())->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ((QSortFilterProxyModel*)m_treeView.model())->setRecursiveFilteringEnabled(true);
    ((QSortFilterProxyModel*)m_treeView.model())->setFilterKeyColumn(1);
    
    //m_treeView.setAlternatingRowColors(true);
    m_treeView.header()->setObjectName("coloredHeader");
    m_treeView.header()->setSectionsMovable(false);
    m_treeView.header()->setStretchLastSection(true);
    
    m_treeView.setDragEnabled(false);
    m_treeView.setIndentation(0);
    m_treeView.header()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
    m_treeView.setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
    m_treeView.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    
    m_treeView.sortByColumn(0, Qt::AscendingOrder);
    m_treeView.setFirstColumnSelectChekboxesEnabled(true, false);
    m_treeView.setInvisibleSelection(true);
    
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this](const QString& str){
        m_searchStr = str;
        m_treeView.setFilterRegExp(m_searchStr);
        m_treeView.expandAll();
        m_treeView.setSortingEnabled(false);m_treeView.setSortingEnabled(true);
    });
    connect(m_treeView.selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](){
        if(m_inUpdateList)
            return;
        m_checkedUsers.clear();
        m_checkedGroups.clear();
        
        auto selRows = m_treeView.selectedRows();
        for(const auto& row : selRows)
        {
            auto typeAndId = row.data(Qt::UserRole).value<QPair<int, quint64>>();
            auto type = typeAndId.first;
            auto id = typeAndId.second;
            
            if(type == 1){
                m_checkedUsers.insert(id);
            }
            else if(type == 2){
                m_checkedGroups.insert(id);
            }
        }
    });
}

void UsersGroupsSelectionForm::updateList()
{
    m_inUpdateList = true;
    m_treeView.clear();
    auto filterRegExp = m_treeView.getFilterRegExp();
    m_treeView.setFilterRegExp("");
    m_treeView.setSorting(false);
    
    QModelIndexList indexesToSelect;
    QModelIndexList indexesToDisable;
    
    if(m_users.size() > 0)
    {
        QVector<QVariant> data{tr("Users:")};
        m_treeView.insertRow(m_treeView.rowCount(QModelIndex()) - 1, QModelIndex(), &data);
        m_treeView.setFirstColumnSpanned(m_treeView.rowCount(QModelIndex()) - 1, QModelIndex(), true);
        auto parentIndex = m_treeView.index(m_treeView.rowCount(QModelIndex()) - 1, 0, QModelIndex());
        m_treeView.setData(parentIndex, QVariant(TreeItem::defaultFlags().setFlag(Qt::ItemIsSelectable, false)), TreeItem::FlagsRole);
        
        for(const auto& user : m_users)
        {
            data = {user.Id, user.Name};
            m_treeView.insertRow(-1, parentIndex, &data);
            auto rowIndex = m_treeView.index(m_treeView.rowCount(parentIndex) - 1, 0, parentIndex);
            m_treeView.setData(rowIndex, QVariant::fromValue(QPair<int, quint64>{1, user.Id}), Qt::UserRole);
            if(m_checkedUsers.contains(user.Id))
                indexesToSelect.append(rowIndex);
            if(m_disabledUsers.contains(user.Id))
                indexesToDisable.append(rowIndex);
        }
        
        m_treeView.expand(parentIndex);
    }
    if(m_groups.size() > 0)
    {
        QVector<QVariant> data{tr("Groups:")};
        m_treeView.insertRow(m_treeView.rowCount(QModelIndex()) - 1, QModelIndex(), &data);
        m_treeView.setFirstColumnSpanned(m_treeView.rowCount(QModelIndex()) - 1, QModelIndex(), true);
        auto parentIndex = m_treeView.index(m_treeView.rowCount(QModelIndex()) - 1, 0, QModelIndex());
        m_treeView.setData(parentIndex, QVariant(TreeItem::defaultFlags().setFlag(Qt::ItemIsSelectable, false)), TreeItem::FlagsRole);
        
        for(const auto& group : m_groups)
        {
            data = {group.Id, group.Name};
            m_treeView.insertRow(-1, parentIndex, &data);
            auto rowIndex = m_treeView.index(m_treeView.rowCount(parentIndex) - 1, 0, parentIndex);
            m_treeView.setData(rowIndex, QVariant::fromValue(QPair<int, quint64>{2, group.Id}), Qt::UserRole);
            if(m_checkedUsers.contains(group.Id))
                indexesToSelect.append(rowIndex);
            if(m_disabledUsers.contains(group.Id))
                indexesToDisable.append(rowIndex);
        }
        
        m_treeView.expand(parentIndex);
    }
    
    m_treeView.setFilterRegExp(filterRegExp);
    m_treeView.setSorting(true);
    
    for(const auto& index : indexesToSelect)
        m_treeView.selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    for(const auto& index : indexesToDisable)
    {
        m_treeView.setData(index, QVariant(TreeItem::defaultFlags().setFlag(Qt::ItemIsSelectable, false)), TreeItem::FlagsRole);
        m_treeView.setIndexRowStyle(index, QColor(), qApp->palette().color(QPalette::ColorRole::Midlight), QColor());
    }
    
    m_inUpdateList = false;
}

UsersGroupsSelectionForm::~UsersGroupsSelectionForm()
{
    delete ui;
}
