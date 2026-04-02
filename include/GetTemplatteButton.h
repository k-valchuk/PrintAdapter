#pragma once

#include <QLayout>
#include "BaseRequestButton.h"
#include "RequestDialog.h"

class GetTemplateButton: public BaseRequestButton {
    Q_OBJECT

    public:
        GetTemplateButton(
            QWidget* pwgt, 
            ServerRequester* server_requester,
            QLayout* layout,
            QButtonGroup* button_group,
            ActionId buttonId
        );

        void sendRequest(ActionId buttonId) override;
};