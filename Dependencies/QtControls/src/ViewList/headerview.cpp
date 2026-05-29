#include "headerview.h"
#include "QAction"
#include "QInputDialog"
#include "QMouseEvent"
#include "QMenu"
#include "QWidgetAction"
#include "QCheckBox"
#include "QPainter"
#include "QApplication"

HeaderView::HeaderView(Qt::Orientation orientation, bool edit, QWidget *parent) : QHeaderView(orientation, parent), currentSectionPos(-1)
{
    contextMenu = new QMenu(this);
    setSectionsMovable(true);//перемещение столбцов
    setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenu->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(slot_showContextMenu(QPoint)));

    //разделитель
    actSeparator = new QAction(this);
    actSeparator->setSeparator(true);
    contextMenu->addAction(actSeparator);
    //итем редактора
    actHeaderEditor = new QAction(tr("Edit header"),this);
    contextMenu->addAction(actHeaderEditor);
    connect(actHeaderEditor, SIGNAL(triggered(bool)), this, SLOT(slot_editHeader()));
    setHeaderEditorVisible(edit);
    setSectionsClickable(true);//кликабельность итемов заголовка

    //обрезка текста, если не помещается
    setTextElideMode(Qt::ElideRight);
}

HeaderView::~HeaderView()
{
}

void HeaderView::createContextMenu(const QVariantList &headerList, const QVector<int> &idlist)
{
    if (!idlist.isEmpty() && idlist.size() != headerList.size())
        return;

    //очищаем
    clearContextActionList();
    //итемы заголовка
    for (int i = 0; i < headerList.size(); ++i)
    {
        QVariant h = headerList[i];
        if(i == 0 && m_firstColumnSelectChekboxesEnabled && h.toString().isEmpty())
            h = tr("Selection checkboxes");
        addContextAction(i, h.toString(), idlist.isEmpty() ? -1 : idlist[i]);
    }
}

void HeaderView::addContextAction(int pos, const QString &nameAct, int idCol)
{
    if (idCol == -1)
        idCol = pos;
    QAction *beforeAct = nullptr;
    QCheckBox *checkBox = new QCheckBox(nameAct, contextMenu);
    if (!this->isSectionHidden(idCol))
        checkBox->setChecked(true);
    QWidgetAction *checkableAction = new QWidgetAction(contextMenu);
    checkableAction->setDefaultWidget(checkBox);    

    checkableAction->setProperty("id", idCol);
    beforeAct = contextMenu->actions().at(pos);

    contextMenu->insertAction(beforeAct, checkableAction);
    connect(checkBox, SIGNAL(toggled(bool)), checkableAction, SIGNAL(toggled(bool)));
    connect(checkableAction, SIGNAL(toggled(bool)), this, SLOT(slot_actionTriggered(bool)));
}

void HeaderView::removeContextAction(int pos)
{
    QWidgetAction *act = qobject_cast<QWidgetAction*>(contextMenu->actions().takeAt(pos));
    if (act)
    {
        contextMenu->removeAction(act);
        QCheckBox *chBox = qobject_cast<QCheckBox *>(act->defaultWidget());
        if (chBox)
            delete chBox;
        delete act;
    }
}

bool HeaderView::clearContextActionList()
{
    int colCount = contextMenu->actions().size() - 2;

    for (int i = colCount - 1; i >= 0; --i)
    {
        removeContextAction(i);
    }
    return true;
}

void HeaderView::setHeaderEditorVisible(bool bV)
{
    bEditable = bV;
    actSeparator->setVisible(bV);
    actHeaderEditor->setVisible(bV);
}

void HeaderView::updateContextActionsState()
{
    auto actList = contextMenu->actions();
    for (int i = 0; i < actList.size() - 2; ++i)
    {
        QWidgetAction *act = qobject_cast<QWidgetAction*>(actList[i]);
        if (act)
        {
            QCheckBox *chBox = qobject_cast<QCheckBox *>(act->defaultWidget());
            if (chBox)
            {
                if (this->isSectionHidden(i))
                    chBox->setChecked(false);
                else
                    chBox->setChecked(true);
            }
        }
    }
}

void HeaderView::setFirstColumnSelectChekboxesEnabled(bool enabled)
{
    m_firstColumnSelectChekboxesEnabled = enabled;
    if(enabled && contextMenu->actions().size() && model()->index(0, 0).data().toString().isEmpty())
        ((QCheckBox*)((QWidgetAction*)contextMenu->actions().first())->defaultWidget())->setText(tr("Selection checkboxes"));
    this->update();
}

void HeaderView::setFirstColumnCheckboxChecked(Qt::CheckState checked)
{
    m_isChecked = checked;
    this->update();
}

void HeaderView::setEnableCustomSeparators(bool enable)
{
    m_customSeparatorsEnabled = enable;
    update();
}

bool HeaderView::isCustomSeparatorsEnabled()
{
    return m_customSeparatorsEnabled;
}

void HeaderView::setCustomSeparatorsColor(QColor color)
{
    m_customSeparatorsColor = color;
    update();
}

QColor HeaderView::getCustomSeparatorsColor()
{
    return m_customSeparatorsColor;
}

void HeaderView::setCustomSeparatorsOpacity(qreal opacity)
{
    m_customSeparatorsOpacity = opacity;
    update();
}

qreal HeaderView::getCustomSeparatorsOpacity()
{
    return m_customSeparatorsOpacity;
}

void HeaderView::setCustomSeparatorsWidth(qreal width)
{
    m_customSeparatorsWidth = width;
    update();
}

qreal HeaderView::getCustomSeparatorsWidth()
{
    return m_customSeparatorsWidth;
}

void HeaderView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton || e->button() == Qt::LeftButton)
    {
        setCurrentSectionPos(logicalIndexAt(e->pos()));
    }
    
    auto cursorPos = e->pos();
    if(m_firstColumnSelectChekboxesEnabled && visualIndex(logicalIndexAt(cursorPos))==0)
    {
        QStyleOptionViewItem option;
        initCheckboxStyleOption(&option);
        
        if(cursorPos.x() < option.rect.right() + 5)
        {
            if(m_isChecked == Qt::Checked)
                m_isChecked = Qt::Unchecked;
            else
                m_isChecked = Qt::Checked;
            
            emit sig_selectAll(m_isChecked == Qt::Checked);
            repaint();
        }
        else
            QHeaderView::mousePressEvent(e);
    }
    else
        QHeaderView::mousePressEvent(e);
}

void HeaderView::mouseDoubleClickEvent(QMouseEvent* e)
{
    auto cursorPos = e->pos();
    if(m_firstColumnSelectChekboxesEnabled && visualIndex(logicalIndexAt(cursorPos))==0)
    {
        QStyleOptionViewItem option;
        initCheckboxStyleOption(&option);
        
        if(cursorPos.x() < option.rect.right() + 5)
        {
            if(m_isChecked == Qt::Checked)
                m_isChecked = Qt::Unchecked;
            else
                m_isChecked = Qt::Checked;
            
            emit sig_selectAll(m_isChecked == Qt::Checked);
            repaint();
        }
        else
            QHeaderView::mouseDoubleClickEvent(e);
    }
    else
        QHeaderView::mouseDoubleClickEvent(e);
}

void HeaderView::mouseMoveEvent(QMouseEvent* e)
{
    if(m_firstColumnSelectChekboxesEnabled)
    {
        QStyleOptionViewItem option;
        initCheckboxStyleOption(&option);
        
        auto oldVal = m_cursorWasOnCheckbox;
        if(e->pos().x() < option.rect.right() + 5)
            m_cursorWasOnCheckbox = true;
        else
            m_cursorWasOnCheckbox = false;
        
        if(oldVal != m_cursorWasOnCheckbox)
            repaint();
    }
    
    QHeaderView::mouseMoveEvent(e);
}

void HeaderView::paintSection(QPainter* painter, const QRect& originalRect, int logicalIndex) const
{
    auto visualColumn = visualIndex(logicalIndex);
    int lastVisualColumn = -1;
    int firstVisualColumn = -1;
    for(int i = 0; i < this->count(); i++)
    {
        if(!this->isSectionHidden(i))
        {
            lastVisualColumn = qMax(lastVisualColumn, visualIndex(i));
            firstVisualColumn = firstVisualColumn >= 0 ? qMin(firstVisualColumn, visualIndex(i)) : visualIndex(i);
        }
    }
    if (m_firstColumnSelectChekboxesEnabled && visualColumn == 0)
    {
        auto text = this->model()->headerData(logicalIndex, this->orientation(),
                                              Qt::DisplayRole).toString();
        
        QStyleOptionViewItem option;
        option.rect = originalRect;
        initCheckboxStyleOption(&option);
        
        auto leftRect = originalRect;
        
        if(text.isEmpty() == false)
            leftRect.setRight(option.rect.right() + 5);
        else
            leftRect.setRight(leftRect.right() + 1);
        auto cursorPos = this->mapFromGlobal(cursor().pos());
        if(leftRect.contains(cursorPos) && cursorPos.x() < leftRect.right())
            option.state |= QStyle::State_MouseOver;
        
        auto rightRect = originalRect;
        rightRect.setLeft(option.rect.right() + 5);
        painter->save();
        
        //в блоке ниже копия приватной реализации настройки и рисования CE_Header
        //QHeaderView::paintSection(painter, rect, logicalIndex);
        //(с заменой получения приватных вещей на публичные способы)
        //потому что иначе не заменить установку State_MouseOver
        //там она ставилась по приватному d->hover, а тут надо по rect.contains(cursor)
        if(text.isEmpty() == false)
        {
            auto rect = rightRect;
            if (!rect.isValid())
                return;
            // get the state of the section
            QStyleOptionHeader opt;
            initStyleOption(&opt);
            QStyle::State state = QStyle::State_None;
            if (isEnabled())
                state |= QStyle::State_Enabled;
            if (window()->isActiveWindow())
                state |= QStyle::State_Active;
            if (this->sectionsClickable()) {
                if (rect.contains(cursorPos))
                    state |= QStyle::State_MouseOver;
                if (rect.contains(cursorPos) &&
                    QApplication::mouseButtons().testFlag(Qt::MouseButton::LeftButton) ||
                    QApplication::mouseButtons().testFlag(Qt::MouseButton::RightButton))
                    state |= QStyle::State_Sunken;
                //else if (d->highlightSelected) {
                //    if (d->sectionIntersectsSelection(logicalIndex))
                //        state |= QStyle::State_On;
                //    if (d->isSectionSelected(logicalIndex))
                //        state |= QStyle::State_Sunken;
                //}
            }
            if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex)
                opt.sortIndicator = (sortIndicatorOrder() == Qt::AscendingOrder)
                                        ? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;
            // setup the style options structure
            QVariant textAlignment = this->model()->headerData(logicalIndex, this->orientation(),
                                                          Qt::TextAlignmentRole);
            opt.rect = rect;
            opt.section = logicalIndex;
            opt.state |= state;
            opt.textAlignment = Qt::Alignment(textAlignment.isValid()
                                                  ? Qt::Alignment(textAlignment.toInt())
                                                  : this->defaultAlignment());
            opt.iconAlignment = Qt::AlignVCenter;
            opt.text = text;
            int margin = 2 * style()->pixelMetric(QStyle::PM_HeaderMargin, nullptr, this);
            const Qt::Alignment headerArrowAlignment = static_cast<Qt::Alignment>(style()->styleHint(QStyle::SH_Header_ArrowAlignment, nullptr, this));
            const bool isHeaderArrowOnTheSide = headerArrowAlignment & Qt::AlignVCenter;
            if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex && isHeaderArrowOnTheSide)
                margin += style()->pixelMetric(QStyle::PM_HeaderMarkSize, nullptr, this);
            const QVariant variant = this->model()->headerData(logicalIndex, this->orientation(),
                                                          Qt::DecorationRole);
            opt.icon = qvariant_cast<QIcon>(variant);
            if (opt.icon.isNull())
                opt.icon = qvariant_cast<QPixmap>(variant);
            if (!opt.icon.isNull()) // see CT_HeaderSection
                margin += style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this) +
                          style()->pixelMetric(QStyle::PM_HeaderMargin, nullptr, this);
            if (this->textElideMode() != Qt::ElideNone) {
                const QRect textRect = style()->subElementRect(QStyle::SE_HeaderLabel, &opt, this);
                opt.text = opt.fontMetrics.elidedText(opt.text, this->textElideMode(), textRect.width() - margin);
            }
            QVariant foregroundBrush = this->model()->headerData(logicalIndex, this->orientation(),
                                                            Qt::ForegroundRole);
            if (foregroundBrush.canConvert<QBrush>())
                opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
            QPointF oldBO = painter->brushOrigin();
            QVariant backgroundBrush = this->model()->headerData(logicalIndex, this->orientation(),
                                                            Qt::BackgroundRole);
            if (backgroundBrush.canConvert<QBrush>()) {
                opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
                opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
                painter->setBrushOrigin(opt.rect.topLeft());
            }
            // the section position
            int visual = visualIndex(logicalIndex);
            Q_ASSERT(visual != -1);
            bool first = false;
            bool last = lastVisualColumn != -1 ? (lastVisualColumn == visualColumn) : false;
            if (first && last)
                opt.position = QStyleOptionHeader::OnlyOneSection;
            else if (first)
                opt.position =  (orientation() == Qt::Horizontal && isRightToLeft()) ? QStyleOptionHeader::End : QStyleOptionHeader::Beginning;
            else if (last)
                opt.position = (orientation() == Qt::Horizontal && isRightToLeft()) ? QStyleOptionHeader::Beginning : QStyleOptionHeader::End;
            else
                opt.position = QStyleOptionHeader::Middle;
            opt.orientation = this->orientation();
            // the selected position
            
            QSet<int> selectedColumns;
            for(auto index : this->selectedIndexes())
                selectedColumns.insert(index.column());
            bool previousSelected = selectedColumns.contains(this->logicalIndex(visual - 1));
            bool nextSelected =  selectedColumns.contains(this->logicalIndex(visual + 1));
            if (previousSelected && nextSelected)
                opt.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
            else if (previousSelected)
                opt.selectedPosition = QStyleOptionHeader::PreviousIsSelected;
            else if (nextSelected)
                opt.selectedPosition = QStyleOptionHeader::NextIsSelected;
            else
                opt.selectedPosition = QStyleOptionHeader::NotAdjacent;
            // draw the section
            style()->drawControl(QStyle::CE_Header, &opt, painter, this);
            painter->setBrushOrigin(oldBO);
        }
        painter->restore();
        
        //дополнительное рисование стилизованного фона под галочкой аналогичено через CE_Header с некоторыми корректировками:
        painter->save();
        {
            auto rect = leftRect;
            rect.setRight(rect.right() - 1);
            if (!rect.isValid())
                return;
            // get the state of the section
            QStyleOptionHeader opt;
            initStyleOption(&opt);
            QStyle::State state = QStyle::State_None;
            if (isEnabled())
                state |= QStyle::State_Enabled;
            if (window()->isActiveWindow())
                state |= QStyle::State_Active;
            if (this->sectionsClickable()) {
                //if (leftRect.contains(cursorPos) && cursorPos.x() < leftRect.right())
                //    state |= QStyle::State_MouseOver;
                if (leftRect.contains(cursorPos) && cursorPos.x() < leftRect.right() &&
                        QApplication::mouseButtons().testFlag(Qt::MouseButton::LeftButton) ||
                    QApplication::mouseButtons().testFlag(Qt::MouseButton::RightButton))
                    state |= QStyle::State_Sunken;
            }
            // setup the style options structure
            QVariant textAlignment = this->model()->headerData(logicalIndex, this->orientation(),
                                                               Qt::TextAlignmentRole);
            opt.rect = rect;
            opt.section = logicalIndex;
            opt.state |= state;
            opt.textAlignment = Qt::Alignment(textAlignment.isValid()
                                                  ? Qt::Alignment(textAlignment.toInt())
                                                  : this->defaultAlignment());
            opt.iconAlignment = Qt::AlignVCenter;
            opt.text = QString();
            int margin = 2 * style()->pixelMetric(QStyle::PM_HeaderMargin, nullptr, this);
            const Qt::Alignment headerArrowAlignment = static_cast<Qt::Alignment>(style()->styleHint(QStyle::SH_Header_ArrowAlignment, nullptr, this));
            const bool isHeaderArrowOnTheSide = headerArrowAlignment & Qt::AlignVCenter;
            if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex && isHeaderArrowOnTheSide)
                margin += style()->pixelMetric(QStyle::PM_HeaderMarkSize, nullptr, this);
            
            QVariant foregroundBrush = this->model()->headerData(logicalIndex, this->orientation(),
                                                                 Qt::ForegroundRole);
            if (foregroundBrush.canConvert<QBrush>())
                opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
            QPointF oldBO = painter->brushOrigin();
            QVariant backgroundBrush = this->model()->headerData(logicalIndex, this->orientation(),
                                                                 Qt::BackgroundRole);
            if (backgroundBrush.canConvert<QBrush>()) {
                opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
                opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
                painter->setBrushOrigin(opt.rect.topLeft());
            }
            // the section position
            int visual = visualIndex(logicalIndex);
            Q_ASSERT(visual != -1);
            bool first = true;
            bool last = false;
            if (first && last)
                opt.position = QStyleOptionHeader::OnlyOneSection;
            else if (first)
                opt.position =  (orientation() == Qt::Horizontal && isRightToLeft()) ? QStyleOptionHeader::End : QStyleOptionHeader::Beginning;
            else if (last)
                opt.position = (orientation() == Qt::Horizontal && isRightToLeft()) ? QStyleOptionHeader::Beginning : QStyleOptionHeader::End;
            else
                opt.position = QStyleOptionHeader::Middle;
            opt.orientation = this->orientation();
            // the selected position
            
            QSet<int> selectedColumns;
            for(auto index : this->selectedIndexes())
                selectedColumns.insert(index.column());
            bool previousSelected = selectedColumns.contains(this->logicalIndex(visual - 1));
            bool nextSelected =  selectedColumns.contains(this->logicalIndex(visual + 1));
            if (previousSelected && nextSelected)
                opt.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
            else if (previousSelected)
                opt.selectedPosition = QStyleOptionHeader::PreviousIsSelected;
            else if (nextSelected)
                opt.selectedPosition = QStyleOptionHeader::NextIsSelected;
            else
                opt.selectedPosition = QStyleOptionHeader::NotAdjacent;
            // draw the section
            style()->drawControl(QStyle::CE_Header, &opt, painter, this);
            painter->setBrushOrigin(oldBO);
        }
        painter->restore();
        
        this->style()->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &option, painter, this->parentWidget());
    }
    else
        QHeaderView::paintSection(painter, originalRect, logicalIndex);
    
    
    if(m_customSeparatorsEnabled && m_customSeparatorsColor.alpha() > 0)
    {
        bool first = firstVisualColumn != -1 ? (firstVisualColumn == visualColumn) : false;
        if(!first)
        {
            painter->save();
            auto rect = originalRect;
            rect.setLeft(rect.left() - 10);
            painter->setClipRect(rect);
            
            QStyleOptionHeader opt;
            initStyleOption(&opt);
            
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->setPen(QPen(m_customSeparatorsColor, m_customSeparatorsWidth, Qt::PenStyle::SolidLine,
                                 Qt::PenCapStyle::RoundCap));
            painter->setOpacity(m_customSeparatorsOpacity);
            
            
            auto height = opt.fontMetrics.boundingRect('Q').height();//opt.fontMetrics.capHeight();
            
            auto p1 = originalRect.topLeft();
            auto p2 = originalRect.bottomLeft();
            
            auto allHeight = p2.y() - p1.y();
            if(allHeight > height)
            {
                auto diff = allHeight - height;
                p1.setY(p1.y() + diff/2 - 3);
                p2.setY(p2.y() - diff/2 - 3);
            }
            
            p1.setX(p1.x());
            p2.setX(p2.x());
            painter->drawLine(p1, p2);
            
            painter->restore();
        }
    }
}

void HeaderView::slot_actionTriggered(bool checked)
{
    QWidgetAction *wAct = qobject_cast<QWidgetAction*>(sender());
    if (wAct)
    {
        if (!checked && count() - hiddenSectionCount() == 1)//запрет на скрытие последнего столбца
        {
            QCheckBox *chBox = qobject_cast<QCheckBox*>(wAct->defaultWidget());
            if (chBox)
                chBox->setChecked(true);
            return;
        }
        int index = wAct->property("id").toInt();
        this->setSectionHidden(index, !checked);// скрыть/показать столбец
        emit sig_sectionHidden(index, !checked);// сообщить
    }
}

void HeaderView::slot_editHeader()
{    
    if (currentSectionPos < 0)
        return;

    bool ok;
    QString oldText = model()->headerData(currentSectionPos, Qt::Horizontal).toString();
    QString text = QInputDialog::getText(0, tr("Edit header"), tr("New header:"), QLineEdit::Normal, oldText, &ok, Qt::Drawer);
    if (ok && !text.isEmpty())
    {
        emit sig_editHeader(currentSectionPos, text);
        QWidgetAction *wAct = qobject_cast<QWidgetAction *>(contextMenu->actions()[currentSectionPos]);
        QCheckBox *chBox = qobject_cast<QCheckBox*>(wAct->defaultWidget());
        chBox->setText(text);
    }
}

void HeaderView::slot_showContextMenu(const QPoint &pos)
{
    contextMenu->exec(mapToGlobal(pos));
}

void HeaderView::setCurrentSectionPos(int pos)
{
    currentSectionPos = pos;
}

void HeaderView::initCheckboxStyleOption(QStyleOptionViewItem* option) const
{
    if (m_isChecked == Qt::Checked)
        option->state = QStyle::State_On;
    else if(m_isChecked == Qt::PartiallyChecked)
        option->state = QStyle::State_NoChange;
    else
        option->state = QStyle::State_Off;
    
    option->checkState = m_isChecked;
    option->state |= QStyle::State_Enabled;
    option->features |= QStyleOptionViewItem::HasCheckIndicator;
    option->rect = this->style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, option, this->parentWidget());
    option->rect.moveTop(option->rect.top() - 3);
}

//**************Стили*****************//

HeaderProxyStyle::HeaderProxyStyle(Qt::Alignment align, QStyle *style) : QProxyStyle (style), iconAlignment(align)
{

}

void HeaderProxyStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == CE_HeaderLabel)
    {
        QStyleOptionHeader *poStyleOptionHeader = (QStyleOptionHeader *)option;

        //если есть иконка
        if(poStyleOptionHeader && !poStyleOptionHeader->icon.isNull())
        {
            poStyleOptionHeader->iconAlignment = iconAlignment;//выравниваем
            QProxyStyle::drawControl(element, poStyleOptionHeader, painter, widget);
            return;
        }
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}
