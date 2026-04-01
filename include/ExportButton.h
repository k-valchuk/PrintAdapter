#pragma once

#include <QLayout>
#include "BaseRequestButton.h"
#include "RequestDialog.h"

class ExportButton: public BaseRequestButton {
    Q_OBJECT

    public:
        ExportButton(
            QWidget* pwgt, 
            ServerRequester* server_requester,
            QLayout* layout 
        );
        void sendRequest(ActionId buttonId) override;
};