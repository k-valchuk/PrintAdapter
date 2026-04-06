#include "TagDialog.h"
#include "Config.h"
#include <QLabel>

TagDialog::TagDialog(QWidget* pwgt) : BaseDialog(pwgt) {
    QVBoxLayout* layout = new QVBoxLayout;

    QLabel* tagNameLabel = new QLabel(NameLabel);
    tagName = new QLineEdit;
    tagNameLabel->setBuddy(tagName);
    layout->addWidget(tagNameLabel);
    layout->addWidget(tagName);
    

    QLabel* tagDescriptionLabel = new QLabel(DescriptionLabel);
    tagDescription = new QPlainTextEdit;
    tagDescriptionLabel->setBuddy(tagDescription);
    layout->addWidget(tagDescriptionLabel);
    layout->addWidget(tagDescription);

    QLabel* tagSubsystemLabel = new QLabel(SubsystemLabel);
    tagSubsystem = new QLineEdit;
    tagSubsystemLabel->setBuddy(tagSubsystem);
    layout->addWidget(tagSubsystemLabel);
    layout->addWidget(tagSubsystem);

    QLabel* tagAliasLabel = new QLabel(AliasLabel);
    tagAlias = new QLineEdit;
    tagAliasLabel->setBuddy(tagAlias);
    layout->addWidget(tagAliasLabel);
    layout->addWidget(tagAlias);

    setBaseLayout(layout);
}

QString TagDialog::getName() const {
    return tagName->text();
}

QString TagDialog::getDescription() const {
    return tagDescription->toPlainText();
}

QString TagDialog::getSubsystem() const {
    return tagSubsystem->text();
}

QString TagDialog::getAlias() const {
    return tagAlias->text();
}