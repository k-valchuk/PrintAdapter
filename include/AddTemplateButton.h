#pragma once

#include <QPushButton>
#include "RequestDialog.h"

class AddTemplateButton: public QPushButton {
    Q_OBJECT

    private:
        ServerRequester* server_requester;

    public:
        AddTemplateButton(QWidget* pwgt, ServerRequester* server_requester);
    
    private slots:
        void sendRequestSlot();
        void getResultRequestSlot(const int& http, const QJsonDocument doc);
        void getErrorRequestSlot(QString message, int httpStatus);
    
    signals:
        void done(QString content, ContentType content_type);
};