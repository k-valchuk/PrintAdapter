#include "TemplateManager.h"
#include "AddLine.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QScrollArea>
#include "RowDelegate.h"

TemplateManager::TemplateManager(QJsonArray tableNames, QWidget* pwgt): QWidget(pwgt) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 20, 0);
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);
    QWidget *scrollContent = new QWidget(scrollArea);
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    for (const QJsonValue& value : tableNames) {
        QJsonObject value_object = value.toObject();
        QString tableLabel = value_object.value("label").toString();
        QString tableType = value_object.value("name").toString();

        QWidget *blockWidget = new QWidget(scrollContent);
        QVBoxLayout *blockLayout = new QVBoxLayout(blockWidget);
        blockLayout->setContentsMargins(0, 0, 0, 0);
        blockLayout->setSpacing(0);

        AddLine* addLine = new AddLine(blockWidget, tableLabel);
        blockLayout->addWidget(addLine);

        QStandardItemModel* tableModel = new QStandardItemModel(0, 2, this);
        m_models.append(tableModel);
        BaseTableView* tableView = new BaseTableView(this, tableModel, new EditableRowDelegate(nullptr, QColor("#323236")));
        tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_allTableViews.append(tableView);
        blockLayout->addWidget(tableView);
        contentLayout->addWidget(blockWidget);

        tableView->setProperty("tableName", tableLabel);
        tableView->setProperty("type", tableType);

        // Соединение удаления шаблона
        connect(tableView, &BaseTableView::deleteTemplate, this, &TemplateManager::deleteTemplate);

        // Соединение добавления шаблона
        connect(addLine, &AddLine::addSignal, this, [tableModel, tableView, this](){
            int row = tableModel->rowCount();

            emit disableTemplateEdit(true);

            if (row > 0){
                tableModel->insertRow(row);
            } else {
                tableModel->insertRow(
                    row,
                    QList<QStandardItem*>{
                        new QStandardItem(),
                        new QStandardItem()
                    }
                );
            }
            editingIndex = tableModel->index(row, 1);

            clearAllSelections();

            emit setIndex(editingIndex);

            tableView->edit(editingIndex);
            tableView->scrollTo(editingIndex, QAbstractItemView::EnsureVisible);

        });

        // Соединение завершения редактирования
        connect(tableView->itemDelegate(),
            &QAbstractItemDelegate::closeEditor,
            this,
            [this, tableModel, tableView](QWidget*, QAbstractItemDelegate::EndEditHint){
                if (!editingIndex.isValid())
                    return;

                QString value =
                tableModel
                    ->data(
                        editingIndex,
                        Qt::EditRole
                    )
                    .toString()
                    .trimmed();
                
                if (value.isEmpty()) {
                    tableModel
                        ->removeRow(
                            editingIndex.row()
                    );
                }
        });

        // Соединение изменение/добавления шаблона
        connect(tableModel,
        &QAbstractItemModel::dataChanged,
        this,
        [this, tableModel, tableView](const QModelIndex& tl, const QModelIndex&){
            QString templateName = tableModel->index(tl.row(), 1).data().toString();
            if (templateName.isEmpty()) {
                return;
            }
            editingIndex = tl;
            emit setIndex(editingIndex);
            int templateId = tableModel->index(tl.row(), 0).data().toInt();
            templateId = templateId ? templateId : 0;
            emit changedTemplate(templateId, templateName, tableView->property("type").toString(), tableView);
        });

        // Соединение выбора только одной строчки из таблиц
        connect(tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, [this, tableView](const QItemSelection &selected, const QItemSelection &deselected) {
            if (selected.isEmpty()) return;

            for (BaseTableView *otherTable : m_allTableViews) {
                if (otherTable != tableView) {
                    otherTable->selectionModel()->blockSignals(true);
                    otherTable->clearSelection();
                    otherTable->selectionModel()->blockSignals(false);
                }
            }
            QModelIndex firstSelectedIndex = selected.indexes().first();
            int row = firstSelectedIndex.row();
            QModelIndex firstColumnIndex = firstSelectedIndex.sibling(row, 0);
            QString templateId = firstColumnIndex.data(Qt::DisplayRole).toString();

            QString currentTableName = tableView->property("tableName").toString();

            emit rowActivated(currentTableName, templateId, firstColumnIndex, tableView);
        });



        contentLayout->addStretch();
        scrollArea->setWidget(scrollContent);
    }
}

void TemplateManager::clearAllSelections() {
    for (BaseTableView *tableView : m_allTableViews) {
        tableView->clearSelection();
    }
}

void TemplateManager::updateTables(QJsonArray data) {
    for (BaseTableView *tableView : m_allTableViews) {
        tableView->selectionModel()->blockSignals(true);
        QStandardItemModel *tableModel = qobject_cast<QStandardItemModel*>(tableView->model());
        if (!tableModel) continue;
        tableModel->removeRows(0, tableModel->rowCount()); 
        QString currentTableType = tableView->property("type").toString();
        for (const QJsonValue& value : data) {
            auto value_object = value.toObject();
            if (value_object.value("type").toString() == currentTableType) {
                tableModel->appendRow({
                    new QStandardItem(QString::number(value_object.value("element_id").toInt())),
                    new QStandardItem(value_object.value("element_name").toString())
                });
            }
        }
        tableView->selectionModel()->blockSignals(false);
    }
}