#pragma once

#include <QLayout>
#include "BaseRequestButton.h"
#include "RequestDialog.h"

class RemoveTemplateButton: public BaseRequestButton {
    Q_OBJECT

    public:
        RemoveTemplateButton(
            QWidget* pwgt, 
            ServerRequester* server_requester,
            QLayout* layout,
            QButtonGroup* button_group,
            ActionId buttonId
        );
        void sendRequest(ActionId buttonId) override;
};