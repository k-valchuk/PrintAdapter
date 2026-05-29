//-------------------------------------------------------------------------------------------------
// Представление таймлайн
//-------------------------------------------------------------------------------------------------
//  Для установки цаетов и иконок можно использовать методы объектов:
//  - scroll_bar_;
//  - scene_;
//  - timeline_indicator_;
//  - timeline_ticks_;
//-------------------------------------------------------------------------------------------------
//  Отрисовка дорожек в методе PaintTrackItem абстрактного класса TimelineTrack
//
//-------------------------------------------------------------------------------------------------
#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QGraphicsView>

#include "timelinescene.h"
#include "timelineitemticks.h"
#include "timelineindicator.h"
#include "timelinetrackslayout.h"
#include "timelinescrollbar.h"
#include "timelinecommon.h"

class TimelineView : public QGraphicsView
{
    Q_OBJECT
public:

    // Задать длительность проекта
    void SetDuration(uint64_t duration, int frame_rate);

    // Задать позицию ползунка в кадрах
    void SetIndicatorPosition(uint64_t value);

    // Текущий кадр ползунка
    uint64_t GetCurrentFramePos() const {return cur_timeline_settings_.cur_indicator_position;}

    // Добавить дорожки
    void AddTracks(const QVector<TimelineTrack *> tracks, int pos = -1);

    // Удалить дорожки
    void RemoveTracks(const std::vector<int> &tracks_pos);

    // Обновить высоту дорожек
    void UpdateTracksHeight(int h);

    // Миниатюра
    void SetThumbnail(uint64_t frame);
    void ClearThumbnail();

signals:
    // Позиция ползунка изменилась
    void IndicatorPositionChanged(uint64_t new_pos);

protected:
    explicit TimelineView(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void scrollContentsBy(int dx, int dy) override;


    TimelineScene scene_;                           // Сцена
    TimelineItemTicks timeline_ticks_;              // Шкала
    TimelineIndicator timeline_indicator_;          // Индикатор
    TimelineScrollBar *scroll_bar_;                 // Горизонтальный скролл

private:
    // Установить позицию таймлайн (точка сцены)
    void SetTimelinePostition(const QPointF &point, uint64_t frame);

    // Обновить размер сцены
    void ReziseTimelineSize(double w, double h, double active_width);
    void UpdateTimelineActiveWidth(int w, int h);

    // Переместить ползунок (позиция сцены)
    void MoveIndicator(const QPoint &pos);

    // Видимая область сцены по оси X
    QPair<double, double> GetVisibleRegion() const;

    // Изменить размер сцены
    void ResizeTimelineScene(int len);

    // Получить размер неиспользуемой области
    double GetUnuseTimelineWidth() const;

    // Обновить отображение позиции индикатора
    void UpdateIndicatorPosition();

    TicksGetter *GetTickGetter();


    TimelineSettings cur_timeline_settings_;         // Текущие настройки таймлайн
    TimelineTracksLayout timeline_tracks_layout_;   // Дорожки таймлайна

};

#endif // TIMELINEVIEW_H
