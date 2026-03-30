#include "ExportButton.h"
#include "ExportDialog.h"

ExportButton::ExportButton(QWidget* pwgt, ServerRequester* server_requester): QPushButton(pwgt), server_requester(server_requester) {
    connect(this, SIGNAL(clicked()), SLOT(sendRequestSlot()));
    connect(server_requester, SIGNAL(done(int, const QJsonDocument)), this, SLOT(getResultRequestSlot(int, const QJsonDocument)));
    connect(server_requester, SIGNAL(error(QString, int)), this, SLOT(getErrorRequestSlot(QString, int)));
    setText("Экспорт Шаблона");
}

void ExportButton::sendRequestSlot() {
    ExportDialog* pExportDialog = new ExportDialog;
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

void ExportButton::getResultRequestSlot(const int& http, const QJsonDocument doc) {
    if (doc.isObject()) {
        qDebug() << "Gocha ExportButton" << "\n";
        const QJsonObject root = doc.object();
        qDebug() << "Send results ExportButton" << "\n";
        emit done(root.value("content").toString(), ContentType::HTML);
    }
}

void ExportButton::getErrorRequestSlot(QString message, int httpStatus) {
    qDebug() << "Error" << message << "\n";
    emit done(message, ContentType::TEXT);
}