//-------------------------------------------------------------------------------------------------
// Компоновщик дорожек таймлайна
//-------------------------------------------------------------------------------------------------
#ifndef TIMELINETRACKSLAYOUT_H
#define TIMELINETRACKSLAYOUT_H

#include "QGraphicsWidget"
#include "qgraphicslinearlayout.h"

#include "timelinetrack.h"

class TimelineTracksLayout : public QGraphicsWidget
{
public:
    TimelineTracksLayout();

    // Добавить дорожки
    void AddTracks(const QVector<TimelineTrack *> &tracks, int pos = -1);

    // Удалить дорожки
    void RemoveTracks(const std::vector<int> &tracks_pos);

    // Задать ширину дорожек
    void SetActiveWidth(double max_w, double active_w);

    // Задать высоту дорожки
    void SetTrackHeight(int h);        

private:
    QGraphicsLinearLayout *main_layout_;
    QVector<TimelineTrack *> tracks_;

    double width_;         // ширина дорожки = ширине сцены
    double active_width_;  // активная длина проекта
};

#endif // TIMELINETRACKSLAYOUT_H
