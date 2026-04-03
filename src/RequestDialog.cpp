#include "RequestDialog.h"
#include "ButtonRequests.h"
#include "BaseRequestButton.h"


RequestDialog::RequestDialog(QWidget *pwgt, ServerRequester* server_requester): QDialog(pwgt), server_requester(server_requester) {
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

    browser = new QTextBrowser(this);
    baseLayout->addWidget(browser);

    QPushButton* cancelButton = new QPushButton("Закрыть");
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    QHBoxLayout* formLayout = new QHBoxLayout(this);
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

void RequestDialog::requestRouting(ActionId buttonId) {
    switch (buttonId) {
        case ActionId::ADD_TEMPLATE:
            addTemplate(this, server_requester);
            break;
        case ActionId::ADD_TAG:
            addTag(this, server_requester);
            break;
        case ActionId::REMOVE_TEMPLATE:
            removeTemplate(this, server_requester);
            break;
        case ActionId::REMOVE_TAG:
            removeTag(this, server_requester);
            break;
        case ActionId::EXPORT:
            break;
        case ActionId::GET_TEMPLATE:
            getTemplate(this, server_requester);
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