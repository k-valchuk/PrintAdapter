//----------------------------------------------------------------------
//Выпадающий список для итема таблицы (со своим activated())
//----------------------------------------------------------------------
#ifndef ITEMCOMBOBOX_H
#define ITEMCOMBOBOX_H

#include "qcombobox.h"

class ItemComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit ItemComboBox(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

signals:
    void sig_hideView();            //закрыть список
    void sig_activated(int);        //выбран элемент
};

#endif // ITEMCOMBOBOX_H
