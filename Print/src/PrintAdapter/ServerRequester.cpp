#include "ServerRequester.h"
#include <QtWidgets>
#include <QMessageBox>
#include <QDebug>

ServerRequester::ServerRequester(QObject* pobj, QString base_url) : QObject(pobj), base_url(base_url) {
    //rest::Executor()->SetBaseUrl("localhost:8111");
}

QJsonDocument ServerRequester::getAllTemplates(bool isActive, QString subsystemId) {
    setCurrentButton(ActionId::GET_ALL_TEMPLATES);
    GetAllTemplates req = GetAllTemplates(isActive, subsystemId);
    slotDone(req);
    return req.getData();
}

QJsonDocument ServerRequester::getTemplateTypes(QString subsystemId) {
    GetTemplateTypes req = GetTemplateTypes(subsystemId);
    slotDone(req);
    return req.getData();
}

void ServerRequester::getAllTags(QString subsystemId) {
    setCurrentButton(ActionId::GET_ALL_TAGS);
    GetAllTags req = GetAllTags(subsystemId);
    slotDone(req);
}

void ServerRequester::getTemplate(QString templateId, QString subsystemId) {
    GetTemplate req = GetTemplate(templateId, subsystemId);
    slotDone(req);
}

void ServerRequester::exportTemplate(int templateId, QString subsystemId) {
    ExportTemplate req = ExportTemplate(templateId, exportJson, subsystemId);
    qDebug() << req.response().answerString();
    slotDone(req);
}

void ServerRequester::removeTemplate(QString templateId, QString subsystemId) {
    DeleteTemplate req = DeleteTemplate(templateId, subsystemId);
    slotDone(req);
}

void ServerRequester::removeTag(QString tagId, QString subsystemId) {
    DeleteTag req = DeleteTag(tagId, subsystemId);
    slotDone(req);
}

QString ServerRequester::addTemplate(const QJsonDocument jsonDoc, QString subsystemId) {
    AddTemplate req = AddTemplate(jsonDoc, subsystemId);
    slotDone(req);
    return QString::number(req.getData().object().value("id").toInt());
}

void ServerRequester::addTag(const QJsonDocument jsonDoc, QString subsystemId) {
    AddTag req = AddTag(jsonDoc, subsystemId);
    slotDone(req);
}

void ServerRequester::setCurrentButton(ActionId buttonId) {
    currentButtonId = buttonId;
}

void ServerRequester::setTemplateName(QString templateName) {
    exportTemplateName = templateName;
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

    QJsonDocument result;
    if (exportTemplateName != "") {
        QString fileContent = req.getDataFile();

        QJsonObject fileData;
        fileData["name"] = exportTemplateName;
        fileData["content"] = fileContent;

        result = QJsonDocument(fileData);
        exportTemplateName = "";
    } else {
        result = req.getData();
    }




    emit done(result, currentButtonId);
}

void ServerRequester::slotError(IPrintRequest req){
    qDebug() << "ServerRequester Error" << req.query().url() << req.response().answerString().toUtf8() << req.response().code() << "\n";
    emit error(req.response().answerString().toUtf8(), req.response().code());
}
