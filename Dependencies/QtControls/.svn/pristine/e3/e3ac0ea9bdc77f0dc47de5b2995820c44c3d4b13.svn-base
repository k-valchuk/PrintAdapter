#ifndef IUSERVIEWSMODEL_H
#define IUSERVIEWSMODEL_H

#include <QDateTime>
#include <QJsonObject>
#include <QList>
#include <optional>
#include <QSet>

struct SharedDataOwners
{
    QSet<quint64> UserIds;
    QSet<quint64> GroupIds;
};

struct SharedData
{
    quint64 Id = 0;
    QString Name;
    QDateTime ModificationDate;
    std::optional<QByteArray> Data = std::nullopt;
    
    SharedDataOwners Owners;
};

struct User
{   
    quint64 Id;
    QString Name;
};

struct Group
{
    quint64 Id;
    QString Name;
};

class ISharedDataModel
{
public:
    virtual QList<SharedData> GetItems(SharedDataOwners filter = SharedDataOwners()) = 0;
    virtual bool LoadData(SharedData& itemToFill) = 0;
    virtual QList<SharedData> ModifyItems(QList<SharedData> items) = 0;
    virtual QList<SharedData> AddItems(QList<SharedData> items) = 0;
    virtual bool DeleteItems(QList<quint64> ids) = 0;
    virtual bool SetDefaultItemForAuthorizedUser(quint64 id) = 0;
    
    virtual QList<User> GetUsers() = 0;
    virtual QList<Group> GetGroups() = 0;
};

class DummySDModel : public ISharedDataModel
{
    quint64 freeId;
    QList<SharedData> indItems;
    QList<User> users;
    QList<Group> groups;
public:
    DummySDModel()
    {
        indItems = {
            {1, "Имя 1", QDateTime::currentDateTime(), QByteArray(), SharedDataOwners{{2, 3}, {}}},
            {2, "Имя 2", QDateTime::currentDateTime(), QByteArray(), SharedDataOwners{{1}, {}}},
            {3, "Имя 3", QDateTime::currentDateTime(), QByteArray(), SharedDataOwners{{1, 2}, {}}},
            {4, "Имя 4", QDateTime::currentDateTime(), QByteArray(), SharedDataOwners{{1, 2, 3}, {}}},
        };
        freeId = indItems.size() + 1;
        
        users = {
            {1, "admin"},
            {2, "maximglin"},
            {3, "user12"},
        };
        
        groups = {
            {1, "group1"},
            {2, "group2"},
            {3, "group3"},
        };
    }
    
    QList<SharedData> GetItems(SharedDataOwners filter) override
    {
        return indItems;
    }
    bool LoadData(SharedData& itemToFill) override
    {
        return true;
    }
    QList<SharedData> ModifyItems(QList<SharedData> items) override
    {
        for(auto& item : items)
        {
            for(auto& mItem : indItems)
                if(mItem.Id == item.Id)
                {
                    item.ModificationDate = QDateTime::currentDateTime();
                    mItem = item;
                }
        }
            
        return items;
    }
    QList<SharedData> AddItems(QList<SharedData> items) override
    {
        for(auto& item : items)
        {
            item.Id = freeId++;
            item.ModificationDate = QDateTime::currentDateTime();
        }
        indItems.append(items);
        return items;
    }
    bool DeleteItems(QList<quint64> ids) override
    {
        for(int i = indItems.size() - 1; i >=0; i--)
            if(ids.contains(indItems[i].Id))
                indItems.removeAt(i);
        return true;
    }
    
    bool SetDefaultItemForAuthorizedUser(quint64 id) override
    {
        return true;
    }
    
    QList<User> GetUsers() override
    {
        return users;
    }
    QList<Group> GetGroups() override
    {
        return groups;
    }
};

#endif // IUSERVIEWSMODEL_H
