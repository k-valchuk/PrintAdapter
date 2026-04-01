#pragma once

#include <QLayout>
#include "BaseRequestButton.h"
#include "RequestDialog.h"

class AddTemplateButton: public BaseRequestButton {
    Q_OBJECT

    public:
        AddTemplateButton(
            QWidget* pwgt, 
            ServerRequester* server_requester,
            QLayout* layout 
        );
        void sendRequest(ActionId buttonId) override;
};