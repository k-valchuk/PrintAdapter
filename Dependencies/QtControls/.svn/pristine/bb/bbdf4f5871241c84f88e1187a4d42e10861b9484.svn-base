#include "TextWidgetObjectHandler.h"

#include "CustomTextEdit.h"
#include "QWidget"
#include "QScrollBar"
#include "QPainter"
#include "QPainterPath"
#include "QWindow"
#include "QTextBlock"

#include "QDebug"

TextWidgetObjectHandler::TextWidgetObjectHandler(CustomTextEdit* textEdit)
    : m_edit(textEdit)
{
    
}

QSizeF TextWidgetObjectHandler::intrinsicSize(QTextDocument* doc, int posInDocument, const QTextFormat& format)
{
    if(!m_edit)
        return QSizeF(0, 0);
    if(!format.hasProperty(QTextFormat::UserProperty))
        return QSizeF(0, 0);
    
    auto tw = format.property(QTextFormat::UserProperty).value<QSharedPointer<TextWidget>>();
    if(!tw->Widget)
    {
        tw->Widget = m_edit->createWidget(tw->Data);
        m_edit->setupWidget(tw->Widget);
    }
    QWidget* wgt = tw->Widget;
    
    if(!wgt)
        return QSizeF(0, 0);
    
    auto size = wgt->sizeHint();
    return size.boundedTo(QSize(m_edit->document()->textWidth()-25, size.height()))
               .expandedTo(wgt->minimumSizeHint());
    //return wgt->sizeHint().expandedTo(QSize(m_edit->document()->textWidth(), 1));
}

void TextWidgetObjectHandler::drawObject(QPainter* painter, const QRectF& wholeRect, QTextDocument* doc, int posInDocument, const QTextFormat& format)
{
    if(!m_edit)
        return;
    if(!format.hasProperty(QTextFormat::UserProperty))
        return;
    
    auto tw = format.property(QTextFormat::UserProperty).value<QSharedPointer<TextWidget>>();
    if(!tw->Widget)
    {
        tw->Widget = m_edit->createWidget(tw->Data);
        m_edit->setupWidget(tw->Widget);
    }
    QWidget* wgt = tw->Widget;
    
    if(!wgt)
        return;
    
    auto size = wgt->sizeHint();
    size = size.boundedTo(QSize(m_edit->document()->textWidth()-25, size.height()))
               .expandedTo(wgt->minimumSizeHint());
    auto rect = wholeRect;
    rect.setSize(size);
    
    if(!m_edit->widgets.contains(wgt))
    {
        QObject::connect(wgt->topLevelWidget()->windowHandle(), &QWindow::focusObjectChanged, this, [this](QObject *object){
            if(auto wgt = qobject_cast<QWidget*>(object))
                m_edit->SetFocusOnTextWidget(wgt);
        });
    }
    bool firstDraw = !m_edit->widgets.contains(wgt);
    m_edit->widgets[wgt] = rect;
    
    painter->save();
    
    if(!m_pixmapCache.contains(wgt))
    {
        int xoffset = 0, yoffset = 0;
        if(auto hscrollbar = m_edit->horizontalScrollBar())
            xoffset = -hscrollbar->value();
        if(auto vscrollbar = m_edit->verticalScrollBar())
            yoffset = -vscrollbar->value();
        QPoint scrollOffset(xoffset, yoffset);
        auto r = rect.toRect();
        r.moveTopLeft(r.topLeft() + scrollOffset);
        auto globalTopLeft = m_edit->mapToGlobal(r.topLeft());
        auto newTopLeft = wgt->parentWidget() ? wgt->parentWidget()->mapFromGlobal(globalTopLeft) : globalTopLeft;
        r.moveTopLeft(newTopLeft);
        
        auto geom = wgt->geometry();
        if(r != geom)
        {
            wgt->setAttribute(Qt::WA_DontShowOnScreen, false);
            wgt->setGeometry(r);
            //wgt->setFixedSize(size);
            wgt->setAttribute(Qt::WA_DontShowOnScreen, true);
        }
        
        //if(firstDraw)
        //    wgt->setFixedSize(size);
        
        m_paintWidget = true;
        
        auto grabRect = rect.toRect();
        grabRect.moveTopLeft(QPoint(0, 0));
        auto pixmap = wgt->grab(grabRect);
        painter->drawPixmap(rect.toRect(), pixmap);
        m_pixmapCache[wgt] = std::move(pixmap);
        
        m_paintWidget = false;
    }
    else
    {
        auto pixmap = m_pixmapCache[wgt];
        painter->drawPixmap(rect.toRect(), pixmap);
    }
    
    //rounded border if needed
    if(tw->BorderRadius > 0)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QPainterPath outer;
        outer.addRect(rect);
        QPainterPath inner;
        inner.addRoundedRect(rect, tw->BorderRadius, tw->BorderRadius);
        auto resulted = outer.subtracted(inner);
        painter->setCompositionMode(QPainter::CompositionMode_Source);
        if(doc)
        {
            QTextCursor textCur(doc);
            textCur.setPosition(posInDocument);
            
            auto charFormat = CustomTextEdit::GetNextCharacterFormat(textCur);
            auto charBg = charFormat.background();
            auto color = charBg;
            if(!charBg.isOpaque())
                color = textCur.block().blockFormat().background();
            if(!color.isOpaque())
                color = m_edit->palette().color(m_edit->backgroundRole());
            painter->fillPath(resulted, color);
        }
        else
        {
            painter->setBackgroundMode(Qt::BGMode::TransparentMode);
            painter->fillPath(resulted, Qt::transparent);
        }
    }
    
    painter->restore();
}

bool TextWidgetObjectHandler::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::ChildAdded)
    {
        auto cev = (QChildEvent*)event;
        if(auto ch = cev->child())
            ch->installEventFilter(this);
    }
    if(event->type() == QEvent::ChildRemoved)
    {
        auto cev = (QChildEvent*)event;
        if(auto ch = cev->child())
            ch->removeEventFilter(this);
    }
    
    if(event->type() == QEvent::Resize && m_edit->widgets.keys().contains((QWidget*)watched))
    {
        //иначе сам не обновляется размер объекта, просто update не помогает нужна перестройка текст лайаута
        auto doc = m_edit->document();
        auto width = doc->textWidth();
        doc->setTextWidth(width);
    }
    
    if(event->type() == QEvent::Paint && !m_paintWidget)
    {
        bool filtered = true;
        if(auto wgt = qobject_cast<QWidget*>(watched))
        {
            auto parentWgt = wgt;
            do
            {
                m_pixmapCache.remove(parentWgt);
                if(parentWgt->windowFlags().testFlag(Qt::Popup)/* && !m_edit->widgets.keys().contains(parentWgt)*/)
                    filtered = false;
                parentWgt = parentWgt->parentWidget();
            }
            while(parentWgt);
        }
        
        if(filtered)
        {
            m_edit->update();
        }
        
        return filtered;
    }
    else if(event->type() == QEvent::Paint && m_paintWidget)
    {
        return false;
    }
    return false;
}
