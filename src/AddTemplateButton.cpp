#include "AddTemplateButton.h"
#include "ExportDialog.h"

AddTemplateButton::AddTemplateButton(QWidget* pwgt, ServerRequester* server_requester) : QPushButton(pwgt), server_requester(server_requester) {
    connect(this, SIGNAL(clicked()), SLOT(sendRequestSlot()));
    connect(server_requester, SIGNAL(done(int, const QJsonDocument)), this, SLOT(getResultRequestSlot(int, const QJsonDocument)));
    connect(server_requester, SIGNAL(error(QString, int)), this, SLOT(getErrorRequestSlot(QString, int)));
    setText("Добавить/изменить шаблон");
}

void AddTemplateButton::sendRequestSlot() {
    ExportDialog* dialog = new ExportDialog;
    if (dialog->exec() == QDialog::Accepted) {
        QJsonObject jsonObj;
        jsonObj["name"] = dialog->getName();
        jsonObj["content"] = dialog->getContent();
        server_requester->addTemplate(
            QJsonDocument(jsonObj)
        );
    }
}

void AddTemplateButton::getResultRequestSlot(const int& http, const QJsonDocument doc) {
    if (doc.isEmpty()) {
        qDebug() << "Gocha AddTemplateButton" << "\n";

        qDebug() << "Send results AddTemplateButton" << "\n";
        emit done(QString("Добавлен шаблон"), ContentType::TEXT);
    }
}

void AddTemplateButton::getErrorRequestSlot(QString message, int httpStatus) {
    qDebug() << "Error" << message << "\n";
    emit done(message, ContentType::TEXT);
}