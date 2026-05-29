#include "TreeGroupingStyleItemDelegate.h"

#include "qapplication.h"
#include "qpainter.h"
#include "TCEditWidget.h"
#include "QPushButton"
#include "treeitem.h"
#include "QDateTimeEdit"
#include "hotKeyEditor.h"
#include "QTimeEdit"
#include "TCOperations.h"
#include "ItemComboBox.h"
#include "QTimer"
#include "treeview.h"
#include "QSizeGrip"
#include "QTreeView"

TreeGroupingStyleItemDelegate::TreeGroupingStyleItemDelegate(bool drawLines, QWidget* parent)
    : StyleItemDelegate(parent),
    m_drawLines(drawLines)
{
    
}

void TreeGroupingStyleItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    StyleItemDelegate::updateEditorGeometry(editor, option, index);
    if(index.siblingAtColumn(0).data(TreeItem::UserRoles::RowSpannedRole).toBool())   
    {
        auto rect = editor->geometry();
        
        if(auto view = qobject_cast<const QAbstractItemView*>(option.widget))
        {
            if(m_firstColumnSelectChekboxesEnabled)
                rect.setLeft(qMax(view->viewport()->rect().left(), view->visualRect(index.siblingAtColumn(0)).right()));
            else
                rect.setLeft(view->viewport()->rect().left());
            rect.setRight(view->viewport()->rect().right());
        }
        
        editor->setGeometry(rect);
        
        DelegateEditor wEditor = getEditor(index);
        if(wEditor.data.canConvert<TextEditorParams>())
            editor->setFixedWidth(rect.width());
    }
}

void TreeGroupingStyleItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& opt, const QModelIndex& originalIndex) const
{
    auto index = originalIndex;
    auto option = opt;
    
    if(index.siblingAtColumn(0).data(TreeItem::UserRoles::RowSpannedRole).toBool())
    {
        bool selectionCheckboxes = m_firstColumnSelectChekboxesEnabled;
        
        if(!(selectionCheckboxes && originalIndex.column() == 0))
        {
            auto dataColumn = index.siblingAtColumn(0).data(TreeItem::UserRoles::RowSpannedDataColumn).toInt();
            index = originalIndex.siblingAtColumn(dataColumn);
            
            if(auto treeView = qobject_cast<const QTreeView*>(option.widget))
            {
                auto testSection = [treeView](int i)
                {
                    auto x1 = treeView->header()->sectionViewportPosition(i);
                    auto x2 = x1 + treeView->header()->sectionSize(i);
                    
                    auto left = treeView->header()->viewport()->rect().left();
                    auto right = treeView->header()->viewport()->rect().right();
                    
                    if(x2 <= left)
                        return false;
                    if(x1 >= right)
                        return false;
                    
                    return true;
                };
                
                int lastVisibleCol = -1;
                for(int i = treeView->model()->columnCount(QModelIndex())-1; i >= 0 ; i--)
                {
                    if(treeView->isColumnHidden(i) || !testSection(i))
                        continue;
                    
                    lastVisibleCol = i;
                    break;
                }
                
                if(originalIndex.column() != lastVisibleCol || lastVisibleCol == -1)
                    return;
                
                option.rect = treeView->visualRect(index.siblingAtColumn(lastVisibleCol));
                if(selectionCheckboxes)
                    option.rect.setLeft(qMax(treeView->viewport()->rect().left(), treeView->visualRect(index.siblingAtColumn(0)).right()));
                else
                    option.rect.setLeft(treeView->viewport()->rect().left());
                option.rect.setRight(treeView->viewport()->rect().right());
            }
        }
    }
    
    QStyleOptionViewItem newOption(option);
    initStyleOption(&newOption, index);
    if(m_invisibleSelection)
        newOption.state.setFlag(QStyle::State_Selected, false);
    if(newOption.state & QStyle::State_Selected) //если выделен
    {
        QVariant var = index.data(TreeItem::HighlightColorRole);
        if (var.isValid())
            newOption.palette.setColor(QPalette::HighlightedText, var.value<QColor>());
        var = index.data(TreeItem::HighlightFontRole);
        if (var.isValid())
            newOption.font = var.value<QFont>();        
    }
    
    QVariant varColor = index.data(Qt::BackgroundColorRole);
    if (varColor.isValid())
    {
        if (option.state & QStyle::State_MouseOver)
        {//не изменяем фон у подсвеченной строки при наведении курсором мыши
            newOption.state &= ~QStyle::State_MouseOver;
        }
        newOption.backgroundBrush = QBrush(varColor.value<QColor>());
    }
    
    QVariant groupColor = index.siblingAtColumn(0).data(TreeItem::GroupBackgroundColorRole);
    bool groupColorOnTop = index.siblingAtColumn(0).data(TreeItem::GroupBgOnTopRole).toBool();
    QVariant groupTextColor = index.siblingAtColumn(0).data(TreeItem::GroupTextColorRole);
    if(groupColor.isValid() || groupTextColor.isValid())
    {
        if(auto treeView = qobject_cast<const QTreeView*>(option.widget))
        {
            auto visualColumn = treeView->header()->visualIndex(index.column());
            auto visualTreeColumn = treeView->header()->visualIndex(treeView->treePosition());
            
            if(visualColumn >= visualTreeColumn)
            {
                if (option.state & QStyle::State_MouseOver)
                {//не изменяем фон у подсвеченной строки при наведении курсором мыши
                    newOption.state &= ~QStyle::State_MouseOver;
                }
                if(groupColor.isValid() && (groupColorOnTop || !varColor.isValid()))
                    newOption.backgroundBrush = QBrush(groupColor.value<QColor>());
                if(groupTextColor.isValid())
                    newOption.palette.setBrush(QPalette::Text, groupTextColor.value<QColor>());
            }
        }
    }
    
    //выравнивание иконки из DecorationRole
    QVariant varDecorAlignment = index.data(TreeItem::DecorationAlignmentRole);
    if (varDecorAlignment.canConvert(QMetaType::QVariantList))
    {//список из двух значений Position и Alignment
        auto varList = varDecorAlignment.toList();
        if (varList.size() == 2)
        {
            newOption.decorationPosition = (QStyleOptionViewItem::Position)varList.first().toInt();
            newOption.decorationAlignment = (Qt::Alignment)varList.last().toInt();
        }
    }
    
    //применить заданные опции стиля к стандартной отрисовки
    if (!drawDefaultPaint(painter, newOption, index))
    {
        ModifyOption(index, newOption);
        const QWidget *widget = option.widget;
        QStyle *style = widget ? widget->style() : QApplication::style();
        if(index.data().canConvert<RichTextData>())
            newOption.text = QString();
        style->drawControl(QStyle::CE_ItemViewItem, &newOption, painter, widget);
        PaintAfter(painter, newOption, index);
        //QStyledItemDelegate::paint(painter, newOption, index);
    }
    
    QRect lineRect = newOption.rect;
    
    //отрисовка масштабируемой картинки
    QVariant varImg = index.data(TreeItem::DecorationPixmapRole);
    if (varImg.isValid())
    {
        if (!drawDecorationPixmap(varImg, painter, option, index))
        {
            
            QPixmap pix;
            
            //получаем картинку и масштабируем по высоте ячейки
            if (varImg.type() == QVariant::Icon)
            {//иконка
                QIcon icon = qvariant_cast<QIcon>(varImg);
                int fontHeight = option.fontMetrics.height();
                int icSize = fontHeight < lineRect.width() ?  fontHeight : lineRect.width();
                pix = icon.pixmap(icSize, icSize);
            }
            else if (varImg.type() == QVariant::Pixmap)
            {//Pixmap
                pix = qvariant_cast<QPixmap>(varImg).scaled(lineRect.height(), lineRect.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                
            }
            
            if (!pix.isNull())
            {//отрисовка по центру
                QPointF point(lineRect.center().x() - pix.width() / 2, lineRect.center().y() - pix.height() / 2);
                painter->drawPixmap(point , pix);
            }
        }
    }
    
    //кастомная отрисовка разделителя строк
    if (drawLines(painter, option, index))
        return;
    
    //рисуем разделитель строк
    QColor lineColor = option.palette.midlight().color();//цвет разделителя
    painter->setPen(lineColor);
    
    auto treeColumn = 0;
    auto leftPosition = 0;
    if(auto treeView = qobject_cast<const QTreeView*>(option.widget))
    {
        treeColumn = treeView->treePosition();
        if(treeColumn == index.column())
        {
            int level = treeView->rootIsDecorated() ? 0 : -1;
            auto zeroColIndex = index.siblingAtColumn(0);
            auto parentIndex = zeroColIndex;
            auto test = index.data();
            while(parentIndex.isValid())
            {
                level++;
                if(level > 0)
                {
                    leftPosition = option.rect.left() - treeView->indentation()*level;
                    auto rightPosition = option.rect.left() - treeView->indentation()*(level - 1);
                    
                    auto hasChildren = [treeView](const QModelIndex& idx)->bool
                    {
                        return idx.model()->hasChildren(idx) && treeView->isExpanded(idx.siblingAtColumn(0));
                    };
                    auto isLastInParent = [](const QModelIndex& idx, const QModelIndex& parent)->bool
                    {
                        if(idx == parent)
                            return false;
                        
                        auto model = idx.model();
                        
                        auto cidx = idx;
                        auto pidx = idx.parent();
                        
                        while(pidx != parent && pidx.isValid())
                        {
                            if(model->rowCount(pidx) - 1 != cidx.row())
                                return false;
                            
                            pidx = pidx.parent();
                            cidx = cidx.parent();
                        }
                        
                        if(model->rowCount(pidx) - 1 == cidx.row() && pidx == parent)
                            return true;
                        else
                            return false;
                    };
                    
                    //нижние кусочки
                    if(!parentIndex.data(TreeItem::GroupBackgroundColorRole).isValid() ||
                      !hasChildren(parentIndex) ||
                      (isLastInParent(zeroColIndex, parentIndex) && !hasChildren(zeroColIndex)))
                        painter->drawLine(QPoint(leftPosition, lineRect.bottomLeft().y()), QPoint(rightPosition, lineRect.bottomRight().y()));
                    
                    //верхние кусочки (надо тестить и думать как сделать)
                    //if (bDrawSideLines)
                    //{
                    //    painter->drawLine(QPoint(leftPosition, lineRect.topLeft().y()), lineRect.topRight());
                    //}  
                }
                
                parentIndex = parentIndex.parent().siblingAtColumn(parentIndex.column());
            }
        }
    }
    
    //нижний раделитель
    painter->drawLine(lineRect.bottomLeft(), lineRect.bottomRight());
    
    //отрисовка боковых бордюров у ячейки
    if (bDrawSideLines)
    {
        auto right = lineRect.right();
        auto top = lineRect.top();
        auto bottom = lineRect.bottom();
        
        if(auto treeView = qobject_cast<const QTreeView*>(option.widget))
        {
            auto visualColumn = treeView->header()->visualIndex(index.column());
            if(visualColumn == 0)
                painter->drawLine(QPoint(0, top), QPoint(0, bottom));
        }
        
        painter->drawLine(QPoint(right, top), QPoint(right, bottom));
    }
}

bool TreeGroupingStyleItemDelegate::drawLines(QPainter* painter, const QStyleOptionViewItem&, const QModelIndex&) const
{
    return !m_drawLines;
}

void TreeGroupingStyleItemDelegate::PaintAfter(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if(index.data().canConvert<RichTextData>())
    {
        QString richtext = index.data().value<RichTextData>();
        DrawRichText(richtext, painter, option, index.data(TreeItem::HighlightColorRole));
    }
}

void TreeGroupingStyleItemDelegate::setDrawSideLines(bool enable)
{
    bDrawSideLines = enable;
}

void TreeGroupingStyleItemDelegate::DrawRichText(const QString& richText, QPainter* painter, const QStyleOptionViewItem& opt,
                                                 const QVariant& highlightColor)
{
    painter->save();
    
    CustomTextDocument doc(opt.rect.height());
    doc.setDocumentMargin(0);
    doc.setDefaultFont(opt.font);
    doc.setHtml(richText);
    
    // shift text right to make icon visible
    // имеется в виду иконка decorationRole, а НЕ иконки в ричтексте
    QSize iconSize = opt.icon.actualSize(opt.rect.size());
    painter->translate(opt.rect.left()+iconSize.width(), opt.rect.top());
    QRect clip(0, 0, opt.rect.width()-iconSize.width(), opt.rect.height());
    
    painter->setClipRect(clip);
    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette = opt.palette;
    ctx.palette.setColor(QPalette::Text,
                         opt.state.testFlag(QStyle::State_Selected) ?
                            (highlightColor.isValid() ? highlightColor.value<QColor>() : opt.palette.color(QPalette::BrightText)) :
                            opt.palette.color(QPalette::Text));
    ctx.palette.setColor(QPalette::Background, QColor(0, 0, 0, 0));
    ctx.clip = clip;
    
    const QString elidedPostfix = "...";
    int deletedChars = 0;
    auto widthToSearch = doc.size().width() <= clip.width() ? -1 : clip.width();
    CacheKey key{richText, widthToSearch, opt.font};
    if(m_richTextToWidthToCharsDeletedCache.contains(key))
    {
        deletedChars = m_richTextToWidthToCharsDeletedCache[key];
        if(deletedChars > 0)
        {
            QTextCursor cursor(&doc);
            cursor.movePosition(QTextCursor::End);
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, deletedChars);
            cursor.removeSelectedText();
            cursor.insertText(elidedPostfix);
        }
    }
    else if (doc.size().width() > clip.width()){
        QTextCursor cursor(&doc);
        cursor.movePosition(QTextCursor::End);
        QFontMetrics metric(opt.font);
        int postfixWidth = metric.horizontalAdvance(elidedPostfix);
        while (!cursor.atStart() && doc.size().width() > clip.width() - postfixWidth) {
            cursor.deletePreviousChar();
            deletedChars++;
            //doc.adjustSize(); //с ним работает фигово, а без него отлично, он почему то разбивает на строчки
        }
        cursor.insertText(elidedPostfix);
    }
    m_richTextToWidthToCharsDeletedCache[key] = deletedChars;
    
    //необходимо задать ширину (clip.width) чтобы работало выравнивание в html QTBUG-22851
    //но при этом нельзя ставить меньше чем doc.size.width, потому что тогда произойдет перенос невлезшего слова на след строку
    doc.setTextWidth(qMax(doc.size().width()+1, (qreal)clip.width()));
    
    doc.documentLayout()->draw(painter, ctx);
    
    painter->restore();
}



QSize TreeGroupingStyleItemDelegate::sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const
{
    QStyleOptionViewItem nopt = opt;
    ModifyOption(index, nopt);
    auto result = StyleItemDelegate::sizeHint(nopt, index);
    
    bool rowSpanned = index.siblingAtColumn(0).data(TreeItem::UserRoles::RowSpannedRole).toBool();
    if(rowSpanned)
    {
        if(index.column() != 0 || !m_firstColumnSelectChekboxesEnabled)
        {
            result.setWidth(0);
            return result;
        }
    }
    
    if(index.data().canConvert<RichTextData>())
    {
        QString richText = index.data().value<RichTextData>();
        CacheKey key{richText, -2, nopt.font};
        if(m_sizeHintCache.contains(key))
        {
            result.setWidth(m_sizeHintCache[key]);
        }
        else
        {
            CustomTextDocument doc(nopt.rect.height()*0.7);
            doc.setDocumentMargin(0);
            doc.setDefaultFont(nopt.font);
            doc.setHtml(richText);
            QSize iconSize = nopt.icon.actualSize(nopt.rect.size()); //icon из decorationRole
            auto resultWidth = doc.size().width() + iconSize.width();
            result.setWidth(resultWidth);
            m_sizeHintCache[key] = resultWidth;
        }
    }
    return result;
}
