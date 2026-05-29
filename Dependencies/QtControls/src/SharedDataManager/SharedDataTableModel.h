#ifndef SHAREDDATATABLEMODEL_H
#define SHAREDDATATABLEMODEL_H

#include "ISharedDataModel.h"
#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QLineEdit>

class ISharedDataForm
{
public:
    virtual void EditItemName(const QModelIndex& index, QString name) = 0;
};

class SharedDataTableModel : public QAbstractTableModel
{
    QVector<SharedData> m_list;
    QSet<int> m_editableColumns;
public:
    SharedDataTableModel(QSet<int> editableColumns = {}) : m_editableColumns(editableColumns)
    {
    }
    
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return m_list.size();
    }
    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return 3;
    }
    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid())
            return QVariant();
        
        if (role == Qt::DisplayRole)
        {
            auto item = ItemByIndex(index);
            switch(index.column())
            {
            case 0:
                return item.Id ? QString("%1").arg(item.Id) : "";
            case 1:
                return item.Name;
            case 2:
                return item.ModificationDate.toString("dd.MM.yyyy hh:mm:ss");
            }
        }
        
        return QVariant();
    }
    
    Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        auto res = QAbstractTableModel::flags(index);
        if(m_editableColumns.contains(index.column()))
            res = res | Qt::ItemFlag::ItemIsEditable;
        
        return res;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (orientation == Qt::Horizontal) {
            if(role == Qt::DisplayRole){
                switch(section)
                {
                case 0:
                    return tr("ID");
                case 1:
                    return tr("Name");
                case 2:
                    return tr("Modification Date");
                }
            }
        }
        return QVariant();
    }
    
public:
    SharedData ItemByRow(int row) const
    {
        return ItemByIndex(this->index(row, 0));
    }
    
    SharedData ItemByIndex(const QModelIndex& index) const
    {
        if (!index.isValid())
            return {};
        
        if (index.row() < m_list.size())
            return m_list[index.row()];
        
        return {};
    }
    
    void AddItems(quint64 afterId, QVector<SharedData> items)
    {
        if (items.size() == 0)
            return;
        
        int row = 0;
        for (int i = 0; i < m_list.size(); i++)
            if (m_list[i].Id == afterId) {
                row = i + 1;
                break;
            }
        
        m_list.reserve(m_list.size() + items.size());
        
        this->beginInsertRows(QModelIndex(), row, row + items.size() - 1);
        
        for (int i = items.size() - 1; i >= 0; i--)
            m_list.insert(row, items[i]);
        
        this->endInsertRows();
        
        //updateColumns();
        emit layoutChanged();
    }
    
    QVector<int> RemoveItems(const QVector<quint64>& ids) // returns list of removed row indexes
    {
        QVector<int> rowsToRemove;
        for (int i = 0; i < m_list.size(); i++) {
            if (ids.contains(m_list[i].Id)) {
                rowsToRemove.append(i);
            }
        }
        
        for (int i = rowsToRemove.size() - 1; i >= 0; i--) {
            auto rowToRemove = rowsToRemove[i];
            this->beginRemoveRows(QModelIndex(), rowToRemove, rowToRemove);
            m_list.remove(rowToRemove);
            this->endRemoveRows();
        }
        
        emit layoutChanged();
        
        return rowsToRemove;
    }
    
    void Clear()
    {
        m_list.clear();
        emit this->layoutChanged();
    }
    
    void ModifyItem(SharedData item)
    {
        for (int i = 0; i < m_list.size(); i++) {
            if (m_list[i].Id == item.Id)
            {
                m_list[i] = item;
            }
        }
        emit this->layoutChanged();
    }
    
    int GetRowById(quint64 id)
    {
        for (int i = 0; i < m_list.size(); i++) {
            if (m_list[i].Id == id)
                return i;
        }
        
        return -1;
    }
};

class SharedObjectNameEditDelegate : public QStyledItemDelegate
{
    ISharedDataForm* m_form = nullptr;
public:
    SharedObjectNameEditDelegate(ISharedDataForm* form, QObject* parent)
        : QStyledItemDelegate(parent), m_form(form)
    {
    }
    
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        return new QLineEdit(parent);
    }
    void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        auto le = (QLineEdit*)editor;
        auto model = (SharedDataTableModel*)index.model();
        le->setText(model->ItemByIndex(index).Name);
    }
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
    {
        auto le = (QLineEdit*)editor;
        auto newName = le->text();
        
        //это только для того чтобы имя обновилось сразу, и не было визуального подлага со старым именем
        //пока запрос выполняется
        auto smodel = (SharedDataTableModel*)model;
        auto item = smodel->ItemByIndex(index);
        auto oldName = item.Name;
        item.Name = newName;
        smodel->ModifyItem(item);
        
        //а там уже реальное обращение к серверу
        //после которого придет сигнал на обновление
        if(oldName != newName)
            m_form->EditItemName(index, newName);
    }
};

#endif // SHAREDDATATABLEMODEL_H
