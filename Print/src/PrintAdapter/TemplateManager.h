#pragma once
#include <QWidget>
#include "BaseTableView.h"
#include <QJsonObject>
#include <QStandardItem>
#include <QList>
#include <QJsonArray>

struct SubTableConfig {
    QString name; 
    int settingsPanelIndex; 
    bool is_single;
};

class TemplateManager: public QWidget {
    Q_OBJECT

    QVector<QStandardItemModel*> m_models;
    QVector<BaseTableView*> m_allTableViews;
    QPersistentModelIndex editingIndex;


    public:
        TemplateManager(QVector<SubTableConfig> tableNames, QWidget* pwgt = nullptr);
        QVector<QStandardItemModel*> getModels() const { return m_models; }
        void clearAllSelections();
        void updateTables(QJsonArray data);
    
    signals:
        void disableTemplateEdit(bool isDisabled);
        void setIndex(QModelIndex index);
        void deleteTemplate(QString templateId);
        void changedTemplate(int templateId, QString templateName, bool is_single, BaseTableView* activeTable);
        void rowActivated(const QString &tableName, const QString &rowId, int settingsPanelIndex, const QModelIndex &index, BaseTableView* activeTable);


};