#include "GroupDataForm.h"
#include "qmessagebox.h"
#include "ui_GroupDataForm.h"

#include <QStandardItemModel>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include "TreeGroupingStyleItemDelegate.h"

void GroupDataForm::updatelabels()
{
    auto strings = labels();
    ui->objectSettingsLabel->setText(strings.objectSettingsLabel);
    ui->groupBox->setTitle(strings.groupBoxNew);
    ui->radioBtnCurrent->setText(strings.radiobtnCurrent);
    ui->radioBtnSelect->setText(strings.radiobtnSelect);
    ui->thirdNameLabel->setText(strings.nameLabel);
    ui->fourthNameLabel->setText(strings.nameLabel);
}

GroupDataForm::GroupDataForm(SharedDataVM* vm, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::GroupDataForm),
    m_otherView({tr("ID"), tr("Name")}, false, new TreeGroupingStyleItemDelegate(true))
{
    ui->setupUi(this);
    
    ui->verticalLayout_9->insertWidget(1, &m_otherView);
    
    if(ui->stackedWidget->currentIndex() != 0) {ui->stackedWidget->setCurrentIndex(0);}
    
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    
    m_model = new SharedDataTableModel({1});
    ui->tableView->horizontalHeader()->setObjectName("coloredHeader");
    ui->tableView->setModel(m_model);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
    ui->tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    
    ui->tableView->setItemDelegateForColumn(1, new SharedObjectNameEditDelegate(this, this));
    ui->tableView->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
    
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        auto indexes = ui->tableView->selectionModel()->selectedRows(0);
        
        if(indexes.size() == 1)
        {
            auto item = m_model->ItemByIndex(indexes.first());
            
            QList<User> users;
            QList<Group> groups;
            for(const auto& user : m_users)
                if(item.Owners.UserIds.contains(user.Id))
                    users.append(user);
            for(const auto& group : m_groups)
                if(item.Owners.GroupIds.contains(group.Id))
                    groups.append(group);
            ui->ownersList->setUsersGroups(users, groups);
        }
    });
    
    m_vm = vm;
    
    connect(m_vm, &SharedDataVM::itemsAdded, this, &GroupDataForm::onItemsAdded, Qt::QueuedConnection);
    connect(m_vm, &SharedDataVM::itemsRemoved, this, &GroupDataForm::onItemsRemoved, Qt::QueuedConnection);
    connect(m_vm, &SharedDataVM::listCleared, this, &GroupDataForm::onListCleared, Qt::QueuedConnection);
    connect(m_vm, &SharedDataVM::itemModified, this, &GroupDataForm::onItemModified, Qt::QueuedConnection);
    connect(m_vm, &SharedDataVM::usersAndGroupsListChanged, this, &GroupDataForm::onUsersGroupsListChanged, Qt::QueuedConnection);
    connect(m_vm, &SharedDataVM::otherListChanged, this, &GroupDataForm::onOtherListChanged, Qt::QueuedConnection);
    
    connect(ui->close, &QPushButton::clicked, this, &GroupDataForm::close);
    connect(ui->removeOwners, &QPushButton::clicked, this, [this]()
            {
                auto indexes = ui->tableView->selectionModel()->selectedRows(0);
                
                if(indexes.size() != 1)
                    return;
                
                auto item = m_model->ItemByIndex(indexes.first());
                
                QSet<quint64> usersToRemove;
                QSet<quint64> groupsToRemove;
                
                for(const auto& user : ui->ownersList->GetSelectedUsers())
                    usersToRemove.insert(user.Id);
                for(const auto& group : ui->ownersList->GetSelectedGroups())
                    groupsToRemove.insert(group.Id);
                
                if(usersToRemove.size() + groupsToRemove.size() == 0)
                    return;
                
                QMessageBox msgBox;
                msgBox.setWindowTitle(QApplication::tr("Remove owners"));
                msgBox.setText(QApplication::tr("Are you sure to remove %1 owners from \"%2\"?")
                                   .arg(usersToRemove.size() + groupsToRemove.size()).arg(item.Name));
                msgBox.addButton(QApplication::tr("Yes"), QMessageBox::YesRole);
                msgBox.addButton(QApplication::tr("No"), QMessageBox::NoRole);
                auto dialogResult = msgBox.exec();
                
                if(dialogResult != 0)
                    return;
                
                for(auto id : usersToRemove)
                    item.Owners.UserIds.remove(id);
                for(auto id : groupsToRemove)
                    item.Owners.GroupIds.remove(id);
                
                m_vm->ModifyItem(item);
            });
    connect(ui->addOwners, &QPushButton::clicked, this, [this]()
            {
                auto indexes = ui->tableView->selectionModel()->selectedRows(0);
                
                if(indexes.size() != 1)
                    return;
                
                auto item = m_model->ItemByIndex(indexes.first());
                ui->ownersSelector->clearSelectionAndEnable();
                ui->ownersSelector->setCheckedItems(item.Owners.UserIds, item.Owners.GroupIds);
                ui->ownersSelector->disableItems(item.Owners.UserIds, item.Owners.GroupIds);
                
                ui->stackedWidget->setCurrentIndex(4);
            });
    connect(ui->cancelAddOwnersBtn, &QPushButton::clicked, this, [this]()
            {
                ui->stackedWidget->setCurrentIndex(0);
            });
    connect(ui->confiermAddOwnersBtn, &QPushButton::clicked, this, [this]()
            {
                auto users = ui->ownersSelector->GetSelectedUsers();
                auto groups = ui->ownersSelector->GetSelectedGroups();
                
                auto indexes = ui->tableView->selectionModel()->selectedRows(0);
                if(indexes.size() == 1)
                {
                    auto item = m_model->ItemByIndex(indexes.first());
                    for(auto user : users)
                        item.Owners.UserIds.insert(user.Id);
                    for(auto group : groups)
                        item.Owners.GroupIds.insert(group.Id);
                    m_vm->ModifyItem(item);
                }
                
                ui->stackedWidget->setCurrentIndex(0);
            });
    
    connect(ui->removeItem, &QPushButton::clicked, this, [this](){
        auto indexes = ui->tableView->selectionModel()->selectedRows(0);
        QList<quint64> ids;
        for(auto index : indexes)
            ids.append(m_model->ItemByIndex(index).Id);
        
        if(ids.size() == 1)
        {
            auto item = m_model->ItemByIndex(indexes.first());
            QMessageBox msgBox;
            msgBox.setWindowTitle(QApplication::tr("Remove view"));
            msgBox.setText(QApplication::tr("Are you sure to remove view \"%1\"?").arg(item.Name));
            msgBox.addButton(QApplication::tr("Yes"), QMessageBox::YesRole);
            msgBox.addButton(QApplication::tr("No"), QMessageBox::NoRole);
            auto dialogResult = msgBox.exec();
            
            if(dialogResult == 0)
                m_vm->RemoveItems(ids);
        }
    });
    
    connect(ui->addItem, &QPushButton::clicked, this, [this](){
        m_idToOverwrite = 0;
        ui->thirdSave->setText(tr("Save"));
        ui->fourthSave->setText(tr("Save"));
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->overwriteItem, &QPushButton::clicked, this, [this](){
        auto indexes = ui->tableView->selectionModel()->selectedRows(0);
        if(indexes.size() == 1)
        {
            ui->thirdSave->setText(tr("Overwrite"));
            ui->fourthSave->setText(tr("Overwrite"));
            m_idToOverwrite = m_model->ItemByIndex(indexes.first()).Id;
            ui->stackedWidget->setCurrentIndex(1);
        }
    });
    connect(ui->secondBack, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->secondNext, &QPushButton::clicked, this, [this](){
        if(ui->radioBtnCurrent->isChecked())
        {
            auto item = m_model->ItemByRow(m_model->GetRowById(m_idToOverwrite));
            QString name = tr("Новый вид");
            if(m_idToOverwrite != 0)
                name = m_model->ItemByRow(m_model->GetRowById(m_idToOverwrite)).Name;
            ui->thirdNameEdit->setText(name);
            ui->stackedWidget->setCurrentIndex(2);
        }
        else
            ui->stackedWidget->setCurrentIndex(3);
    });
    connect(ui->thirdBack, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentIndex(1);
    });
    
    //ui->otherView->setAlternatingRowColors(true);
    m_otherView.header()->setObjectName("coloredHeader");
    m_otherView.header()->setSectionsMovable(true);
    m_otherView.header()->setStretchLastSection(true);
    m_otherView.header()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
    m_otherView.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    m_otherView.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    m_otherView.setFirstColumnSelectChekboxesEnabled(true, false);
    m_otherView.setInvisibleSelection(true);
    m_otherView.setIndentation(0);
    m_otherView.setDragEnabled(false);
    
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](){
        auto selectedUserId = ui->comboBox->currentData().toULongLong();
        
        m_otherView.clear();
        
        bool first = true;
        for(auto oItem : m_otherList)
        {
            if(!oItem.Owners.UserIds.contains(selectedUserId))
                continue;
            
            QVector<QVariant> data = {oItem.Id, oItem.Name};
            m_otherView.insertRow(-1, QModelIndex(), &data);
            auto rowIndex = m_otherView.index(m_otherView.rowCount(QModelIndex()) - 1, 0, QModelIndex());
            m_otherView.setData(rowIndex, oItem.Id, Qt::UserRole);
        }
        
        m_otherView.selectionModel()->select(m_otherView.index(0, 0, QModelIndex()), QItemSelectionModel::Select | QItemSelectionModel::Rows);
    });
    
    connect(&m_otherView, &TreeView::sig_selectionChanged, this, [this](){
        auto selRows = m_otherView.selectedRows();
        
        if(selRows.size() == 0)
            return;
        
        auto id = selRows.first().data(Qt::UserRole).toULongLong();
        auto name = selRows.first().siblingAtColumn(1).data().toString();
        ui->fourthNameEdit->setText(name);
    });
    
    connect(ui->thirdSave, &QPushButton::clicked, this, [this](){
        SharedData item;
        if(m_idToOverwrite != 0)
            item = m_model->ItemByRow(m_model->GetRowById(m_idToOverwrite));
        
        item.Name = ui->thirdNameEdit->text();
        FillNewItem(item);
        if(m_idToOverwrite == 0)
            m_vm->AddNewItem(item);
        else
            m_vm->ModifyItem(item);
        
        ui->stackedWidget->setCurrentIndex(0);
    });
    
    connect(ui->fourthBack, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentIndex(1);
    });
    
    connect(ui->fourthSave, &QPushButton::clicked, this, [this](){
        auto selRows = m_otherView.selectedRows();
        if(selRows.size() == 0)
            return;
        quint64 otherSelectedItemId = selRows.first().data(Qt::UserRole).toULongLong();
        
        if(m_idToOverwrite == 0)
        {
            SharedData item;
            item.Name = ui->fourthNameEdit->text();
            m_vm->AddNewItemWithDataFromOther(item, otherSelectedItemId);
        }
        else
        {
            auto item = m_model->ItemByRow(m_model->GetRowById(m_idToOverwrite));
            m_vm->ModifyItemWithDataFromOther(item, otherSelectedItemId);
            item.Name = ui->fourthNameEdit->text();
            item.Data = std::nullopt;
            m_vm->ModifyItem(item);
        }
        
        ui->stackedWidget->setCurrentIndex(0);
    });
    
    m_vm->UpdateList();
}

GroupDataForm::~GroupDataForm()
{
    delete ui;
    delete m_model;
}

void GroupDataForm::EditItemName(const QModelIndex& index, QString name)
{
    auto item = m_model->ItemByIndex(index);
    item.Name = name;
    m_vm->ModifyItem(item);
}

void GroupDataForm::onItemsAdded(quint64 afterId, QList<SharedData> items)
{
    bool first = m_model->rowCount() == 0;
    m_model->AddItems(afterId, items.toVector());
    if(first)
        ui->tableView->selectRow(0);
}

void GroupDataForm::onItemsRemoved(QList<quint64> ids)
{
    m_model->RemoveItems(ids.toVector());
}

void GroupDataForm::onListCleared()
{
    m_model->Clear();
}

void GroupDataForm::onItemModified(SharedData item)
{
    m_model->ModifyItem(item);
    
    //just to update owners list
    emit ui->tableView->selectionModel()->selectionChanged(QItemSelection(), QItemSelection());
}

void GroupDataForm::onUsersGroupsListChanged(QList<User> users, QList<Group> groups)
{
    m_users = users;
    m_groups = groups;
    
    //just to update owners list
    emit ui->tableView->selectionModel()->selectionChanged(QItemSelection(), QItemSelection());
    
    ui->comboBox->clear();
    for(auto user : m_users)
    {
        ui->comboBox->addItem(user.Name, user.Id);
    }
    
    ui->ownersSelector->setUsersGroups(users, groups);
}

void GroupDataForm::onOtherListChanged(QList<SharedData> items)
{
    m_otherList = items;
    emit ui->comboBox->currentIndexChanged(ui->comboBox->currentIndex());
}
