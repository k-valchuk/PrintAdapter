#ifndef DRAGGABLEFRAME_H
#define DRAGGABLEFRAME_H

#include <QFrame>
#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include "qtimer.h"

class DraggableFrame : public QFrame
{
    Q_OBJECT
    
    bool m_draggingEnabled = true;
    bool m_isPressed = false;
    bool m_isDragging = false;
    QPoint m_initialPos;
    QPoint m_midDiff{0, 0};
    QPoint m_currentPos;
    QLabel* m_previewFrame;
    QScrollArea* m_parentScrollArea = nullptr;
    QTimer* timerUp, *timerDown;
    QPoint middle(QPoint point);
public:
    DraggableFrame(QWidget* parent) : DraggableFrame(true, parent) {}
    DraggableFrame(bool draggingEnabled, QWidget* parent);
    void SetPreviewOpacity(qreal opacity);
    void SetPreviewBorder(bool visible);
    
    void SetDraggingEnabled(bool enabled);
    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
    virtual void onClick() {}
signals:
    void dragAboutToStart();
    void dragStarted();
    void dragPositionChanged(QPoint pos, QPoint middle);
    void dropped(QPoint pos);
    void dragCanceled();
    void clicked();
};

#endif // DRAGGABLEFRAME_H
