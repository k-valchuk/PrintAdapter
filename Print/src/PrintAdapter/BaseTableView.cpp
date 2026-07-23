#include "BaseTableView.h"
#include <QHeaderView>
#include <QStyleFactory>
#include <QFile>
#include "RowDelegate.h"
#include "RemoveConfirmDialog.h"

BaseTableView::BaseTableView(QWidget *pwgt, QStandardItemModel* itemModel, BaseDelegate* itemDelegate): QTableView(pwgt) {

    
    setModel(itemModel);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(selectionModel(), &QItemSelectionModel::selectionChanged, viewport(), qOverload<>(&QWidget::update));
    setSelectionMode(QAbstractItemView::SingleSelection);
    
    resizeRowsToContents();
    
    setFrameShape(QFrame::NoFrame);
    setShowGrid(false);
    horizontalHeader()->setVisible(true);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); 
    verticalHeader()->setDefaultSectionSize(34);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);

    setMouseTracking(true);
    QFile styleFile(":/styles/tables.qss");
    QString styles;
    if (styleFile.open(QFile::ReadOnly)){
        styles += styleFile.readAll();
    }
    setStyleSheet(styles);
    itemDelegate->setParent(this);
    setItemDelegate(itemDelegate);
    connect(
        itemDelegate,
        &BaseDelegate::removeRequested,
        this,
        [this, itemModel](int row)
        {
            QString templateName = itemModel->data(itemModel->index(row, 1)).toString();
            RemoveConfirmDialog* pRemoveConfirmDialog = new RemoveConfirmDialog(this, templateName);
            if (pRemoveConfirmDialog->exec() == QDialog::Rejected) {
                return;
            }
                
            QModelIndex idx = itemModel->index(row, 0);

            QString templateId = itemModel->data(idx).toString();
            itemModel->removeRow(row);
            emit deleteTemplate(templateId);
        }
    );


}