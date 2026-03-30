#include "GetTemplatteButton.h"
#include "GetTemplateDialog.h"

GetTemplateButton::GetTemplateButton(QWidget* pwgt, ServerRequester* server_requester): QPushButton(pwgt), server_requester(server_requester) {
    connect(this, SIGNAL(clicked()), SLOT(sendRequestSlot()));
    connect(server_requester, SIGNAL(done(int, const QJsonDocument)), this, SLOT(getResultRequestSlot(int, const QJsonDocument)));
    connect(server_requester, SIGNAL(error(QString, int)), this, SLOT(getErrorRequestSlot(QString, int)));
    setText("Показать шаблон");
}

void GetTemplateButton::sendRequestSlot() {
    GetTemplateDialog* dialog = new GetTemplateDialog;
    if (dialog->exec() == QDialog::Accepted) {
        qDebug() << "Send" << "\n";
        server_requester->getTemplate(dialog->getContent());
        qDebug() << "Wait" << "\n";
    }
}

void GetTemplateButton::getResultRequestSlot(const int& http, const QJsonDocument doc) {
    if (doc.isObject()) {
        qDebug() << "Gocha GetTemplateButton" << "\n";
        const QJsonObject root = doc.object();

        qDebug() << "Send results GetTemplateButton" << "\n";
        emit done(root.value("content").toString(), ContentType::HTML);
    }
}

void GetTemplateButton::getErrorRequestSlot(QString message, int httpStatus) {
    qDebug() << "Error" << message << "\n";
    emit done(message, ContentType::TEXT);
}