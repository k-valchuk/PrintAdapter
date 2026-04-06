#include "GetElementDialog.h"
#include "Config.h"

GetElementDialog::GetElementDialog(QWidget* pwgt) : BaseDialog(pwgt) {
    QLabel* templateNameLabel = new QLabel(ElementIdLabel);
    templateId = new QLineEdit();
    templateNameLabel->setBuddy(templateId);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(templateNameLabel);
    layout->addWidget(templateId);
    setBaseLayout(layout);
}

QString GetElementDialog::getContent() const {
    return templateId->text();
}