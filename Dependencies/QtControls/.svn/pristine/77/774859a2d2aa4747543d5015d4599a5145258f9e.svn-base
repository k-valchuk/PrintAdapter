#include "timelineitemticks.h"

#include <cmath>

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace {

// Отступ в пикселях для отрисовки шкалы
constexpr int kLeftSpace{10};
// Высота таймлайна
constexpr int kTimelineHeight{35};
// Шаг отрисовки риски в пикселях
constexpr int kTickStep{10};
// Высота риски в пикселях
constexpr int kBigTickHeight{12};
constexpr int kMidTickHeight{10};
constexpr int kSmallTickHeight{4};

constexpr int kBigDigitSize = 12;

constexpr int kSmallDigitSize = 10;

// Представление таймкода на шкале
enum class ViewFrameMode : int
{
    kFull,
    kFrames,
    KSeconds
};
}


// Получить строку таймкода без первых нулей
static QString GetStrTimeCodeFromValue(uint64_t value, int frame_rate, ViewFrameMode view)
{
    if (value == 0)
        return "0";

    if (view == ViewFrameMode::kFrames)
        return QString::number(value % frame_rate).rightJustified(2, '0');

    char b[4] = { 0 };
    b[0] = (char)(value % frame_rate);
    b[1] = (char)(static_cast<int>(value / frame_rate) % 60);
    if (view == ViewFrameMode::kFull)
    {
        b[2] = (char)(static_cast<int>(value / frame_rate / 60) % 60);
        b[3] = (char)(static_cast<int>(value / frame_rate / 60 / 60) % 100);
    }

    QString str;
    if (b[3] > 0)
        str.append(QString("%1:").arg(b[3], 2, 10, (QLatin1Char)'0'));
    if (!str.isEmpty() || b[2] > 0)
        str.append(QString("%1:").arg(b[2], 2, 10, (QLatin1Char)'0'));

    str.append(QString("%1:%2").arg(b[1], 2, 10, (QLatin1Char)'0').arg(b[0], 2, 10, (QLatin1Char)'0'));

    return str;
}

TimelineItemTicks::TimelineItemTicks() : visible_draw_region_{0.0, 0.0}
{
    duration_ = 0;
    frame_rate_ = 25;
    timeline_width_ = 0;
    active_timeline_width = 0;
    draw_thumb_ = false;

    SetDefaultScale();
}

void TimelineItemTicks::SetActiveDuration(uint64_t duration, int frame_rate)
{
    duration_ = duration;
    frame_rate_ = frame_rate;
    SetDefaultScale();
    active_timeline_width = FrameToXpos(duration_);

    prepareGeometryChange();
}

void TimelineItemTicks::SetTimelineWidth(double width)
{
    if (timeline_width_ != width)
    {
        timeline_width_ = width;
        prepareGeometryChange();
    }
}


QPointF TimelineItemTicks::GetIndicatorPosition(uint64_t value) const
{
    return GetIndicatorScenePos(FrameToXpos(value));
}

QPointF TimelineItemTicks::GetIndicatorPosition(const QPointF &pos) const
{
    int count_ticks = std::round((pos.x() - kLeftSpace) / tick_step_);
    double x = (double)count_ticks * tick_step_ + kLeftSpace;

    if (x < kLeftSpace)
        x = kLeftSpace;
    else if (x > active_timeline_width)
        x = active_timeline_width;

    return GetIndicatorScenePos(x);

}

QRectF TimelineItemTicks::boundingRect() const
{
    return QRectF(0, 0, timeline_width_, kTimelineHeight);
}

double TimelineItemTicks::FrameToXpos(uint64_t frame) const
{
    return (double)kLeftSpace + tick_step_ * std::round((double)frame / (double)frames_in_one_tick_);
}

void TimelineItemTicks::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (visible_draw_region_.second > 0.1)
    {
        QRectF rect = option->rect;
        rect.setLeft(visible_draw_region_.first);
        rect.setRight(visible_draw_region_.second);

        // Фон
        painter->fillRect(rect, option->palette.window());

        // Нижняя линия
        painter->setPen(QPen(option->palette.midlight().color()));
        painter->drawLine(rect.left(), kTimelineHeight, rect.right(), kTimelineHeight);

        // Меняем позицию painter для обхода проблемы максимальной позици отрисовки текста по оси X
        QTransform wt = painter->worldTransform();
        wt.translate(rect.left(), 0);
        painter->setWorldTransform(wt);

        auto p_font = painter->font();
        // Количество тиков от 0 до границы видимой области
        int countTicks = (rect.right() - kLeftSpace) / tick_step_ + 1;

        auto DrawDigits = [this, &p_font](QPainter *p, int value, int font_size, const QColor &font_color, double x, double y, ViewFrameMode view)
        {
            p_font.setPixelSize(font_size);
            p->setFont(p_font);
            p->setPen(QColor(font_color));
            auto str = GetStrTimeCodeFromValue(value, frame_rate_, view);
            QFontMetrics f_metrics(p_font);
            double len_str = f_metrics.horizontalAdvance(str);
            p->drawText(QPointF(x - len_str / 2.0, y + f_metrics.height()), str);
        };

        int tick_num = 0;
        if (rect.left() > kLeftSpace)
            tick_num = (rect.left() - kLeftSpace) / tick_step_ + 1;

        // Рисуем тики
        for (; tick_num < countTicks; tick_num++)
        {
            painter->setPen(small_tick_color_);

            double x_tick_pos = tick_num * tick_step_ + kLeftSpace - rect.left();// учет transform painter
            double i_height = kSmallTickHeight;
            if (tick_num % 25 == 0)// каждые 25 большой тик
            {
                i_height = kBigTickHeight;
                DrawDigits(painter, tick_num * frames_in_one_tick_, kBigDigitSize, big_digit_color_, x_tick_pos, i_height, ViewFrameMode::kFull);
                painter->setPen(big_tick_color_);
            }
            else if (tick_num % 5 == 0 )// средний тик каждые 5
            {
                i_height = kMidTickHeight;
                auto view = (frames_in_one_tick_ == 1) ? ViewFrameMode::kFrames : ViewFrameMode::KSeconds;
                DrawDigits(painter, tick_num * frames_in_one_tick_, kSmallDigitSize, small_digit_color_, x_tick_pos, i_height - 3.0, view);
                painter->setPen(small_tick_color_);
            }

            // Рисуем тик
            painter->drawLine(QPointF(x_tick_pos, 0.0), QPointF(x_tick_pos, i_height));

        }

        painter->resetTransform();
        // Миниатюра
        if (draw_thumb_ && thumb_frame_ <= duration_)
        {
            double pos_x = FrameToXpos(thumb_frame_) - (double)thumb_img.width() / 2.0;
            painter->drawPixmap(QPointF(pos_x, 0.0), thumb_img);
        }
    }
}

QPointF TimelineItemTicks::GetIndicatorScenePos(double x) const
{
    return mapToScene(QPointF(x, kTimelineHeight));
}

void TimelineItemTicks::SetDefaultScale()
{
    tick_step_ = kTickStep;
    frames_in_one_tick_ = 1;
}

uint64_t TimelineItemTicks::GetIndicatorFramePosition(double pos_x) const
{
    return std::round((pos_x - (double)kLeftSpace) / tick_step_ * (double)frames_in_one_tick_);
}

int TimelineItemTicks::GetHeight() const
{
    return kTimelineHeight;
}

void TimelineItemTicks::SetVisibleDrawRegion(const QPair<double, double> &region)
{
    if (visible_draw_region_ != region)
    {
        visible_draw_region_ = region;
        update();
    }
}

void TimelineItemTicks::SetThumbPosition(uint64_t thumb_frame)
{
    draw_thumb_ = true;
    thumb_frame_ = thumb_frame;
}

void TimelineItemTicks::ClearThumbPosition()
{
    draw_thumb_ = false;
}

void TimelineItemTicks::SetTickDigitColors(const QColor &bdc, const QColor &btc, const QColor &sdc, const QColor &stc)
{
    big_digit_color_ = bdc;
    big_tick_color_ = btc;
    small_digit_color_ = sdc;
    small_tick_color_ = stc;
}

void TimelineItemTicks::SetActiveWidth(double w)
{
    double pix_for_frame = w / (double)duration_;
    if ((int)pix_for_frame >= kTickStep)
    {
        frames_in_one_tick_ = 1;
        tick_step_ = pix_for_frame;
    }
    else
    {
        tick_step_ = kTickStep;
        frames_in_one_tick_ = std::ceil(tick_step_ / pix_for_frame);
    }
    active_timeline_width = FrameToXpos(duration_);
}



