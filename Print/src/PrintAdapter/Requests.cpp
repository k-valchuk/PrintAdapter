#include "Requests.h"
#include <QJsonObject>

GetAllTemplates::GetAllTemplates(bool isActive, QString subsystemId) {
    query()
        .setUrl(
            QString("/api/%1/config/print/db/get_all_templates").arg(subsystemId.toLower())
            
        )
        .setQueryParameter("active", isActive ? "true" : "false")
        .setMethod(rest::Method::GET);
    exec();
}

GetTemplateTypes::GetTemplateTypes(QString subsystemId) {
    query()
        .setUrl(
            QString("/api/%1/config/print/db/get_template_types").arg(subsystemId.toLower())
            
        )
        .setMethod(rest::Method::GET);
    exec();
}



GetAllTags::GetAllTags(QString subsystemId) {
    query()
        .setUrl(
            QString("/api/%1/config/print/db/get_all_tags").arg(subsystemId.toLower())
        )
        .setMethod(rest::Method::GET);
    exec();
}



GetTemplate::GetTemplate(QString templateId, QString subsystemId) {
    query()
        .setUrl(
            QString("/api/%1/config/print/db/get_template").arg(subsystemId.toLower())
        )
        .setQueryParameter("id", templateId)
        .setMethod(rest::Method::GET);
    exec();
}



ExportTemplate::ExportTemplate(int templateId, QJsonDocument json_doc, QString subsystemId) {
    QJsonObject jsonObj;
    jsonObj["template_id"] = templateId;
    jsonObj["data"] = json_doc.object();
    jsonObj["subsystem"] = subsystemId.toLower();
    jsonObj["format"] = "html";
    query()
        .setUrl(
            QString("/api/core/print/export")
        )
        .setMethod(rest::Method::POST)
        .setData(QJsonDocument(jsonObj).toJson());
    exec();
}



DeleteTemplate::DeleteTemplate(QString templateId, QString subsystemId) {
    query()
        .setUrl(
            QString("/api/%1/config/print/db/delete_template").arg(subsystemId.toLower())
        )
        .setQueryParameter("id", templateId)
        .setMethod(rest::Method::DEL);
    exec();
}

DeleteTag::DeleteTag(QString tagId, QString subsystemId) {
    query()
        .setUrl(
            QString("/api/%1/config/print/db/delete_tag").arg(subsystemId.toLower())
        )
        .setQueryParameter("id", tagId)
        .setMethod(rest::Method::DEL);
    exec();
}



AddTemplate::AddTemplate(QJsonDocument jsonDoc, QString subsystemId) {
    query()
        .setUrl(
            QString("/api/%1/config/print/db/add_template").arg(subsystemId.toLower())
        )
        .setMethod(rest::Method::POST)
        .setData(jsonDoc.toJson());
    exec();
}

AddTag::AddTag(QJsonDocument jsonDoc, QString subsystemId) {
    query()
        .setUrl(
            QString("/api/%1/config/print/db/add_tag").arg(subsystemId.toLower())
        )
        .setMethod(rest::Method::POST)
        .setData(jsonDoc.toJson());
    exec();
}
