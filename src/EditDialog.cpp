#include "EditDialog.h"
#include "AddLine.h"

#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>

EditDialog::EditDialog(QWidget *pwgt, ServerRequester* server_requester_): BaseDialog(pwgt, "Выбор шаблона печати"), server_requester(server_requester_) {
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 20, 5);
    QHBoxLayout* mainWidgetsLayout = new QHBoxLayout(this);

    QVBoxLayout* tableLayout = new QVBoxLayout(this);
    QGroupBox *groupBoxToggle = new QGroupBox(this);
    groupBoxToggle->setObjectName("emptyGroupBox");

    QHBoxLayout *layout = new QHBoxLayout(groupBoxToggle);
    button_group = new QButtonGroup(groupBoxToggle);
    button_group->setExclusive(true);

    QPushButton *btn1 = new QPushButton("Новости", groupBoxToggle);
    btn1->setObjectName("toogleButton");
    btn1->setProperty("name", "NEWS");
    btn1->setCheckable(true);
    

    QPushButton *btn2 = new QPushButton("Логи", groupBoxToggle);
    btn2->setCheckable(true);
    btn2->setObjectName("toogleButton");
    btn2->setProperty("name", "LOGS");

    QPushButton *btn3 = new QPushButton("Эфир", groupBoxToggle);
    btn3->setCheckable(true);
    btn3->setObjectName("toogleButton");
    btn3->setProperty("name", "BROADCAST");

    connect(button_group, qOverload<QAbstractButton*>(&QButtonGroup::buttonClicked), [this](QAbstractButton *button) {
        if (button) {
            currentSubSystem = button->property("name").toString();
            server_requester->getAllTemplates(false, currentSubSystem);
        }
    });
    btn1->setChecked(true);
    QTimer::singleShot(0, btn1, &QPushButton::click);

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

    
    layout->addWidget(btn1);
    layout->addWidget(btn2);
    layout->addWidget(btn3);

    button_group->addButton(btn1, 1);
    button_group->addButton(btn2, 2);
    button_group->addButton(btn3, 3);

    tableLayout->addWidget(groupBoxToggle);

    AddLine* rundownAddLine = new AddLine(this, "Выпуск");
    tableLayout->addWidget(rundownAddLine);




    templateRundownModel = new QStandardItemModel(0, 2, this);
    templateRundownTableView = new BaseTableView(this, templateRundownModel, QColor("#323236"));
    tableLayout->addWidget(templateRundownTableView);


    AddLine* storyAddLine = new AddLine(this, "Отдельное событие");
    tableLayout->addWidget(storyAddLine);
    templateStoryModel = new QStandardItemModel(0, 2, this);
    templateStoryTableView = new BaseTableView(this, templateStoryModel, QColor("#323236"));
    tableLayout->addWidget(templateStoryTableView);


    mainWidgetsLayout->addLayout(tableLayout);

    QTabWidget *tabWidget = new QTabWidget(this);

    QWidget *tab1 = new QWidget();
    QVBoxLayout *layout1 = new QVBoxLayout(tab1);

    QWidget *tab2 = new QWidget();
    QVBoxLayout *layout2 = new QVBoxLayout(tab2); 

    layout2->addWidget(new QLabel("Редактирование структуры шаблона", tab2));
    layout2->addWidget(new QTextEdit(tab2));

    tabWidget->addTab(tab1, "Свойства");
    tabWidget->addTab(tab2, "Разметка");

    tabWidget->setTabPosition(QTabWidget::North); 
    mainWidgetsLayout->addWidget(tabWidget);
    baseLayout->addLayout(mainWidgetsLayout);


    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setContentsMargins(343, 90, 0, 0);
    QPushButton* cancelButton = new QPushButton("Отмена", this);
    cancelButton->setObjectName("cancelButton");
    
    connect(
        cancelButton, SIGNAL(clicked()),
        this, SLOT(close())
    );
    
    QPushButton* chooseButton = new QPushButton("Сохранить");
    chooseButton->setObjectName("applyButton");

    connect(
        chooseButton, SIGNAL(clicked()),
        this, SLOT(accepted())
    );

    button_layout->addWidget(cancelButton);
    button_layout->addWidget(chooseButton);
    baseLayout->addLayout(button_layout);

    setBaseLayout(baseLayout);
}

void EditDialog::updateTable(QJsonArray value_array, QStandardItemModel* itemModel, bool single) {
    int i = 0;
    itemModel->clear();
    int rowsCount = itemModel->rowCount(); 
    for (const QJsonValue& value : value_array) {
        QList<QStandardItem*> rowData;
        qDebug() << "is_single?" << single << (value.toObject().value("is_single").toBool() == single);
        if (value.toObject().value("is_single").toBool() == single) {
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
}

void EditDialog::showResponseSlot(int httpStatus, const QJsonDocument jsonDoc, ActionId buttonId) {
    switch (buttonId) {
        case ActionId::GET_ALL_TEMPLATES:
            updateTable(jsonDoc.array(), templateRundownModel, false);
            updateTable(jsonDoc.array(), templateStoryModel, true);
            break;
        default:
            return;
    }
}


void EditDialog::getErrorRequestSlot(QString message, int httpStatus) {
    QMessageBox::critical(this, ErrorTitle, message);
}
  