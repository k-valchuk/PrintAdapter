#ifndef PRINTMODULE_H
#define PRINTMODULE_H


#include <IModule.h>
#include "IPrintInterface.h"

class PrintModule : public IModule {

    class PrintInterface: public IPrintInterface {
        PrintModule* m_module;

        public:
        void OpenShowTemplate(QString subSystemId, QJsonDocument jsonDoc, QWidget* parentWidget) override;

    };

    PrintInterface m_printInterface;

    public:
        PrintModule();
        ~PrintModule();

        QUuid GetModuleTypeIdentifier() override;

        void FillSettingsMenu(IMenuManager<IWindowManager*>* mm) override;
        int GetWidgetType(IModuleWidget* wgt) override;
        Version GetVersion() override;
        QString GetName() override;

        void OnThemeChanged(IThemeManager* tm) override
        {
            auto themeStr = tm->CurrentTheme() == IWindowManager::ColorTheme::Dark ? "dark" : "dark"; // TODO: светлая тема нужна
            tm->LoadStyleSheet(QString(":/styles/%1/combobox.qss").arg(themeStr));
            tm->LoadStyleSheet(QString(":/styles/%1/dialogs.qss").arg(themeStr));
            tm->LoadStyleSheet(QString(":/styles/%1/qlistview.qss").arg(themeStr));
            tm->LoadStyleSheet(QString(":/styles/%1/scrollarea.qss").arg(themeStr));
            tm->LoadStyleSheet(QString(":/styles/%1/scrollbox.qss").arg(themeStr));
            tm->LoadStyleSheet(QString(":/styles/%1/tables.qss").arg(themeStr));
            tm->LoadStyleSheet(QString(":/styles/%1/tabwidget.qss").arg(themeStr));
            tm->LoadStyleSheet(QString(":/styles/%1/toolbar.qss").arg(themeStr));
        }

        void Init(IWindowManager *wm, IModuleLoader *loader) {}
        IModuleWidget *CreateWidget(int typeId) {return nullptr;}


        QSet<IBaseInterface *> GetSupportedInterfaces() override {
            return {&m_printInterface};
        }
};

#endif // PRINTMODULE_H
