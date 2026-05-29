//----------------------------------------------------------------------
//Отрисовка Drop индикатора
//----------------------------------------------------------------------

#ifndef DROPINDICATORPROXYSTYLE_H
#define DROPINDICATORPROXYSTYLE_H

#include "QProxyStyle"

class DropIndicatorProxyStyle : public QProxyStyle
{
public:
    DropIndicatorProxyStyle(const QColor &diColor, QStyle* style = 0);
    void setDropIndocatorColor(const QColor &color){dropIndicatorColor = color;}    //установить цвет индикатора
protected:
    void drawPrimitive( PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const override;

private:
    QColor dropIndicatorColor;  //цвет индикатора
};

#endif // DROPINDICATORPROXYSTYLE_H
