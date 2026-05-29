//----------------------------------------------------------------------
// Виджет выпадающего списка с возможностью выбора нескольких элементов
//----------------------------------------------------------------------
#ifndef CHECKCOMBOBOX_H
#define CHECKCOMBOBOX_H

#include "QComboBox"
#include "QStandardItemModel"

//модель списка
class CheckListModel : public QStandardItemModel
{
    Q_OBJECT
public:
    CheckListModel(QObject *parent = nullptr);
    void setCheckedItems(const QStringList &items);                 //задать список выбранных элементов
    QStringList getCheckedItems() const {return  checkedItems;}     //получить список выбранных элементов

signals:
    void checkedItemsUpdated();                  //обновился список выбранных элементов

private slots:
    //события изменения в списке
    void slotModelRowsInserted(const QModelIndex &, int start, int end);
    void slotModelRowsRemoved(const QModelIndex &parent, int, int);
    void slotModelItemChanged(QStandardItem *item);

private:
    QStringList checkedItems;   //список выбранных элементов

    void collectCheckedItems(); //заполнить список выбранных элементов
};

//выпадающий список
class CheckCombobox : public QComboBox
{
    Q_OBJECT

public:
    CheckCombobox(QWidget *parent = nullptr);
    virtual ~CheckCombobox();

    QStringList checkedItems() const;                           //получить список выбранных элементов
    void setCheckedItems(const QStringList &items);             //задать список выбранных элементов

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

signals:
    void checkItemsChanged(const QStringList checkList);        //оповещение об изменении списка выбранных элементов

private slots:
    void slotItemTextClicked(const QModelIndex &ind);           //нажатие пользователя на текст итема
    void slotChackedItemsUpdated();                             //список выбранных элементов изменился

private:
    CheckListModel *listModel;              //модель списка
    QString displayText;                    //отображемый список в строку
    const QRect displayRectDelta;           //дельта отображаемого списка (для отрисовки)

    void updateDisplayText();               //обновить список в строке
};

#endif
