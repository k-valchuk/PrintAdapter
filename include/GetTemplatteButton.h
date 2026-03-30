#pragma once

#include <QtWidgets>
#include "ServerRequester.h"
#include "RequestDialog.h"

class GetTemplateButton: public QPushButton {
    Q_OBJECT

    private:
        ServerRequester* server_requester;

    public:
        GetTemplateButton(QWidget* pwgt, ServerRequester* server_requester);
    
    private slots:
        void sendRequestSlot();
        void getResultRequestSlot(const int& http, const QJsonDocument doc);
        void getErrorRequestSlot(QString message, int httpStatus);
    
    signals:
        void done(QString content, ContentType content_type);
};