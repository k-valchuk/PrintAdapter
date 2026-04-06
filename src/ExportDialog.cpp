#include "ExportDialog.h"
#include <QtWidgets>

ExportDialog::ExportDialog(QWidget* pwgt): QDialog(pwgt) {
    QLabel* templateIdLabel = new QLabel("Id шаблона");
    templateId = new QLineEdit();
    templateIdLabel->setBuddy(templateId);

    requestBody = new QPlainTextEdit();
    QVBoxLayout* layout = new QVBoxLayout;
    QPushButton* okButton = new QPushButton("Ок");

    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    layout->addWidget(templateIdLabel);
    layout->addWidget(templateId);
    layout->addWidget(requestBody);
    layout->addWidget(okButton);
    setLayout(layout);
}

QString ExportDialog::getContent() const {
    return requestBody->toPlainText();
}

QString ExportDialog::getName() const {
    return templateId->text();
}