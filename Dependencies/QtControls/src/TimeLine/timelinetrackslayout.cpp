#include "timelinetrackslayout.h"

TimelineTracksLayout::TimelineTracksLayout() : QGraphicsWidget()
{
    active_width_ = 0;
    width_ = 0;

    main_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    main_layout_->setSpacing(0);
    main_layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
}

void TimelineTracksLayout::AddTracks(const QVector<TimelineTrack *> &tracks, int pos)
{
    if (pos == -1)
        pos = tracks_.size();

    for (auto track : tracks)
    {
        track->SetActiveWidth(width_, active_width_);
        main_layout_->insertItem(pos, track);
        tracks_.insert(pos, track);

        ++pos;
    }
}

void TimelineTracksLayout::RemoveTracks(const std::vector<int> &tracks_pos)
{
    std::vector<int> sortVec{tracks_pos};

    // Сортируем для удаления
    std::sort(sortVec.begin(), sortVec.end(), std::greater<int>());

    for (int i : sortVec)
    {
        auto item = tracks_.at(i);
        main_layout_->removeItem(item);
        delete item;
        tracks_.remove(i);
    }
}

void TimelineTracksLayout::SetActiveWidth(double max_w, double active_w)
{
    if (max_w != width_ || active_w != active_width_)
    {
        width_ = max_w;
        active_width_ = active_w;

        for (auto track : tracks_)
        {
            track->SetActiveWidth(max_w, active_w);
        }
    }
}

void TimelineTracksLayout::SetTrackHeight(int h)
{
    for (auto track : tracks_)
    {
        track->SetTrackHeight(h);
    }
}

