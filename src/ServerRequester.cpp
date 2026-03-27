#include "ServerRequester.h"
#include "Requester.h"
#include <QtWidgets>
#include <QMessageBox>

ServerRequester::ServerRequester(QObject* pobj, QString base_url) : QObject(pobj), base_url(base_url) {
    requester = new Requester(this);

    connect(requester, SIGNAL(done(const int&, const QByteArray&)), this, SLOT(slotDone(const int&, const QByteArray&)));
    connect(requester, SIGNAL(error(QString, int)), this, SLOT(slotError(QString, int)));
}

void ServerRequester::getAllTemplates() {
    requester->restRequest(QUrl(QString("%1/db/get_all_templates").arg(base_url)), RequestTypes::GET, nullptr);
}

void ServerRequester::slotDone(const int& http, const QByteArray& byteArray){

    if (http < 200 || http >= 300) {
        slotError(QString("Bad HTTP request: %1").arg(http), http);
        return;
    }
    QJsonParseError pe{};
    const QJsonDocument doc = QJsonDocument::fromJson(byteArray, &pe);
    if (pe.error  != QJsonParseError::NoError) {
        slotError(pe.errorString(), http);
        return;
    }

    emit done(http, doc);
}

void ServerRequester::slotError(QString message, int httpStatus){
    emit error(message, httpStatus);
}
