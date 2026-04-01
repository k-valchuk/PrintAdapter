#include "GetTemplatteButton.h"
#include "GetTemplateDialog.h"

GetTemplateButton::GetTemplateButton(
    QWidget* pwgt, 
    ServerRequester* server_requester,
    QLayout* layout 
): BaseRequestButton(pwgt, server_requester, layout, "Показать шаблон") {}

void GetTemplateButton::sendRequest(ActionId buttonId) {
    GetTemplateDialog* dialog = new GetTemplateDialog;
    if (dialog->exec() == QDialog::Accepted) {
        server_requester->setCurrentButton(buttonId);
        server_requester->getTemplate(dialog->getContent());
    }
}