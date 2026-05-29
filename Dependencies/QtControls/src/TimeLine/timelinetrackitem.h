//-------------------------------------------------------------------------------------------------
// Дорожка таймлайна
//-------------------------------------------------------------------------------------------------
#ifndef TIMELINETRACKITEM_H
#define TIMELINETRACKITEM_H

#include "QGraphicsRectItem"
#include "QGraphicsLayoutItem"

class IPaintTrackItem
{
public:
    virtual void PaintTrackItem(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;
};

class TimelineTrackItem : public QGraphicsLayoutItem, public QGraphicsItem
{
public:
    explicit TimelineTrackItem(IPaintTrackItem *item_painter, QGraphicsItem *parent = nullptr);
    ~TimelineTrackItem();

    // Обновить геометрию
    void SetTrackActiveWidth(double w, double active_w);
    void SetTrackHeight(double h);

    // Перерисовка
    void RepaintItem();

    inline double GetItemHeight() const {return item_height_;}
    inline double GetItemWidth() const {return item_width_;}
    inline double GetItemActiveWidth() const {return item_active_width_;}

protected:
    // QGraphicsLayoutItem
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;
    void setGeometry(const QRectF &geom) override;

    // QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private:
    double item_height_;
    double item_width_;
    double item_active_width_;


    IPaintTrackItem *track_item_painter_;
};

#endif // TIMELINETRACKITEM_H
