#include "timelinetrack.h"


TimelineTrack::TimelineTrack(QGraphicsLayoutItem *parent) : QGraphicsLinearLayout(Qt::Vertical, parent),
    parent_item_(this)
{
    tick_getter_ = nullptr;
    timeline_settings_ = nullptr;

    setSpacing(0);
    setContentsMargins(0, 0, 0, 0);

    addItem(&parent_item_);
}

TimelineTrack::~TimelineTrack()
{
    for (auto item : items_)
    {
        removeItem(item);
        delete item;
    }
}

void TimelineTrack::AddChildrensTracks(const QVector<TimelineTrack *> &chi_tracks, int pos)
{    
    for (auto chi : chi_tracks)
    {
        chi->SetTickGetter(tick_getter_);
        if (pos == -1)
        {
            items_.append(chi);
            addItem(chi);
        }
        else
        {
            items_.insert(pos, chi);
            insertItem(pos, chi);
        }
        // по умолчанию скрываем дочерние
        chi->SetTrackVisible(false);
    }
}

void TimelineTrack::RemoveChildrenTracks(const QVector<int> &tracks_pos)
{
    QVector<int> sortVec{tracks_pos};

    // Сортируем для удаления
    std::sort(sortVec.begin(), sortVec.end(), std::greater<int>());

    for (int i : sortVec)
    {
        auto item = items_.at(i);
        removeItem(item);
        delete item;
        items_.remove(i);
    }
}

void TimelineTrack::SetTrackVisible(bool vis)
{
    parent_item_.setVisible(vis);
}

void TimelineTrack::SetChildrensTracksVisible(bool vis)
{
    for (auto chi : items_)
    {
        chi->SetTrackVisible(vis);
    }
    updateGeometry();
}

void TimelineTrack::SetActiveWidth(double max_w, double active_w)
{
    parent_item_.SetTrackActiveWidth(max_w, active_w);
    for (auto it : items_)
    {
        it->SetActiveWidth(max_w, active_w);
    }
    updateGeometry();

}

void TimelineTrack::SetTrackHeight(int h)
{
    parent_item_.SetTrackHeight(h);
    for (auto it : items_)
    {
        it->SetTrackHeight(h);
    }
    updateGeometry();
}


void TimelineTrack::SetTimelineSettings(TimelineSettings *timeline_set)
{
    timeline_settings_ = timeline_set;
    for (auto it : items_)
    {
        it->SetTimelineSettings(timeline_set);
    }
}

void TimelineTrack::SetTickGetter(TicksGetter *tick_getter)
{
    tick_getter_ = tick_getter;
    for (auto item : items_)
    {
        item->SetTickGetter(tick_getter_);
    }
}

int TimelineTrack::GetIndicatorPosition() const
{
    if (timeline_settings_)
        return timeline_settings_->cur_indicator_position;

    return -1;
}
