//-------------------------------------------------------------------------------------------------
// Индикатор позиции таймлайн
//-------------------------------------------------------------------------------------------------
#ifndef TIMELINEINDICATOR_H
#define TIMELINEINDICATOR_H

#include <QGraphicsItem>

class TimelineIndicator : public QGraphicsItem
{
public:
    TimelineIndicator();

    void SetHeight(double h);

    void SetIsMoving(bool is_move) {indicator_is_moving_ = is_move;}
    bool IsMoving() const {return indicator_is_moving_;}

    void SetIndicatorColor(const QColor &color);
    void SetCursorPixmap(const QPixmap &pix) {cursor_pixmap_ = pix;}

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    double height_;
    bool indicator_is_moving_;
    QPixmap cursor_pixmap_;
    QColor indicator_color_;

};

#endif // TIMELINEINDICATOR_H
