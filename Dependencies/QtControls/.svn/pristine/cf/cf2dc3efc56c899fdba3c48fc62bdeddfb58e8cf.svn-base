#ifndef USERVIEWSVM_H
#define USERVIEWSVM_H

#include <QObject>
#include <QThread>
#include "ISharedDataModel.h"
#include <QList>
#include <QVector>
#include <QSharedPointer>

class SharedDataVM : public QObject
{
    Q_OBJECT
private:
    
    QThread* m_thread = nullptr;
    
    QSharedPointer<ISharedDataModel> m_model;
    QSharedPointer<ISharedDataModel> m_otherModel;
    
    QList<SharedData> m_items;
    QList<SharedData> m_otherItems;
    void invoke(std::function<void()> action);
public:
    explicit SharedDataVM(QSharedPointer<ISharedDataModel> indModel, QObject *parent = nullptr);
    explicit SharedDataVM(QSharedPointer<ISharedDataModel> groupModel,
                          QSharedPointer<ISharedDataModel> indModel,
                          QObject *parent = nullptr);
    ~SharedDataVM();
    
public slots:
    void UpdateList();
    
    void AddNewItem(SharedData item, bool setAsDefault = false);
    void RemoveItems(QList<quint64> id);
    void ModifyItem(SharedData item, bool setAsDefault = false);
    
    void AddNewItemWithDataFromOther(SharedData nItem, quint64 otherItemId);
    void ModifyItemWithDataFromOther(SharedData nItem, quint64 otherItemId);
signals:
    void needToEnterName(quint64 id);
    void itemsAdded(quint64 afterId, QList<SharedData> items);
    void itemsRemoved(QList<quint64> ids);
    void itemModified(SharedData item);
    void listCleared();
    
    void usersAndGroupsListChanged(QList<User> users, QList<Group> groups);
    //для другого списка (создание группового объекта на осонове индивидуального)
    void otherListChanged(QList<SharedData> items);
    
    void newItemSaved(quint64 id);
};

Q_DECLARE_METATYPE(SharedData)
Q_DECLARE_METATYPE(QList<SharedData>)
Q_DECLARE_METATYPE(QVector<SharedData>)

Q_DECLARE_METATYPE(User)
Q_DECLARE_METATYPE(QList<User>)
Q_DECLARE_METATYPE(Group)
Q_DECLARE_METATYPE(QList<Group>)

#endif // USERVIEWSVM_H
