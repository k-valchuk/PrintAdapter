/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "FlowLayout.h"

FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing, int additionToHorSizeHint)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing), m_additionToHorSizeHint(additionToHorSizeHint)
{
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing, int additionToHorSizeHint)
    : m_hSpace(hSpacing), m_vSpace(vSpacing), m_additionToHorSizeHint(additionToHorSizeHint)
{
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

void FlowLayout::addItem(QLayoutItem *item)
{
    if(m_insert)
        itemList.insert(m_insertAt, item);
    else
        itemList.append(item);
}

void FlowLayout::insertWidget(QWidget* widget, int at)
{
    m_insert = true;
    m_insertAt = at;
    addWidget(widget);
    m_insert = false;
}

void FlowLayout::insertWidgetAfter(QWidget* widget, QWidget* after)
{
    m_insert = true;
    m_insertAt = this->count();
    for(int i = 0; i < this->count(); i++)
    {
        auto item = this->itemAt(i);
        if(item && item->widget() == after)
        {
            m_insertAt = i + 1;
            break;
        }
    }
    addWidget(widget);
    m_insert = false;
}

void FlowLayout::insertWidgetBefore(QWidget* widget, QWidget* before)
{
    m_insert = true;
    m_insertAt = 0;
    for(int i = 0; i < this->count(); i++)
    {
        auto item = this->itemAt(i);
        if(item && item->widget() == before)
        {
            m_insertAt = i;
            break;
        }
    }
    addWidget(widget);
    m_insert = false;
}

void FlowLayout::insertWidgetBeforeLast(QWidget* widget)
{
    m_insert = true;
    m_insertAt = count() - 1;
    addWidget(widget);
    m_insert = false;
}

int FlowLayout::horizontalSpacing() const
{
    if (m_hSpace >= 0) {
        return m_hSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

int FlowLayout::verticalSpacing() const
{
    if (m_vSpace >= 0) {
        return m_vSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}

int FlowLayout::count() const
{
    return itemList.size();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    return itemList.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    return nullptr;
}

void FlowLayout::SetElementsAlignment(ItemsAlignment alignment)
{
    if(m_alignment == alignment)
        return;
    m_alignment = alignment;
    this->update();
}

void FlowLayout::SetConsiderInvisibleWidgets(bool consider)
{
    if(m_considerInvisibleWidgets == consider)
        return;
    m_considerInvisibleWidgets = consider;
    this->update();
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return { };
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    int height = doLayout(QRect(0, 0, this->geometry().width(), 0), true);
    return height;
}

int FlowLayout::heightForKnownWidth(int width)
{
    int height = doLayout(QRect(0, 0, width, 0), true);
    return height;
}

void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    QSize size;
    for (const QLayoutItem *item : qAsConst(itemList))
        size = size.expandedTo(item->minimumSize());
    
    const QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    size += QSize(m_additionToHorSizeHint, 0);
    
    return size;
}

int FlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;
    
    QList<QList<QPair<QLayoutItem*, QRect>>> rowToItemRect;
    rowToItemRect.append(QList<QPair<QLayoutItem*, QRect>>{});
    int row = 0;
    int sameWidth = -1;
    for (QLayoutItem *item : qAsConst(itemList)) {
        QWidget *wid = item->widget();
        
        if(!wid || (!wid->isVisible() && !m_considerInvisibleWidgets))
            continue;
        
        int spaceX = horizontalSpacing();
        if (spaceX == -1)
            spaceX = wid->style()->layoutSpacing(
                QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        int spaceY = verticalSpacing();
        if (spaceY == -1)
            spaceY = wid->style()->layoutSpacing(
                QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
        
        auto widgetWidth = item->sizeHint().width();
        if(wid && wid->sizePolicy().horizontalPolicy() == QSizePolicy::Expanding)
        {
            widgetWidth = effectiveRect.right() - x;
            //wid->resize(widgetWidth, wid->height());
        }
        if(m_alignment == ItemsAlignment::SameSizeWithLeft)
        {
            if(sameWidth < 0)
                sameWidth = widgetWidth;
            else
                widgetWidth = sameWidth;
        }
        int nextX = x + widgetWidth + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            row++;
            rowToItemRect.append(QList<QPair<QLayoutItem*, QRect>>{});
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + widgetWidth + spaceX;
            lineHeight = 0;
        }
        
        if (!testOnly)
        {
            rowToItemRect[row].append(QPair<QLayoutItem*, QRect>{item, QRect(QPoint(x, y), item->sizeHint())});
            //item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        }
        
        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    
    if(!testOnly)
    {
        QMap<int, int> spaces; //для SameSpaceWithLeft
        
        int rownum = 0;
        for(const auto& row : rowToItemRect)
        {
            if(row.isEmpty()) continue;
            
            auto freeSpace = qMax(effectiveRect.right() - row.last().second.right(), 0);
            for(int i = 0; i < row.size(); i++)
            {
                auto itemRect = row[i].second;
                
                switch(m_alignment)
                {
                case ItemsAlignment::Left:
                    break;
                case ItemsAlignment::Center:
                {
                    itemRect.translate(freeSpace/2, 0);
                    break;
                }
                case ItemsAlignment::Width:
                {
                    if(i > 0)
                        itemRect.translate((freeSpace/(row.size() - 1))*i, 0);
                    break;
                }
                case ItemsAlignment::WidthWithMargins:
                {
                    itemRect.translate((freeSpace/(row.size() + 1))*(i + 1), 0);
                    break;
                }
                case ItemsAlignment::Right:
                {
                    itemRect.translate(freeSpace, 0);
                    break;
                }
                case ItemsAlignment::SameSizeWithLeft:
                {
                    if(!spaces.contains(i))
                        spaces[i] = (freeSpace/(row.size() + 1))*(i + 1);
                    
                    itemRect.translate(spaces[i], 0);
                    break;
                }
                }
                
                auto item = row[i].first;
                item->setGeometry(itemRect);
            }
            
            rownum++;
        }
    }
    
    return y + lineHeight - rect.y() + bottom;
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    } else {
        return static_cast<QLayout *>(parent)->spacing();
    }
}

