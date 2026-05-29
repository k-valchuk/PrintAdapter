#include "AnimatedTextLabel.h"
#include <QGraphicsEffect>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>

//----------------------------------------------------------------------------
AnimatedTextLabel::AnimatedTextLabel(QWidget* parent) : QLabel(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect;
    setGraphicsEffect(opacityEffect);

    m_animation = new QPropertyAnimation(opacityEffect, "opacity");
    m_animation->setDuration(m_duration);
    m_animation->setStartValue(m_startOpacityAnimValue);
    m_animation->setEndValue(m_endOpacityAnimValue);
    m_animationConnect = connect(m_animation, &QPropertyAnimation::finished, this, &AnimatedTextLabel::slotAnimationFinished);
    m_animation->start();

    m_fontToDraw.setFamily("Roboto");
    m_fontToDraw.setWeight(QFont::DemiBold);
    m_fontToDraw.setPixelSize(40);

    setLabelText(m_textToDraw);
}

//----------------------------------------------------------------------------
void AnimatedTextLabel::setLabelText(QString text)
{
    m_textToDraw = text;
    setFixedSize({QFontMetrics(m_fontToDraw).width(m_textToDraw),QFontMetrics(m_fontToDraw).height()});
}

//----------------------------------------------------------------------------
void AnimatedTextLabel::setTextBrush(QColor color)
{
    m_textColor = color;
}

//----------------------------------------------------------------------------
void AnimatedTextLabel::setTextFont(QFont font)
{
    m_fontToDraw = font;
}

//----------------------------------------------------------------------------
void AnimatedTextLabel::setDuration(int duration)
{
    disconnect(m_animation, &QPropertyAnimation::finished, this, &AnimatedTextLabel::slotAnimationFinished);
    m_animation->stop();
    m_duration = duration;
    m_animation->setDuration(m_duration);
    connect(m_animation, &QPropertyAnimation::finished, this, &AnimatedTextLabel::slotAnimationFinished);
    m_animation->start();
}

//----------------------------------------------------------------------------
void AnimatedTextLabel::setStartOpacityValue(qreal startVal)
{
    disconnect(m_animation, &QPropertyAnimation::finished, this, &AnimatedTextLabel::slotAnimationFinished);
    m_animation->stop();
    m_startOpacityAnimValue = startVal;
    m_animation->setStartValue(startVal);
    connect(m_animation, &QPropertyAnimation::finished, this, &AnimatedTextLabel::slotAnimationFinished);
    m_animation->start();
}

//----------------------------------------------------------------------------
void AnimatedTextLabel::setEndOpacityValue(qreal endVal)
{
    disconnect(m_animation, &QPropertyAnimation::finished, this, &AnimatedTextLabel::slotAnimationFinished);
    m_animation->stop();
    m_endOpacityAnimValue = endVal;
    m_animation->setEndValue(endVal);
    connect(m_animation, &QPropertyAnimation::finished, this, &AnimatedTextLabel::slotAnimationFinished);
    m_animation->start();
}

//----------------------------------------------------------------------------
void AnimatedTextLabel::setTextVisible(bool isVisible)
{
    m_isTextVisible = isVisible;
}

//----------------------------------------------------------------------------
void AnimatedTextLabel::paintEvent(QPaintEvent* event)
{
    if(!m_isTextVisible){
        event->accept();
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black,1));
    painter.setBrush(m_textColor);
    QPainterPath ppath;

    ppath.addText({0, (qreal)QFontMetrics(m_fontToDraw).height() - 2}, m_fontToDraw, m_textToDraw);
    painter.drawPath(ppath);
}

//----------------------------------------------------------------------------
void AnimatedTextLabel::slotAnimationFinished()
{
    if(m_animation->direction() == QAbstractAnimation::Forward)
        m_animation->setDirection(QAbstractAnimation::Backward);
    else
        m_animation->setDirection(QAbstractAnimation::Forward);
    m_animation->start();
}

