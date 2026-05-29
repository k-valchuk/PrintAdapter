#include "PrintModule.h"
#include "version.h"
#include <QApplication>
#include "EditDialog.h"
#include "RequestDialog.h"

PrintModule::PrintModule() {}

PrintModule::~PrintModule() {}

int PrintModule::GetWidgetType(IModuleWidget* wgt){
    return 0;
}


QUuid PrintModule::GetModuleTypeIdentifier()
{
    return QUuid::fromString(QString("f9d71c26-a790-4e95-9776-e39d31046fe8"));
}

QString PrintModule::GetName()
{
    return QApplication::tr("Настройка печати шаблонов");
}


IModule::Version PrintModule::GetVersion()
{
    return IModule::Version{VER_FILEVERSION};
}

void PrintModule::FillSettingsMenu(IMenuManager<IWindowManager*>* mm) {
    mm->AddAction(LAMBDA(QApplication::tr("Настройка печати шаблонов")), [](IWindowManager* wm) {
        EditDialog* pEditDialog = new EditDialog(wm->GetMainWindow());
        pEditDialog->setAttribute(Qt::WA_DeleteOnClose); 
        pEditDialog->show(); 
    });
}

void PrintModule::PrintInterface::OpenShowTemplate(QString subSystemId, QJsonDocument jsonDoc, QWidget* parentWidget){
    if (jsonDoc.isEmpty()){
        QFile file(":/json_data/mock.json");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Could not open file:" << file.errorString();
        }
        QByteArray jsonData = file.readAll();
        file.close();
        QJsonParseError parseError;
        jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "Parse error at" << parseError.offset << ":" << parseError.errorString();
        }
    }
    RequestDialog* pShowTemplatesDialog = new RequestDialog(parentWidget, jsonDoc, subSystemId);
    pShowTemplatesDialog->setAttribute(Qt::WA_DeleteOnClose); 
    pShowTemplatesDialog->show(); 
}