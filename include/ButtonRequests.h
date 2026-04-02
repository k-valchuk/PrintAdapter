#pragma once
#include "TagDialog.h"
#include "ExportDialog.h"
#include "ServerRequester.h"
#include "GetTemplateDialog.h"

void addTag(QWidget* pwgt, ServerRequester* server_requester) {
    TagDialog* dialog = new TagDialog(pwgt);
    if (dialog->exec() == QDialog::Accepted) {
        QJsonObject jsonObj;
        
        jsonObj["name"] = dialog->getName();
        jsonObj["description"] = dialog->getDescription();
        jsonObj["subsystem"] = dialog->getSubsystem();
        jsonObj["alias"] = dialog->getAlias();
        server_requester->addTag(
            QJsonDocument(jsonObj)
        );
    }
    delete dialog;
}

void addTemplate(QWidget* pwgt, ServerRequester* server_requester) {
    ExportDialog* dialog = new ExportDialog(pwgt);
    if (dialog->exec() == QDialog::Accepted) {
        QJsonObject jsonObj;
        jsonObj["name"] = dialog->getName();
        jsonObj["content"] = dialog->getContent();
        server_requester->addTemplate(
            QJsonDocument(jsonObj)
        );
    }
    delete dialog;
}

void exportTemplate(QWidget* pwgt, ServerRequester* server_requester) {
    ExportDialog* pExportDialog = new ExportDialog(pwgt);
    if (pExportDialog->exec() == QDialog::Accepted){
        QJsonObject jsonObj;
        jsonObj["template_name"] = pExportDialog->getName();
        jsonObj["data"] = QJsonDocument::fromJson(
            pExportDialog->getContent().toUtf8()
        ).object();
        server_requester->exportTemplate(
            QJsonDocument(jsonObj)
        );

    }
    delete pExportDialog;
}

void getTemplate(QWidget* pwgt, ServerRequester* server_requester) { // TODO 
    GetTemplateDialog* dialog = new GetTemplateDialog(pwgt);
    if (dialog->exec() == QDialog::Accepted) {
        server_requester->getTemplate(dialog->getContent());
    }
    delete dialog;
}

void removeTag(QWidget* pwgt, ServerRequester* server_requester) { // TODO 
    GetTemplateDialog* dialog = new GetTemplateDialog(pwgt);
    if (dialog->exec() == QDialog::Accepted) {
        server_requester->removeTag(dialog->getContent());
    }
    delete dialog;
}

void removeTemplate(QWidget* pwgt, ServerRequester* server_requester) { // TODO 
    GetTemplateDialog* dialog = new GetTemplateDialog;
    if (dialog->exec() == QDialog::Accepted) {
        server_requester->removeTemplate(dialog->getContent());
    }
    delete dialog;
}