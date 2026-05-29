#include "OperatorList.h"
#include "ui_OperatorList.h"

void OperatorList::updateList()
{
    m_list.clear();
    for(auto op : m_ops)
    {
        auto item = new QStandardItem();
        item->setText(op.Name);
        item->setData(op.Key);
        
        m_list.appendRow(item);
    }
}

OperatorList::OperatorList(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OperatorList)
{
    ui->setupUi(this);
    
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listView->setModel(&m_list);
    
    connect(ui->listView, &QAbstractItemView::clicked, this, [this](QModelIndex index)
            {
                if(!index.isValid())
                    return;
                
                auto item = m_list.itemFromIndex(index);
                if(!item)
                    return;
                
                auto data = item->data();
                this->hide();
                emit this->operatorSelected(data);
            });
}

void OperatorList::SetOpsList(QList<FilterOperator> ops)
{
    m_ops = ops;
    
    updateList();
}

void OperatorList::showEvent(QShowEvent* event)
{
    ui->listView->setFocus();
}

OperatorList::~OperatorList()
{
    delete ui;
}

