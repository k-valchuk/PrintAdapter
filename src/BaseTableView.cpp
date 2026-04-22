#include "BaseTableView.h"
#include <QHeaderView>
#include <QStyleFactory>
#include "RowDelegate.h"

BaseTableView::BaseTableView(QWidget *pwgt, QStandardItemModel* itemModel): QTableView(pwgt) {

    setModel(itemModel);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(selectionModel(), &QItemSelectionModel::selectionChanged, viewport(), qOverload<>(&QWidget::update));
    setSelectionMode(QAbstractItemView::SingleSelection);
    
    setFrameShape(QFrame::NoFrame);
    setShowGrid(false);
    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);
    verticalHeader()->setDefaultSectionSize(34);

    connect(
        this, SIGNAL(customContextMenuRequested(const QPoint)),
        pwgt, SLOT(templateContextMenu(const QPoint))
    );

    setStyleSheet("border: none; background-color: transparent; font: normal normal normal 15px/18px Roboto;");
    setItemDelegate(new RowDelegate(this));
}