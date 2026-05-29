#include "ServerRequester.h"
#include <QtWidgets>
#include <QMessageBox>

ServerRequester::ServerRequester(QObject* pobj, QString base_url) : QObject(pobj), base_url(base_url) {
    rest::Executor()->SetBaseUrl("localhost:8000"); // TODO: временно + убрать потом наверное
}

void ServerRequester::getAllTemplates(bool isActive, QString subsystemId) {
    setCurrentButton(ActionId::GET_ALL_TEMPLATES);
    GetAllTemplates req = GetAllTemplates(isActive, subsystemId);
    slotDone(req);
}

void ServerRequester::getAllTags() {
    setCurrentButton(ActionId::GET_ALL_TAGS);
    GetAllTags req = GetAllTags();
    slotDone(req);
}

void ServerRequester::getTemplate(QString templateId) {
    GetTemplate req = GetTemplate(templateId);
    slotDone(req);
}

void ServerRequester::exportTemplate(int templateId) {
    ExportTemplate req = ExportTemplate(templateId, exportJson);
    slotDone(req);
}

void ServerRequester::removeTemplate(QString templateId) {
    DeleteTemplate req = DeleteTemplate(templateId);
    slotDone(req);
}

void ServerRequester::removeTag(QString tagId) {
    DeleteTag req = DeleteTag(tagId);
    slotDone(req);
}

void ServerRequester::addTemplate(const QJsonDocument jsonDoc) {
    AddTemplate req = AddTemplate(jsonDoc);
    slotDone(req);
}

void ServerRequester::addTag(const QJsonDocument jsonDoc) {
    AddTag req = AddTag(jsonDoc);
    slotDone(req);
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

void ServerRequester::slotDone(IPrintRequest req){
    if (!req.isSuccess()) {
        slotError(req);
        return;
    }


    emit done(req.getData(), currentButtonId);
}

void ServerRequester::slotError(IPrintRequest req){
    qDebug() << "ServerRequester Error" << req.query().url() << req.response().answerString().toUtf8() << req.response().code() << "\n";
    emit error(req.response().answerString().toUtf8(), req.response().code());
}
