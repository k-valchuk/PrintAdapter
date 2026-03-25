#include "RequestDialog.h"
#include "ExportButton.h"

#include <QtWidgets>


QHBoxLayout* createButtons(RequestDialog* requestDialog) {
    QHBoxLayout* layout = new QHBoxLayout;

    ExportButton* exportBtn = new ExportButton;
    exportBtn->setText("Экспорт");
    QPushButton* getTemplateBtn = new QPushButton("Показать шаблон");
    QPushButton* allTemplateBtn = new QPushButton("Все шаблоны");
    QPushButton* addTemplateBtn = new QPushButton("Добавить шаблон");

    layout->addWidget(exportBtn);
    layout->addWidget(getTemplateBtn);
    layout->addWidget(allTemplateBtn);
    layout->addWidget(addTemplateBtn);

    QObject::connect(
        exportBtn, 
        SIGNAL(done(QString, ContentType)), 
        requestDialog, 
        SLOT(changeBrowserContent(QString, ContentType))
    );
    return layout;
}

RequestDialog::RequestDialog(QWidget *pwgt): QDialog(pwgt, Qt::WindowTitleHint | Qt::WindowSystemMenuHint) {
    
    QPushButton* cancelButton = new QPushButton("Закрыть");
    browser = new QTextBrowser(this);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    QVBoxLayout* baseLayout = new QVBoxLayout;

    QHBoxLayout* formLayout = new QHBoxLayout;
    formLayout->addWidget(cancelButton);
    baseLayout->addLayout(createButtons(this));
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