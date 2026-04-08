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

    table = new QTableWidget(this);
    table->setColumnCount(2);
    QStringList headers;
    headers << "ID" << "Название";
    table->setHorizontalHeaderLabels(headers);
    baseLayout->addWidget(table);

    connect(
        table,
        SIGNAL(cellClicked(int, int)),
        this,
        SLOT(onCellClicked(int, int))
    );

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

void RequestDialog::onCellClicked(int row, int column) {
    int idColumn = 0;
    QTableWidgetItem *idItem = table->item(row, idColumn);
    if (idItem) {
        QApplication::clipboard()->setText(idItem->text());
    }
}

void RequestDialog::updateTable(QJsonArray value_array) {
    table->clearContents();
    int rowCount = table->rowCount();
    int i = 0;
    for (const QJsonValue& value : value_array) {
        if (i >= rowCount) {
            table->insertRow(rowCount);
            rowCount = table->rowCount();
        }
        table->setItem(
            i, 0, 
            new QTableWidgetItem(QString::number(value.toObject().value("element_id").toInt()))
        );
        table->setItem(
            i, 1, 
            new QTableWidgetItem(value.toObject().value("element_name").toString())
        );
        i += 1;
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
            response.content = QString("Добавлен шаблон");
            break;
        case ActionId::ADD_TAG:
            response.content = QString("Добавлен тэг");
            break;
        case ActionId::REMOVE_TEMPLATE:
            response.content = QString("Убран шаблон");
            break;
        case ActionId::REMOVE_TAG:
            response.content = QString("Убран тэг");
            break;
        case ActionId::EXPORT:
        case ActionId::GET_TEMPLATE:
            response.content = jsonDoc.object().value("content").toString();
            response.content_type = ContentType::HTML;
            break;
        case ActionId::GET_ALL_TEMPLATES:
            updateTable(jsonDoc.array());
            response.content_type = ContentType::EMPTY;
            break;
    }
    return response;
}

void RequestDialog::showResponseSlot(int httpStatus, const QJsonDocument jsonDoc, ActionId buttonId) {
    ContentModel response = responseRouting(jsonDoc, buttonId);
    if (response.content_type == ContentType::HTML) {
        PrintableReport* printableReport = new PrintableReport(QPrinter::HighResolution, response.content, this);
        printableReport->preview(nullptr, "Печать");
    } else if (response.content_type == ContentType::TEXT) {
        QMessageBox::information(this, "Результат", response.content);
    }
}

void RequestDialog::getErrorRequestSlot(QString message, int httpStatus){
    QMessageBox::critical(this, "Ошибка", message);
}