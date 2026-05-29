#include "DraggableFrame.h"
#include "qdebug.h"
#include "qevent.h"
#include "qguiapplication.h"
#include "qpaintengine.h"
#include "qscrollbar.h"
#include "qtimer.h"
#include <QMouseEvent>

QPoint DraggableFrame::middle(QPoint point)
{
     return point + m_midDiff;
}

DraggableFrame::DraggableFrame(bool draggingEnabled, QWidget* parent)
    : QFrame(parent), m_draggingEnabled(draggingEnabled)
{
    QWidget* topLevelWnd = this->window();
    m_previewFrame = new QLabel(nullptr);
    m_previewFrame->setWindowFlag(Qt::FramelessWindowHint, true);
    SetPreviewBorder(true);
    m_previewFrame->setWindowOpacity(0.5);
    m_previewFrame->hide();
    //this->setMouseTracking(true);
    
    timerDown = new QTimer();
    timerDown->setInterval(350);
    timerDown->callOnTimeout([this](){
        if(!m_parentScrollArea)
            return;
        auto vbar = m_parentScrollArea->verticalScrollBar();
        auto value = vbar->value();
        int addition = this->height();
        if(value + addition <= vbar->maximum())
            m_parentScrollArea->verticalScrollBar()->setValue(value + addition);
        else
            m_parentScrollArea->verticalScrollBar()->setValue(vbar->maximum());
        
        m_currentPos.setY(mapToParent(mapFromGlobal(QCursor::pos())).y());
        emit dragPositionChanged(m_currentPos, middle(m_currentPos));
    });
    
    timerUp = new QTimer();
    timerUp->setInterval(350);
    timerUp->callOnTimeout([this](){
        if(!m_parentScrollArea)
            return;
        auto vbar = m_parentScrollArea->verticalScrollBar();
        auto value = vbar->value();
        int addition = this->height();
        if(value - addition >= vbar->minimum())
            m_parentScrollArea->verticalScrollBar()->setValue(value - addition);
        else
            m_parentScrollArea->verticalScrollBar()->setValue(vbar->minimum());
        
        m_currentPos.setY(mapToParent(mapFromGlobal(QCursor::pos())).y());
        emit dragPositionChanged(m_currentPos, middle(m_currentPos));
    });
}

void DraggableFrame::SetPreviewOpacity(qreal opacity)
{
    m_previewFrame->setWindowOpacity(opacity);
}

void DraggableFrame::SetPreviewBorder(bool visible)
{
    if(visible)
        m_previewFrame->setStyleSheet("QLabel{"
                                      "border: 1px solid palette(highlight);"
                                      "}");
    else
        m_previewFrame->setStyleSheet("QLabel{"
                                      "border: none;"
                                      "}");
}

void DraggableFrame::SetDraggingEnabled(bool enabled)
{
    m_draggingEnabled = enabled;
    if(!enabled && m_isDragging)
    {
        m_isPressed = false;
        m_isDragging = false;
        QGuiApplication::restoreOverrideCursor();
        timerDown->stop();
        timerUp->stop();
        m_previewFrame->hide();
        releaseKeyboard();
        releaseMouse();
    }
}

void DraggableFrame::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_isPressed = true;
    }
}

void DraggableFrame::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(m_isPressed && m_isDragging == false)
        {
            onClick();
            emit clicked();
        }
        if(m_isDragging)
            emit dropped(event->pos());
        
        m_isPressed = false;
        m_isDragging = false;
        QGuiApplication::restoreOverrideCursor();
        timerDown->stop();
        timerUp->stop();
        m_previewFrame->hide();
        releaseKeyboard();
        releaseMouse();
    }
}

void DraggableFrame::mouseMoveEvent(QMouseEvent* event)
{
    if(m_isPressed)
    {
        if(m_draggingEnabled && m_isDragging == false)
        {
            emit dragAboutToStart();
            
            m_isDragging = true;
            m_initialPos = event->pos();
            m_previewFrame->move(mapToGlobal(QPoint(0, 0)));
            m_previewFrame->show();
            m_previewFrame->setFixedSize(this->size());
            m_previewFrame->setPixmap(QPixmap());
            m_previewFrame->setPixmap(this->grab());
            grabKeyboard();
            grabMouse();
            
            if(m_initialPos.y() > this->rect().bottom() || m_initialPos.y() < this->rect().top())
                m_initialPos = QPoint(m_initialPos.x(), this->rect().center().y());
            
            m_midDiff = this->rect().center() - event->pos();
            
            emit dragStarted();
            QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
            
            m_parentScrollArea = nullptr;
            QWidget* parent = this->parentWidget();
            while(parent)
            {
                if(auto scrollArea = qobject_cast<QScrollArea*>(parent))
                {
                    m_parentScrollArea = scrollArea;
                    break;
                }
                parent = parent->parentWidget();
            }
        }
    }
    
    if(m_draggingEnabled && m_isDragging)
    {
        m_currentPos = event->pos();
        auto mappedToParentPos = mapToParent(event->pos());
        emit dragPositionChanged(mappedToParentPos, middle(mappedToParentPos));
        
        auto pos = m_currentPos - m_initialPos;
        pos.setX(0);
        m_previewFrame->move(mapToGlobal(pos));
        
        
        if(m_parentScrollArea)
        {
            if(m_parentScrollArea->mapToGlobal(m_parentScrollArea->geometry().bottomLeft()).y() - mapToGlobal(m_currentPos).y() < 50)
            {
                if(!timerDown->isActive())
                    timerDown->start();
            }
            else
                timerDown->stop();
            
            if(mapToGlobal(m_currentPos).y() - m_parentScrollArea->mapToGlobal(m_parentScrollArea->geometry().topLeft()).y() < 60)
            {
                if(!timerUp->isActive())
                    timerUp->start();
            }
            else
                timerUp->stop();
        }
        else
        {
            timerDown->stop();
            timerUp->stop();
        }
    }
}

void DraggableFrame::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_isPressed = false;
        m_isDragging = false;
        QGuiApplication::restoreOverrideCursor();
        timerDown->stop();
        timerUp->stop();
        m_previewFrame->hide();
        releaseKeyboard();
        releaseMouse();
    }
}

void DraggableFrame::keyPressEvent(QKeyEvent* event)
{
    if(m_isDragging)
        emit dragCanceled();
    m_isPressed = false;
    m_isDragging = false;
    QGuiApplication::restoreOverrideCursor();
    timerDown->stop();
    timerUp->stop();
    m_previewFrame->hide();
    releaseKeyboard();
    releaseMouse();
    QFrame::keyPressEvent(event);
}

void DraggableFrame::wheelEvent(QWheelEvent* event)
{
    if(m_isDragging && m_parentScrollArea)
    {
        QCoreApplication::sendEvent(m_parentScrollArea, event);
    }
    return QFrame::wheelEvent(event);
}

