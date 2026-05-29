#ifndef ONOFFSWITCH_H
#define ONOFFSWITCH_H

#include <QWidget>
#include <QEasingCurve>

class OnOffSwitch : public QWidget
{
    Q_OBJECT
    
    int m_animationTime = 175;
    QEasingCurve m_easingCurve = QEasingCurve::Type::InOutQuad;
    
    QColor m_backgroundOff = QColor(85, 85, 85);
    QColor m_backgroundOn = QColor("#456A9E");
    QColor m_circleOff = QColor(168, 168, 168);
    QColor m_circleOn = QColor("#E2E2E2");
    
    bool m_state = false;
    double m_progress = 0.0;
    
    int m_width = 36;
    int m_height = 20;
    double m_circleSize = 0.7;
    
    bool m_onlyProgrammaticalChange = false;
public:
    
    explicit OnOffSwitch(QWidget *parent = nullptr);
    
    void Toggle(bool state, bool withAnim = true);
    void SetOnlyProgrammaticalChange(bool onlyProgrammaticalChange) {m_onlyProgrammaticalChange = onlyProgrammaticalChange;}
protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    
signals:
    void toggled(bool state);
    void toggleRequested(bool toState);
};

#endif // ONOFFSWITCH_H
