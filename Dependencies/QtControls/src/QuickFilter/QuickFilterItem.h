#ifndef QUICKFILTERITEM_H
#define QUICKFILTERITEM_H

#include <QStandardItem>
#include "FilterExpressions.h"
#include <functional>

class IQuickFilterItem : public QObject
{
    Q_OBJECT
protected:
    IQuickFilterItem* m_parent = nullptr;
    QList<IQuickFilterItem*> m_children;
    
    IQuickFilterItem* Root()
    {
        auto ptr = this;
        while(ptr->m_parent != nullptr)
            ptr = ptr->m_parent;
        return ptr;
    }
    
    void emitTreeChanged(IQuickFilterItem* parent, bool supressFilterUpdate = false)
    {
        emit Root()->treeChanged(parent, supressFilterUpdate);
    }
    
public:
    IQuickFilterItem(IQuickFilterItem* parent) : m_parent(parent)
    {
        
    }
    
    virtual QList<QStandardItem*> GetItems() = 0;
    
    IQuickFilterItem* Parent()
    {
        return m_parent;
    }
    
    QSharedPointer<QSignalBlocker> blockTreeChangedSignal()
    {
        return QSharedPointer<QSignalBlocker>::create(Root());
    }

    virtual QList<IQuickFilterItem*> Children() = 0;
    
    virtual ~IQuickFilterItem()
    {
        for(auto child : m_children)
            delete child;
    }
    
    virtual void RemoveItem(IQuickFilterItem* item)
    {
        m_children.removeAll(item);
        delete item;
        emitTreeChanged(this);
    }
    
    void emitTreeChanged(bool supressFilterUpdate = false)
    {
        emitTreeChanged(this, supressFilterUpdate);
    }
 
signals:
    void treeChanged(IQuickFilterItem* parent, bool supressFilterUpdate);
};

class QuickFilterCategory;
class QuickFilterPreset : public IQuickFilterItem
{
protected:
    QString m_name;
    std::function<IExpression*()> m_expAction;
    bool m_checked;
public:
    QuickFilterPreset(QuickFilterCategory* parent, QString name,
                      std::function<IExpression*()> formFilterExpAction, bool initiallyChecked = false)
        : IQuickFilterItem((IQuickFilterItem*)parent),
        m_name(name), m_expAction(formFilterExpAction),
        m_checked(initiallyChecked)
    {
        
    }
    
    QList<QStandardItem*> GetItems() override
    {
        auto item = new QStandardItem();
        item->setCheckable(false);
        //item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));
        item->setCheckState(m_checked ? Qt::Checked : Qt::Unchecked);
        item->setText(m_name);
        QVariant object;
        object.setValue(this);
        item->setData(object);
        item->setEditable(false);
        return {item};
    }
    
    QList<IQuickFilterItem*> Children() override
    {
        return {};
    }
    
    IExpression* CreateExpression()
    {
        return m_expAction();
    }
    
    void SetChecked(bool isChecked)
    {
        m_checked = isChecked;
        emitTreeChanged(this);
    }
    
    bool IsChecked()
    {
        return m_checked;
    }
};


class QuickFilterAggrPreset: public QuickFilterPreset
{
    int m_count;
public:
    QuickFilterAggrPreset(QuickFilterCategory* parent, QString name, int count,
                      std::function<IExpression*()> formFilterExpAction, bool initiallyChecked = false)
        : QuickFilterPreset(parent, name, formFilterExpAction, initiallyChecked),
        m_count(count)
    {
        
    }
    
    QList<QStandardItem*> GetItems() override
    {
        auto item = new QStandardItem();
        item->setCheckable(false);
        //item->setFlags(item->flags() & (~Qt::ItemIsUserCheckable));
        item->setCheckState(m_checked ? Qt::Checked : Qt::Unchecked);
        item->setText(m_name);
        QVariant object;
        object.setValue(this);
        item->setData(object);
        item->setEditable(false);
        
        if(m_count >= 0)
        {
            auto countItem = new QStandardItem(QString("%1").arg(m_count));
            countItem->setTextAlignment(Qt::AlignRight);
            countItem->setData(object);
            countItem->setEditable(false);
            return {item, countItem};
        }
        else
            return {item};
    }
};

class QuickFilterCategory : public IQuickFilterItem
{
    QList<QuickFilterPreset*> m_filters;
    
    QString m_name;
    bool m_singleSelection;
    bool m_orOperator;
    bool m_searchBar = false;
    bool m_showMoreBtn = false;
    bool m_allLoaded = true;
    int  m_priority = 1;
public:
    QuickFilterCategory(IQuickFilterItem* parent, QString name,
                        bool singleSelection = false, bool isOrOperator = false,
                        bool searchBarNeeded = false, bool shMoreNeeded = false, bool allLoaded = true)
        : IQuickFilterItem(parent), m_name(name), m_singleSelection(singleSelection), m_orOperator(isOrOperator),
        m_searchBar(searchBarNeeded), m_showMoreBtn(shMoreNeeded), m_allLoaded(allLoaded)
    {
        
    }
    
    QuickFilterPreset* AddFilter(QString name, std::function<IExpression*()> formFilterExpAction, bool initiallyChecked = false)
    {
        auto f = new QuickFilterPreset(this, name, formFilterExpAction, initiallyChecked);
        m_filters.append(f);
        m_children.append(f);
        emitTreeChanged(this);
        return f;
    }
    QuickFilterAggrPreset* AddAggrFilter(QString name, int count, std::function<IExpression*()> formFilterExpAction, bool initiallyChecked = false)
    {
        auto f = new QuickFilterAggrPreset(this, name, count, formFilterExpAction, initiallyChecked);
        m_filters.append(f);
        m_children.append(f);
        emitTreeChanged(this);
        return f;
    }
    
    void SetName(const QString& name)
    {
        m_name = name;
        emitTreeChanged(this);
    }
    
    QString GetName()
    {
        return m_name;
    }
    
    void Clear()
    {
        auto copy = m_children;
        
        m_children.clear();
        m_filters.clear();
        
        for(auto child : copy)
            delete child;
        
        emitTreeChanged(this);
    }
    
    void RemoveItem(IQuickFilterItem* item) override
    {
        if(auto filter = dynamic_cast<QuickFilterPreset*>(item))
            m_filters.removeAll(filter);
        IQuickFilterItem::RemoveItem(item);
    }
    
    QList<QStandardItem*> GetItems() override
    {
        auto item = new QStandardItem();
        item->setText(m_name);
        item->setIcon(QIcon(":/filter/calendar_filter"));
        QVariant object;
        object.setValue(this);
        item->setData(object);
        for(auto child: Children())
            item->appendRow(child->GetItems());
        
        item->setEditable(false);
        return {item};
    }
    
    bool IsSingleSelectionOnly()
    {
        return m_singleSelection;
    }
    
    bool IsOrOperator()
    {
        return m_orOperator;
    }
    
    QList<IQuickFilterItem*> Children() override
    {
        return m_children;
    }
    
    bool SearchBarNeeded()
    {
        return m_searchBar;
    }
    bool ShowMoreButtonNeeded()
    {
        return m_showMoreBtn;
    }
    bool AllLoaded()
    {
        return m_allLoaded;
    }
    
    void SetFlags(bool searchBarNeeded = false, bool shMoreNeeded = false, bool allLoaded = true)
    {
        m_searchBar = searchBarNeeded;
        m_showMoreBtn = shMoreNeeded;
        m_allLoaded = allLoaded;
        
        emitTreeChanged(this, false);
    }
    
    QList<QuickFilterPreset*> AllFilterNodes()
    {
        return m_filters;
    }
    
    void SetPriority(int priority)
    {
        m_priority = priority;
        emitTreeChanged(Root(), false);
    }
    int Priority()
    {
        return m_priority;
    }
};

class QuickFilterRoot : public IQuickFilterItem
{
    QList<QuickFilterCategory*> m_cats;
    
public:
    QuickFilterRoot() : IQuickFilterItem(nullptr) {}
    
    QList<QuickFilterCategory*> AllChildCategoriesRecursive();
    
    QList<QuickFilterPreset*> AllFilterNodes()
    {
        QList<QuickFilterPreset*> res;
        for(auto cat : m_cats)
            if(cat)
                res.append(cat->AllFilterNodes());
        return res;
    }
    
    void RemoveItem(IQuickFilterItem* item) override
    {
        if(auto cat = dynamic_cast<QuickFilterCategory*>(item))
            m_cats.removeAll(cat);
        IQuickFilterItem::RemoveItem(item);
    }
    
    QuickFilterCategory* AddCategory(QString name, bool singleSelection = false, bool isOrOperator = false,
                                     bool searchNeeded = true, bool shMoreNeeded = true, bool allLoaded = true)
    {
        auto newCat = new QuickFilterCategory(this, name, singleSelection, isOrOperator, searchNeeded, shMoreNeeded, allLoaded);
        m_children.append(newCat);
        m_cats.append(newCat);
        emitTreeChanged(this);
        return newCat;
    }
    
    QList<QStandardItem*> GetItems() override
    {
        return {};
    }
    QList<IQuickFilterItem*> Children() override
    {
        return m_children;
    }
};

#endif // QUICKFILTERITEM_H
