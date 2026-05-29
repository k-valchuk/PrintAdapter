#pragma once
#include <QtWidgets>


class SpoilerToolButton : public QToolButton
{
    Q_OBJECT
public:
    SpoilerToolButton(QWidget *parent = nullptr) : QToolButton(parent)
    {
        setMouseTracking(true);
    };

    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;

signals:
    void sigChangeHoverState(bool state);
};

