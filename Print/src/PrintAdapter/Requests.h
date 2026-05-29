#pragma once
#include "Request.h"
#include <QJsonDocument>
#include <QJsonArray>

class IPrintRequest: public rest::Request {

    public:
        QJsonDocument getData() {return QJsonDocument::fromJson(response().answerString().toUtf8());};

};


struct GetAllTemplates: public IPrintRequest {
    GetAllTemplates(bool isActive, QString subsystemId);

};

struct GetAllTags: public IPrintRequest{
    
    GetAllTags();

};

struct GetTemplate: public IPrintRequest{
    
    GetTemplate(QString templateId);

};

struct ExportTemplate: public IPrintRequest{
    
    ExportTemplate(int templateId, QJsonDocument json_doc);

};

struct DeleteTemplate: public IPrintRequest{
    
    DeleteTemplate(QString templateId);

};

struct DeleteTag: public IPrintRequest{
    
    DeleteTag(QString tagId);

};

struct AddTemplate: public IPrintRequest{
    
    AddTemplate(QJsonDocument jsonDoc);

};


struct AddTag: public IPrintRequest{
    
    AddTag(QJsonDocument jsonDoc);

};
