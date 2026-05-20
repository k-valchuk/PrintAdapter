#include "BaseTableView.h"
#include <QHeaderView>
#include <QStyleFactory>
#include <QFile>
#include "RowDelegate.h"

BaseTableView::BaseTableView(QWidget *pwgt, QStandardItemModel* itemModel, QColor rowColor): QTableView(pwgt) {

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

    QFile styleFile(":/styles/tables.qss");
    QString styles;
    if (styleFile.open(QFile::ReadOnly)){
        styles += styleFile.readAll();
    }
    setStyleSheet(styles);
    setItemDelegate(new RowDelegate(this, rowColor));
}