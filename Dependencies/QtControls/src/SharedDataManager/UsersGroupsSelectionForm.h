#ifndef USERSGROUPSSELECTIONFORM_H
#define USERSGROUPSSELECTIONFORM_H

#include <QFrame>
#include "ISharedDataModel.h"
#include "treeview.h"

namespace Ui {
class UsersGroupsSelectionForm;
}

class QStandardItemModel;
class UsersGroupsSelectionForm : public QFrame
{
    Q_OBJECT
    
    QString m_searchStr = "";
    
    QList<User> m_users;
    QList<Group> m_groups;
    
    QSet<quint64> m_checkedUsers;
    QSet<quint64> m_checkedGroups;
    
    QSet<quint64> m_disabledUsers;
    QSet<quint64> m_disabledGroups;
    
    TreeView m_treeView;
    bool m_inUpdateList = false;
    
    void updateList();
public:
    explicit UsersGroupsSelectionForm(QWidget *parent = nullptr);
    ~UsersGroupsSelectionForm();
    
    void setUsersGroups(QList<User> users, QList<Group> groups) {
        m_users = users;
        m_groups = groups;
        updateList();
    }
    
    void clearSelectionAndEnable(){
        m_disabledUsers.clear();
        m_disabledGroups.clear();
        m_checkedUsers.clear();
        m_checkedGroups.clear();
        updateList();
    }
    void disableItems(QSet<quint64> users, QSet<quint64> groups){
        for(const auto& id : users)
            m_disabledUsers.insert(id);
        for(const auto& id : groups)
            m_disabledGroups.insert(id);
        updateList();
    }
    void setCheckedItems(QSet<quint64> users, QSet<quint64> groups){
        for(const auto& id : users)
            m_checkedUsers.insert(id);
        for(const auto& id : groups)
            m_checkedGroups.insert(id);
        updateList();
    }
    
    QList<User> GetSelectedUsers() {
        QList<User> selectedUsers;
        for(const auto& user : m_users)
            if(m_checkedUsers.contains(user.Id))
                selectedUsers.append(user);
        return selectedUsers;
    }
    QList<Group> GetSelectedGroups(){
        QList<Group> selectedGroups;
        for(const auto& group : m_groups)
            if(m_checkedGroups.contains(group.Id))
                selectedGroups.append(group);
        return selectedGroups;
    }
private:
    Ui::UsersGroupsSelectionForm *ui;
};

typedef QPair<int, quint64> intQuint64Pair;
Q_DECLARE_METATYPE(intQuint64Pair)

#endif // USERSGROUPSSELECTIONFORM_H
