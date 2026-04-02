#include "RequestDialog.h"
#include "ExportButton.h"
#include "GetAllTemplatesButton.h"
#include "GetTemplatteButton.h"
#include "AddTemplateButton.h"
#include "AddTagButton.h"
#include "RemoveTemplateButton.h"
#include "RemoveTagButton.h"


RequestDialog::RequestDialog(QWidget *pwgt, ServerRequester* server_requester): QDialog(pwgt), server_requester(server_requester) {
    QVBoxLayout* baseLayout = new QVBoxLayout;
    button_group = new QButtonGroup(this); 
    QHBoxLayout* layout = new QHBoxLayout;

    ExportButton* exportButton = new ExportButton(
        this, server_requester, layout, button_group, 
        ActionId::EXPORT
    );
    GetTemplateButton* getTemplateButton = new GetTemplateButton(
        this, server_requester, layout, button_group, 
        ActionId::GET_TEMPLATE
    );
    GetAllTemplatesButton* getAllTemplatesButton = new GetAllTemplatesButton(
        this, server_requester, layout, button_group,
        ActionId::GET_ALL_TEMPLATES
    );
    AddTemplateButton* addTemplateButton = new AddTemplateButton(
        this, server_requester, layout, button_group, 
        ActionId::ADD_TEMPLATE
    );
    RemoveTemplateButton* removeTemplateButton = new RemoveTemplateButton(
        this, server_requester, layout, button_group,
        ActionId::REMOVE_TEMPLATE
    );
    AddTagButton* addTagButton = new AddTagButton(
        this, server_requester, layout, button_group,
        ActionId::ADD_TAG
    );
    RemoveTagButton* removeTagButton = new RemoveTagButton(
        this, server_requester, layout, button_group,
        ActionId::REMOVE_TAG
    );

    connect(
        button_group, 
        SIGNAL(buttonClicked(QAbstractButton*)), 
        this, 
        SLOT(handleButtonClicked(QAbstractButton*))
    );
    baseLayout->addLayout(layout);

    browser = new QTextBrowser(this);
    baseLayout->addWidget(browser);

    QPushButton* cancelButton = new QPushButton("Закрыть");
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    QHBoxLayout* formLayout = new QHBoxLayout;
    formLayout->addWidget(cancelButton);
    baseLayout->addLayout(formLayout);

    setLayout(baseLayout);

    connect(
        server_requester, 
        SIGNAL(done(int, const QJsonDocument, ActionId)), 
        this, 
        SLOT(changeBrowserContent(int, const QJsonDocument, ActionId))
    );
    connect(
        server_requester, 
        SIGNAL(error(QString, int)), 
        this, 
        SLOT(getErrorRequestSlot(QString, int))
    );

}

void RequestDialog::handleButtonClicked(QAbstractButton* button) {
    BaseRequestButton* requestButton = qobject_cast<BaseRequestButton*>(button);
    if (requestButton){
        int id = button_group->id(requestButton);
        ActionId buttonId = static_cast<ActionId>(id);
        requestButton->sendRequest(buttonId);
    }
}

RequestDialog::ContentModel RequestDialog::responseRouting(const QJsonDocument jsonDoc, ActionId buttonId) {
    ContentModel response;
    switch (buttonId) {
        case ActionId::ADD_TEMPLATE:
            response.content = QString("Добавлен шаблон");
            response.content_type = ContentType::TEXT;
            break;
        case ActionId::ADD_TAG:
            response.content = QString("Добавлен тэг");
            response.content_type = ContentType::TEXT;
            break;
        case ActionId::REMOVE_TEMPLATE:
            response.content = QString("Убран шаблон");
            response.content_type = ContentType::TEXT;
            break;
        case ActionId::REMOVE_TAG:
            response.content = QString("Убран тэг");
            response.content_type = ContentType::TEXT;
            break;
        case ActionId::EXPORT:
        case ActionId::GET_TEMPLATE:
            response.content = jsonDoc.object().value("content").toString();
            response.content_type = ContentType::HTML;
            break;
        case ActionId::GET_ALL_TEMPLATES:
            const QJsonArray root = jsonDoc.array();
            QVector<QString> content;
            for (const QJsonValue& value : root) {
                content.append(value.toString());
            }
            response.content = content.toList().join("\n");
            response.content_type = ContentType::TEXT;
            break;
    }
    return response;
}

void RequestDialog::changeBrowserContent(int httpStatus, const QJsonDocument jsonDoc, ActionId buttonId) {
    ContentModel response = responseRouting(jsonDoc, buttonId);
    if (response.content_type == ContentType::HTML) {
        browser->setHtml(response.content);
    } else if (response.content_type == ContentType::TEXT) {
        browser->setText(response.content);
    }
}

void RequestDialog::getErrorRequestSlot(QString message, int httpStatus){
    browser->setText(message);
}