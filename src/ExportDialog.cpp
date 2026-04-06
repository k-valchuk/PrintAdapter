#include "ExportDialog.h"
#include "Config.h"
#include <QtWidgets>

ExportDialog::ExportDialog(QWidget* pwgt): BaseDialog(pwgt) {
    QLabel* templateIdLabel = new QLabel(TemplateIdLabel);
    templateId = new QLineEdit();
    templateIdLabel->setBuddy(templateId);

    requestBody = new QPlainTextEdit();
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(templateIdLabel);
    layout->addWidget(templateId);
    layout->addWidget(requestBody);
    setBaseLayout(layout);
}

QString ExportDialog::getContent() const {
    return requestBody->toPlainText();
}

QString ExportDialog::getName() const {
    return templateId->text();
}