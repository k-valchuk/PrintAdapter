#include "GetAllTemplatesButton.h"

GetAllTemplatesButton::GetAllTemplatesButton(
    QWidget* pwgt, 
    ServerRequester* server_requester,
    QLayout* layout,
    QButtonGroup* button_group,
    ActionId buttonId 
) : BaseRequestButton(
    pwgt, 
    server_requester, 
    layout, 
    "Все шаблоны",
    button_group, 
    buttonId
) {}

void GetAllTemplatesButton::sendRequest(ActionId buttonId) {
    server_requester->setCurrentButton(buttonId);
    server_requester->getAllTemplates();
}
