#pragma once

#include <QPushButton>
#include <QLayout>
#include "ServerRequester.h"

class BaseRequestButton: public QPushButton {
    Q_OBJECT

    protected:
        ServerRequester* server_requester;

    public:
        BaseRequestButton(
            QWidget* pwgt, 
            ServerRequester* server_requester, 
            QLayout* layout, 
            const QString& buttonName,
            QButtonGroup* button_group,
            ActionId buttonId
        );

        virtual void sendRequest(ActionId buttonId){};

};