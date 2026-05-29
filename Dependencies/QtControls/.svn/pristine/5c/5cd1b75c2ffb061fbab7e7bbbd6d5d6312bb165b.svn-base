#ifndef OPERATORLIST_H
#define OPERATORLIST_H

#include <QFrame>
#include <QStandardItemModel>
#include "IFilterElement.h"

namespace Ui {
class OperatorList;
}

class OperatorList : public QFrame
{
    Q_OBJECT
    
    QList<FilterOperator> m_ops;
    QStandardItemModel m_list;
    
    void updateList();
public:
    OperatorList(QWidget *parent = nullptr);
    ~OperatorList();
    
    
    void SetOpsList(QList<FilterOperator> ops);
private:
    Ui::OperatorList *ui;
    
signals:
    void operatorSelected(QVariant opKey);
    
protected:
    void showEvent(QShowEvent* event) override;
};

#endif // OPERATORLIST_H
