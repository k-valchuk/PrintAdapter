#include "OnOffSwitch.h"

#include "QPainter"
#include "QPainterPath"
#include "QtMath"
#include "QPropertyAnimation"
#include "QMouseEvent"

OnOffSwitch::OnOffSwitch(QWidget *parent)
    : QWidget{parent}
{
    this->setCursor(Qt::CursorShape::PointingHandCursor);
    this->setFixedSize(m_width, m_height);
}

void OnOffSwitch::Toggle(bool state, bool withAnim)
{
    if(state == m_state)
        return;
    
    m_state = state;
    
    auto progressAnim = new QVariantAnimation(this);
    progressAnim->setDuration(withAnim ? m_animationTime : 0);
    progressAnim->setStartValue(state ? 0.0 : 1.0);
    progressAnim->setEndValue(!state ?  0.0 : 1.0);
    progressAnim->setEasingCurve(m_easingCurve);
    connect(progressAnim, &QVariantAnimation::valueChanged, this, [this](const QVariant& val){
        m_progress = val.toDouble();
        repaint();
    });
    progressAnim->start(QAbstractAnimation::DeleteWhenStopped);
    
    
    emit toggled(state);
}

class SwitchAnimationInterpolator : public QVariantAnimation
{
public:
    QVariant interpolate(const QVariant &from, const QVariant &to, qreal progress)
    {
        this->setStartValue(from); this->setEndValue(to); //чтобы выбрался правильный итерполятор, реально ниче не считают
        return interpolated(from, to, progress);
    }
};

void OnOffSwitch::paintEvent(QPaintEvent* event)
{
    static SwitchAnimationInterpolator anim;
    
    auto rect = this->rect();
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QPainterPath bg;
    bg.addRoundRect(rect, 100); //100 это проценты а не пиксели
    auto bgcol = anim.interpolate(m_backgroundOff, m_backgroundOn, m_progress).value<QColor>();
    painter.fillPath(bg, bgcol);
    
    QPainterPath circle;
    auto circleSize = QSizeF(rect.height()*m_circleSize, rect.height()*m_circleSize);
    
    auto offset = (rect.height() - circleSize.height())/2.0;
    auto circleRectLeft = QRectF(QPointF(rect.left() + offset, rect.top() + offset), circleSize);
    auto circleRectRight = QRectF(QPointF(rect.right() - circleSize.width() - offset, rect.top() + offset), circleSize);
    
    auto circleRect = anim.interpolate(circleRectLeft, circleRectRight, m_progress).value<QRect>();
    circle.addEllipse(circleRect); //100 это проценты а не пиксели
    auto circleCol = anim.interpolate(m_circleOff, m_circleOn, m_progress).value<QColor>();
    painter.fillPath(circle, circleCol);
    
    QWidget::paintEvent(event);
}

void OnOffSwitch::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        if(!m_onlyProgrammaticalChange)
            Toggle(!m_state, true);
        else
            emit toggleRequested(!m_state);
    }
    
    QWidget::mouseReleaseEvent(event);
}
