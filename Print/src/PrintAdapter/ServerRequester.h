#pragma once
#include <QtWidgets>
#include "Requests.h"
#include "Config.h"

class ServerRequester: public QObject {
    Q_OBJECT

    private:
        QString base_url;
        ActionId currentButtonId;
        QJsonDocument exportJson;
    
    public:
        ServerRequester(QObject* pobj, QString base_url);
        void getAllTemplates(bool isActive, QString subsystemId);
        void getAllTags();
        void getTemplate(QString templateId);
        void removeTemplate(QString templateId);
        void exportTemplate(int templateId);
        void addTemplate(const QJsonDocument jsonDoc);
        void setCurrentButton(ActionId buttonId);
        void addTag(const QJsonDocument jsonDoc);
        void removeTag(QString tagName);

        void setExportJson(QJsonDocument json_doc);
        QJsonDocument getExportJson();

    private slots:
        void slotError(IPrintRequest);
        void slotDone(IPrintRequest);
    
    signals:
        void done(const QJsonDocument jsonDoc, ActionId buttonId);
        void error(QString message, int httpStatus);
};