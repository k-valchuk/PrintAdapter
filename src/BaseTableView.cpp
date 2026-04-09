#include "BaseTableView.h"
#include <QHeaderView>

BaseTableView::BaseTableView(QWidget *pwgt, QStandardItemModel* itemModel): QTableView(pwgt) {
    itemModel->setHorizontalHeaderLabels({"ID", "Название"});
    setModel(itemModel);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    verticalHeader()->hide();
}