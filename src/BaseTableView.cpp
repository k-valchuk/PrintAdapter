#include "BaseTableView.h"
#include <QHeaderView>
#include <QStyleFactory>

BaseTableView::BaseTableView(QWidget *pwgt, QStandardItemModel* itemModel): QTableView(pwgt) {
    horizontalHeader()->hide();

    setModel(itemModel);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    verticalHeader()->hide();

    QPalette palette;
    palette.setColor(QPalette::Base, QColor(45, 44, 50));
    palette.setColor(QPalette::Text, QColor(143, 143, 145));
    setPalette(palette);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        this, SIGNAL(customContextMenuRequested(const QPoint)),
        pwgt, SLOT(templateContextMenu(const QPoint))
    );

    setStyleSheet("border: none;");

    verticalHeader()->setStyleSheet(
        "QHeaderView::section { background-color: #2b2d32; color: #9d9d9f; }"
    );
}