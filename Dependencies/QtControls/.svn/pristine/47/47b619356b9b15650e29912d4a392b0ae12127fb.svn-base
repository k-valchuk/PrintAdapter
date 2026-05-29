//-------------------------------------------------------------------------------------------------
// Кнопка панели закладок для DockWidget
//-------------------------------------------------------------------------------------------------
#pragma once

#include <QPushButton>

class DockWidgetTabButton : public QPushButton
{
	Q_OBJECT
public:
    DockWidgetTabButton(const QString& text, Qt::Orientation orient);
    ~DockWidgetTabButton();

public:
    //табы из панели закладок
    void setAction(QAction* action) { tabAction = action; }
    QAction* getAction() const { return tabAction; }

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual QSize sizeHint() const override;

private:
    QAction* tabAction;                 //таб в панели закладок
    Qt::Orientation tabOrientation;     //ориентация кнопки

    void setTabText(const QString& text);                   //вместить текст в кнопку
    QStyleOptionButton getStyleOption() const;              //получить стили для отрисовки кнопки

};
