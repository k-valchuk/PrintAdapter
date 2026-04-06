#include "GetTemplateDialog.h"

GetTemplateDialog::GetTemplateDialog(QWidget* pwgt) : QDialog(pwgt) {
    QLabel* templateNameLabel = new QLabel("Id элемента");
    templateId = new QLineEdit();
    templateNameLabel->setBuddy(templateId);

    QVBoxLayout* layout = new QVBoxLayout;
    QPushButton* okButton = new QPushButton("Ок");
    QPushButton* cancelButton = new QPushButton("Отмена");

    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));

    layout->addWidget(templateNameLabel);
    layout->addWidget(templateId);
    QVBoxLayout* horizontal_layout = new QVBoxLayout;
    horizontal_layout->addWidget(okButton);
    horizontal_layout->addWidget(cancelButton);
    layout->addLayout(horizontal_layout);
    setLayout(layout);
}

QString GetTemplateDialog::getContent() const {
    return templateId->text();
}