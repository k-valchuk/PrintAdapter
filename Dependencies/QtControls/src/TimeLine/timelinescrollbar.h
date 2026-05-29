//-------------------------------------------------------------------------------------------------
// Полоса прокрутки таймлайн
//-------------------------------------------------------------------------------------------------
#ifndef TIMELINESCROLLBAR_H
#define TIMELINESCROLLBAR_H

#include <QScrollBar>
#include "QIcon"

class TimelineScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    enum class ChangeSide : int
    {
        kNone,
        kLeft,
        kRight
    };

    explicit TimelineScrollBar(QWidget *parent = nullptr);
    ~TimelineScrollBar(){}

    void SetArrowIcons(const QIcon &ar_left, const QIcon &ar_right){arrow_left_ = ar_left; arrow_right_ = ar_right;}

protected:
    void paintEvent(QPaintEvent *pe) override;
    void mousePressEvent(QMouseEvent *me) override;
    void mouseMoveEvent(QMouseEvent *me) override;
    void mouseReleaseEvent(QMouseEvent *me) override;

signals:
    void ResizeScrollBar(int len);

private:
    // Поиск границ изменения размера
    void SearchResizeBounds(const QPoint &pos);

    // Получить область ползунка
    inline QRect GetHandleRect() const;

    // Изменить размер ползунка
    void ResizeScrollBarPageStep(int len);

    // Полный размер области
    inline int GetAreaWidth() const;

    // Данные для изменения размера ползунка
    QIcon arrow_left_, arrow_right_;
    bool handle_is_resizing_;
    bool is_available_to_resize_;
    bool is_pressed_;
    int clicked_pos_x_;
    double clicked_value_;
    double clicked_area_width_;
    ChangeSide change_side_;
};

#endif // TIMELINESCROLLBAR_H
