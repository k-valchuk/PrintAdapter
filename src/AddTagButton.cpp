#include "AddTagButton.h"
#include "TagDialog.h"

AddTagButton::AddTagButton(
    QWidget* pwgt, 
    ServerRequester* server_requester,
    QLayout* layout,
    QButtonGroup* button_group,
    ActionId buttonId
) : BaseRequestButton(
    pwgt, 
    server_requester, 
    layout, 
    "Добавить/изменить тэг", 
    button_group, 
    buttonId
) {}


void AddTagButton::sendRequest(ActionId buttonId) {
    TagDialog* dialog = new TagDialog(this);
    if (dialog->exec() == QDialog::Accepted) {
        QJsonObject jsonObj;
        
        jsonObj["name"] = dialog->getName();
        jsonObj["description"] = dialog->getDescription();
        jsonObj["subsystem"] = dialog->getSubsystem();
        jsonObj["alias"] = dialog->getAlias();
        server_requester->setCurrentButton(buttonId);
        server_requester->addTag(
            QJsonDocument(jsonObj)
        );
    }
}