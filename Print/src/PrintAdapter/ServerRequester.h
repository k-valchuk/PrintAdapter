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
        QString exportTemplateName = "";
    
    public:
        ServerRequester(QObject* pobj, QString base_url);
        QJsonDocument getAllTemplates(bool isActive, QString subsystemId);
        void getAllTags(QString subsystemId);
        void getTemplate(QString templateId, QString subsystemId);
        void removeTemplate(QString templateId, QString subsystemId);
        void exportTemplate(int templateId, QString subsystemId);
        QString addTemplate(const QJsonDocument jsonDoc, QString subsystemId);
        void setCurrentButton(ActionId buttonId);
        void addTag(const QJsonDocument jsonDoc, QString subsystemId);
        void removeTag(QString tagName, QString subsystemId);

        void setTemplateName(QString templateName);
        void setExportJson(QJsonDocument json_doc);
        QJsonDocument getExportJson();

        

    private slots:
        void slotError(IPrintRequest, bool showError);
        void slotDone(IPrintRequest req, bool showError);
    
    signals:
        void done(const QJsonDocument jsonDoc, ActionId buttonId);
        void error(QString message, int httpStatus);
};