#include "RequestDialog.h"
#include "ButtonRequests.h"
#include "PrintableReport.h"


RequestDialog::RequestDialog(QWidget *pwgt, ServerRequester* server_requester_): BaseDialog(pwgt), server_requester(server_requester_) {
    QVBoxLayout* baseLayout = new QVBoxLayout(this);

    templatesModel = new QStandardItemModel(0, 2, this);
    templateTableView = new BaseTableView(this, templatesModel);

    tagsModel = new QStandardItemModel(0, 2, this);
    tagTableView = new BaseTableView(this, tagsModel);
    
    QHBoxLayout* table_layout = new QHBoxLayout(this);
    table_layout->addWidget(templateTableView);
    table_layout->addWidget(tagTableView);
    baseLayout->addLayout(table_layout);

    setBaseLayout(baseLayout);

    connect(
        server_requester, 
        SIGNAL(done(int, const QJsonDocument, ActionId)), 
        this, 
        SLOT(showResponseSlot(int, const QJsonDocument, ActionId))
    );
    connect(
        server_requester, 
        SIGNAL(error(QString, int)), 
        this, 
        SLOT(getErrorRequestSlot(QString, int))
    );

    templateTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tagTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
        templateTableView, &QTableView::customContextMenuRequested,
        this, &RequestDialog::templateContextMenu
    );
    connect(
        tagTableView, &QTableView::customContextMenuRequested,
        this, &RequestDialog::tagContextMenu
    );

    server_requester->getAllTemplates();
    QTimer::singleShot(150, this, [this]() {
        server_requester->getAllTags();
    });

    applyTheme();
}

void RequestDialog::applyTheme() {
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    setPalette(darkPalette);
}

void RequestDialog::updateTable(QJsonArray value_array, QStandardItemModel* itemModel) {
    int i = 0;
    int rowsCount = itemModel->rowCount(); 
    for (const QJsonValue& value : value_array) {
        QList<QStandardItem*> rowData;
        rowData << new QStandardItem(QString::number(value.toObject().value("element_id").toInt()));
        rowData << new QStandardItem(value.toObject().value("element_name").toString());
        if (i >= rowsCount) {
            itemModel->appendRow(rowData);
        } else {
            itemModel->removeRow(i);
            itemModel->insertRow(i, rowData);
        }
        i++;
    }
}

void RequestDialog::templateContextMenu(const QPoint &pos) {
    QModelIndex index = templateTableView->indexAt(pos);
    QString dbId = templateTableView->model()->data(templateTableView->model()->index(index.row(), 0)).toString();
    if (!index.isValid()) return;
    QMenu menu(this);
    menu.addAction(
        "Добавить Шаблон",
        this,
        [this]{
            server_requester->setCurrentButton(ActionId::ADD_TEMPLATE);
            baseRequest<AddTemplateDialog>(this, server_requester, addTemplate);
        }
    );
    menu.addAction(
        "Показать Шаблон",
        this,
        [this, dbId]{
            server_requester->setCurrentButton(ActionId::GET_TEMPLATE);
            server_requester->getTemplate(dbId);
        }
    );
    menu.addAction(
        "Экспорт Шаблона",
        this,
        [this, dbId]{
            server_requester->setCurrentButton(ActionId::GET_TEMPLATE);
            server_requester->exportTemplate(dbId.toInt());
        }
    );
    menu.addAction(
        "Удалить Шаблон",
        this,
        [this, dbId]{
            server_requester->setCurrentButton(ActionId::REMOVE_TEMPLATE);
            server_requester->removeTemplate(dbId);
        }
    );

    menu.exec(templateTableView->viewport()->mapToGlobal(pos));
}

void RequestDialog::tagContextMenu(const QPoint &pos) {
    QModelIndex index = tagTableView->indexAt(pos);
    QString dbId = tagTableView->model()->data(tagTableView->model()->index(index.row(), 0)).toString();
    if (!index.isValid()) return;
    QMenu menu(this);
    menu.addAction(
        "Добавить Тэг",
        this,
        [this]{
            server_requester->setCurrentButton(ActionId::ADD_TAG);
            baseRequest<TagDialog>(this, server_requester, addTag);
        }
    );
    menu.addAction(
        "Удалить Тэг",
        this,
        [this, dbId]{
            server_requester->setCurrentButton(ActionId::REMOVE_TAG);
            server_requester->removeTag(dbId);
        }
    );
    menu.exec(tagTableView->viewport()->mapToGlobal(pos));
}

void RequestDialog::removeRowByID(int itemID, QStandardItemModel* itemModel){
    for (int i = 0; i < itemModel->rowCount(); i++) {
        if (itemModel->data(itemModel->index(i, 0)).toInt() == itemID) {
            itemModel->removeRow(i);
            break;
        }
    }
}

RequestDialog::ContentModel RequestDialog::responseRouting(const QJsonDocument jsonDoc, ActionId buttonId) {
    ContentModel response;
    response.content_type = ContentType::TEXT;
    QList<QStandardItem*> rowData{};
    switch (buttonId) {
        case ActionId::ADD_TEMPLATE:
            rowData << new QStandardItem(QString::number(jsonDoc.object().value("ID").toInt()));
            rowData << new QStandardItem(jsonDoc.object().value("name").toString());
            templatesModel->appendRow(rowData);
            break;
        case ActionId::ADD_TAG:
            rowData << new QStandardItem(QString::number(jsonDoc.object().value("ID").toInt()));
            rowData << new QStandardItem(jsonDoc.object().value("name").toString());
            tagsModel->appendRow(rowData);
            break;
        case ActionId::REMOVE_TEMPLATE:
            removeRowByID(
                jsonDoc.object().value("ID").toInt(),
                templatesModel
            );
            break;
        case ActionId::REMOVE_TAG:
            removeRowByID(
                jsonDoc.object().value("ID").toInt(),
                tagsModel
            );
            break;
        case ActionId::EXPORT:
        case ActionId::GET_TEMPLATE:
            response.content = jsonDoc.object().value("content").toString();
            response.content_type = ContentType::HTML;
            break;
        case ActionId::GET_ALL_TEMPLATES:
            updateTable(jsonDoc.array(), templatesModel);
            break;
        case ActionId::GET_ALL_TAGS:
            updateTable(jsonDoc.array(), tagsModel);
            break;
    }
    return response;
}

void RequestDialog::showResponseSlot(int httpStatus, const QJsonDocument jsonDoc, ActionId buttonId) {
    ContentModel response = responseRouting(jsonDoc, buttonId);
    if (response.content_type == ContentType::HTML) {
        PrintableReport* printableReport = new PrintableReport(QPrinter::HighResolution, response.content, this);
        printableReport->preview(nullptr, PrintTitle);
    }
}

void RequestDialog::getErrorRequestSlot(QString message, int httpStatus){
    QMessageBox::critical(this, ErrorTitle, message);
}