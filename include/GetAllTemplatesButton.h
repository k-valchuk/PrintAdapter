#pragma once
#include "BaseRequestButton.h"
#include "ServerRequester.h"

class GetAllTemplatesButton: public BaseRequestButton {
    Q_OBJECT

    public:
        GetAllTemplatesButton(
            QWidget* pwgt, 
            ServerRequester* server_requester,
            QLayout* layout,
            QButtonGroup* button_group,
            ActionId buttonId
        );
    
        void sendRequest(ActionId buttonId) override;
};