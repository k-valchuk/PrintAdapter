#pragma once
#include "BaseRequestButton.h"
#include "RequestDialog.h"

class GetAllTemplatesButton: public BaseRequestButton {
    Q_OBJECT

    public:
        GetAllTemplatesButton(
            QWidget* pwgt, 
            ServerRequester* server_requester,
            QLayout* layout
        );
    
        void sendRequest(ActionId buttonId) override;
};