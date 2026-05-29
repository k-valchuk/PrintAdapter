#ifndef ANIMATEDTEXTLABEL_H
#define ANIMATEDTEXTLABEL_H

#include <QLabel>

class QPropertyAnimation;

class AnimatedTextLabel : public QLabel
{
    Q_OBJECT
public:
    AnimatedTextLabel(QWidget* parent = nullptr);

    void setLabelText(QString text);
    void setTextBrush(QColor color);
    void setTextFont(QFont font);
    void setDuration(int duration);
    void setStartOpacityValue(qreal startVal);
    void setEndOpacityValue(qreal endVal);

    void setTextVisible(bool isVisible);

protected:
    void paintEvent(QPaintEvent* event) final;

private:
    QPropertyAnimation* m_animation;

    QString m_textToDraw = "REC";
    QColor m_textColor = QColor("#FF0000");
    QFont m_fontToDraw;
    bool m_isTextVisible = false;

    qreal m_startOpacityAnimValue = 1.0;
    qreal m_endOpacityAnimValue = 0.25;
    int m_duration = 350;

    QMetaObject::Connection m_animationConnect;

protected slots:
    void slotAnimationFinished();
};

#endif // ANIMATEDTEXTLABEL_H
