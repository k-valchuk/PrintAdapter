#include "GetAllTemplatesButton.h"

GetAllTemplatesButton::GetAllTemplatesButton(
    QWidget* pwgt, 
    ServerRequester* server_requester,
    QLayout* layout 
) : BaseRequestButton(pwgt, server_requester, layout, "Все шаблоны") {}

void GetAllTemplatesButton::sendRequest(ActionId buttonId) {
    server_requester->setCurrentButton(buttonId);
    server_requester->getAllTemplates();
}
