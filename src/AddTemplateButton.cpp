#include "AddTemplateButton.h"
#include "ExportDialog.h"

AddTemplateButton::AddTemplateButton(
    QWidget* pwgt, 
    ServerRequester* server_requester,
    QLayout* layout,
    QButtonGroup* button_group,
    ActionId buttonId
) : BaseRequestButton(
    pwgt, 
    server_requester, 
    layout, 
    "Добавить/изменить шаблон",
    button_group, 
    buttonId
) {}

void AddTemplateButton::sendRequest(ActionId buttonId) {
    ExportDialog* dialog = new ExportDialog;
    if (dialog->exec() == QDialog::Accepted) {
        QJsonObject jsonObj;
        jsonObj["name"] = dialog->getName();
        jsonObj["content"] = dialog->getContent();
        server_requester->setCurrentButton(buttonId);
        server_requester->addTemplate(
            QJsonDocument(jsonObj)
        );
    }
}

