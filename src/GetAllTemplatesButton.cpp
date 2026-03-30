#include "GetAllTemplatesButton.h"
#include <QDebug>

GetAllTemplatesButton::GetAllTemplatesButton(QWidget* pwgt, ServerRequester* server_requester) : QPushButton(pwgt), server_requester(server_requester) {
    connect(this, SIGNAL(clicked()), SLOT(sendRequestSlot()));
    connect(server_requester, SIGNAL(done(int, const QJsonDocument)), this, SLOT(getResultRequestSlot(int, const QJsonDocument)));
    connect(server_requester, SIGNAL(error(QString, int)), this, SLOT(getErrorRequestSlot(QString, int)));
    setText("Все шаблоны");
}

void GetAllTemplatesButton::sendRequestSlot() {
    qDebug() << "Send" << "\n";
    server_requester->getAllTemplates();
    qDebug() << "Wait" << "\n";
}

 void GetAllTemplatesButton::getResultRequestSlot(const int& http, const QJsonDocument doc) {
    qDebug() << "Gocha GetAllTemplatesButton" << "\n";
    const QJsonArray root = doc.array();

    QVector<QString> content;
    for (const QJsonValue& value : root) {
        content.append(value.toString());
    }
    
    if (!content.isEmpty()) {
        qDebug() << "Send results GetAllTemplatesButton" << "\n";
        emit done(content.toList().join("\n"), ContentType::TEXT);
    }
}

void GetAllTemplatesButton::getErrorRequestSlot(QString message, int httpStatus) {
    qDebug() << "Error" << message << "\n";
    emit done(message, ContentType::TEXT);
}