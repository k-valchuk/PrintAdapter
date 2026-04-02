#pragma once

#include <QLayout>
#include "BaseRequestButton.h"
#include "ServerRequester.h"

class AddTagButton: public BaseRequestButton {
    Q_OBJECT

    public:
        AddTagButton(
            QWidget* pwgt, 
            ServerRequester* server_requester,
            QLayout* layout,
            QButtonGroup* button_group,
            ActionId buttonId
        );
        void sendRequest(ActionId buttonId) override;
};