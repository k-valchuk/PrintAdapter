//-------------------------------------------------------------------------------------------------
// Приложение с доп. функционалом:
// - проверка на единственный экземпляр приложения
// - подключена темная тема

//-------------------------------------------------------------------------------------------------
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QUuid>
#include <QSharedMemory>

class Application;

class IApplicationTheme
{
public:
    virtual void SetTheme(Application* app) = 0;
    virtual QUuid GetThemeId() = 0;
};

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv, IApplicationTheme *pTheme = nullptr);
    virtual ~Application();

    void SetTheme(IApplicationTheme* theme);

    bool checkSingleApplication(const QString &keyID);              //проверка на единственный экземпляр приложения
private:
    QSharedMemory sharedMemorySingleApp;

protected:
    bool event(QEvent* e) override;

signals:
    void languageChanged();
    void themeChanged(IApplicationTheme* theme);
};

#endif // APPLICATION_H
