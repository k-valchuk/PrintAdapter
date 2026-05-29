//-------------------------------------------------------------------------------------------------
// Шкала таймлайн
//-------------------------------------------------------------------------------------------------

#ifndef TIMELINEITEMTICKS_H
#define TIMELINEITEMTICKS_H

#include <QGraphicsItem>

class TicksGetter
{
public:
    // Кадры в пиксели
    virtual double FrameToXpos(uint64_t frame) const = 0;

};

class TimelineItemTicks : public QGraphicsItem, public TicksGetter
{    
public:
    TimelineItemTicks();

    // Установить активную длительность таймлайна с указанием частоты кадров
    void SetActiveDuration(uint64_t duration, int frame_rate = 25);

    // Установить новую ширину таймлайн
    void SetTimelineWidth(double width);

    // Получить активную ширину таймлайна (под проект)
    double GetActiveWidth() const {return active_timeline_width;}

    // Получить координату(сцены) ползунка по номеру кадра
    QPointF GetIndicatorPosition(uint64_t value) const;

    // Получить позицию таймлайна по координатам(сцены)
    QPointF GetIndicatorPosition(const QPointF &pos) const;

    // Получить позицию ползунка в кадрах по координате
    uint64_t GetIndicatorFramePosition(double pos_x) const;

    // Получить высоту шкалы в px
    int GetHeight() const;

    // Установить видимую область шкалы
    void SetVisibleDrawRegion(const QPair<double, double> &region);

    // Позицию миниатюры
    void SetThumbPosition(uint64_t thumb_frame);
    void ClearThumbPosition();    

    // Задать миниатюру
    void SetThumbImage(const QPixmap &pix){thumb_img = pix;}

    // Задать цвета рисок и цифр
    void SetTickDigitColors(const QColor &bdc, const QColor &btc, const QColor &sdc, const QColor &stc);

    // Установить ширину проекта
    void SetActiveWidth(double w);    

    // Область шкалы
    QRectF boundingRect() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    // Кадры в пиксели
    double FrameToXpos(uint64_t frame) const override;

private:
    // Позиция индикатора в координатах сцены
    QPointF GetIndicatorScenePos(double x) const;

    // Масштабирование по умолчанию
    void SetDefaultScale();


    // Длительность проекта
    uint64_t duration_;    
    // Частота кадров
    int frame_rate_;
    // Ширина таймлайна
    double timeline_width_;
    // Активная ширина таймлайна
    double active_timeline_width;
    // Количество кадров в одном тике
    int frames_in_one_tick_;
    // Шаг тика в пикселях
    double tick_step_;

    // Позиция миниатюры
    uint64_t thumb_frame_;
    bool draw_thumb_;
    QPixmap thumb_img;

    // Область отрисовки шкалы (x_start, x_end)
    QPair<double, double> visible_draw_region_;

    // Цвета
    QColor big_digit_color_;
    QColor big_tick_color_;
    QColor small_digit_color_;
    QColor small_tick_color_;
};

#endif // TIMELINEITEMTICKS_H
