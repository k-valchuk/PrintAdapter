#include "RequestDialog.h"
#include "ExportButton.h"
#include "GetAllTemplatesButton.h"
#include "GetTemplatteButton.h"
#include "AddTemplateButton.h"



QHBoxLayout* createButtons(RequestDialog* requestDialog, ServerRequester* server_requester) {
    QHBoxLayout* layout = new QHBoxLayout;

    ExportButton* exportBtn = new ExportButton(requestDialog, server_requester);
    GetTemplateButton* getTemplateBtn = new GetTemplateButton(requestDialog, server_requester);
    QPushButton* allTemplateBtn = new GetAllTemplatesButton(requestDialog, server_requester);
    QPushButton* addTemplateBtn = new AddTemplateButton(requestDialog, server_requester);

    layout->addWidget(exportBtn);
    layout->addWidget(getTemplateBtn);
    layout->addWidget(allTemplateBtn);
    layout->addWidget(addTemplateBtn);

    QObject::connect(
        allTemplateBtn, 
        SIGNAL(done(QString, ContentType)), 
        requestDialog, 
        SLOT(changeBrowserContent(QString, ContentType))
    );

    QObject::connect(
        getTemplateBtn, 
        SIGNAL(done(QString, ContentType)), 
        requestDialog, 
        SLOT(changeBrowserContent(QString, ContentType))
    );

    QObject::connect(
        addTemplateBtn, 
        SIGNAL(done(QString, ContentType)), 
        requestDialog, 
        SLOT(changeBrowserContent(QString, ContentType))
    );

    QObject::connect(
        exportBtn, 
        SIGNAL(done(QString, ContentType)), 
        requestDialog, 
        SLOT(changeBrowserContent(QString, ContentType))
    );
    return layout;
}

RequestDialog::RequestDialog(QWidget *pwgt, ServerRequester* server_requester): QDialog(pwgt), server_requester(server_requester) {
    
    QPushButton* cancelButton = new QPushButton("Закрыть");
    browser = new QTextBrowser(this);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    QVBoxLayout* baseLayout = new QVBoxLayout;

    QHBoxLayout* formLayout = new QHBoxLayout;
    formLayout->addWidget(cancelButton);
    baseLayout->addLayout(createButtons(this, server_requester));
    baseLayout->addWidget(browser);
    baseLayout->addLayout(formLayout);

    setLayout(baseLayout);
}

void RequestDialog::changeBrowserContent(QString content, ContentType content_type) {
    if (content_type == ContentType::HTML) {
        browser->setHtml(content);
    } else if (content_type == ContentType::TEXT) {
        browser->setText(content);
    }
}