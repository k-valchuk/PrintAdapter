#include "timelineview.h"

#include <QResizeEvent>

namespace {
// Расположение объектов по оси Z
enum class StackingOrderPosition : int
{
    kBottom = 0,
    kMiddle = 50,
    kTop = 100
};

}

TimelineView::TimelineView(QWidget *parent) : QGraphicsView(parent)
{
    scroll_bar_ = new TimelineScrollBar();
    setHorizontalScrollBar(scroll_bar_);

    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint( QPainter::SmoothPixmapTransform, true);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setScene(&scene_);

    scene_.addItem(&timeline_ticks_);
    scene_.addItem(&timeline_indicator_);

    timeline_indicator_.setZValue((double)StackingOrderPosition::kTop);
    timeline_ticks_.setZValue((double)StackingOrderPosition::kMiddle);

    scene_.addItem(&timeline_tracks_layout_);
    timeline_tracks_layout_.setPos(0, timeline_ticks_.GetHeight() + 1);

    SetIndicatorPosition(0);

    connect(scroll_bar_, &TimelineScrollBar::ResizeScrollBar, this, &TimelineView::ResizeTimelineScene);
}

void TimelineView::SetDuration(uint64_t duration, int frame_rate)
{
    if (duration > cur_timeline_settings_.cur_indicator_position)
        SetIndicatorPosition(0);

    timeline_ticks_.SetActiveDuration(duration, frame_rate);
    UpdateTimelineActiveWidth(this->width(), this->height());
}

void TimelineView::SetIndicatorPosition(uint64_t value)
{
    auto pos = timeline_ticks_.GetIndicatorPosition(value);
    SetTimelinePostition(pos, value);
}

void TimelineView::AddTracks(const QVector<TimelineTrack *> tracks, int pos)
{
    for (auto tr : tracks)
    {
        tr->SetTickGetter(GetTickGetter());
        tr->SetTimelineSettings(&cur_timeline_settings_);
    }
    timeline_tracks_layout_.AddTracks(tracks, pos);
}

void TimelineView::RemoveTracks(const std::vector<int> &tracks_pos)
{
    timeline_tracks_layout_.RemoveTracks(tracks_pos);
}

void TimelineView::UpdateTracksHeight(int h)
{
    timeline_tracks_layout_.SetTrackHeight(h);
}

void TimelineView::SetThumbnail(uint64_t frame)
{
    timeline_ticks_.SetThumbPosition(frame);
}

void TimelineView::ClearThumbnail()
{
    timeline_ticks_.ClearThumbPosition();
}

TicksGetter *TimelineView::GetTickGetter()
{
    return &timeline_ticks_;
}

void TimelineView::resizeEvent(QResizeEvent *event)
{
    UpdateTimelineActiveWidth(event->size().width(), event->size().height());

    QGraphicsView::resizeEvent(event);
}

void TimelineView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);

    auto item = itemAt(event->pos());
    if (item && item == &timeline_ticks_)
    {// Начало перемещение ползунка
        timeline_indicator_.SetIsMoving(true);
        MoveIndicator(event->pos());
    }
}

void TimelineView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    if (timeline_indicator_.IsMoving())
    {// Остановили перемещение ползунка
        timeline_indicator_.SetIsMoving(false);
    }
}

void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    if (timeline_indicator_.IsMoving())
    {// Перемещение ползунка
        MoveIndicator(event->pos());
    }
}

void TimelineView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);

    if (dy != 0)
    {
        // Перемещаем шкалу и индикатор (всегда сверху)
        QPointF left_corner_view_to_scene = mapToScene(QPoint(0,0));
        timeline_ticks_.setPos(0, left_corner_view_to_scene.y());
        UpdateIndicatorPosition();
    }

    if (dx != 0)
    {
        timeline_ticks_.SetVisibleDrawRegion(GetVisibleRegion());
    }
}

void TimelineView::SetTimelinePostition(const QPointF &point, uint64_t frame)
{
    // Новая позиция ползунка
    timeline_indicator_.setPos(point);
    cur_timeline_settings_.cur_indicator_position = frame;
    // Прокрутка до отображения полузнка
    ensureVisible(&timeline_indicator_, 20, 0);

    emit IndicatorPositionChanged(frame);
}

void TimelineView::ReziseTimelineSize(double w, double h, double active_width)
{
    double width = w - 1;
    scene_.setSceneRect(0, 0, width, h);
    timeline_ticks_.SetTimelineWidth(width);
    timeline_indicator_.SetHeight(h);
    timeline_ticks_.SetVisibleDrawRegion(GetVisibleRegion());
    scene_.SetActiveWidth(active_width);
    timeline_tracks_layout_.SetActiveWidth(width, active_width);
}


void TimelineView::UpdateTimelineActiveWidth(int w, int h)
{
    h -= scene_.GetHeightSceneSpace();

    double active_width = timeline_ticks_.GetActiveWidth();

    // Ширина с учетом довеска
    auto min_add_w = GetUnuseTimelineWidth();
    double event_width = w;
    if (event_width  - active_width < min_add_w)
        event_width = active_width + min_add_w;

    double event_height = std::max(scene_.itemsBoundingRect().height(), (double)h);

    // Изменяем размеры сцены если представление больше сцены
    if (event_width != scene_.width() || scene_.height() != event_height)
    {
        ReziseTimelineSize(event_width, event_height, active_width);
    }
}

void TimelineView::MoveIndicator(const QPoint &pos)
{
    auto scene_pos = timeline_ticks_.GetIndicatorPosition(mapToScene(pos));
    SetTimelinePostition(scene_pos, timeline_ticks_.GetIndicatorFramePosition(scene_pos.x()));
}

QPair<double, double> TimelineView::GetVisibleRegion() const
{
    QPointF left_corner = mapToScene(QPoint(0,0));
    QPointF right_corner = mapToScene(QPoint(this->width(),0));

    return {left_corner.x(), right_corner.x()};
}

void TimelineView::ResizeTimelineScene(int len)
{
    timeline_ticks_.SetActiveWidth((double)len - GetUnuseTimelineWidth());
    ReziseTimelineSize(len, scene_.height(), timeline_ticks_.GetActiveWidth());
    UpdateIndicatorPosition();
}

double TimelineView::GetUnuseTimelineWidth() const
{
    return (double)this->width() / 2.0;
}

void TimelineView::UpdateIndicatorPosition()
{
    timeline_indicator_.setPos(timeline_ticks_.GetIndicatorPosition(cur_timeline_settings_.cur_indicator_position));
}
