//-------------------------------------------------------------------------------------------------
// Дорожка таймлайна
//-------------------------------------------------------------------------------------------------
// - Возможность задать дочерние дорожки
//-------------------------------------------------------------------------------------------------

#ifndef TIMELINETRACK_H
#define TIMELINETRACK_H

#include "QGraphicsLinearLayout"

#include "timelinetrackitem.h"
#include "timelinecommon.h"
#include "timelineitemticks.h"

class TimelineTrack : public QGraphicsLinearLayout, public IPaintTrackItem
{
public:
    explicit TimelineTrack(QGraphicsLayoutItem *parent = nullptr);
    ~TimelineTrack();

    // Добавить дочерние дорожки
    void AddChildrensTracks(const QVector<TimelineTrack *> &chi_tracks, int pos = -1);

    // Удалить дочерние дорожки
    void RemoveChildrenTracks(const QVector<int> &tracks_pos);

    // Установить видимость дорожки
    void SetTrackVisible(bool vis);

    // Установить видимость дочерних дорожек
    void SetChildrensTracksVisible(bool vis);

    // Обновить ширину дорожки
    void SetActiveWidth(double max_w, double active_w);

    // Обновить высоту дорожки
    void SetTrackHeight(int h);

    // Установить настройки таймлайн
    void SetTimelineSettings(TimelineSettings *timeline_set);

    // Список дочерних дорожек
    QVector<TimelineTrack *> GetItems() const {return items_;}

    // Задать объект для получения данных шкалы
    void SetTickGetter(TicksGetter *tick_getter);

protected:
    // Позиция ползунка
    int GetIndicatorPosition() const;
    TimelineTrackItem parent_item_;         // Основная дорожка
    QVector<TimelineTrack *> items_;        // Дочерние дорожки

    TimelineSettings *timeline_settings_;

    TicksGetter *tick_getter_;

};

#endif // TIMELINETRACK_H
