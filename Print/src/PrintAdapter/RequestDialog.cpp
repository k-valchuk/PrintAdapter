#include "RequestDialog.h"
#include "PrintDialog.h"
#include "ShowRowDelegate.h"
#include <QDebug>


RequestDialog::RequestDialog(QWidget *pwgt, QJsonDocument json_doc, QString subsystemId_): BaseDialog(pwgt, "Выбор шаблона печати") {
    
    subsystemId = subsystemId_;
    server_requester = new ServerRequester(this, BASE_URL);
    server_requester->setExportJson(json_doc);
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 20, 5);

    templatesModel = new QStandardItemModel(0, 2, this);
    templateTableView = new BaseTableView(this, templatesModel, new ShowRowDelegate(nullptr, QColor("#3D3D41")));
    templateTableView->setColumnHidden(0, true);
    
    QHBoxLayout* table_layout = new QHBoxLayout(this);
    table_layout->addWidget(templateTableView);
    baseLayout->addLayout(table_layout);


    connect(
        server_requester, 
        SIGNAL(done(QJsonDocument, ActionId)), 
        this, 
        SLOT(showResponseSlot(QJsonDocument, ActionId))
    );
    connect(
        server_requester, 
        SIGNAL(error(QString, int)), 
        this, 
        SLOT(getErrorRequestSlot(QString, int))
    );

    

    server_requester->getAllTemplates(true, subsystemId);

    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setContentsMargins(0, 30, 0, 0);
    button_layout->addStretch();
    QPushButton* cancelButton = new QPushButton("Отмена", this);
    cancelButton->setObjectName("cancelButton");
    
    connect(
        cancelButton, SIGNAL(clicked()),
        this, SLOT(close())
    );
    
    QPushButton* chooseButton = new QPushButton("Выбрать");
    chooseButton->setObjectName("applyButton");

    connect(
        chooseButton, SIGNAL(clicked()),
        this, SLOT(printTemplate())
    );

    button_layout->addWidget(cancelButton);
    button_layout->addWidget(chooseButton);
    baseLayout->addLayout(button_layout);

    setBaseLayout(baseLayout);

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

void RequestDialog::printTemplate(){
    QModelIndex index = templateTableView->selectionModel()->currentIndex();
    QString dbId = templateTableView->model()->data(templateTableView->model()->index(index.row(), 0)).toString();
    server_requester->setTemplateName(templateTableView->model()->data(templateTableView->model()->index(index.row(), 1)).toString());
    server_requester->setCurrentButton(ActionId::EXPORT);
    server_requester->exportTemplate(dbId.toInt(), subsystemId);
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
    switch (buttonId) {
        case ActionId::EXPORT:
        case ActionId::GET_TEMPLATE:
            response.content = jsonDoc.object().value("content").toString();
            response.content_type = ContentType::HTML;
            response.title = jsonDoc.object().value("name").toString();
            break;
        case ActionId::GET_ALL_TEMPLATES:
            updateTable(jsonDoc.array(), templatesModel);
            break;
        default:
            break;
    }
    return response;
}

void RequestDialog::showResponseSlot(const QJsonDocument jsonDoc, ActionId buttonId) {
    ContentModel response = responseRouting(jsonDoc, buttonId);
    if (response.content_type == ContentType::HTML) {
        PrintDialog* dialog = new PrintDialog(nullptr, response.content, response.title);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->show();
    }
}

void RequestDialog::getErrorRequestSlot(QString message, int httpStatus){
    QMessageBox::critical(this, ErrorTitle, message);
}