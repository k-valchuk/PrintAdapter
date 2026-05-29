#ifndef GROUPDATAFORM_H
#define GROUPDATAFORM_H

#include <QFrame>
#include "SharedDataVM.h"
#include "SharedDataTableModel.h"
#include "treeview.h"

namespace Ui {
class GroupDataForm;
}

class GroupDataForm : public QFrame, public ISharedDataForm
{
    Q_OBJECT
    
    SharedDataTableModel* m_model = nullptr;
    SharedDataVM* m_vm;
    
    QList<User> m_users;
    QList<Group> m_groups;
    QList<SharedData> m_otherList;
    quint64 m_idToOverwrite = 0;
    
    TreeView m_otherView;
    
    void updatelabels();
protected:
    virtual void FillNewItem(SharedData& item) {
        
    }
    virtual void FillExistingItem(SharedData& item) {
        
    }
    
    struct LabelsTexts
    {
        QString objectSettingsLabel;
        QString groupBoxNew;
        QString groupBoxRW;
        QString radiobtnCurrent;
        QString radiobtnSelect;
        QString nameLabel;
    };
    
    virtual LabelsTexts labels() {
        return LabelsTexts{
            tr("Object settings"),
            tr("Object creation"),
            tr("Object overwrite"),
            tr("Create object from current settings"),
            tr("Copy from existing user's preset"),
            tr("Object name")
        };
    }
public:
    explicit GroupDataForm(SharedDataVM* vm, QWidget *parent = nullptr);
    ~GroupDataForm();
    
    void EditItemName(const QModelIndex& index, QString name) override;
public slots:
    void onItemsAdded(quint64 afterId, QList<SharedData> items);
    void onItemsRemoved(QList<quint64> ids);
    void onListCleared();
    void onItemModified(SharedData item);
    
    void onUsersGroupsListChanged(QList<User> users, QList<Group> groups);
    void onOtherListChanged(QList<SharedData> items);
private:
    Ui::GroupDataForm *ui;
    
protected:
    void showEvent(QShowEvent* event) override
    {
        updatelabels();
    }
};

#endif // GROUPDATAFORM_H
