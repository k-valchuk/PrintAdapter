#include "IndividualUserDataForm.h"
#include "qmessagebox.h"
#include "ui_IndividualUserDataForm.h"

#include <QInputDialog>
#include <QStyledItemDelegate>

IndividualUserDataForm::IndividualUserDataForm(SharedDataVM* vm, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::IndividualUserDataForm)
{
    ui->setupUi(this);
    
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    
    m_model = new SharedDataTableModel({1});
    ui->tableView->setModel(m_model);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
    ui->tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->tableView->horizontalHeader()->setObjectName("coloredHeader");
    
    ui->tableView->setItemDelegateForColumn(1, new SharedObjectNameEditDelegate(this, this));
    ui->tableView->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
    
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        auto indexes = selected.indexes();
        
        
    });
    
    m_vm = vm;
    
    connect(m_vm, &SharedDataVM::itemsAdded, this, &IndividualUserDataForm::onItemsAdded, Qt::QueuedConnection);
    connect(m_vm, &SharedDataVM::itemsRemoved, this, &IndividualUserDataForm::onItemsRemoved, Qt::QueuedConnection);
    connect(m_vm, &SharedDataVM::listCleared, this, &IndividualUserDataForm::onListCleared, Qt::QueuedConnection);
    connect(m_vm, &SharedDataVM::itemModified, this, &IndividualUserDataForm::onItemModified, Qt::QueuedConnection);
    connect(m_vm, &SharedDataVM::needToEnterName, this, [this](quint64 VMid){
            auto row = m_model->GetRowById(VMid);
            if(row != -1)
            {
                ui->tableView->selectRow(row);
                ui->tableView->edit(m_model->index(row, 1));
            }
        }, Qt::QueuedConnection);
    
    
    connect(ui->addView, &QPushButton::clicked, this, [this](){
        SharedData item;
        item.Name = tr("New view");
        FillNewItem(item);
        m_vm->AddNewItem(item, true);
    });
    connect(ui->removeView, &QPushButton::clicked, this, [this](){
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
    
    connect(ui->rewriteView, &QPushButton::clicked, this, [this](){
        auto indexes = ui->tableView->selectionModel()->selectedRows(0);
        if(indexes.size() == 1)
        {
            auto item = m_model->ItemByIndex(indexes.first());
            QMessageBox msgBox;
            msgBox.setWindowTitle(QApplication::tr("Overwrite view"));
            msgBox.setText(QApplication::tr("Are you sure to overwrite view \"%1\"?").arg(item.Name));
            msgBox.addButton(QApplication::tr("Yes"), QMessageBox::YesRole);
            msgBox.addButton(QApplication::tr("No"), QMessageBox::NoRole);
            auto dialogResult = msgBox.exec();
            
            if(dialogResult == 0)
            {
                auto item = m_model->ItemByIndex(indexes.first());
                FillExistingItem(item);
                m_vm->ModifyItem(item, true);
            }
        }
    });
    
    connect(ui->close, &QPushButton::clicked, this, &QWidget::close);
    
    m_vm->UpdateList();
}

IndividualUserDataForm::~IndividualUserDataForm()
{
    delete ui;
    delete m_model;
}

void IndividualUserDataForm::EditItemName(const QModelIndex& index, QString name)
{
    auto item = m_model->ItemByIndex(index);
    item.Name = name;
    m_vm->ModifyItem(item);
}

void IndividualUserDataForm::onItemsAdded(quint64 afterId, QList<SharedData> views)
{
    m_model->AddItems(afterId, views.toVector());
}

void IndividualUserDataForm::onItemsRemoved(QList<quint64> ids)
{
    m_model->RemoveItems(ids.toVector());
}

void IndividualUserDataForm::onListCleared()
{
    m_model->Clear();
}

void IndividualUserDataForm::onItemModified(SharedData item)
{
    m_model->ModifyItem(item);
}
