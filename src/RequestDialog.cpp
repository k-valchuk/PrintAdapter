#include "RequestDialog.h"
#include "PrintableReport.h"


RequestDialog::RequestDialog(QWidget *pwgt, ServerRequester* server_requester_): BaseDialog(pwgt, "Выбор шаблона печати"), server_requester(server_requester_) {
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 20, 30);

    templatesModel = new QStandardItemModel(0, 2, this);
    templateTableView = new BaseTableView(this, templatesModel);
    templateTableView->setColumnHidden(0, true);
    
    QHBoxLayout* table_layout = new QHBoxLayout(this);
    table_layout->addWidget(templateTableView);
    baseLayout->addLayout(table_layout);


    connect(
        server_requester, 
        SIGNAL(done(int, const QJsonDocument, ActionId)), 
        this, 
        SLOT(showResponseSlot(int, const QJsonDocument, ActionId))
    );
    connect(
        server_requester, 
        SIGNAL(error(QString, int)), 
        this, 
        SLOT(getErrorRequestSlot(QString, int))
    );

    

    server_requester->getAllTemplates();

    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setContentsMargins(313, 90, 0, 0);
    QPushButton* cancelButton = new QPushButton("Отмена");
    cancelButton->setFixedHeight(30);
    cancelButton->setMinimumWidth(93);
    cancelButton->setStyleSheet(
        "QPushButton {border: 1px solid #494949; border-radius: 2px; font: normal normal normal 15px/18px Roboto; color: #6F8CB7; background-color: transparent; margin-right: 10px;}"
        "QPushButton:hover{border: 1px solid #6F8CB7;}"
    );
    
    connect(
        cancelButton, SIGNAL(clicked()),
        this, SLOT(close())
    );
    
    QPushButton* chooseButton = new QPushButton("Выбрать");
    chooseButton->setFixedHeight(30);
    chooseButton->setMinimumWidth(114);
    chooseButton->setStyleSheet(
        "QPushButton{font: normal normal normal 15px/18px Roboto; background: #6F8CB7; color: #FFFFFF; border-radius: 2px; margin-left: 10px;}"
        "QPushButton:hover{background: #9abcff;}"
    );

    connect(
        chooseButton, SIGNAL(clicked()),
        this, SLOT(printTemplate())
    );

    button_layout->addWidget(cancelButton);
    button_layout->addWidget(chooseButton);
    baseLayout->addLayout(button_layout);

    setBaseLayout(baseLayout);

    applyTheme();
}

void RequestDialog::applyTheme() {
    
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 44, 50));
    darkPalette.setColor(QPalette::WindowText, Qt::white);

   
    setPalette(darkPalette);
}

void RequestDialog::updateTable(QJsonArray value_array, QStandardItemModel* itemModel) {
    int i = 0;
    int rowsCount = itemModel->rowCount(); 
    for (const QJsonValue& value : value_array) {
        QList<QStandardItem*> rowData;
        rowData << new QStandardItem(QString::number(value.toObject().value("element_id").toInt()));
        rowData << new QStandardItem(value.toObject().value("element_name").toString());
        if (i >= rowsCount) {
            itemModel->appendRow(rowData);
        } else {
            itemModel->removeRow(i);
            itemModel->insertRow(i, rowData);
        }
        i++;
    }
}

void RequestDialog::printTemplate(){
    QModelIndex index = templateTableView->selectionModel()->currentIndex();
    QString dbId = templateTableView->model()->data(templateTableView->model()->index(index.row(), 0)).toString();
    server_requester->setCurrentButton(ActionId::EXPORT);
    server_requester->exportTemplate(dbId.toInt());
}

void RequestDialog::removeRowByID(int itemID, QStandardItemModel* itemModel){
    for (int i = 0; i < itemModel->rowCount(); i++) {
        if (itemModel->data(itemModel->index(i, 0)).toInt() == itemID) {
            itemModel->removeRow(i);
            break;
        }
    }
}

RequestDialog::ContentModel RequestDialog::responseRouting(const QJsonDocument jsonDoc, ActionId buttonId) {
    ContentModel response;
    response.content_type = ContentType::TEXT;
    QList<QStandardItem*> rowData{};
    switch (buttonId) {
        case ActionId::EXPORT:
        case ActionId::GET_TEMPLATE:
            response.content = jsonDoc.object().value("content").toString();
            response.content_type = ContentType::HTML;
            break;
        case ActionId::GET_ALL_TEMPLATES:
            updateTable(jsonDoc.array(), templatesModel);
            break;
    }
    return response;
}

void RequestDialog::showResponseSlot(int httpStatus, const QJsonDocument jsonDoc, ActionId buttonId) {
    ContentModel response = responseRouting(jsonDoc, buttonId);
    if (response.content_type == ContentType::HTML) {
        PrintableReport* printableReport = new PrintableReport(QPrinter::HighResolution, response.content, this);
        printableReport->preview(nullptr, PrintTitle);
    }
}

void RequestDialog::getErrorRequestSlot(QString message, int httpStatus){
    QMessageBox::critical(this, ErrorTitle, message);
}