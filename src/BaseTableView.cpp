#include "BaseTableView.h"
#include <QHeaderView>
#include <QStyleFactory>
#include <QFile>
#include "RowDelegate.h"

BaseTableView::BaseTableView(QWidget *pwgt, QStandardItemModel* itemModel, QColor rowColor, bool editable): QTableView(pwgt) {

    
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

    setMouseTracking(true);
    QFile styleFile(":/styles/tables.qss");
    QString styles;
    if (styleFile.open(QFile::ReadOnly)){
        styles += styleFile.readAll();
    }
    setStyleSheet(styles);
    auto* rowDelegate = new RowDelegate(this, rowColor, editable);
    setItemDelegate(rowDelegate);
    if (editable) {
        connect(
            rowDelegate,
            &RowDelegate::removeRequested,
            this,
            [this, itemModel](int row)
            {
                QModelIndex idx = itemModel->index(row, 0);

                QString templateId = itemModel->data(idx).toString();
                itemModel->removeRow(row);
                emit deleteTemplate(templateId);
            }
        );

    }
}