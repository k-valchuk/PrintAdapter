#ifndef FILTERSLIST_H
#define FILTERSLIST_H

#include <QFrame>
#include <QStandardItemModel>
#include "IFilterElement.h"
#include "qlistview.h"
#include <QLineEdit>
#include <QKeyEvent>

namespace Ui {
class FiltersList;
}

class FiltersListLineEdit : public QLineEdit
{
    Q_OBJECT
    
public:
    FiltersListLineEdit(QWidget* parent = nullptr)
        : QLineEdit(parent) {}
    
protected:
    void keyPressEvent(QKeyEvent* event) override
    {
        if(event->key() == Qt::Key_Down || event->key() == Qt::Key_Up)
        {
            emit moveToList(event->key() == Qt::Key_Down);
            return;
        }
        QLineEdit::keyPressEvent(event);
    }
    
signals:
    void moveToList(bool down);
};

class FilterRow;
class FiltersList : public QFrame
{
    Q_OBJECT
    
    FilterRow* m_parentRow;
    
    QStandardItemModel m_list;
    bool supressClick = false;
    QString m_searchStr = QString();
    
    void updateList();
public:
    explicit FiltersList(FilterRow* parentRow, QWidget *parent = nullptr);
    ~FiltersList();
    
    void SetFocusToSearch();
protected:
    void showEvent(QShowEvent* event) override;
private:
    Ui::FiltersList *ui;
    
signals:
    void filterSelected(QVariant filterKey);
};

#endif // FILTERSLIST_H
