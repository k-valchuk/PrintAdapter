#include "RemoveTemplateButton.h"
#include "GetTemplateDialog.h"

RemoveTemplateButton::RemoveTemplateButton(
    QWidget* pwgt, 
    ServerRequester* server_requester,
    QLayout* layout,
    QButtonGroup* button_group,
    ActionId buttonId 
) : BaseRequestButton(
    pwgt, 
    server_requester, 
    layout, 
    "Удалить шаблон",
    button_group, 
    buttonId
) {}

void RemoveTemplateButton::sendRequest(ActionId buttonId) {
    GetTemplateDialog* dialog = new GetTemplateDialog;
    if (dialog->exec() == QDialog::Accepted) {
        server_requester->setCurrentButton(buttonId);
        server_requester->removeTemplate(dialog->getContent());
    }
}