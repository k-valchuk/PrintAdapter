#include "checkCombobox.h"
#include "QStylePainter"
#include "QAbstractItemView"

CheckCombobox::CheckCombobox(QWidget *parent) : QComboBox(parent), displayRectDelta(4, 1, -25, 0)
{
    listModel = new CheckListModel();
    setModel(listModel);

    //нажатие на текст итема, а не на галку
    connect(this->view(), SIGNAL(pressed(QModelIndex)), this, SLOT(slotItemTextClicked(QModelIndex)));
    //изменился список выбранных элементов
    connect(listModel, SIGNAL(checkedItemsUpdated()), this, SLOT(slotChackedItemsUpdated()));
}

CheckCombobox::~CheckCombobox()
{
}

QStringList CheckCombobox::checkedItems() const
{
    return listModel->getCheckedItems();
}

void CheckCombobox::setCheckedItems(const QStringList &items)
{
    listModel->setCheckedItems(items);
}

void CheckCombobox::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));
    QStyleOptionComboBox option;
    initStyleOption(&option);
    painter.drawComplexControl(QStyle::CC_ComboBox, option);
    QRect textRect = rect().adjusted(displayRectDelta.left(), displayRectDelta.top(),
                                     displayRectDelta.right(), displayRectDelta.bottom());
    painter.drawText(textRect, Qt::AlignVCenter, displayText);
}

void CheckCombobox::resizeEvent(QResizeEvent *)
{
    updateDisplayText();
}

void CheckCombobox::updateDisplayText()
{
    QRect textRect = rect().adjusted(displayRectDelta.left(), displayRectDelta.top(),
                                     displayRectDelta.right(), displayRectDelta.bottom());
    QFontMetrics fontMetrics(font());
    displayText = listModel->getCheckedItems().join("; ");
    displayText = fontMetrics.elidedText(displayText, Qt::TextElideMode::ElideRight, textRect.width());
}

void CheckCombobox::slotItemTextClicked(const QModelIndex& ind)
{//нажатие на текст итема
    QStandardItem *currentItem = listModel->item(ind.row());
    Qt::CheckState checkState = static_cast<Qt::CheckState>(currentItem->data(Qt::CheckStateRole).toInt());
    checkState = (checkState == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
    currentItem->setData(checkState, Qt::CheckStateRole);
}

void CheckCombobox::slotChackedItemsUpdated()
{
    updateDisplayText();
    repaint();
    emit checkItemsChanged(listModel->getCheckedItems());
}

//*********     Модель    *****************//

CheckListModel::CheckListModel(QObject *parent) : QStandardItemModel (parent)
{
    //изменения в списке
    connect(this, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(slotModelRowsInserted(QModelIndex,int,int)));
    connect(this, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(slotModelRowsRemoved(QModelIndex,int,int)));
    connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotModelItemChanged(QStandardItem*)));
}

void CheckListModel::setCheckedItems(const QStringList &items)
{
    blockSignals(true);

    //отмечаем выбранные
    for (auto &it : items)
    {
        for (int i = 0; i < rowCount(); ++i)
        {
            auto modelItem = item(i);
            if (modelItem->text() == it)
                modelItem->setData(Qt::Checked, Qt::CheckStateRole);
        }
    }

    blockSignals(false);

    collectCheckedItems();//обновить список
}

void CheckListModel::slotModelRowsInserted(const QModelIndex &, int start, int end)
{
    blockSignals(true);

    //возможность checkbox
    for (int i = start; i <= end; ++i)
    {
        item(i)->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item(i)->setData(Qt::Unchecked, Qt::CheckStateRole);
    }

    blockSignals(false);

}

void CheckListModel::slotModelRowsRemoved(const QModelIndex &, int , int )
{
    collectCheckedItems();
}

void CheckListModel::slotModelItemChanged(QStandardItem *)
{
    collectCheckedItems();
}

void CheckListModel::collectCheckedItems()
{
    checkedItems.clear();

    for (int i = 0; i < rowCount(); ++i)
    {
        QStandardItem *currentItem = item(i);

        Qt::CheckState checkState = static_cast<Qt::CheckState>(currentItem->data(Qt::CheckStateRole).toInt());

        if (checkState == Qt::Checked)
        {
            checkedItems.push_back(currentItem->text());
        }
    }

    emit checkedItemsUpdated();
}
