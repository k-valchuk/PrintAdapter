#pragma once
#include <QtWidgets>
#include "Requester.h"

class ServerRequester: public QObject {
    Q_OBJECT

    private:
        Requester* requester;
        QString base_url;
    
    public:
        ServerRequester(QObject* pobj, QString base_url);
        void getAllTemplates();
        void getTemplate(QString templateName);
        void exportTemplate(const QJsonDocument jsonDoc);
        void addTemplate(const QJsonDocument jsonDoc);

    private slots:
        void slotError(QString message, int httpStatus);
        void slotDone(const int&, const QByteArray&);
    
    signals:
        void done(int httpStatus, const QJsonDocument jsonDoc);
        void error(QString message, int httpStatus);
};