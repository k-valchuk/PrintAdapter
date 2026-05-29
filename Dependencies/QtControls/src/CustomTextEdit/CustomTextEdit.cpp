#include "CustomTextEdit.h"

#include "TextWidgetObjectHandler.h"

#include "QApplication"
#include "QTextObjectInterface"
#include "QTextDocumentWriter"
#include "QBuffer"
#include "QMimeData"
#include "QTextDocument"
#include "QTextDocumentFragment"
#include "QScrollBar"
#include "QEvent"
#include "QMouseEvent"
#include "QKeyEvent"
#include "QWheelEvent"
#include "QPainter"
#include "QWindow"
#include "QLineEdit"
#include "QStyle"
#include "QDebug"

class CustomTextEditMimeData : public QMimeData
{
public:
    inline CustomTextEditMimeData(const QTextDocumentFragment &aFragment, const QMap<int, QVariant>& aWidgetsPositionsAndData)
        : fragment(aFragment),
        widgetsPositionsAndData(aWidgetsPositionsAndData)
    { }
    
    virtual QStringList formats() const override
    {
        if (!fragment.isEmpty())
            return QStringList() << QString::fromLatin1("text/plain") << QString::fromLatin1("text/html")
//#if QT_CONFIG(textmarkdownwriter)
//                               << QString::fromLatin1("text/markdown")
//#endif
#ifndef QT_NO_TEXTODFWRITER
                                 << QString::fromLatin1("application/vnd.oasis.opendocument.text")
#endif
                                 << QString::fromLatin1("custom.text.edit/widgets");
        else
            return QMimeData::formats();
    }
protected:
    virtual QVariant retrieveData(const QString &mimeType, QVariant::Type type) const override
    {
        if (!fragment.isEmpty())
            setup();
        return QMimeData::retrieveData(mimeType, type);
    }
private:
    void setup() const
    {
        CustomTextEditMimeData *that = const_cast<CustomTextEditMimeData *>(this);
#ifndef QT_NO_TEXTHTMLPARSER
        that->setData(QLatin1String("text/html"), fragment.toHtml().toUtf8());
#endif
//#if QT_CONFIG(textmarkdownwriter)
//        this->setData(QLatin1String("text/markdown"), fragment.toMarkDown().toUtf8());
//#endif
#ifndef QT_NO_TEXTODFWRITER
        {
            QBuffer buffer;
            QTextDocumentWriter writer(&buffer, "ODF");
            writer.write(fragment);
            buffer.close();
            that->setData(QLatin1String("application/vnd.oasis.opendocument.text"), buffer.data());
        }
#endif
        that->setText(fragment.toPlainText());
        
        if(!widgetsPositionsAndData.isEmpty())
        {
            QBuffer buffer;
            QDataStream stream(&buffer);
            buffer.open(QBuffer::OpenModeFlag::WriteOnly);
            stream << widgetsPositionsAndData;
            buffer.close();
            that->setData(QLatin1String("custom.text.edit/widgets"), buffer.data());
        }
        fragment = QTextDocumentFragment();
    }
    
    mutable QTextDocumentFragment fragment;
    mutable QMap<int, QVariant> widgetsPositionsAndData;
};

//-----------------

class CustomTextEditViewportEventsFilter : public QObject
{
    CustomTextEdit* m_edit;
public:
    CustomTextEditViewportEventsFilter(CustomTextEdit* edit) : m_edit(edit){}
    
    bool eventFilter(QObject* watched, QEvent* event)
    {
        if(event->type() == QEvent::MouseMove)
        {
            bool filtered = m_edit->SendEventToWidget(event);
            m_edit->m_lastCursorPosition = static_cast<QMouseEvent*>(event)->pos();
            return filtered;
        }
        else if(event->type() == QEvent::MouseButtonPress)
        {
            m_edit->setEditorActive(true);
            bool filtered = m_edit->SendEventToWidget(event);
            if(!filtered)
            {
                m_edit->m_mouseCapturedByEdit = true;
                
                if(m_edit->m_focusedWidget){
                    {QSignalBlocker bl{m_edit->m_focusedWidget->topLevelWidget()->windowHandle()};
                        m_edit->m_focusedWidget->clearFocus();
                    }
                    QFocusEvent fev(QEvent::FocusOut, Qt::MouseFocusReason);
                    qApp->notify(m_edit->m_focusedWidget, &fev);
                    
                    auto fnt = m_edit->m_focusedWidget->font();
                    m_edit->m_focusedWidget->setProperty("CTEfocus", false);
                    m_edit->m_focusedWidget->style()->unpolish(m_edit->m_focusedWidget);
                    m_edit->m_focusedWidget->style()->polish(m_edit->m_focusedWidget);
                    m_edit->m_focusedWidget->setFont(fnt);
                    
                    m_edit->m_focusedWidget = nullptr;
                }
            }
            return filtered;
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            bool filtered = m_edit->SendEventToWidget(event);
            m_edit->m_mouseCapturedByEdit = false;
            return filtered;
        }
        else if(event->type() == QEvent::MouseButtonDblClick)
        {
            bool filtered = m_edit->SendEventToWidget(event);
            if(!filtered)
            {
                if(m_edit->m_focusedWidget){
                    {QSignalBlocker bl{m_edit->m_focusedWidget->topLevelWidget()->windowHandle()};
                        m_edit->m_focusedWidget->clearFocus();
                    }
                    QFocusEvent fev(QEvent::FocusOut, Qt::MouseFocusReason);
                    qApp->notify(m_edit->m_focusedWidget, &fev);
                    
                    auto fnt = m_edit->m_focusedWidget->font();
                    m_edit->m_focusedWidget->setProperty("CTEfocus", false);
                    m_edit->m_focusedWidget->style()->unpolish(m_edit->m_focusedWidget);
                    m_edit->m_focusedWidget->style()->polish(m_edit->m_focusedWidget);
                    m_edit->m_focusedWidget->setFont(fnt);
                    
                    m_edit->m_focusedWidget = nullptr;
                }
            }
            return filtered;
        }
        else if(event->type() == QEvent::Wheel)
        {
            bool filtered = m_edit->SendEventToWidget(event);
            return filtered;
        }
        else if(event->type() == QEvent::ContextMenu)
        {
            bool filtered = m_edit->SendEventToWidget(event);
            return filtered;
        }
        
        return false;
    }
};


class CustomTextEditMainEventsFilter : public QObject
{
    CustomTextEdit* m_edit;
public:
    CustomTextEditMainEventsFilter(CustomTextEdit* edit) : m_edit(edit){}
    
    bool eventFilter(QObject* watched, QEvent* event)
    {
        if(event->type() == QEvent::KeyPress)
        {
            bool filtered = m_edit->SendEventToWidget(event);
            return filtered;
        }
        else if(event->type() == QEvent::KeyRelease)
        {
            bool filtered = m_edit->SendEventToWidget(event);
            return filtered;
        }
        else if(event->type() == QEvent::ContextMenu)
        {
            bool filtered = m_edit->SendEventToWidget(event);
            return filtered;
        }
        
        return false;
    }
};

//------------------

QPointF CustomTextEdit::childTopLeftR(QWidget* wgt, QWidget* parent)
{
    QPointF result(0, 0);
    if(wgt == parent)
        return result;
    
    while (wgt && wgt != parent) {
        result += QPointF(wgt->pos());
        wgt = wgt->parentWidget();
    }
    
    return result;
}

bool CustomTextEdit::SendEventToWidget(QEvent* ev)
{
    bool result = false;
    
    int xoffset = 0, yoffset = 0;
    if(auto hscrollbar = this->horizontalScrollBar())
        xoffset = -hscrollbar->value();
    if(auto vscrollbar = this->verticalScrollBar())
        yoffset = -vscrollbar->value();
    QPoint scrollOffset(xoffset, yoffset);
    
    if(auto wev = dynamic_cast<QWheelEvent*>(ev))
    {
        if(m_focusedWidget && !m_widgetsUnderSelection.contains(m_focusedWidget))
        {
            QPoint localCursorPosition;
            QPoint oldLocalCursorPosition;
            
            auto wgts = widgets.keys();
            auto parentWgt = m_focusedWidget.data();
            while(parentWgt && !wgts.contains(parentWgt))
                parentWgt = parentWgt->parentWidget();
            if(parentWgt && wgts.contains(parentWgt))
            {
                auto widget = parentWgt;
                auto rect = widgets[widget];
                auto evPos =
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                                wev->pos();
#else
                                wev->position();
#endif
                auto localWgtPos = evPos - rect.topLeft() - scrollOffset;
                auto wgt = widget->childAt(localWgtPos.toPoint());
                if(!wgt) wgt = widget;
                
                //auto childTopLeft = wgt->mapTo(widget, QPoint(0, 0));
                auto childTopLeft = childTopLeftR(wgt, widget).toPoint();
                localWgtPos = localWgtPos - childTopLeft;
                
                auto childRect = wgt->rect();
                //childRect.moveTopLeft(childTopLeft);
                
                //m_lastWidgetUnderCursor = wgt;
                oldLocalCursorPosition = m_lastCursorPosition - rect.topLeft().toPoint() - childTopLeft - scrollOffset;
                localCursorPosition = localWgtPos.toPoint();
                
                QWheelEvent nwev(localCursorPosition, wev->delta(), wev->buttons(), wev->modifiers(), wev->orientation());
                result = qApp->notify(wgt, &nwev);
            }
        }
    }
    else if(auto cev = dynamic_cast<QContextMenuEvent*>(ev))
    {
        if(m_mouseCapturedByEdit)
             return false;
        
        if(!m_mouseGrabber)
            m_mouseGrabber = nullptr;
        
        if(cev->reason() != QContextMenuEvent::Reason::Mouse && m_focusedWidget)
        {
            auto pos = m_focusedWidget->inputMethodQuery(
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                Qt::ImMicroFocus
#else
                Qt::ImCursorRectangle
#endif
                ).toRect().center();
            
            QContextMenuEvent cmev(cev->reason(), pos,
                                   //TODO сменить mapToGlobal на ручной маппинг, может и не нужно
                                   //если остается заплаточный setGeometry в drawObject'е
                                   m_focusedWidget->mapToGlobal(pos),
                                   QApplication::keyboardModifiers());
            result = qApp->notify(m_focusedWidget, &cmev) || result;
            return result;
        }
        
        QPoint localCursorPosition;
        QPoint oldLocalCursorPosition;
        QWidget* receiver = nullptr;
        auto wgts = widgets.keys();
        auto cevPos = cev->pos();
        
        if(!m_mouseGrabber)
        {
            for(const auto& widget : wgts)
            {
                auto rect = widgets[widget];
                auto localWgtPos = cevPos - rect.topLeft() - scrollOffset;
                auto wgt = widget->childAt(localWgtPos.toPoint());
                if(!wgt) wgt = widget;
             
                //auto childTopLeft = wgt->mapTo(widget, QPoint(0, 0));
                auto childTopLeft = childTopLeftR(wgt, widget).toPoint();
                localWgtPos = localWgtPos - childTopLeft;
             
                auto childRect = wgt->rect();
                //childRect.moveTopLeft(childTopLeft);
             
                if(childRect.contains(localWgtPos.toPoint()) && !m_widgetsUnderSelection.contains(widget))
                {
                    receiver = wgt;
                    oldLocalCursorPosition = m_lastCursorPosition - rect.topLeft().toPoint() - childTopLeft - scrollOffset;
                    localCursorPosition = localWgtPos.toPoint();
                }
            }
        }
        else
        {
            auto parentGrabber = m_mouseGrabber.data();
            while(parentGrabber && !wgts.contains(parentGrabber))
                parentGrabber = parentGrabber->parentWidget();
            if(parentGrabber && wgts.contains(parentGrabber))
            {
                auto widget = parentGrabber;
                auto rect = widgets[widget];
                auto localWgtPos = cev->pos() - rect.topLeft() - scrollOffset;
                auto wgt = widget->childAt(localWgtPos.toPoint());
                if(!wgt) wgt = widget;
             
                //auto childTopLeft = wgt->mapTo(widget, QPoint(0, 0));
                auto childTopLeft = childTopLeftR(wgt, widget).toPoint();
                localWgtPos = localWgtPos - childTopLeft;
             
                auto childRect = wgt->rect();
                //childRect.moveTopLeft(childTopLeft);
             
                receiver = m_mouseGrabber;
                oldLocalCursorPosition = m_lastCursorPosition - rect.topLeft().toPoint() - childTopLeft - scrollOffset;
                localCursorPosition = localWgtPos.toPoint();
            }
        }
        
        if(receiver)
        {
            QContextMenuEvent cmev(cev->reason(), localCursorPosition,
                                   cev->globalPos(), QApplication::keyboardModifiers());
            result = qApp->notify(receiver, &cmev) || result;
        }
    }
    else if(auto mev = dynamic_cast<QMouseEvent*>(ev))
    {
        if(m_mouseCapturedByEdit)
            return false;
        
        if(!m_mouseGrabber)
            m_mouseGrabber = nullptr;
        
        auto oldWidgetUnderCursor = m_lastWidgetUnderCursor;
        m_lastWidgetUnderCursor = nullptr;
        QPoint localCursorPosition;
        QPoint oldLocalCursorPosition;
        auto wgts = widgets.keys();
        if(!m_mouseGrabber)
        {
            for(const auto& widget : wgts)
            {
                auto rect = widgets[widget];
                auto localWgtPos = mev->pos() - rect.topLeft() - scrollOffset;
                auto wgt = widget->childAt(localWgtPos.toPoint());
                if(!wgt) wgt = widget;
                
                //auto childTopLeft = wgt->mapTo(widget, QPoint(0, 0));
                auto childTopLeft = childTopLeftR(wgt, widget).toPoint();
                localWgtPos = localWgtPos - childTopLeft;
                
                auto childRect = wgt->rect();
                //childRect.moveTopLeft(childTopLeft);
                
                if(childRect.contains(localWgtPos.toPoint()) && !m_widgetsUnderSelection.contains(widget))
                {
                    m_lastWidgetUnderCursor = wgt;
                    oldLocalCursorPosition = m_lastCursorPosition - rect.topLeft().toPoint() - childTopLeft - scrollOffset;
                    localCursorPosition = localWgtPos.toPoint();
                }
            }
        }
        else
        {
            auto parentGrabber = m_mouseGrabber.data();
            while(parentGrabber && !wgts.contains(parentGrabber))
                parentGrabber = parentGrabber->parentWidget();
            if(parentGrabber && wgts.contains(parentGrabber))
            {
                auto widget = parentGrabber;
                auto rect = widgets[widget];
                auto localWgtPos = mev->pos() - rect.topLeft() - scrollOffset;
                auto wgt = m_mouseGrabber; //потому что локальная позиция должна считаться относительно mouseGrabber'а а не чайлдов
                
                auto childTopLeft = childTopLeftR(wgt, widget).toPoint();
                localWgtPos = localWgtPos - childTopLeft;
                
                m_lastWidgetUnderCursor = m_mouseGrabber;
                oldLocalCursorPosition = m_lastCursorPosition - rect.topLeft().toPoint() - childTopLeft - scrollOffset;
                localCursorPosition = localWgtPos.toPoint();
            }
        }
        
        if(oldWidgetUnderCursor != m_lastWidgetUnderCursor)
        {
            //qDebug() << oldWidgetUnderCursor << m_lastWidgetUnderCursor;
            //enter and leave events
            
            if(oldWidgetUnderCursor)
            {
                QEvent leaveEv(QEvent::Leave);
                qApp->notify(oldWidgetUnderCursor, &leaveEv);
                
                auto parentWgt = oldWidgetUnderCursor;
                while(parentWgt && parentWgt != m_lastWidgetUnderCursor)
                {
                    QHoverEvent hoverLeaveEv(QEvent::HoverLeave, localCursorPosition, oldLocalCursorPosition,
                                             QApplication::keyboardModifiers());
                    qApp->notify(parentWgt, &hoverLeaveEv);
                    
                    parentWgt = parentWgt->parentWidget();
                }
            }
            
            if(m_lastWidgetUnderCursor)
            {
                QEnterEvent enterEv(localCursorPosition, mev->windowPos(), mev->screenPos());
                result = qApp->notify(m_lastWidgetUnderCursor, &enterEv) || result;
                
                QHoverEvent hoverEnterEv(QEvent::HoverEnter, localCursorPosition, oldLocalCursorPosition,
                                         QApplication::keyboardModifiers());
                result = qApp->notify(m_lastWidgetUnderCursor, &hoverEnterEv) || result;
            }
        }
        
        if(m_lastWidgetUnderCursor)
        {
            this->viewport()->setCursor(m_lastWidgetUnderCursor->cursor());
            
            if(mev->type() == QEvent::MouseButtonPress || mev->type() == QEvent::MouseButtonDblClick)
            {
                
                if(m_focusedWidget && m_lastWidgetUnderCursor != m_focusedWidget)
                {
                    {QSignalBlocker bl{m_focusedWidget->topLevelWidget()->windowHandle()};
                        m_focusedWidget->clearFocus();
                    }
                    
                    QFocusEvent fev(QEvent::FocusOut, Qt::MouseFocusReason);
                    qApp->notify(m_focusedWidget, &fev) || result;
                    
                    auto fnt = m_focusedWidget->font();
                    m_focusedWidget->setProperty("CTEfocus", false);
                    m_focusedWidget->style()->unpolish(m_focusedWidget);
                    m_focusedWidget->style()->polish(m_focusedWidget);
                    m_focusedWidget->setFont(fnt);
                    
                    m_focusedWidget = nullptr;
                }
                if(m_focusedWidget != m_lastWidgetUnderCursor)
                {
                    auto oldFocusedWidget = qApp->focusWidget();
                    {QSignalBlocker bl{m_lastWidgetUnderCursor->topLevelWidget()->windowHandle()};
                        m_lastWidgetUnderCursor->setFocus(Qt::FocusReason::MouseFocusReason);
                    }
                    
                    auto fnt = m_lastWidgetUnderCursor->font();
                    m_lastWidgetUnderCursor->setProperty("CTEfocus", true);
                    m_lastWidgetUnderCursor->style()->unpolish(m_lastWidgetUnderCursor);
                    m_lastWidgetUnderCursor->style()->polish(m_lastWidgetUnderCursor);
                    m_lastWidgetUnderCursor->setFont(fnt);
                    
                    QFocusEvent fev(QEvent::FocusIn, Qt::MouseFocusReason);
                    result = qApp->notify(m_lastWidgetUnderCursor, &fev) || result;
                    emit qApp->focusChanged(oldFocusedWidget, m_lastWidgetUnderCursor);
                    
                    setEditorActive(false);
                    m_focusedWidget = m_lastWidgetUnderCursor;
                }
            }
            
            QMouseEvent mouseEv(*mev);
            mouseEv.setLocalPos(localCursorPosition);
            result = qApp->notify(m_lastWidgetUnderCursor, &mouseEv) || result;
            
            if(mev->type() == QEvent::MouseMove)
            {
                QHoverEvent hoverMoveEv(QEvent::HoverMove, localCursorPosition, oldLocalCursorPosition,
                                        QApplication::keyboardModifiers());
                result = qApp->notify(m_lastWidgetUnderCursor, &hoverMoveEv) || result;
            }
            
            if(mev->type() == QEvent::MouseButtonPress || mev->type() == QEvent::MouseButtonDblClick)
            {
                m_mouseGrabber = m_lastWidgetUnderCursor;
            }
        }
        else
        {
            this->viewport()->setCursor(Qt::IBeamCursor);
        }
        
        if(mev->type() == QEvent::MouseButtonRelease)
        {
            //if(mev->button() == Qt::RightButton && m_lastWidgetUnderCursor)
            //{
            //    QContextMenuEvent cmev(QContextMenuEvent::Reason::Mouse,
            //                           localCursorPosition, mev->globalPos(), QApplication::keyboardModifiers());
            //    result = qApp->notify(m_lastWidgetUnderCursor, &cmev) || result;
            //}
            
            auto oldloc = oldLocalCursorPosition;
            auto loc = localCursorPosition;
            for(const auto& widget : widgets.keys())
            {
                auto rect = widgets[widget];
                auto localWgtPos = mev->pos() - rect.topLeft() - scrollOffset;
                auto wgt = widget->childAt(localWgtPos.toPoint());
                if(!wgt) wgt = widget;
                
                //auto childTopLeft = wgt->mapTo(widget, QPoint(0, 0));
                auto childTopLeft = childTopLeftR(wgt, widget).toPoint();
                localWgtPos = localWgtPos - childTopLeft;
                
                auto childRect = wgt->rect();
                //childRect.moveTopLeft(childTopLeft);
                
                if(childRect.contains(localWgtPos.toPoint()))
                {
                    m_lastWidgetUnderCursor = wgt;
                    oldLocalCursorPosition = m_lastCursorPosition - rect.topLeft().toPoint() - childTopLeft - scrollOffset;
                    localCursorPosition = localWgtPos.toPoint();
                }
            }
            
            if(m_mouseGrabber && m_mouseGrabber != m_lastWidgetUnderCursor)
            {
                QEvent leaveEv(QEvent::Leave);
                qApp->notify(m_mouseGrabber, &leaveEv);
                
                auto parentWgt = m_mouseGrabber;
                while(parentWgt && parentWgt != m_lastWidgetUnderCursor)
                {
                    QHoverEvent hoverLeaveEv(QEvent::HoverLeave, loc, oldloc,
                                             QApplication::keyboardModifiers());
                    qApp->notify(parentWgt, &hoverLeaveEv);
                    
                    parentWgt = parentWgt->parentWidget();
                }
                
                QEnterEvent enterEv(localCursorPosition, mev->windowPos(), mev->screenPos());
                result = qApp->notify(m_lastWidgetUnderCursor, &enterEv) || result;
                
                QHoverEvent hoverEnterEv(QEvent::HoverEnter, localCursorPosition, oldLocalCursorPosition,
                                         QApplication::keyboardModifiers());
                result = qApp->notify(m_lastWidgetUnderCursor, &hoverEnterEv) || result;
            }
            
            m_mouseGrabber = nullptr;
        }
    }
    else if(auto kev = dynamic_cast<QKeyEvent*>(ev))
    {
        if(m_focusedWidget)
        {
            QPointer<QWidget> receiver = m_focusedWidget->focusWidget();
            if (!receiver)
                receiver = m_focusedWidget;
            
            do {
                result = QCoreApplication::sendEvent(receiver, ev);
                if ((result && kev->isAccepted()) || (receiver == m_focusedWidget))
                    break;
                receiver = receiver->parentWidget();
            } while (receiver);
        }
    }
    
    return result;
}

void CustomTextEdit::setupWidget(QWidget* wgt)
{
    if(!wgt)
        return;
    
    wgt->setAttribute(Qt::WA_DontShowOnScreen, true);
    wgt->setAttribute(Qt::WA_QuitOnClose, false);
    wgt->setWindowFlag(Qt::WindowType::FramelessWindowHint, true);
    for(auto wgt : QList<QWidget*>{wgt->findChildren<QWidget*>()} + QList<QWidget*>{wgt})
        wgt->installEventFilter(handler);
    wgt->show();
    
    auto lineEdits = wgt->findChildren<QLineEdit*>();
    if(auto lineEdit = qobject_cast<QLineEdit*>(wgt))
        lineEdits.prepend(lineEdit);
    for(auto lineEdit : lineEdits)
    {
        connect(lineEdit, &QLineEdit::selectionChanged, lineEdit, [this, lineEdit](){
            if(m_focusedWidget == lineEdit)
                lineEdit->setFocus();
        });
    }
    
    connect(wgt, SIGNAL(leaveToCTE(CustomTextEdit::EnterDirection)), this, SLOT(onWidgetLeave(CustomTextEdit::EnterDirection)));
    connect(this, SIGNAL(maxWidthChanged(int)), wgt, SLOT(onCTEMaxWidthChanged(int)));
    connect(this, SIGNAL(CTEScaleChanged(double)), wgt, SLOT(onCTEScaleChanged(double)));
    //connect(this, SIGNAL(widgetEnter(bool)), wgt, SLOT(onEnterFromCTE(bool)));
    
    //reemit info
    auto maxWidth = qRound(this->document()->textWidth() - 25);
    QMetaObject::invokeMethod(wgt, "onCTEMaxWidthChanged", Qt::DirectConnection, Q_ARG(int, maxWidth));
    QMetaObject::invokeMethod(wgt, "onCTEScaleChanged", Qt::DirectConnection, Q_ARG(double, m_currentScale));
}

void CustomTextEdit::setEditorActive(bool active)
{
    if(active)
    {
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    else
    {
        setTextInteractionFlags(Qt::NoTextInteraction);
        auto cur = textCursor();
        cur.clearSelection();
        setTextCursor(cur);
    }
}


CustomTextEdit::CustomTextEdit(QWidget* parent) : QTextEdit(parent)
{
    qRegisterMetaType<QSharedPointer<TextWidget>>();
    m_baseFntSize = this->font().pointSizeF();
    
    this->setMouseTracking(true);
    
    this->handler = new TextWidgetObjectHandler(this);
    this->document()->documentLayout()->registerHandler(QTextFormat::UserObject, handler);
    
    m_mouseEventsFilter = new CustomTextEditViewportEventsFilter(this);
    this->viewport()->installEventFilter(m_mouseEventsFilter);
    m_keyEventsFilter = new CustomTextEditMainEventsFilter(this);
    this->installEventFilter(m_keyEventsFilter);
    
    QObject::connect(this, &QTextEdit::textChanged, this, [this](){
        UpdateInternalWidgetsData();
    });
    
    QObject::connect(this, &QTextEdit::selectionChanged, this, [this](){
        m_widgetsUnderSelection.clear();
        auto selStart = this->textCursor().selectionStart();
        auto selEnd = this->textCursor().selectionEnd();
        QTextCursor cur(this->document());
        cur.setPosition(selStart);
        for(; cur.position() < selEnd; cur.movePosition(QTextCursor::NextCharacter))
        {
            auto tw = GetWidget(cur);
            if(!tw)
                continue;
            if(!tw->Widget)
                tw->Widget = this->createWidget(tw->Data);
            
            if(tw->Widget)
                m_widgetsUnderSelection.insert(tw->Widget);
        }
    });
}

CustomTextEdit::~CustomTextEdit()
{
    delete handler;
}

void CustomTextEdit::SetFocusOnTextWidget(QWidget* wgt)
{
    if(m_focusedWidget && wgt != m_focusedWidget)
    {
        {QSignalBlocker bl{m_focusedWidget->topLevelWidget()->windowHandle()};
            m_focusedWidget->clearFocus();
        }
        
        QFocusEvent fev(QEvent::FocusOut, Qt::MouseFocusReason);
        qApp->notify(m_focusedWidget, &fev);
        
        auto fnt = m_focusedWidget->font();
        m_focusedWidget->setProperty("CTEfocus", false);
        m_focusedWidget->style()->unpolish(m_focusedWidget);
        m_focusedWidget->style()->polish(m_focusedWidget);
        m_focusedWidget->setFont(fnt);
        
        m_focusedWidget = nullptr;
    }
    
    if(wgt)
    {QSignalBlocker bl{wgt->topLevelWidget()->windowHandle()};
        wgt->setFocus(Qt::FocusReason::MouseFocusReason);
    }
    
    if(wgt)
    {
        auto oldFocusedWidget = qApp->focusWidget();
        QFocusEvent fev(QEvent::FocusIn, Qt::MouseFocusReason);
        qApp->notify(wgt, &fev);
        emit qApp->focusChanged(oldFocusedWidget, wgt);
        
        auto fnt = wgt->font();
        wgt->setProperty("CTEfocus", true);
        wgt->style()->unpolish(wgt);
        wgt->style()->polish(wgt);
        wgt->setFont(fnt);
        
        setEditorActive(false);
    }
    else
        setEditorActive(true);
    m_focusedWidget = wgt;
}

void CustomTextEdit::insertWidget(QTextCursor& cur, QWidget* wgt)
{
    Q_ASSERT_X(cur.document() == this->document(), "CustomTextEdit", "inserting widget in not that document");
    
    setupWidget(wgt);
    
    QTextCharFormat f;
    f.setObjectType(QTextFormat::UserObject);
    auto tw = QSharedPointer<TextWidget>::create(wgt);
    f.setProperty(QTextFormat::UserProperty, QVariant::fromValue(tw));
    
    auto otherFmt = GetNextCharacterFormat(cur);
    mergeWidgetFormatWithOther(f, otherFmt);
    cur.insertText(QString(QChar::ObjectReplacementCharacter), f);
    
    m_createdWidgets.insert(tw);
}

void CustomTextEdit::insertWidget(QTextCursor& cur, const QVariant& data)
{
    Q_ASSERT_X(cur.document() == this->document(), "CustomTextEdit", "inserting widget in not that document");
    
    auto wgt = createWidget(data);
    
    setupWidget(wgt);
    
    QTextCharFormat f;
    f.setObjectType(QTextFormat::UserObject);
    auto tw = QSharedPointer<TextWidget>::create(wgt);
    f.setProperty(QTextFormat::UserProperty, QVariant::fromValue(tw));
    
    auto otherFmt = GetNextCharacterFormat(cur);
    mergeWidgetFormatWithOther(f, otherFmt);
    cur.insertText(QString(QChar::ObjectReplacementCharacter), f);
    
    m_createdWidgets.insert(tw);
}

QTextCharFormat CustomTextEdit::GetNextCharacterFormat(const QTextCursor& cur)
{
    QTextCharFormat format;
    if(cur.atBlockStart())
    {
        format = cur.charFormat();
    }
    else
    {
        auto curcopy = cur;
        curcopy.movePosition(QTextCursor::NextCharacter);
        format = curcopy.charFormat();
    }
    return format;
}

QSharedPointer<TextWidget> CustomTextEdit::GetWidget(const QTextCursor& cur) const
{
    int pos = cur.position();
    auto symbol = cur.document()->characterAt(pos);
    if(symbol != QChar::ObjectReplacementCharacter)
        return nullptr;
    
    auto format = GetNextCharacterFormat(cur);
    
    return GetWidgetFromFormat(format);
}

QSharedPointer<TextWidget> CustomTextEdit::GetWidgetFromFormat(const QTextCharFormat& format)
{
    if(format.objectType() != QTextFormat::UserObject)
        return nullptr;
    
    if(!format.hasProperty(QTextFormat::UserProperty))
        return nullptr;
    
    auto var = format.property(QTextFormat::UserProperty);
    if(!var.canConvert<QSharedPointer<TextWidget>>())
        return nullptr;
    
    return var.value<QSharedPointer<TextWidget>>();
}

void CustomTextEdit::UpdateInternalWidgetsData()
{
    QSet<QSharedPointer<TextWidget>> widgetsToRemove = m_createdWidgets;
    for(QTextCursor cur(this->document()); !cur.atEnd(); cur.movePosition(QTextCursor::NextCharacter))
    {
        auto tw = GetWidget(cur);
        if(!tw)
            continue;
        if(!tw->Widget)
            tw->Widget = this->createWidget(tw->Data);
        
        widgetsToRemove.remove(tw);
    }
    
    for(auto tw : widgetsToRemove)
    {
        if(tw->Widget)
        {
            tw->Data = this->serializeWidget(tw->Widget, tw->Data);
            this->widgets.remove(tw->Widget);
            this->destroyWidget(tw->Widget);
        }
    }
}

QWidget* CustomTextEdit::createWidget(const QVariant& mimeData) const
{
    return nullptr;
}

QVariant CustomTextEdit::serializeWidget(QWidget* wgt, const QVariant& existingData) const
{
    return QVariant();
}

void CustomTextEdit::destroyWidget(QWidget* wgt)
{
    delete wgt;
}

void CustomTextEdit::mergeWidgetFormatWithOther(QTextCharFormat& wf, const QTextCharFormat& other)
{
    wf.setBackground(other.background());
}

void CustomTextEdit::onWidgetLeave(CustomTextEdit::EnterDirection to)
{
    auto sigSender = sender();
    if(!sigSender)
        return;
    auto widget = qobject_cast<QWidget*>(sigSender);
    
    for(QTextCursor cur(this->document()); !cur.atEnd(); cur.movePosition(QTextCursor::NextCharacter))
    {
        auto tw = GetWidget(cur);
        if(!tw)
            continue;
        
        if(tw->Widget == widget)
        {
            SetFocusOnTextWidget(nullptr);
            if(to == EnterDirection::PreviousChar)
                ;//курсор и так там
            else if(to == EnterDirection::NextChar)
                cur.movePosition(QTextCursor::NextCharacter);
            else if(to == EnterDirection::PreviousLine)
            {
                cur.movePosition(QTextCursor::StartOfLine);
                cur.movePosition(QTextCursor::PreviousCharacter);
            }
            else if(to == EnterDirection::NextLine)
            {
                cur.movePosition(QTextCursor::EndOfLine);
                cur.movePosition(QTextCursor::NextCharacter);
            }
            this->setTextCursor(cur);
            break;
        }
    }
}

QMimeData* CustomTextEdit::createMimeDataFromSelection() const
{
    auto cur = this->textCursor();
    auto fr = cur.selection();
    QTextDocument result;
    QTextCursor ncur(&result);
    ncur.insertFragment(fr);
    ncur.setPosition(0);
    QMap<int, QVariant> widgetPositionsAndData;
    
    for(; !ncur.atEnd(); ncur.movePosition(QTextCursor::NextCharacter))
    {
        auto tw = GetWidget(ncur);
        if(!tw)
            continue;
        if(!tw->Widget)
            createWidget(tw->Data);
        QWidget* wgt = tw->Widget;
        
        auto pos = ncur.position();
        QTextImageFormat imageFormat;
        if(wgt)
        {
            auto format = GetNextCharacterFormat(ncur);
            auto drawer = dynamic_cast<QTextObjectInterface*>(this->handler);
            auto rect = QRectF(QPointF(0, 0), drawer->intrinsicSize(this->document(), pos, format));
            QImage image(rect.toRect().size(), QImage::Format_ARGB32_Premultiplied);
            QPainter painter(&image);
            drawer->drawObject(&painter, rect, nullptr, pos, format);
            
            QBuffer buffer;
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, "PNG");
            auto const encoded = QString(buffer.data().toBase64());
            
            imageFormat.setName(QString("data:image/png;base64,%1").arg(encoded));
            imageFormat.setHeight(rect.size().height());
            imageFormat.setWidth(rect.size().width());
            
            tw->Data = serializeWidget(tw->Widget, tw->Data);
            widgetPositionsAndData[pos] = tw->Data;
        } 
        
        {//set image format instead of custom format
            QTextCursor ec(&result);
            ec.setPosition(pos);
            ec.setPosition(pos + 1, QTextCursor::KeepAnchor);
            ec.setCharFormat(imageFormat);
        }
    }
    
    ncur.select(QTextCursor::Document);
    return new CustomTextEditMimeData(ncur.selection(), widgetPositionsAndData);
}

void CustomTextEdit::insertFromMimeData(const QMimeData* source)
{
    if(source->hasFormat(QLatin1String("custom.text.edit/widgets")))
    {
        QTextDocument result;
        QTextCursor ncur(&result);
        ncur.insertFragment(QTextDocumentFragment::fromHtml(source->html()));
        ncur.setPosition(0);
        
        restoreInsertedWidgets(&result, 0, source);
        
        ncur.select(QTextCursor::Document);
        this->textCursor().insertFragment(ncur.selection());
    }
    else
        QTextEdit::insertFromMimeData(source);
}

void CustomTextEdit::wheelEvent(QWheelEvent* ev)
{
    //if(!SendEventToWidget(ev)){
        if (ev->modifiers() & Qt::ControlModifier) {
            float delta = ev->angleDelta().y() / 120.f;
            if(delta != 0.f)
            {
                zoomInF(delta);
                m_currentScale = this->font().pointSizeF()/m_baseFntSize;
                emit CTEScaleChanged(m_currentScale);
            }
            return;
        }
        QAbstractScrollArea::wheelEvent(ev);
        updateMicroFocus();
        //}
}

void CustomTextEdit::resizeEvent(QResizeEvent* ev)
{
    QTextEdit::resizeEvent(ev);
    
    //dynamic_cast<TextWidgetObjectHandler*>(this->handler)->m_pixmapCache.clear();
    
    //auto maxWidth = document()->textWidth()-25;
    //for(auto wgt : widgets.keys())
    //{
    //    auto size = wgt->sizeHint();
    //    size = size.boundedTo(QSize(maxWidth, size.height()));
    //    size = size.expandedTo(wgt->minimumSizeHint());
        //wgt->setAttribute(Qt::WA_DontShowOnScreen, false);
        //wgt->setFixedSize(size);
        //wgt->setAttribute(Qt::WA_DontShowOnScreen, true);
    //}
    //emit maxWidthChanged(maxWidth);
}

void CustomTextEdit::keyPressEvent(QKeyEvent* ev)
{
    static const auto onEnterFromCTE = "onEnterFromCTE";
    
    if(ev->modifiers().testFlag(Qt::ShiftModifier))
        return QTextEdit::keyPressEvent(ev);
    
    if(ev->key() == Qt::Key_Right)
    {
        auto cur = this->textCursor();
        auto tw = GetWidget(cur);
        if(tw && tw->Widget)
        {
            bool res;
            if(QMetaObject::invokeMethod(tw->Widget, onEnterFromCTE,
                                         Qt::DirectConnection,
                                         Q_RETURN_ARG(bool, res),
                                         Q_ARG(CustomTextEdit::EnterDirection, CustomTextEdit::EnterDirection::PreviousChar))
               && res)
                return;
        }
    }
    if(ev->key() == Qt::Key_Left)
    {
        auto cur = this->textCursor();
        cur.movePosition(QTextCursor::PreviousCharacter);
        auto tw = GetWidget(cur);
        if(tw && tw->Widget)
        {
            bool res;
            if(QMetaObject::invokeMethod(tw->Widget, onEnterFromCTE,
                                         Qt::DirectConnection,
                                         Q_RETURN_ARG(bool, res),
                                         Q_ARG(CustomTextEdit::EnterDirection, CustomTextEdit::EnterDirection::NextChar))
                && res)
            return;
        }
    }
    if(ev->key() == Qt::Key_Down)
    {
        auto cur = this->textCursor();
        cur.movePosition(QTextCursor::Down);
        auto tw = GetWidget(cur);
        if(!tw) {
            cur.movePosition(QTextCursor::PreviousCharacter);
            tw = GetWidget(cur);
        }
        if(tw && tw->Widget)
        {
            bool res;
            if(QMetaObject::invokeMethod(tw->Widget, onEnterFromCTE,
                                         Qt::DirectConnection,
                                         Q_RETURN_ARG(bool, res),
                                         Q_ARG(CustomTextEdit::EnterDirection, CustomTextEdit::EnterDirection::PreviousLine))
               && res)
                return;
        }
    }
    if(ev->key() == Qt::Key_Up)
    {
        auto cur = this->textCursor();
        cur.movePosition(QTextCursor::Up);
        auto tw = GetWidget(cur);
        if(!tw) {
            cur.movePosition(QTextCursor::PreviousCharacter);
            tw = GetWidget(cur);
        }
        if(tw && tw->Widget)
        {
            bool res;
            if(QMetaObject::invokeMethod(tw->Widget, onEnterFromCTE,
                                        Qt::DirectConnection,
                                        Q_RETURN_ARG(bool, res),
                                        Q_ARG(CustomTextEdit::EnterDirection, CustomTextEdit::EnterDirection::NextLine))
                && res)
                return;
        }
    }
    QTextEdit::keyPressEvent(ev);
}

void CustomTextEdit::restoreInsertedWidgets(QTextDocument* doc, int startPosition, const QMimeData* source)
{
    if(source->hasFormat(QLatin1String("custom.text.edit/widgets")))
    {
        QMap<int, QVariant> widgetsPositionsAndData;
        auto buffer = source->data(QLatin1String("custom.text.edit/widgets"));
        QDataStream stream(buffer);
        stream >> widgetsPositionsAndData;
        
        for(auto pos : widgetsPositionsAndData.keys())
        {
            auto wdata = widgetsPositionsAndData[pos];
            
            QTextCharFormat widgetCharFormat;
            widgetCharFormat.setObjectType(QTextFormat::UserObject);
            auto wgt = createWidget(wdata);
            setupWidget(wgt);
            auto tw = QSharedPointer<TextWidget>::create(wgt);
            widgetCharFormat.setProperty(QTextFormat::UserProperty, QVariant::fromValue(tw));
            m_createdWidgets.insert(tw);
            
            {
                QTextCursor ec(doc);
                ec.setPosition(pos + startPosition);
                auto otherFmt = GetNextCharacterFormat(ec);
                mergeWidgetFormatWithOther(widgetCharFormat, otherFmt);
                ec.setPosition(pos + 1 + startPosition, QTextCursor::KeepAnchor);
                
                ec.removeSelectedText();
                ec.insertText(QString(QChar::ObjectReplacementCharacter), widgetCharFormat);
            }
        }
    }
}


