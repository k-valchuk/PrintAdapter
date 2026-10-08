#pragma once

#include <QButtonGroup>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QCheckBox>
#include <QPersistentModelIndex>
#include <QStackedWidget>
#include "BaseTableView.h"
#include "BaseDialog.h"
#include "ServerRequester.h"
#include "TemplateManager.h"
#include "SubsystemButton.h"
#include "CheckPanel.h"


class EditDialog: public BaseDialog {
    Q_OBJECT

    private:
        ServerRequester* server_requester;
        QButtonGroup* button_group;


        CheckPanel* activePanel;

        QStandardItemModel* tagDataModel;

        QPersistentModelIndex editingIndex;

        QVector<TemplateManager*> m_allManagers;
        QModelIndex m_currentSelectedIndex;
        BaseTableView* m_activeTable = nullptr;

        QTextEdit* templateEdit;
        QString currentSubSystem;

        SubsystemButton* btn_news;
        QStackedWidget* stackedWidget;

        int defaultSettingIndex;
        int rundownSettingIndex;



        QVector<QVector<SubTableConfig>> m_appMenuConfig;

        QPushButton* infoButton;
        QPushButton* importButton;
        QPushButton* exportButton;

        void initData();
        void setupUi();
        QWidget* createNavigationPanel(QGroupBox* groupBoxToggle);
        QWidget* createEditSpace();
        void setupConnections();

        QWidget* createTemplateStatePanel();
        QWidget* createNewsSettingsPanel();
        QWidget* createBreakSettingsPanel();
        QFrame* createTemplateEditorPanel();
        QHBoxLayout* createTemplateEditorTitlePanel(QWidget* templateEditor);

    private slots:
        void saveTemplate();
    
    public:
        EditDialog(QWidget* pwgt);
    
    public slots:
        void showResponseSlot(const QJsonDocument jsonDoc, ActionId buttonId);
        void getErrorRequestSlot(QString message, int httpStatus);
        void changeStateTemplateEdit(bool);
        void setPersistentIndex(QModelIndex);
        void onDeleteTemplate(QString templateId);
        void onRowActivated(const QString &tableName, const QString &templateId, const QModelIndex &index, BaseTableView* activeTable);
        void onChangeTemplate(int templateId, const QString templateName, QString templateType, BaseTableView* activeTable);
};