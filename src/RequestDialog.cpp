#include "RequestDialog.h"
#include "ButtonRequests.h"
#include "BaseRequestButton.h"
#include "PrintableReport.h"


RequestDialog::RequestDialog(QWidget *pwgt, ServerRequester* server_requester): BaseDialog(pwgt), server_requester(server_requester) {
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    button_group = new QButtonGroup(this); 
    QHBoxLayout* layout = new QHBoxLayout(this);

    for (const auto& [buttonId, buttonLabel] : ACTIONS_MAP) {
        auto button = new BaseRequestButton(
            this, 
            server_requester, 
            buttonLabel
        );
        layout->addWidget(button);
        button_group->addButton(button, static_cast<int>(buttonId));
    }

    connect(
        button_group, 
        SIGNAL(buttonClicked(QAbstractButton*)), 
        this, 
        SLOT(handleButtonClicked(QAbstractButton*))
    );
    baseLayout->addLayout(layout);

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

void RequestDialog::requestRouting(ActionId buttonId) {
    switch (buttonId) {
        case ActionId::ADD_TEMPLATE:
            baseRequest<AddTemplateDialog>(this, server_requester, addTemplate);
            break;
        case ActionId::ADD_TAG:
            baseRequest<TagDialog>(this, server_requester, addTag);
            break;
        case ActionId::REMOVE_TEMPLATE:
            baseRequest<GetElementDialog>(this, server_requester, removeTemplate);
            break;
        case ActionId::REMOVE_TAG:
            baseRequest<GetElementDialog>(this, server_requester, removeTag);
            break;
        case ActionId::EXPORT:
            baseRequest<GetElementDialog>(this, server_requester, exportTemplate);
            break;
        case ActionId::GET_TEMPLATE:
            baseRequest<GetElementDialog>(this, server_requester, getTemplate);
            break;
        case ActionId::GET_ALL_TEMPLATES:
            server_requester->getAllTemplates();
            break;
        case ActionId::GET_ALL_TAGS:
            server_requester->getAllTags();
            break;
    }
}

void RequestDialog::handleButtonClicked(QAbstractButton* button) {
    BaseRequestButton* requestButton = qobject_cast<BaseRequestButton*>(button);
    if (requestButton){
        int id = button_group->id(requestButton);
        ActionId buttonId = static_cast<ActionId>(id);
        requestButton->setCurrentButton(buttonId);
        requestRouting(buttonId);
    }
}

RequestDialog::ContentModel RequestDialog::responseRouting(const QJsonDocument jsonDoc, ActionId buttonId) {
    ContentModel response;
    response.content_type = ContentType::TEXT;
    switch (buttonId) {
        case ActionId::ADD_TEMPLATE:
            response.content = QString(AddTemplateLabel);
            break;
        case ActionId::ADD_TAG:
            response.content = QString(AddTagLabel);
            break;
        case ActionId::REMOVE_TEMPLATE:
            response.content = QString(RemoveTemplateLabel);
            break;
        case ActionId::REMOVE_TAG:
            response.content = QString(RemoveTagLabel);

            break;
        case ActionId::EXPORT:
        case ActionId::GET_TEMPLATE:
            response.content = jsonDoc.object().value("content").toString();
            response.content_type = ContentType::HTML;
            break;
        case ActionId::GET_ALL_TEMPLATES:
            updateTable(jsonDoc.array(), templatesModel);
            response.content_type = ContentType::EMPTY;
            break;
        case ActionId::GET_ALL_TAGS:
            updateTable(jsonDoc.array(), tagsModel);
            response.content_type = ContentType::EMPTY;
            break;
    }
    return response;
}

void RequestDialog::showResponseSlot(int httpStatus, const QJsonDocument jsonDoc, ActionId buttonId) {
    ContentModel response = responseRouting(jsonDoc, buttonId);
    if (response.content_type == ContentType::HTML) {
        PrintableReport* printableReport = new PrintableReport(QPrinter::HighResolution, response.content, this);
        printableReport->preview(nullptr, PrintTitle);
    } else if (response.content_type == ContentType::TEXT) {
        QMessageBox::information(this, "ResultTitle", response.content);
    }
}

void RequestDialog::getErrorRequestSlot(QString message, int httpStatus){
    QMessageBox::critical(this, ErrorTitle, message);
}