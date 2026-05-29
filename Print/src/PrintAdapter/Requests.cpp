#include "Requests.h"
#include <QJsonObject>

GetAllTemplates::GetAllTemplates(bool isActive, QString subsystemId) {
    query()
        .setUrl(
            QString("/db/get_all_templates")
            
        )
        .setQueryParameter("active", isActive ? "true" : "false")
        .setQueryParameter("subsystem", subsystemId)
        .setMethod(rest::Method::GET);
    exec();
}



GetAllTags::GetAllTags() {
    query()
        .setUrl(
            QString("/db/get_all_tags")
        )
        .setMethod(rest::Method::GET);
    exec();
}



GetTemplate::GetTemplate(QString templateId) {
    query()
        .setUrl(
            QString("/db/get_template")
        )
        .setQueryParameter("id", templateId)
        .setMethod(rest::Method::GET);
    exec();
}



ExportTemplate::ExportTemplate(int templateId, QJsonDocument json_doc) {
    QJsonObject jsonObj;
    jsonObj["template_id"] = templateId;
    jsonObj["data"] = json_doc.object();
    query()
        .setUrl(
            QString("/export").arg(templateId)
        )
        .setMethod(rest::Method::POST)
        .setData(QJsonDocument(jsonObj).toJson());
    exec();
}



DeleteTemplate::DeleteTemplate(QString templateId) {
    query()
        .setUrl(
            QString("/db/delete_template")
        )
        .setQueryParameter("id", templateId)
        .setMethod(rest::Method::DEL);
    exec();
}

DeleteTag::DeleteTag(QString tagId) {
    query()
        .setUrl(
            QString("/db/delete_tag")
        )
        .setQueryParameter("id", tagId)
        .setMethod(rest::Method::DEL);
    exec();
}



AddTemplate::AddTemplate(QJsonDocument jsonDoc) {
    query()
        .setUrl(
            QString("/db/add_template")
        )
        .setMethod(rest::Method::POST)
        .setData(jsonDoc.toJson());
    exec();
}

AddTag::AddTag(QJsonDocument jsonDoc) {
    query()
        .setUrl(
            QString("/db/add_tag")
        )
        .setMethod(rest::Method::POST)
        .setData(jsonDoc.toJson());
    exec();
}
