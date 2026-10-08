#pragma once
#include "Request.h"
#include <QJsonDocument>
#include <QJsonArray>

class IPrintRequest: public rest::Request {

    public:
        QString getDataFile() {
            return response().answerString().toUtf8();
        };
        QJsonDocument getData() {return QJsonDocument::fromJson(response().answerString().toUtf8());};

};


struct GetAllTemplates: public IPrintRequest {
    GetAllTemplates(bool isActive, QString subsystemId);

};

struct GetAllTags: public IPrintRequest{
    
    GetAllTags(QString subsystemId);

};

struct GetTemplate: public IPrintRequest{
    
    GetTemplate(QString templateId, QString subsystemId);

};

struct ExportTemplate: public IPrintRequest{
    
    ExportTemplate(int templateId, QJsonDocument json_doc, QString subsystemId);

};

struct DeleteTemplate: public IPrintRequest{
    
    DeleteTemplate(QString templateId, QString subsystemId);

};

struct DeleteTag: public IPrintRequest{
    
    DeleteTag(QString tagId, QString subsystemId);

};

struct AddTemplate: public IPrintRequest{
    
    AddTemplate(QJsonDocument jsonDoc, QString subsystemId);

};


struct AddTag: public IPrintRequest{
    
    AddTag(QJsonDocument jsonDoc, QString subsystemId);

};

struct GetTemplateTypes: public IPrintRequest{
    
    GetTemplateTypes(QString subsystemId);

};
