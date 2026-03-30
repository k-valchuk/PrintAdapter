#include "ExportDialog.h"
#include <QtWidgets>

ExportDialog::ExportDialog(QWidget* pwgt): QDialog(pwgt) {
    QLabel* templateNameLabel = new QLabel("Имя шаблона");
    templateName = new QLineEdit();
    templateNameLabel->setBuddy(templateName);

    requestBody = new QPlainTextEdit();
    QVBoxLayout* layout = new QVBoxLayout;
    QPushButton* okButton = new QPushButton("Ок");

    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    layout->addWidget(templateNameLabel);
    layout->addWidget(templateName);
    layout->addWidget(requestBody);
    layout->addWidget(okButton);
    setLayout(layout);
}

QString ExportDialog::getContent() const {
    return requestBody->toPlainText();
}

QString ExportDialog::getName() const {
    return templateName->text();
}