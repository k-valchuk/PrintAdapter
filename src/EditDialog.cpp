#include "EditDialog.h"
#include "AddLine.h"

#include <QTabWidget>
#include <QTimer>
#include <QDebug>
#include <QStackedWidget>

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

    button_group->addButton(btn1, 1);

    tableLayout->addWidget(groupBoxToggle);

    AddLine* rundownAddLine = new AddLine(this, "Выпуск");
    tableLayout->addWidget(rundownAddLine);




    templateRundownModel = new QStandardItemModel(0, 2, this);
    templateRundownTableView = new BaseTableView(this, templateRundownModel, QColor("#323236"), true);
    tableLayout->addWidget(templateRundownTableView);

    connect(
        templateRundownTableView,
        &BaseTableView::deleteTemplate,
        this,
        [this](QString templateId){
            server_requester->setCurrentButton(ActionId::DELETE_TEMPLATE);
            qDebug() << "templateId" << templateId;
            server_requester->removeTemplate(templateId);
        }
    );

    connect(rundownAddLine, &AddLine::addSignal, this, [this](){
        int row = templateRundownModel->rowCount();

        templateEdit->setDisabled(true);

        if (row > 0){
            templateRundownModel->insertRow(row);
        } else {
            templateRundownModel->insertRow(
                row,
                QList<QStandardItem*>{
                    new QStandardItem(),
                    new QStandardItem()
                }
            );
        }

        editingIndex = templateRundownModel->index(row, 1);

        templateRundownTableView->edit(editingIndex);
        templateRundownTableView->scrollTo(editingIndex, QAbstractItemView::EnsureVisible);

    });

    connect(templateRundownTableView->itemDelegate(),
        &QAbstractItemDelegate::closeEditor,
        this,
        [this](QWidget*, QAbstractItemDelegate::EndEditHint){
            if (!editingIndex.isValid())
                return;

            QString value =
            templateRundownModel
                ->data(
                    editingIndex,
                    Qt::EditRole
                )
                .toString()
                .trimmed();
            
            if (value.isEmpty())
        {
            templateRundownModel
                ->removeRow(
                    editingIndex.row()
                );
        }
    });

    connect(templateRundownModel,
        &QAbstractItemModel::dataChanged,
        this,
        [this](const QModelIndex& tl,
               const QModelIndex&)
    {
        QString templateName = templateRundownModel->data(tl).toString();
        if (templateName.isEmpty()) {
            return;
        }
        editingIndex = tl;

        QJsonObject jsonObj;
        jsonObj["name"] = templateName;
        QString templateContent = " ";
        if (templateEdit->isEnabled()) {
            templateContent = templateEdit->toPlainText();
        }
        jsonObj["content"] = templateContent;
        jsonObj["subsystem"] = currentSubSystem;
        jsonObj["is_single"] = false;

        server_requester->setCurrentButton(ActionId::ADD_TEMPLATE);
        server_requester->addTemplate(QJsonDocument(jsonObj));
    });


    AddLine* storyAddLine = new AddLine(this, "Отдельное событие");
    tableLayout->addWidget(storyAddLine);
    templateStoryModel = new QStandardItemModel(0, 2, this);
    templateStoryTableView = new BaseTableView(this, templateStoryModel, QColor("#323236"), true);
    tableLayout->addWidget(templateStoryTableView);

    
    
    connect(storyAddLine,
        &AddLine::addSignal,
        this,
        [this]()
    {
        int row =
            templateStoryModel
                ->rowCount();
        
        templateEdit->setDisabled(true);

        if (row > 0){
            templateStoryModel->insertRow(row);
        } else {
            templateStoryModel->insertRow(
                row,
                QList<QStandardItem*>{
                    new QStandardItem(),
                    new QStandardItem()
                }
            );
        }

        editingIndex = templateStoryModel->index(row, 1);

        templateStoryTableView
            ->edit(editingIndex);
        templateStoryTableView->scrollTo(editingIndex, QAbstractItemView::EnsureVisible);
    });

    connect(
    templateStoryTableView->itemDelegate(),
    &QAbstractItemDelegate::closeEditor,
    this,
    [this](QWidget*,
           QAbstractItemDelegate::EndEditHint)
    {
        if (!editingIndex.isValid())
            return;

        QString value =
            templateStoryModel
                ->data(
                    editingIndex,
                    Qt::EditRole
                )
                .toString()
                .trimmed();

        if (value.isEmpty())
        {
            templateStoryModel
                ->removeRow(
                    editingIndex.row()
                );
        }

    });

    connect(templateStoryModel,
        &QAbstractItemModel::dataChanged,
        this,
        [this](const QModelIndex& tl,
               const QModelIndex&)
    {
        QString templateName = templateStoryModel->data(tl).toString();

        if (templateName.isEmpty()) {
            return;
        }
        editingIndex = tl;

        QJsonObject jsonObj;
        qDebug() << "subsystem" << currentSubSystem;
        jsonObj["name"] = templateName;
        QString templateContent = templateEdit->toPlainText();
        jsonObj["content"] = !templateContent.isEmpty() ? templateContent : " ";
        jsonObj["subsystem"] = currentSubSystem;
        jsonObj["is_single"] = true;

        server_requester->setCurrentButton(ActionId::ADD_TEMPLATE);
        server_requester->addTemplate(QJsonDocument(jsonObj));
    });


    mainWidgetsLayout->addLayout(tableLayout);

    QTabWidget *tabWidget = new QTabWidget(this);

    QWidget *tab1 = new QWidget();
    QVBoxLayout *layout1 = new QVBoxLayout(tab1);
    QLabel* panelStateLabel = new QLabel("Состояние шаблона", tab1);
    panelStateLabel->setObjectName("baseLabel");
    layout1->addWidget(panelStateLabel);
    activeCheckBox = new QCheckBox("Активен", tab1);
    activeCheckBox->setDisabled(true);
    layout1->addWidget(activeCheckBox);

    QWidget* defaultPanel = new QWidget(tab1);

    QWidget* newsSettingsPanel = new QWidget(tab1);
    QVBoxLayout *newsSettingsLayout = new QVBoxLayout(newsSettingsPanel);
    QLabel* panelStateColumnLabel = new QLabel("Учитывать состояния в столбцах", newsSettingsPanel);
    panelStateColumnLabel->setObjectName("baseLabel");
    newsSettingsLayout->addWidget(panelStateColumnLabel, 0, Qt::AlignLeft);
    QHBoxLayout *stateLayout = new QHBoxLayout(newsSettingsPanel);
    prompterCheckBox = new QCheckBox("Суфлер", newsSettingsPanel);
    skipCheckBox = new QCheckBox("Пропуск", newsSettingsPanel);
    stateLayout->addWidget(prompterCheckBox);
    stateLayout->addWidget(skipCheckBox);
    stateLayout->addStretch();
    newsSettingsLayout->addLayout(stateLayout);


    QLabel* panelBreakLabel = new QLabel("Разрыв страницы после", newsSettingsPanel);
    panelBreakLabel->setObjectName("baseLabel");
    newsSettingsLayout->addWidget(panelBreakLabel, 0, Qt::AlignLeft);

    QHBoxLayout *breakLayout1 = new QHBoxLayout(newsSettingsPanel);
    separatorCheckBox = new QCheckBox("Разделителя", newsSettingsPanel);
    storyCheckBox = new QCheckBox("События", newsSettingsPanel);
    breakLayout1->addWidget(separatorCheckBox);
    breakLayout1->addWidget(storyCheckBox);
    breakLayout1->addStretch();
    newsSettingsLayout->addLayout(breakLayout1);

    QHBoxLayout *breakLayout2 = new QHBoxLayout(newsSettingsPanel);
    blockCheckBox = new QCheckBox("Блока", newsSettingsPanel);
    rubricCheckBox = new QCheckBox("Рубрики", newsSettingsPanel);
    breakLayout2->addWidget(blockCheckBox);
    breakLayout2->addWidget(rubricCheckBox);
    breakLayout2->addStretch();
    newsSettingsLayout->addLayout(breakLayout2);


    QStackedWidget* stackedWidget = new QStackedWidget(tab1);
    int panelDefaultIndex = stackedWidget->addWidget(defaultPanel);
    int panelNewsSettingIndex = stackedWidget->addWidget(newsSettingsPanel);
    layout1->addWidget(stackedWidget);
    layout1->addStretch();


    connect(templateRundownTableView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, [this, stackedWidget, panelNewsSettingIndex](const QItemSelection &selected, const QItemSelection &deselected) {
        if (selected.isEmpty()) {
            return;
        }
        templateEdit->setEnabled(true);
        templateStoryTableView->selectionModel()->blockSignals(true);
        templateStoryTableView->clearSelection();
        templateStoryTableView->selectionModel()->blockSignals(false);
        stackedWidget->setCurrentIndex(panelNewsSettingIndex);
        activeCheckBox->setEnabled(true);

        QModelIndex firstSelectedIndex = selected.indexes().first();
        int row = firstSelectedIndex.row();
        QModelIndex firstColumnIndex = firstSelectedIndex.sibling(row, 0);
        QString templateId = firstColumnIndex.data(Qt::DisplayRole).toString();

        qDebug() << "ID?" << templateId;
        server_requester->setCurrentButton(ActionId::GET_TEMPLATE);
        server_requester->getTemplate(templateId);
    });

    connect(templateStoryTableView->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, [this, stackedWidget, panelDefaultIndex](const QItemSelection &selected, const QItemSelection &deselected) {
        if (selected.isEmpty()) {
            return;
        }
        templateEdit->setEnabled(true);
        templateRundownTableView->selectionModel()->blockSignals(true);
        templateRundownTableView->clearSelection();
        templateRundownTableView->selectionModel()->blockSignals(false);
        stackedWidget->setCurrentIndex(panelDefaultIndex);
        activeCheckBox->setEnabled(true);

        QModelIndex firstSelectedIndex = selected.indexes().first();
        int row = firstSelectedIndex.row();
        QModelIndex firstColumnIndex = firstSelectedIndex.sibling(row, 0);
        QString templateId = firstColumnIndex.data(Qt::DisplayRole).toString();

        server_requester->setCurrentButton(ActionId::GET_TEMPLATE);
        server_requester->getTemplate(templateId);
    });


    QWidget *tab2 = new QWidget();
    QVBoxLayout* layout2 = new QVBoxLayout(tab2); 
    QHBoxLayout* tabTitleLayout = new QHBoxLayout(tab2);
    QLabel* panelLabel = new QLabel("Редактирование структуры шаблона", tab2);
    panelLabel->setObjectName("baseLabel");
    tabTitleLayout->addWidget(panelLabel);
    tabTitleLayout->addStretch();
    QPushButton* infoButton = new QPushButton("Инфо", tab2);
    infoButton->setObjectName("GrayButton");
    tabTitleLayout->addWidget(infoButton);
    QPushButton* importButton = new QPushButton("Импорт", tab2);
    importButton->setObjectName("GrayButton");
    tabTitleLayout->addWidget(importButton);
    QPushButton* exportButton = new QPushButton("Экспорт", tab2);
    exportButton->setObjectName("GrayButton");
    tabTitleLayout->addWidget(exportButton);
    layout2->addLayout(tabTitleLayout);
    templateEdit = new QTextEdit(tab2);
    templateEdit->setDisabled(true);
    templateEdit->setObjectName("EditArea");
    templateEdit->setAcceptRichText(false);
    layout2->addWidget(templateEdit);

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
        chooseButton, &QPushButton::clicked,
        this, [this](){
            QTableView* activeTable = nullptr;
            if (templateRundownTableView->selectionModel()->hasSelection()) {
                activeTable = templateRundownTableView;
            } else if (templateStoryTableView->selectionModel()->hasSelection()) {
                activeTable = templateStoryTableView;
            }

            if (!activeTable) {
                return;
            }
            QModelIndex idx = activeTable->selectionModel()->selectedRows().first();

            int row = idx.row();
            
            QString templateName = activeTable->model()->index(row, 1).data().toString();
            
            if (templateName.isEmpty()) {
                return;
            }

            QJsonObject jsonObj;
            jsonObj["name"] = templateName;
            QString templateContent = templateEdit->toPlainText();
            jsonObj["content"] = !templateContent.isEmpty() ? templateContent : " ";
            jsonObj["subsystem"] = currentSubSystem;
            jsonObj["is_single"] = false;
            jsonObj["is_active"] = activeCheckBox->isChecked();
            if (activeTable == templateRundownTableView) {
                QJsonObject render_data;
                render_data["prompt_filter"] = prompterCheckBox->isChecked();
                render_data["skip_filter"] = skipCheckBox->isChecked();

                render_data["sep_break"] = separatorCheckBox->isChecked();
                render_data["story_break"] = storyCheckBox->isChecked();
                render_data["block_break"] = blockCheckBox->isChecked();
                render_data["rubric_break"] = rubricCheckBox->isChecked();

                jsonObj["render_data"] = render_data;
            }

            server_requester->setCurrentButton(ActionId::NONE_ACTION);
            server_requester->addTemplate(QJsonDocument(jsonObj));

        }
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
        case ActionId::ADD_TEMPLATE:
            qDebug() << "ADD_TEMPLATE";
            if (!jsonDoc.object().value("is_single").toBool()) {
                qDebug() << "ID" << QString::number(jsonDoc.object().value("ID").toInt());
                templateRundownModel->setData(
                    templateRundownModel->index(editingIndex.row(), 0),
                    QString::number(jsonDoc.object().value("ID").toInt())
                );
                templateRundownTableView->selectRow(editingIndex.row());
            } else {
                qDebug() << "ID" << QString::number(jsonDoc.object().value("ID").toInt()) << "index" << editingIndex;
                templateStoryModel->setData(
                    templateStoryModel->index(editingIndex.row(), 0),
                    QString::number(jsonDoc.object().value("ID").toInt())
                );

                templateStoryTableView->selectRow(editingIndex.row());
            }
            editingIndex = QPersistentModelIndex();
        case ActionId::GET_TEMPLATE:
            activeCheckBox->setChecked(jsonDoc.object().value("is_active").toBool());
            templateEdit->clear();
            templateEdit->setPlainText(jsonDoc.object().value("content").toString());
            if (!jsonDoc.object().value("is_single").toBool()) {
                prompterCheckBox->setChecked(jsonDoc.object().value("render_data").toObject().value("prompt_filter").toBool());
                skipCheckBox->setChecked(jsonDoc.object().value("render_data").toObject().value("skip_filter").toBool());
                separatorCheckBox->setChecked(jsonDoc.object().value("render_data").toObject().value("sep_break").toBool());
                storyCheckBox->setChecked(jsonDoc.object().value("render_data").toObject().value("story_break").toBool());
                blockCheckBox->setChecked(jsonDoc.object().value("render_data").toObject().value("block_break").toBool());
                rubricCheckBox->setChecked(jsonDoc.object().value("render_data").toObject().value("rubric_break").toBool());
            }
            break;
        default:
            return;
    }
    server_requester->setCurrentButton(ActionId::NONE_ACTION);
}


void EditDialog::getErrorRequestSlot(QString message, int httpStatus) {
    QMessageBox::critical(this, ErrorTitle, message);
}
  