#include "timelineindicator.h"

#include <QGraphicsScene>
#include <QPainter>


TimelineIndicator::TimelineIndicator()
{
    height_ = 0;
    indicator_is_moving_ = false;
}

void TimelineIndicator::SetHeight(double h)
{
    if (height_ != h)
    {
        height_ = h;
        prepareGeometryChange();
    }
}

void TimelineIndicator::SetIndicatorColor(const QColor &color)
{
    indicator_color_ = color;
    indicator_color_.setAlphaF(0.7);
}

QRectF TimelineIndicator::boundingRect() const
{
    return QRectF(0, 0, 0, 0);
}

void TimelineIndicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (height_ > 0)
    {
        painter->setPen(indicator_color_);
        painter->drawLine(0, 0, 0, height_);
        painter->drawPixmap(-cursor_pixmap_.width() / 2, -cursor_pixmap_.height(), cursor_pixmap_);
    }
}
