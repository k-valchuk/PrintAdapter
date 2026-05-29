#include "application.h"
#include <QMessageBox>
#include <QFile>

class DefaultTheme : public IApplicationTheme
{
public:
    QUuid GetThemeId() override
    {
        return QUuid::fromString(QString("dff75216-8f89-47f6-b3d9-9341b60cfc8f"));
    }
    
    void SetTheme(Application *app)
    {
        //задать палитру
        QPalette mainPal(app->palette());
        mainPal.setColor(QPalette::Window, QColor("#272727"));
        mainPal.setColor(QPalette::Shadow, QColor("#353535"));//цвет строки верхнего уровня в списке
        mainPal.setColor(QPalette::Highlight, QColor("#38444d"));//выделение
        
        mainPal.setColor(QPalette::Mid, QColor("#1F1F1F"));//темнее Dark
        mainPal.setColor(QPalette::Button, QColor("#2F363A"));//фон кнопки
        mainPal.setColor(QPalette::Midlight, QColor("#333333"));//фон разделителя строк
        
        //текст
        mainPal.setColor(QPalette::Text, QColor("#A8A8A8" ));
        mainPal.setColor(QPalette::WindowText, QColor("#A8A8A8"));
        mainPal.setColor(QPalette::ButtonText, QColor("#A8A8A8"));
        mainPal.setColor(QPalette::Dark, QColor("#7F7F7F"));//альтернативный цвет текста
        mainPal.setColor(QPalette::HighlightedText, QColor("#6B9CBF"));//выделенный текст
        
        app->setPalette(mainPal);
        
        app->setFont(QFont("Calibri", 11, QFont::Normal));
        
        QString qssStr;
        auto f = [&qssStr](const QString& nameFile)
        {
            QFile styleFile;
            styleFile.setFileName(nameFile);
            styleFile.open(QFile::ReadOnly);
            qssStr += styleFile.readAll();
            styleFile.close();
        };
        f(":/calendar.qss");
        f(":/checkbox.qss");
        f(":/combobox.qss");
        f(":/datetime.qss");
        f(":/dialog.qss");
        f(":/dockwidget.qss");
        f(":/groupbox.qss");
        f(":/headerview.qss");
        f(":/label.qss");
        f(":/lineedit.qss");
        f(":/listview.qss");
        f(":/mainwindow.qss");
        f(":/menus.qss");
        f(":/messagebox.qss");
        f(":/plaintextedit.qss");
        f(":/progressbar.qss");
        f(":/pushbutton.qss");
        f(":/radiobutton.qss");
        f(":/scrollarea.qss");
        f(":/scrollbox.qss");
        f(":/sizegrip.qss");
        f(":/slider.qss");
        f(":/spinbox.qss");
        f(":/splitter.qss");
        f(":/tabbar.qss");
        f(":/tableview.qss");
        f(":/tabwidget.qss");
        f(":/toolbar.qss");
        f(":/tooltip.qss");
        f(":/treeview.qss");    
        f(":/ads.qss");
        app->setStyleSheet(qssStr);
    }
};

Application::Application(int &argc, char **argv, IApplicationTheme *pTheme) : QApplication(argc, argv)
{
    if (!pTheme)
    {
        static DefaultTheme theme;
        pTheme = &theme;
    }

    SetTheme(pTheme);
}

Application::~Application()
{

}

void Application::SetTheme(IApplicationTheme *theme)
{
    theme->SetTheme(this);
    emit themeChanged(theme);
}

bool Application::checkSingleApplication(const QString &keyID)
{
    #ifndef Q_OS_WIN32
        // в linux разделяемая память не освобождается при аварийном завершении приложения,
        // поэтому необходимо избавиться от данного мусора
        QSharedMemory nixFixSharedMemory(keyID);
        if(nixFixSharedMemory.attach())
        {
            nixFixSharedMemory.detach();
        }
    #endif

    sharedMemorySingleApp.setKey(keyID);    // установить ключ
    bool isRunning;                         // переменная для проверки уже запущенного приложения
    if (sharedMemorySingleApp.attach())     // пытаемся присоединить экземпляр разделяемой памяти к уже существующему сегменту
    {
        isRunning = true;                   // если успешно, то определяем, что уже есть запущенный экземпляр
    }
    else
    {
        //в противном случае выделяем 1 байт памяти и определяем, что других экземпляров не запущено
        sharedMemorySingleApp.create(1);
        isRunning = false;
    }

    // Если уже запущен один экземпляр приложения, то сообщаем об этом пользователю
    // и завершаем работу текущего экземпляра приложения
    if(isRunning)
    {
        QMessageBox::warning(0, tr("Error!"), tr("The application is already running!"));
        return false;
    }
    return true;
}

bool Application::event(QEvent* e)
{
    if(e->type() == QEvent::LanguageChange)
        emit languageChanged();

    return QApplication::event(e);
}
