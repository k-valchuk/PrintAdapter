#include "SpoilerToolButton.h"


void SpoilerToolButton::enterEvent(QEvent *e)
{
    emit sigChangeHoverState(true);
    QToolButton::enterEvent(e);
    setCursor(Qt::PointingHandCursor);
}
void SpoilerToolButton::leaveEvent(QEvent *e)
{
    emit sigChangeHoverState(false);
    QToolButton::leaveEvent(e);
    setCursor(Qt::ArrowCursor);
}
