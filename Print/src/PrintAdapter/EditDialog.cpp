#include "EditDialog.h"
#include "AddLine.h"
#include <QTabWidget>
#include <QTimer>
#include <QSplitter>
#include <QDebug>
#include "TemplateHighlighter.h"

EditDialog::EditDialog(QWidget *pwgt): BaseDialog(pwgt, "Настройки шаблонов печати") {
    initData();
    setupUi();
    setupConnections();

    btn_news->setChecked(true);
    QTimer::singleShot(0, btn_news, &QPushButton::click);
}

void EditDialog::initData() {
    server_requester = new ServerRequester(this, BASE_URL);
    tagDataModel = new QStandardItemModel(this);
}

void EditDialog::setupUi() {
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 10, 5);

    QGroupBox* groupBoxToggle = new QGroupBox(this);
    groupBoxToggle->setObjectName("emptyGroupBox");
    groupBoxToggle->setMaximumHeight(50);
    baseLayout->addWidget(groupBoxToggle);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setObjectName("mySplitter");

    splitter->addWidget(createNavigationPanel(groupBoxToggle));
    splitter->addWidget(createEditSpace());

    baseLayout->addWidget(splitter);

    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setContentsMargins(0, 40, 0, 0);
    button_layout->addStretch();
    QPushButton* cancelButton = new QPushButton("Отмена", this);
    cancelButton->setObjectName("cancelButton");
    
    connect(
        cancelButton, SIGNAL(clicked()),
        this, SLOT(close())
    );

    QPushButton* chooseButton = new QPushButton("Сохранить");
    chooseButton->setObjectName("applyButton");
    connect(chooseButton, &QPushButton::clicked, this, &EditDialog::saveTemplate);
    button_layout->addWidget(cancelButton);
    button_layout->addWidget(chooseButton);
    baseLayout->addLayout(button_layout);
    setBaseLayout(baseLayout);
}


QWidget* EditDialog::createNavigationPanel(QGroupBox* groupBoxToggle) {
    QWidget* tableWidget = new QWidget(this);
    tableWidget->setObjectName("EmptyStyle");
    tableWidget->setMinimumWidth(500);

    QVBoxLayout* tableLayout = new QVBoxLayout(tableWidget);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(0);

   

    QHBoxLayout* navLayout = new QHBoxLayout(groupBoxToggle);
    navLayout->setContentsMargins(0, 0, 0, 0);

    button_group = new QButtonGroup(groupBoxToggle);
    button_group->setExclusive(true);

    
    btn_news = new SubsystemButton("Новости", ":/icons/nav-icon-news-active.svg", ":/icons/nav-icon-news-inactive.svg", "toogleButton", "news", groupBoxToggle);
    SubsystemButton* btn_plan = new SubsystemButton("Планирование", ":/icons/nav-icon-news-active.svg", ":/icons/nav-icon-news-inactive.svg", "toogleButton", "plan", groupBoxToggle);
    //SubsystemButton* btn_logs = new SubsystemButton("Логи", ":/icons/tab-icon-logging-active.svg", ":/icons/tab-icon-logging-inactive.svg", "toogleButton", "logs", QVector<QString>{}, groupBoxToggle);
    //SubsystemButton* btn_broadcast = new SubsystemButton("Эфир", ":/icons/tab-icon-broadcast-active.svg", ":/icons/tab-icon-broadcast-inactive.svg", "toogleButton", "broadcast", QVector<QString>{}, groupBoxToggle);

    stackedWidget = new QStackedWidget(this);
    QVector<SubsystemButton*> buttons = {btn_news, btn_plan};

    for (int i = 0; i < buttons.size(); ++i) {
        SubsystemButton *btn = buttons[i];
        button_group->addButton(btn, i);
        navLayout->addWidget(btn);

        QJsonArray tableNames = server_requester->getTemplateTypes(btn->subsystem).array();
        TemplateManager* manager = new TemplateManager(tableNames, this);
        stackedWidget->addWidget(manager);
        m_allManagers.append(manager);

        connect(manager, &TemplateManager::deleteTemplate, this, &EditDialog::onDeleteTemplate);
        connect(manager, &TemplateManager::changedTemplate, this,  &EditDialog::onChangeTemplate);
        connect(manager, &TemplateManager::rowActivated, this, &EditDialog::onRowActivated);
        connect(manager, &TemplateManager::setIndex, this, &EditDialog::setPersistentIndex);
        connect(manager, &TemplateManager::disableTemplateEdit, this, &EditDialog::changeStateTemplateEdit);
    }

    QWidget* stretcher = new QWidget(groupBoxToggle);
    stretcher->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    stretcher->setObjectName("togglePlaceholder");
    navLayout->addWidget(stretcher);

    tableLayout->addWidget(stackedWidget);

    return tableWidget;
}

QWidget* EditDialog::createEditSpace() {
    QWidget* editSpace = new QWidget(this);
    QVBoxLayout* editSpaceLayout = new QVBoxLayout(editSpace);
    QHBoxLayout* editPanelsLayout = new QHBoxLayout();

    editPanelsLayout->addWidget(createTemplateStatePanel());

    editSpaceLayout->addLayout(editPanelsLayout);
    editSpaceLayout->addWidget(createTemplateEditorPanel());

    return editSpace;
}

QWidget* EditDialog::createTemplateStatePanel() {
    QHash<QString, QString> checkBoxMap = {
        {"Активен", "is_active"}
    };

    activePanel = new CheckPanel(
        "Состояние шаблона",
        checkBoxMap,
        this
    );
    activePanel->setCheckBoxActive("is_active", false);
    return activePanel;
}

QFrame* EditDialog::createTemplateEditorPanel() {
    QFrame* templateEditor = new QFrame(this);
    templateEditor->setObjectName("SettingPanel");

    QVBoxLayout* layout = new QVBoxLayout(templateEditor);
    layout->setSpacing(12);
    layout->setContentsMargins(10, 14, 10, 10);

    layout->addLayout(createTemplateEditorTitlePanel(templateEditor));

    templateEdit = new QTextEdit(templateEditor);
    new TemplateHighlighter(templateEdit->document());
    templateEdit->setDisabled(true);
    templateEdit->setObjectName("EditArea");
    templateEdit->setAcceptRichText(false);
    QHBoxLayout* editLayout = new QHBoxLayout(templateEditor);
    editLayout->setSpacing(0);
    editLayout->addWidget(templateEdit);
    QListView* tagList = new QListView(templateEditor);
    tagList->setMaximumWidth(306);
    tagList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tagList->setVisible(false);
    tagList->setModel(tagDataModel);
    connect(infoButton, &QCheckBox::toggled, tagList, &QWidget::setVisible);
    editLayout->addWidget(tagList);

    layout->addLayout(editLayout);

    return templateEditor;
}

QHBoxLayout* EditDialog::createTemplateEditorTitlePanel(QWidget* templateEditor) {
    QHBoxLayout* tabTitleLayout = new QHBoxLayout(templateEditor);

    QLabel* editorLabel = new QLabel("Редактирование структуры шаблона", templateEditor);
    editorLabel->setObjectName("baseLabel");
    tabTitleLayout->addWidget(editorLabel);
    tabTitleLayout->addStretch();

    infoButton = new QPushButton("Инфо", templateEditor);
    infoButton->setCheckable(true);
    infoButton->setObjectName("GrayButton");
    tabTitleLayout->addWidget(infoButton);
    importButton = new QPushButton("Импорт", templateEditor);
    importButton->setObjectName("GrayButton");
    tabTitleLayout->addWidget(importButton);
    exportButton = new QPushButton("Экспорт", templateEditor);
    exportButton->setObjectName("GrayButton");
    tabTitleLayout->addWidget(exportButton);

    return tabTitleLayout;
}

void EditDialog::setupConnections() {
    // Переключение страниц в QStackedWidget
    connect(button_group, QOverload<int>::of(&QButtonGroup::buttonClicked),
            stackedWidget, &QStackedWidget::setCurrentIndex);

    // Ответы сервера
    connect(server_requester, SIGNAL(done(const QJsonDocument, ActionId)), this, SLOT(showResponseSlot(const QJsonDocument, ActionId)));
    connect(server_requester, SIGNAL(error(QString, int)), this, SLOT(getErrorRequestSlot(QString, int)));

    // Запрос данных при клике на подсистемы
    connect(button_group, qOverload<QAbstractButton*>(&QButtonGroup::buttonClicked), this, [this](QAbstractButton *button) {
   
        SubsystemButton *subBtn = qobject_cast<SubsystemButton*>(button);
        if (!subBtn) return;
        
        currentSubSystem = subBtn->subsystem;
        
        QJsonArray serverData = server_requester->getAllTemplates(false, currentSubSystem).array();
        
        if (TemplateManager *currentManager = qobject_cast<TemplateManager*>(stackedWidget->currentWidget())) {
            currentManager->updateTables(serverData);
        }
        server_requester->getAllTags(currentSubSystem);
    });

    // Импорт шаблона
    connect(
        importButton,
        &QPushButton::clicked,
        this,
        [this](){
            QString filePath = QFileDialog::getOpenFileName(
            this,
            "Выберите файл для импорта",      
            QDir::homePath(),                
                "Текстовые файлы (*.txt *.html);;Все файлы (*.*)"
            );

            if (filePath.isEmpty()) {
                return; 
            }

            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                
                QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для чтения!");
                return;
            }

            
            QTextStream in(&file);
            in.setCodec("UTF-8"); 
            QString fileContent = in.readAll(); 
            file.close();


            templateEdit->setPlainText(fileContent);

        }
    );

    // Экспорт шаблона
    connect(
        exportButton,
        &QPushButton::clicked,
        this,
        [this](){

            if (!m_activeTable || !m_currentSelectedIndex.isValid()) {
                return;
            }

            int row = m_currentSelectedIndex.row();
            const QAbstractItemModel *model = m_currentSelectedIndex.model();
            QString templateName = model->index(row, 1).data(Qt::DisplayRole).toString();
            
            if (templateName.isEmpty()) {
                return;
            }

            QString filePath = QFileDialog::getSaveFileName(
                this,
                "Экспорт шаблона",
                QDir::homePath() + "/" + templateName + ".html",
                "Веб-страницы (*.html *.htm)"
            );

            if (filePath.isEmpty()) {
                return;
            }

            QFile file(filePath);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QMessageBox::critical(this, "Ошибка", "Не удалось создать или открыть файл для записи!");
                return;
            }

            QTextStream out(&file);
            out.setCodec("UTF-8");

            out << templateEdit->toPlainText();

            file.close();
        }
    );
}


void EditDialog::showResponseSlot(const QJsonDocument jsonDoc, ActionId buttonId) {
    QStandardItemModel* tableModel = nullptr;
    switch (buttonId) {
        case ActionId::GET_TEMPLATE:
            activePanel->setCheckBoxState("is_active", jsonDoc.object().value("is_active").toBool());
            templateEdit->clear();
            templateEdit->setPlainText(jsonDoc.object().value("content").toString());
            break;
        case ActionId::GET_ALL_TAGS:
            tagDataModel->clear();
            for (const QJsonValue& value : jsonDoc.array()) {
                QStandardItem *item = new QStandardItem(value.toObject().value("element_name").toString());
                item->setData(value.toObject().value("element_id").toInt(), Qt::UserRole); 
                tagDataModel->appendRow(item);
            }
            break;
        default:
            break;
    }
    server_requester->setCurrentButton(ActionId::NONE_ACTION);
}

void EditDialog::changeStateTemplateEdit(bool isDisabled){
    templateEdit->setDisabled(isDisabled);
}

void EditDialog::setPersistentIndex(QModelIndex index){
    editingIndex = index;
}

void EditDialog::onDeleteTemplate(QString templateId) {
    server_requester->setCurrentButton(ActionId::DELETE_TEMPLATE);
    server_requester->removeTemplate(templateId, currentSubSystem);
}


void EditDialog::onRowActivated(const QString &tableName, const QString &templateId, const QModelIndex &index, BaseTableView* activeTable) {
    TemplateManager *currentManager = qobject_cast<TemplateManager*>(sender());
    for (TemplateManager *manager : m_allManagers) {
        if (manager != currentManager) {
            manager->clearAllSelections(); 
        }
    }
    m_currentSelectedIndex = index;
    m_activeTable = activeTable;
    templateEdit->setEnabled(true);

    activePanel->setCheckBoxActive("is_active", true);
    server_requester->setCurrentButton(ActionId::GET_TEMPLATE);
    server_requester->getTemplate(templateId, currentSubSystem);
}

void EditDialog::onChangeTemplate(int templateId, const QString templateName, QString templateType, BaseTableView* activeTable) {
    QJsonObject jsonObj;
    if (templateId) {
        jsonObj["ID"] = templateId;
    }
    jsonObj["name"] = templateName;
    QString templateContent = " ";
    jsonObj["type"] = templateType;
    qDebug() << "templateType" << templateType;
    if (templateEdit->isEnabled()) {
        templateContent = templateEdit->toPlainText();
        jsonObj["is_active"] = activePanel->getCheckBoxState("is_active");
    }
    jsonObj["content"] = templateContent;

    server_requester->setCurrentButton(ActionId::ADD_TEMPLATE);
    QString realTemplateId = server_requester->addTemplate(QJsonDocument(jsonObj), currentSubSystem);
    activeTable->blockSignals(true);
    QStandardItemModel* tableModel = qobject_cast<QStandardItemModel*>(activeTable->model());
    tableModel->setData(
        tableModel->index(editingIndex.row(), 0),
        realTemplateId
    );
    activeTable->selectRow(editingIndex.row());
    activeTable->blockSignals(false); 
    m_activeTable = activeTable;
    editingIndex = QPersistentModelIndex();
}

void EditDialog::saveTemplate() {
    if (!m_activeTable || !m_currentSelectedIndex.isValid()) {
        return;
    }

    int row = m_currentSelectedIndex.row();
    const QAbstractItemModel *model = m_currentSelectedIndex.model();
    QString templateName = model->index(row, 1).data(Qt::DisplayRole).toString();
    
    if (templateName.isEmpty()) {
        return;
    }
    int templateId = model->index(row, 0).data(Qt::DisplayRole).toInt(); 

    QJsonObject jsonObj;
    jsonObj["ID"] = templateId;
    jsonObj["name"] = templateName;
    QString templateContent = templateEdit->toPlainText();
    jsonObj["content"] = !templateContent.isEmpty() ? templateContent : " ";
    jsonObj["type"] = m_activeTable->property("type").toString();
    jsonObj["is_active"] = activePanel->getCheckBoxState("is_active");

    server_requester->setCurrentButton(ActionId::NONE_ACTION);
    server_requester->addTemplate(QJsonDocument(jsonObj), currentSubSystem);
}


void EditDialog::getErrorRequestSlot(QString message, int httpStatus) {
    QMessageBox::critical(this, ErrorTitle, message);
}
  