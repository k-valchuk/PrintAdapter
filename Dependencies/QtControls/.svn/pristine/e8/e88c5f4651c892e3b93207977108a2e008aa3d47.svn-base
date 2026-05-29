#include "timelinescene.h"

#include <QPainter>

TimelineScene::TimelineScene(QObject *parent)
    : QGraphicsScene{parent}
{
    active_width_ = 0;
    height_scene_space_ = 0;
}

void TimelineScene::drawBackground(QPainter *painter, const QRectF &)
{    
    QRectF act_rect(sceneRect());
    act_rect.setWidth(active_width_);
    act_rect.setHeight(act_rect.height() + height_scene_space_);
    painter->fillRect(act_rect, active_color_);

    QRectF no_act_rect(sceneRect());
    no_act_rect.setLeft(active_width_);
    no_act_rect.setHeight(sceneRect().height() + height_scene_space_);
    painter->fillRect(no_act_rect, no_active_color_);
}
