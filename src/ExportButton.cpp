#include "ExportButton.h"
#include "ExportDialog.h"

ExportButton::ExportButton(
    QWidget* pwgt, 
    ServerRequester* server_requester,
    QLayout* layout 
): BaseRequestButton(pwgt, server_requester, layout, "Экспорт Шаблона") {}

void ExportButton::sendRequest(ActionId buttonId) {
    ExportDialog* pExportDialog = new ExportDialog;
    if (pExportDialog->exec() == QDialog::Accepted){
        QJsonObject jsonObj;
        jsonObj["template_name"] = pExportDialog->getName();
        jsonObj["data"] = QJsonDocument::fromJson(
            pExportDialog->getContent().toUtf8()
        ).object();
        server_requester->setCurrentButton(buttonId);
        server_requester->exportTemplate(
            QJsonDocument(jsonObj)
        );

    }
    delete pExportDialog;
}