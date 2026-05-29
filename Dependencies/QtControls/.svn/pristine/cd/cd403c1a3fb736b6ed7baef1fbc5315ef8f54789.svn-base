#ifndef FILTERPRESETITEM_H
#define FILTERPRESETITEM_H

#include <QTreeWidgetItem>
#include <QPushButton>
#include "FilterExpressions.h"
#include "IFilterElement.h"

class IFilterPresetItem : public QObject
{
    Q_OBJECT
protected:
    IFilterPresetItem* m_parent = nullptr;
    QList<IFilterPresetItem*> m_children;
    QString m_name;
    
protected slots:
    void emitTreeChanged()
    {
        emit Root()->treeChanged();
    }

public:
    IFilterPresetItem(QString name, IFilterPresetItem* parent)
        : m_parent(parent), m_name(name)
    {
        
    }
    
    IFilterPresetItem* Root()
    {
        auto ptr = this;
        while(ptr->m_parent != nullptr)
            ptr = ptr->m_parent;
        return ptr;
    }
    
    IFilterPresetItem* Parent()
    {
        return m_parent;
    }
    
    QString Name()
    {
        return m_name;
    }
    
    QList<IFilterPresetItem*> Children()
    {
        return m_children;
    };
    
    virtual ~IFilterPresetItem()
    {
        for(auto child : m_children)
            delete child;
    }
    
    void Tree(std::function<void(IFilterPresetItem*)> action)
    {
        if(m_parent != nullptr)
            action(this);
        for(auto child : m_children)
            child->Tree(action);
    }
    
    
    void RemoveChid(IFilterPresetItem* item)
    {
        if(m_children.contains(item))
            m_children.removeAll(item);
        
        delete item;
        
        emitTreeChanged();
    }
    
    void Clear(bool deleteChildren = true)
    {
        if(deleteChildren)
            for(auto child : m_children)
                delete child;
        m_children.clear();
        emitTreeChanged();
    }
    
signals:
    void treeChanged();
    void valueChanged();
};

class FilterPreset : public IFilterPresetItem
{
    QSharedPointer<IExpression> m_exp;
    quint64 m_id;
public:
    FilterPreset(quint64 id, QString name, IFilterPresetItem* parent, QSharedPointer<IExpression> exp)
        : IFilterPresetItem(name, parent), m_exp(exp), m_id(id)
    {
        
    }
    
    QSharedPointer<IExpression> Expression()
    {
        return m_exp;
    }
    
    void SetName(QString newName)
    {
        m_name = newName;
        emit valueChanged();
    }
    
    void SetExpression(QSharedPointer<IExpression> newExp)
    {
        m_exp = newExp;
        emit valueChanged();
    }
    
    quint64 Id() {return m_id;}
};

class FilterPresetGroup : public IFilterPresetItem
{
    QSharedPointer<IFilterElement> m_allFieldsFilter = nullptr;
    QList<QSharedPointer<IFilterElement>>  m_filters;
public:
    FilterPresetGroup(QString name, IFilterPresetItem* parent)
        : IFilterPresetItem(name, parent)
    {
        
    }
    
    FilterPresetGroup* AddGroup(QString name)
    {
        auto group = new FilterPresetGroup(name, this);
        m_children.append(group);
        emitTreeChanged();
        return group;
    }
    
    FilterPreset* AddPreset(quint64 id, QString name, QSharedPointer<IExpression> exp = nullptr)
    {
        auto preset = new FilterPreset(id, name, this, exp);
        m_children.append(preset);
        emitTreeChanged();
        return preset;
    }
    
    void AddPreset(FilterPreset* preset)
    {
        m_children.append(preset);
        emitTreeChanged();
    }
    
    //для того чтобы показывать превью с облачками чтобы из этой структуры можно было получить фильтры
    void SetFilters(QSharedPointer<IFilterElement> allFieldsFilter,
                    QList<QSharedPointer<IFilterElement>> filters)
    {
        m_allFieldsFilter = allFieldsFilter;
        m_filters = filters;
    }
    QSharedPointer<IFilterElement> GetAllFieldsFilter() {return m_allFieldsFilter;}
    QList<QSharedPointer<IFilterElement>> GetFilters() {return m_filters;}
};



#endif // FILTERPRESETITEM_H
