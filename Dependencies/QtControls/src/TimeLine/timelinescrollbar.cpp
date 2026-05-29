#include "timelinescrollbar.h"

#include <cmath>

#include "QPaintEvent"
#include "QPainter"
#include "QStyle"
#include "QStyleOptionSlider"

namespace {

constexpr int kArrowWidth = 14;
constexpr int kMinHandleWidth = kArrowWidth * 2;
constexpr int kMaxCursorChangedDistance = 5;

}

TimelineScrollBar::TimelineScrollBar(QWidget *parent) : QScrollBar(parent), handle_is_resizing_(false),
    is_available_to_resize_(false), is_pressed_(false), change_side_(ChangeSide::kNone)
{    
    setMouseTracking(true);
}

void TimelineScrollBar::paintEvent(QPaintEvent *pe)
{
    QScrollBar::paintEvent(pe);

    QRect rect = GetHandleRect();

    QPainter painter(this);
    int h = rect.height() - 4;
    QPixmap pix = arrow_left_.pixmap(kArrowWidth, h);
    QPoint p{rect.topLeft()};
    p.setY(p.y() + 2);
    painter.drawPixmap(p, pix);

    pix = arrow_right_.pixmap(kArrowWidth, h);
    p.setX(rect.right() - kArrowWidth);
    painter.drawPixmap(p, pix);
}

void TimelineScrollBar::mousePressEvent(QMouseEvent *me)
{
    if (me->button() == Qt::LeftButton)
    {
        is_pressed_ = true;
        if (is_available_to_resize_)
        {
            clicked_pos_x_ = me->pos().x();
            handle_is_resizing_ = true;
            clicked_value_ = sliderPosition();
            clicked_area_width_ = GetAreaWidth();
            return;
        }

    }

    QScrollBar::mousePressEvent(me);
}

void TimelineScrollBar::mouseMoveEvent(QMouseEvent *me)
{
    if (handle_is_resizing_)
    {// Изменяем размер
        int diff_pos_x = me->pos().x() - clicked_pos_x_;
        clicked_pos_x_ = me->pos().x();

        ResizeScrollBarPageStep(diff_pos_x);
        return;
    }
    else if (!is_pressed_)
    {// Ищем рамки изменения размера
        SearchResizeBounds(me->pos());
    }
    QScrollBar::mouseMoveEvent(me);
}

void TimelineScrollBar::mouseReleaseEvent(QMouseEvent *me)
{
    QScrollBar::mouseReleaseEvent(me);
    handle_is_resizing_ = false;
    is_pressed_ = false;
}

void TimelineScrollBar::SearchResizeBounds(const QPoint &pos)
{
    ChangeSide cur_side_ = ChangeSide::kNone;
    QRect rect = GetHandleRect();

    if (rect.contains(pos))
    {
        if (pos.x() - rect.left() <= kMaxCursorChangedDistance)
        {
            cur_side_ = ChangeSide::kLeft;
        }
        else if (rect.right() - pos.x() <= kMaxCursorChangedDistance)
        {
            cur_side_ = ChangeSide::kRight;
        }
    }

    if (cur_side_ != change_side_)
    {
        change_side_ = cur_side_;
        if (change_side_ == ChangeSide::kNone)
        {
            is_available_to_resize_ = false;
            setCursor(Qt::ArrowCursor);
        }
        else
        {
            setCursor(Qt::SizeHorCursor);
            is_available_to_resize_ = true;
        }
    }
}

QRect TimelineScrollBar::GetHandleRect() const
{
    QStyleOptionSlider option;
    initStyleOption(&option);
    return style()->subControlRect(QStyle::CC_ScrollBar, &option, QStyle::SC_ScrollBarSlider, this);
}

void TimelineScrollBar::ResizeScrollBarPageStep(int len)
{
    if (change_side_ == ChangeSide::kLeft)
        len *= -1;

    QRect handle_rect = GetHandleRect();
    // Размер ползунка (не должен быть меньше определенного размера)
    int new_handl_width = handle_rect.width() + len;
    if (new_handl_width < kMinHandleWidth)
        new_handl_width = kMinHandleWidth;

    // Проверка на выход за границы
    int max_handl_width = 0;
    if (change_side_ == ChangeSide::kRight)
        max_handl_width = width() - handle_rect.x();
    else
        max_handl_width = handle_rect.x() + handle_rect.width();

    if (new_handl_width > max_handl_width)
        new_handl_width = max_handl_width;


    if (handle_rect.width() != new_handl_width)
    {
        int res_width = width() * pageStep() / new_handl_width; // Новый размер области

        int area_width = GetAreaWidth();
        if (res_width != area_width)
        {
            // Новый размер области применить
            emit ResizeScrollBar(res_width);

            // Обновить позицию ползунка
               double new_position;
               if (change_side_ == ChangeSide::kRight)
                    new_position = std::round(clicked_value_ * res_width / clicked_area_width_); // оставить на месте
               else
                   new_position = (double)res_width / (double)width() * (handle_rect.x() - len); // смещение по курсору


            setSliderPosition(new_position);
        }
    }
}

int TimelineScrollBar::GetAreaWidth() const
{
    return maximum() + pageStep();
}
