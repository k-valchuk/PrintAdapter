#include "SharedDataVM.h"

SharedDataVM::SharedDataVM(QSharedPointer<ISharedDataModel> indModel, QObject *parent)
    : QObject{parent}, m_model(indModel)
{
    qRegisterMetaType<SharedData>();
    qRegisterMetaType<QList<SharedData>>();
    qRegisterMetaType<QVector<SharedData>>();
    qRegisterMetaType<User>();
    qRegisterMetaType<QList<User>>();
    qRegisterMetaType<Group>();
    qRegisterMetaType<QList<Group>>();
    
    m_thread = new QThread();
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    this->moveToThread(m_thread);
    m_thread->start();
}

SharedDataVM::SharedDataVM(QSharedPointer<ISharedDataModel> groupModel, QSharedPointer<ISharedDataModel> indModel, QObject* parent)
    : QObject{parent}, m_model(groupModel), m_otherModel(indModel)
{
    qRegisterMetaType<SharedData>();
    qRegisterMetaType<QList<SharedData>>();
    qRegisterMetaType<QVector<SharedData>>();
    qRegisterMetaType<User>();
    qRegisterMetaType<QList<User>>();
    qRegisterMetaType<Group>();
    qRegisterMetaType<QList<Group>>();
    
    m_thread = new QThread();
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    this->moveToThread(m_thread);
    m_thread->start();
}

void SharedDataVM::invoke(std::function<void()> action)
{
    QMetaObject::invokeMethod(this, action, Qt::QueuedConnection);
}

SharedDataVM::~SharedDataVM()
{
    m_thread->quit();
    m_thread->wait();
}

void SharedDataVM::UpdateList()
{
    invoke([this](){
        if(!m_model)
            return;
        
        emit listCleared();
        m_items.clear();
        
        if(m_otherModel)
        {
            auto users = m_model->GetUsers();
            auto groups = m_model->GetGroups();
            emit usersAndGroupsListChanged(users, groups);
            
            auto otherItems = m_otherModel->GetItems();
            m_otherItems = otherItems;
            emit otherListChanged(m_otherItems);
        }
        
        auto items = m_model->GetItems();
        m_items = items;
        
        emit itemsAdded(0, m_items);
    });
}

void SharedDataVM::AddNewItem(SharedData item, bool setAsDefault)
{
    invoke([this, item, setAsDefault]() mutable {
        if(!m_model)
            return;
        
        auto addedItems = m_model->AddItems({item});
        
        for(const auto& item : addedItems)
        {
            auto lastId = m_items.size() > 0 ? m_items.last().Id : 0;
            m_items.append(item);
            
            emit itemsAdded(lastId, {item});
        }
        if(addedItems.size())
        {
            needToEnterName(addedItems.last().Id);
            if(setAsDefault)
                if(m_model->SetDefaultItemForAuthorizedUser(addedItems.last().Id))
                    emit newItemSaved(addedItems.last().Id);
        }
    });
}


void SharedDataVM::AddNewItemWithDataFromOther(SharedData nItem, quint64 otherItemId)
{
    invoke([this, nItem, otherItemId]() mutable {
        if(!m_model)
            return;
        
        for(auto& oItem : m_otherItems)
        {
            if(oItem.Id == otherItemId)
            {
                m_model->LoadData(oItem);
                
                nItem.Data = oItem.Data;
                break;
            }
        }
        
        auto addedItems = m_model->AddItems({nItem});
        
        for(const auto& item : addedItems)
        {
            auto lastId = m_items.size() > 0 ? m_items.last().Id : 0;
            m_items.append(item);
            
            emit itemsAdded(lastId, {item});
        }
        if(addedItems.size())
            needToEnterName(addedItems.last().Id);
    });
}

void SharedDataVM::ModifyItemWithDataFromOther(SharedData nItem, quint64 otherItemId)
{
    invoke([this, nItem, otherItemId]() mutable {
        if(!m_model)
            return;
        
        for(auto& oItem : m_otherItems)
        {
            if(oItem.Id == otherItemId)
            {
                m_model->LoadData(oItem);
                
                nItem.Data = oItem.Data;
                break;
            }
        }
        
        auto modifiedItems = m_model->ModifyItems({nItem});
        
        for(const auto& item : modifiedItems)
        {
            for(auto& mItem : m_items)
                if(mItem.Id == item.Id)
                    mItem = item;
            
            emit itemModified(item);
        }
    });
}

void SharedDataVM::RemoveItems(QList<quint64> ids)
{
    invoke([this, ids](){
        if(!m_model)
            return;
        
        if(m_model->DeleteItems(ids))
            emit itemsRemoved(ids);
    });
}

void SharedDataVM::ModifyItem(SharedData editedItem, bool setAsDefault)
{
    invoke([this, editedItem, setAsDefault](){
        if(!m_model)
            return;
        
        auto modifiedItems = m_model->ModifyItems({editedItem});
        
        bool containsItem = false;
        for(const auto& item : modifiedItems)
        {
            for(auto& mItem : m_items)
                if(mItem.Id == item.Id)
                    mItem = item;
            
            emit itemModified(item);
            if(item.Id == editedItem.Id)
                containsItem = true;
        }
        
        if(!containsItem)
        {
            for(auto& mItem : m_items)
                if(mItem.Id == editedItem.Id)
                    emit itemModified(mItem);
        }
        
        if(modifiedItems.size())
        {
            if(setAsDefault)
                m_model->SetDefaultItemForAuthorizedUser(modifiedItems.last().Id);
        }
    });
}

