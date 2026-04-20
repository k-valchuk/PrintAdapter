#pragma once
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

void addTemplate(AddTemplateDialog* dialog, ServerRequester* server_requester) {
    QJsonObject jsonObj;
    jsonObj["name"] = dialog->getName();
    jsonObj["content"] = dialog->getContent();
    server_requester->addTemplate(
        QJsonDocument(jsonObj)
    );
}