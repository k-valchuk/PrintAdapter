#include "timelinetrackitem.h"

TimelineTrackItem::TimelineTrackItem(IPaintTrackItem *item_painter, QGraphicsItem *parent) : QGraphicsLayoutItem(), QGraphicsItem(parent)
{
    item_height_ = 0.0;
    item_width_ = 0.0;
    item_active_width_ = 0.0;

    track_item_painter_ = item_painter;

    setGraphicsItem(this);
}

TimelineTrackItem::~TimelineTrackItem()
{
}

QSizeF TimelineTrackItem::sizeHint(Qt::SizeHint , const QSizeF &) const
{
    return QSizeF(item_width_, item_height_);
}

void TimelineTrackItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

QRectF TimelineTrackItem::boundingRect() const
{
    return QRectF(QPointF(0,0), geometry().size());
}

void TimelineTrackItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (track_item_painter_)
        track_item_painter_->PaintTrackItem(painter, option, widget);
}

void TimelineTrackItem::SetTrackActiveWidth(double w, double active_w)
{
    item_width_ = w;
    item_active_width_ = active_w;
    updateGeometry();
}

void TimelineTrackItem::SetTrackHeight(double h)
{
    item_height_ = h;
    updateGeometry();
}

void TimelineTrackItem::RepaintItem()
{
    update(boundingRect());
}
