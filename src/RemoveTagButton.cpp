#include "RemoveTagButton.h"
#include "GetTemplateDialog.h"

RemoveTagButton::RemoveTagButton(
    QWidget* pwgt, 
    ServerRequester* server_requester,
    QLayout* layout,
    QButtonGroup* button_group,
    ActionId buttonId
) : BaseRequestButton(
    pwgt, 
    server_requester, 
    layout, 
    "Удалить тэг",
    button_group, 
    buttonId
) {}

void RemoveTagButton::sendRequest(ActionId buttonId) {
    GetTemplateDialog* dialog = new GetTemplateDialog;
    if (dialog->exec() == QDialog::Accepted) {
        server_requester->setCurrentButton(buttonId);
        server_requester->removeTag(dialog->getContent());
    }
}