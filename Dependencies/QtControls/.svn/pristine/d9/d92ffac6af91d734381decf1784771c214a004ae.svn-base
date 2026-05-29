//-------------------------------------------------------------------------------------------------
// Заголовок AutoHideDockWidget
//-------------------------------------------------------------------------------------------------
#pragma once

#include "QFrame"

class QPushButton;
class QLabel;

class AutoHideDockWidgetTitle : public QFrame
{
	Q_OBJECT
public:
    AutoHideDockWidgetTitle(bool bHide = true);
    ~AutoHideDockWidgetTitle();

public:
    void setText(const QString& text);                      //установить текст в заголовок

protected:
    void mouseDoubleClickEvent(QMouseEvent *) override;     //игнорирование дбл. клик

signals:
    void sig_autoHideButton_pressed();                      //нажата кнопка свернуть
    void sig_closeButton_pressed();                         //нажата кнопка закрыть

private:
    QPushButton* autoHideButton;
    QPushButton* closeButton;
    QLabel* textLabel;    
};
