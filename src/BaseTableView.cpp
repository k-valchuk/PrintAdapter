#include "BaseTableView.h"
#include <QHeaderView>
#include <QStyleFactory>

BaseTableView::BaseTableView(QWidget *pwgt, QStandardItemModel* itemModel): QTableView(pwgt) {
    itemModel->setHorizontalHeaderLabels({"ID", "Название"});
    setModel(itemModel);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    verticalHeader()->hide();
    setStyle(QStyleFactory::create("Fusion"));

    QPalette palette;
    palette.setColor(QPalette::Base, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    setPalette(palette);

    setStyleSheet("gridline-color: #444; border: none;");

    horizontalHeader()->setStyleSheet(
        "QHeaderView::section { background-color: #3d3d3d; color: white; border: 1px solid #555; padding: 4px; }"
    );
    verticalHeader()->setStyleSheet(
        "QHeaderView::section { background-color: #3d3d3d; color: white; }"
    );
}