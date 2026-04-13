#pragma once
#include "TagDialog.h"
#include "AddTemplateDialog.h"
#include "ServerRequester.h"

template <typename TDialog, typename Func>
void baseRequest(
    QWidget* pwgt, 
    ServerRequester* server_requester, 
    Func requestFunc
) {
    TDialog* dialog = new TDialog(pwgt);
    if (dialog->exec() == QDialog::Accepted) {
        requestFunc(dialog, server_requester);
    }
    delete dialog;
}

void addTag(TagDialog* dialog, ServerRequester* server_requester) {
    QJsonObject jsonObj;
    jsonObj["name"] = dialog->getName();
    jsonObj["description"] = dialog->getDescription();
    jsonObj["subsystem"] = dialog->getSubsystem();
    jsonObj["alias"] = dialog->getAlias();
    server_requester->addTag(
        QJsonDocument(jsonObj)
    );
}

void addTemplate(AddTemplateDialog* dialog, ServerRequester* server_requester) {
    QJsonObject jsonObj;
    jsonObj["name"] = dialog->getName();
    jsonObj["content"] = dialog->getContent();
    server_requester->addTemplate(
        QJsonDocument(jsonObj)
    );
}