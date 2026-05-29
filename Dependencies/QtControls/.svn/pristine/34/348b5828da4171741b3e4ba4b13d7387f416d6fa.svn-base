#include "BramPlayerSlider.h"
#include "QStyle"
#include "QMouseEvent"
#include "QPainter"
#include "QStyleOptionSlider"

BramPlayerSlider::BramPlayerSlider(QWidget *parent) : QSlider(parent)
{
}

void BramPlayerSlider::setMarkers(const QVector<SliderMarker> &marVec)
{
    markers.clear();
    for (auto &m : marVec)
        markers.append(m.posStart);
    std::sort(markers.begin(), markers.end());//сортировка по возрастанию
    update();
}

void BramPlayerSlider::setMarkIn()
{
    markIn = value();
    //изменяем markOut если он не установлен или меньше markIn
    if (markOut == -1)
        markOut = maximum();
    if (markIn > markOut)
        markOut = markIn;

    update();
    //сообщить об изменении
    emit sig_changedMarkInMarkOut(markIn, markOut);
}

void BramPlayerSlider::setMarkOut()
{
    markOut = value();
    //изменяем markIn если он не установлен или больше markOut
    if (markIn == -1)
        markIn = minimum();
    if (markOut < markIn)
        markIn = markOut;

    update();
    //сообщить об изменении
    emit sig_changedMarkInMarkOut(markIn, markOut);
}

void BramPlayerSlider::clearMarkInOut()
{
    markIn = -1;
    markOut = -1;
    update();
}

void BramPlayerSlider::setMarkInMarkOut(int in, int out)
{
    if (in == 0 && (out == maximum() || out == 0))
    {//не задавать если в конечных точках
        clearMarkInOut();
        return;
    }
    markIn = in;
    markOut = out;
    update();
}

void BramPlayerSlider::setEnabled(bool bEn)
{
    bEnabled = bEn;
    update();
}

void BramPlayerSlider::setValueByUser(int value)
{
    if (value > maximum() || value < minimum())//вне диапазона
        return;

    setValue(value);
    //сообщить об изменении
    emit sig_valueChangedByUser(value);
}

void BramPlayerSlider::goNextMarker()
{
    //находим первый больше текущего
    int val = maximum();
    for (int v : markers)
    {
        if (v > value())
        {
            val = v;
            break;
        }
    }
    //установить
    setValueByUser(val);
}

void BramPlayerSlider::goPrevMarker()
{
    //находим первый меньше текущего
    int val = minimum();
    for (auto it = markers.rbegin(); it != markers.rend(); ++it)
    {
        if (*it < value())
        {
            val = *it;
            break;
        }
    }
    //установить
    setValueByUser(val);
}

void BramPlayerSlider::goToMarkIn()
{
    setValueByUser(markIn);
}

void BramPlayerSlider::goToMarkOut()
{
    setValueByUser(markOut);
}

void BramPlayerSlider::paintEvent(QPaintEvent *)
{
    static const QColor colorMarker(195, 195, 195);     //цвет маркера
    static const QColor colorMark(255, 153, 102);       //цвет маркинов
    static const QColor colorGrayLine(114, 114, 114);   //серая линия
    static const QColor colorBorder(142, 142, 142);     //цвет рамки
    static const QColor colorGreenLine(46, 159, 45);    //зеленая линия

    static const int hSlider = 8;            //высота отрисовки слайдера
    static const int difH = hSlider / 2;
    //получить позицию из значения
    static auto getPosFromValue = [&](int val) -> int
    {
        return QStyle::sliderPositionFromValue(minimum(), maximum(), val, width());
    };

    QPainter painter(this);
    painter.setPen(colorBorder);
    int pxIn = 0, pxOut = width();

    if (!bEnabled)//слайдер недоступен
    {
        painter.setBrush(colorGrayLine);
        painter.drawRect(pxIn, hSlider, pxOut - pxIn, hSlider);
        return;
    }

    //отрисовка серой области
    if (markIn != -1)
    {
        painter.setBrush(colorGrayLine);

        pxIn = getPosFromValue(markIn);
        painter.drawRect(0, hSlider, pxIn, hSlider);

        pxOut = getPosFromValue(markOut);
        painter.drawRect(pxOut, hSlider, width() -  pxOut, hSlider);
    }

    //выбранная область
    painter.setBrush(colorGreenLine);
    painter.drawRect(pxIn, hSlider, pxOut - pxIn, hSlider);

    //маркеры
    painter.setPen(QPen(colorMarker, 2));
    for (int val : markers)
    {
        int px = getPosFromValue(val);
        painter.drawLine(px, hSlider + 1, px, height() - hSlider + 1);
    }

    //ползунок
    int pxValue = getPosFromValue(value());
    painter.drawLine(pxValue, height() - difH , pxValue, hSlider - difH);

    //markIn markOut
    if (markIn != -1)
    {
        painter.setPen(QPen(colorMark, 2));

        painter.drawLine(pxIn, hSlider, pxIn, height() - difH);
        if (markIn != markOut)//если отличаются
        {
            painter.drawLine(pxIn, height() - difH, pxIn + 4, height() - difH);

            painter.drawLine(pxOut, hSlider, pxOut, height() - difH);
            painter.drawLine(pxOut, height() - difH, pxOut - 4, height() - difH);
        }
    }
}

void BramPlayerSlider::mousePressEvent(QMouseEvent *me)
{
    QSlider::mousePressEvent(me);
    setSliderValueToMousePosition(me);
}

void BramPlayerSlider::mouseMoveEvent(QMouseEvent *me)
{
    QSlider::mouseMoveEvent(me);
    setSliderValueToMousePosition(me);
}

void BramPlayerSlider::setSliderValueToMousePosition(QMouseEvent *me)
{
    int value = QStyle::sliderValueFromPosition(minimum(), maximum(), me->x(), width());
    setValue(value);
    //сообщить об изменении
    emit sig_valueChangedByUser(value);
}
