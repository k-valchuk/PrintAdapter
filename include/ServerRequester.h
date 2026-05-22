#pragma once
#include <QtWidgets>
#include "Requester.h"
#include "Config.h"

class ServerRequester: public QObject {
    Q_OBJECT

    private:
        Requester* requester;
        QString base_url;
        ActionId currentButtonId;
        QJsonDocument exportJson;
    
    public:
        ServerRequester(QObject* pobj, QString base_url);
        void getAllTemplates(bool isActive, QString subsystemName);
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
        void slotError(QString message, int httpStatus);
        void slotDone(const int&, const QByteArray&);
    
    signals:
        void done(int httpStatus, const QJsonDocument jsonDoc, ActionId buttonId);
        void error(QString message, int httpStatus);
};