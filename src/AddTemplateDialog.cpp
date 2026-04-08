#include "AddTemplateDialog.h"
#include "Config.h"
#include <QtWidgets>

AddTemplateDialog::AddTemplateDialog(QWidget* pwgt): BaseDialog(pwgt) {
    QLabel* templateIdLabel = new QLabel(NameLabel);
    templateName = new QLineEdit();
    templateIdLabel->setBuddy(templateName);

    requestBody = new QPlainTextEdit();
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(templateIdLabel);
    layout->addWidget(templateName);
    layout->addWidget(requestBody);
    setBaseLayout(layout);
}

QString AddTemplateDialog::getContent() const {
    return requestBody->toPlainText();
}

QString AddTemplateDialog::getName() const {
    return templateName->text();
}