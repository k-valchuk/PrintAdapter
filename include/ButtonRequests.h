#pragma once
#include "TagDialog.h"
#include "ExportDialog.h"
#include "ServerRequester.h"
#include "GetElementDialog.h"

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

void addTemplate(ExportDialog* dialog, ServerRequester* server_requester) {
    QJsonObject jsonObj;
    jsonObj["name"] = dialog->getName();
    jsonObj["content"] = dialog->getContent();
    server_requester->addTemplate(
        QJsonDocument(jsonObj)
    );
}

void exportTemplate(ExportDialog* dialog, ServerRequester* server_requester) {
    QJsonObject jsonObj;
    jsonObj["template_name"] = dialog->getName();
    jsonObj["data"] = QJsonDocument::fromJson(
        dialog->getContent().toUtf8()
    ).object();
    server_requester->exportTemplate(
        QJsonDocument(jsonObj)
    );
}

void getTemplate(GetElementDialog* dialog, ServerRequester* server_requester) {
    server_requester->getTemplate(dialog->getContent());
}

void removeTag(GetElementDialog* dialog, ServerRequester* server_requester) {
    server_requester->removeTag(dialog->getContent());
}

void removeTemplate(GetElementDialog* dialog, ServerRequester* server_requester) { 
    server_requester->removeTemplate(dialog->getContent());
}