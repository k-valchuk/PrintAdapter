#include "ServerRequester.h"
#include "Requester.h"
#include <QtWidgets>
#include <QMessageBox>

ServerRequester::ServerRequester(QObject* pobj, QString base_url) : QObject(pobj), base_url(base_url) {
    requester = new Requester(this);

    connect(requester, SIGNAL(done(const int&, const QByteArray&)), this, SLOT(slotDone(const int&, const QByteArray&)));
    connect(requester, SIGNAL(error(QString, int)), this, SLOT(slotError(QString, int)));
}

void ServerRequester::getAllTemplates(bool isActive, QString subsystemName) {
    setCurrentButton(ActionId::GET_ALL_TEMPLATES);
    qDebug() << "subsystemName" << subsystemName;
    requester->restRequest(QUrl(QString("%1/db/get_all_templates?active=%2&subsystem=%3").arg(base_url).arg(isActive).arg(subsystemName)), RequestTypes::GET, nullptr);
}

void ServerRequester::getAllTags() {
    setCurrentButton(ActionId::GET_ALL_TAGS);
    requester->restRequest(QUrl(QString("%1/db/get_all_tags").arg(base_url)), RequestTypes::GET, nullptr);
}

void ServerRequester::getTemplate(QString templateId) {
    requester->restRequest(QUrl(QString("%1/db/get_template?id=%2").arg(base_url).arg(templateId)), RequestTypes::GET, nullptr);
}

void ServerRequester::exportTemplate(int templateId) {
    QJsonObject jsonObj;
    jsonObj["template_id"] = templateId;
    jsonObj["data"] = exportJson.object();
    requester->restRequest(
        QUrl(QString("%1/export").arg(base_url)), 
        RequestTypes::POST, 
        QJsonDocument(jsonObj).toJson()
    );
}

void ServerRequester::removeTemplate(QString templateId) {
    requester->restRequest(
        QUrl(QString("%1/db/delete_template?id=%2").arg(base_url).arg(templateId)), 
        RequestTypes::DELETE_RESOURCE, 
        nullptr
    );
}

void ServerRequester::removeTag(QString tagId) {
    requester->restRequest(
        QUrl(QString("%1/db/delete_tag?id=%2").arg(base_url).arg(tagId)), 
        RequestTypes::DELETE_RESOURCE, 
        nullptr
    );
}

void ServerRequester::addTemplate(const QJsonDocument jsonDoc) {
    requester->restRequest(
        QUrl(QString("%1/db/add_template").arg(base_url)), 
        RequestTypes::POST, 
        jsonDoc.toJson()
    );
}

void ServerRequester::addTag(const QJsonDocument jsonDoc) {
    requester->restRequest(
        QUrl(QString("%1/db/add_tag").arg(base_url)), 
        RequestTypes::POST, 
        jsonDoc.toJson()
    );
}

void ServerRequester::setCurrentButton(ActionId buttonId) {
    currentButtonId = buttonId;
}

void ServerRequester::setExportJson(QJsonDocument json_doc){
    exportJson = json_doc;
}

QJsonDocument ServerRequester::getExportJson(){
    return exportJson;
}

void ServerRequester::slotDone(const int& http, const QByteArray& byteArray){

    if (http < 200 || http >= 300) {
        slotError(QString("Bad HTTP request: %1").arg(http), http);
        return;
    }
    QJsonParseError pe{};
    const QJsonDocument doc = QJsonDocument::fromJson(byteArray, &pe);
    if (pe.error  != QJsonParseError::NoError) {
        const QJsonDocument doc = QJsonDocument();
        emit done(http, doc, currentButtonId);
        return;
    }

    emit done(http, doc, currentButtonId);
}

void ServerRequester::slotError(QString message, int httpStatus){
    qDebug() << "ServerRequester Error" << message << httpStatus << "\n";
    emit error(message, httpStatus);
}
