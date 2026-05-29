//-------------------------------------------------------------------------------------------------
// DockWidget с автосворачиванием в закладки
//-------------------------------------------------------------------------------------------------
#pragma once

#include "QDockWidget"

class QPushButton;
class QVBoxLayout;
class AutoHideDockWidgetTitle;

namespace ads {
    class CDockWidget;
}

class AutoHideDockWidget : public QDockWidget
{
	Q_OBJECT
public:
    explicit AutoHideDockWidget(const QString& title, Qt::DockWidgetArea defArea);
    ~AutoHideDockWidget();

    //текст заголовка
	void setWindowTitle(const QString& title);

    //сторона прикрепления
    Qt::DockWidgetArea getArea() const { return area; }

    //показать из закладки
    void slide(int maxLength);

    //получить/задать данные dockWidget в закладке
    int getUserLength() const {return userLength;}
    void setUserLength(int len){userLength = len;}

    //актуальная ширина и высота dock с учетом userLength
    int getActualWidth() const;
    int getActualHeight() const;

    void setDockWidgetADS(ads::CDockWidget* dock) {ptr_adsDockWidget = dock;}
    ads::CDockWidget* getDockWidgetADS()const {return ptr_adsDockWidget;}

    //пользовательская длина окна из закладки
    static int getUserLengthInTab(ads::CDockWidget *doc);
    static void setUserLengthInTab(ads::CDockWidget *doc, int value);

protected:
    bool eventFilter(QObject *target, QEvent *event) override;

signals:
    void sig_pinned(AutoHideDockWidget* dockWidget, bool bClose);           //dockWidget прикреплен
    void sig_close(AutoHideDockWidget* dockWidget);                         //закрыть dockWidget

private slots:
    void slot_closeDockWidget();                                //закрыть dockWidget
    void slot_autoHideStateToggled();                           //автосворачивание

private:
    Qt::DockWidgetArea area;                        //сторона прикрепления
    AutoHideDockWidgetTitle* titleWidget;           //заголовок

    int userLength;                                 //ширина заданная пользователем
    bool bDragMode;                                 //режим изменения размера виджета пользователем

    ads::CDockWidget *ptr_adsDockWidget;            //указатель на dock widget из ads


    bool mouseInResizePosition(const QPoint &pos);  //находится ли указатель мыши на границе виджета
};
