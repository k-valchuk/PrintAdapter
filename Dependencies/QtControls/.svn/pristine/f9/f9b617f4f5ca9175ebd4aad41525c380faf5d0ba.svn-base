#include "styleItemDelegate.h"
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
#include "CustomComboBox.h"
#include "CustomDateTimeEdit.h"

StyleItemDelegate::StyleItemDelegate(QWidget *parent) : QStyledItemDelegate(parent), bDrawSideLines(false)
{
    qRegisterMetaType<ItemList>();
    qRegisterMetaType<GroupedItemList>();
    qRegisterMetaType<TextEditorParams>();
}

void StyleItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{   
    QStyleOptionViewItem newOption(option);
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
        QStyledItemDelegate::paint(painter, newOption, index);

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
    
    auto treeColumn = 0;
    auto leftPosition = 0;
    if(auto treeView = qobject_cast<const QTreeView*>(option.widget))
    {
        treeColumn = treeView->treePosition();
        if(treeColumn > 0 && treeColumn == index.column())
        {
            int level = treeView->rootIsDecorated() ? 0 : -1;
            auto parentIndex = index;
            while(parentIndex.isValid())
            {
                level++;
                parentIndex = parentIndex.parent();
            }
            
            leftPosition = option.rect.left() - treeView->indentation()*qMax(level, 0);
        }
    }
    
    //нижний раделитель
    painter->setPen(lineColor);
    painter->drawLine(lineRect.bottomLeft(), lineRect.bottomRight());
    if (index.column() == treeColumn)//для ветки
        painter->drawLine(QPoint(leftPosition, lineRect.bottomLeft().y()), lineRect.bottomRight());

    //отрисовка боковых бордюров у ячейки
    if (bDrawSideLines)
    {
        painter->drawLine(lineRect.topLeft(), lineRect.bottomLeft());
        painter->drawLine(lineRect.topRight(), lineRect.bottomRight());
        //верхний разделитель с боковыми линиями
        painter->drawLine(lineRect.topLeft(), lineRect.topRight());
        if (index.column() == treeColumn)//для ветки
            painter->drawLine(QPoint(leftPosition, lineRect.topLeft().y()), lineRect.topRight());
    }
}

/* виджет при редактировании ячейки (редактор)*/
QWidget *StyleItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *wid = nullptr;
    emit sig_itemEdit(index);

    DelegateEditor wEditor = getEditor(index);
    QStyleOptionViewItem newOption(option);

    if (wEditor.wID != DelegateEditor::DefaultWidget)
    {
        QVariant varTA = index.data(Qt::TextAlignmentRole);

        if (wEditor.wID == DelegateEditor::TCEditWidget)
        {
            TCEdit *tcEdit = new TCEdit(parent);
            tcEdit->setFont(option.font);
            tcEdit->setFrameRate(frameRate);
            tcEdit->setAlignment(Qt::Alignment(varTA.toInt()));
            wid = tcEdit;
        }
        else if (wEditor.wID == DelegateEditor::LineEdit)
        {
            QLineEdit *lineEdit = new QLineEdit(parent);
            lineEdit->setAlignment(Qt::Alignment(varTA.toInt()));
            connect(lineEdit, SIGNAL(textEdited(QString)), SIGNAL(sig_textEdited(QString)));
            //connect(lineEdit, SIGNAL(textEdited(QString)), SLOT(slot_commitEditorData()));
            wid =  lineEdit;
        }
        else if (wEditor.wID == DelegateEditor::ComboBox)
        {
            ItemComboBox *cBox = new ItemComboBox(parent);
            if (wEditor.data.isValid())
            {
                if (wEditor.data.canConvert<PtrQStringList>())
                {
                    PtrQStringList list = wEditor.data.value<PtrQStringList>();
                    cBox->addItems(*list);
                }
                else if (wEditor.data.canConvert(QMetaType::QStringList))
                {
                    cBox->addItems(wEditor.data.toStringList());
                }
                else if (wEditor.data.canConvert<PtrItemList>())
                {
                    auto listPtr = wEditor.data.value<PtrItemList>();
                    int row = 0;
                    for(const auto& itemData : *listPtr)
                    {
                        cBox->addItem(QString());
                        for(auto role : itemData.keys())
                            cBox->setItemData(row, itemData[role], role);
                        row++;
                    }
                }
                else if (wEditor.data.canConvert<ItemList>())
                {
                    auto list = wEditor.data.value<ItemList>();
                    int row = 0;
                    for(const auto& itemData : list)
                    {
                        cBox->addItem(QString());
                        for(auto role : itemData.keys())
                            cBox->setItemData(row, itemData[role], role);
                        row++;
                    }
                }
            }
            //применить выравнивание
            cBox->setEditable(true);
            cBox->lineEdit()->setFont(option.font);
            cBox->lineEdit()->setReadOnly(true);
            cBox->lineEdit()->setAlignment(Qt::Alignment(varTA.toInt()));

            for(int k = 0; k < cBox->count(); k++)
                cBox->setItemData(k, varTA, Qt::TextAlignmentRole);
            //закрытие редактора при выборе
            connect(cBox, SIGNAL(sig_activated(int)), this, SLOT(slot_commitAndCloseEditor()));
            connect(cBox, SIGNAL(sig_hideView()), this, SLOT(slot_commitAndCloseEditor()));
            connect(cBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), cBox, [cBox](int index){
                updateComboBoxStyle(cBox);
            });

            QTimer::singleShot(200, cBox, &QComboBox::showPopup);//открыть выпадающий список, после создания редактора

            wid =  cBox;
        }
        else if (wEditor.wID == DelegateEditor::CustomComboBox)
        {
            auto cmb = new CustomComboBox(parent);
            const GroupedItemList* list = nullptr;  GroupedItemList mem;
            if (wEditor.data.canConvert<GroupedItemList>())
            {
                mem = wEditor.data.value<GroupedItemList>();
                list = &mem;
            }
            else if(wEditor.data.canConvert<PtrGroupedItemList>())
            {
                list = wEditor.data.value<PtrGroupedItemList>();
            }
            if(list)
            {
                for(const auto& group : *list)
                {
                    const auto& groupKey = group.GroupKey;
                    const auto& items = group.Items;
                    for(const auto& item : items)
                    {
                        cmb->AddItem(CustomComboBox::Item{item},
                                     CustomComboBox::ItemId{groupKey, item.value(Qt::UserRole, QVariant())});
                    }
                }
                QTimer::singleShot(200, cmb, &CustomComboBox::ShowPopup);
            }
            wid = cmb;
        }
        else if (wEditor.wID == DelegateEditor::DateTime)
        {
            QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(parent);
            dateTimeEdit->setAlignment(Qt::Alignment(varTA.toInt()));
            dateTimeEdit->setDisplayFormat(DateTimeFormat);
            dateTimeEdit->setCalendarPopup(true);
            wid = dateTimeEdit;
        }
        else if (wEditor.wID == DelegateEditor::CustomDateTime)
        {
            CustomDateTimeEdit* dateTimeEdit = new CustomDateTimeEdit(parent);
            dateTimeEdit->setAlignment(Qt::Alignment(varTA.toInt()));
            if(wEditor.data.canConvert<CustomDateTimeEdit::Format>())
                dateTimeEdit->setFormat(wEditor.data.value<CustomDateTimeEdit::Format>());
            dateTimeEdit->setCalendarShown(true);
            wid = dateTimeEdit;
        }
        else if (wEditor.wID == DelegateEditor::Time)
        {
            QTimeEdit *timeEdit = new QTimeEdit(parent);
            timeEdit->setAlignment(Qt::Alignment(varTA.toInt()));
            timeEdit->setDisplayFormat(TimeFormat);
            wid = timeEdit;
        }
        else if (wEditor.wID == DelegateEditor::HotKey)
        {
            HotKeyEditor *hkEditor = new HotKeyEditor(parent);
            connect(hkEditor, SIGNAL(sig_editingFinished()), this, SLOT(slot_commitAndCloseEditor()));
            wid = hkEditor;
        }
        else if (wEditor.wID == DelegateEditor::TextEdit)
        {
            ItemTextEdit *textEdit = new ItemTextEdit(parent);
            //добавить виджет изменения размера (появляется только при видимых скролах)
            textEdit->setWindowFlag(Qt::SubWindow, true); // introduced in Qt 5.9
            textEdit->setCornerWidget(new QSizeGrip(textEdit));
            if (wEditor.data.isValid() && wEditor.data.canConvert<PtrIGetTextEditParams>())
                textEdit->setTextEditParams(wEditor.data.value<PtrIGetTextEditParams>());
            connect(textEdit, SIGNAL(sig_textChanged(QString)), SIGNAL(sig_textEdited(QString)));
            connect(textEdit, SIGNAL(sig_editingFinished()), this, SLOT(slot_commitAndCloseEditor()));

            wid = textEdit;
        }
    }
    if (!wid)
        wid = QStyledItemDelegate::createEditor(parent, newOption, index);

    wid->setFont(option.font);//применить шрифт списка к editor

    return wid;
}

/*передача данных из модели в редактор*/
void StyleItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    DelegateEditor wEditor = getEditor(index);

    if (wEditor.wID != DelegateEditor::DefaultWidget)
    {
        QString indexData = index.data().toString();

        if (wEditor.wID == DelegateEditor::TCEditWidget)
        {
            TC value = TCOperations::strToTC(indexData, frameRate);
            TCEdit *tcEdit = static_cast<TCEdit*>(editor);           
            tcEdit->setTC(value);
        }
        else if (wEditor.wID == DelegateEditor::LineEdit)
        {
            QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
            lineEdit->setText(indexData);
        }
        else if (wEditor.wID == DelegateEditor::ComboBox)
        {
            QComboBox *cBox = static_cast<QComboBox*>(editor);
            auto cbrow = cBox->findText(indexData);
            if (cbrow != -1)
                cBox->setCurrentIndex(cbrow);
            
            updateComboBoxStyle(cBox);
        }
        else if (wEditor.wID == DelegateEditor::CustomComboBox)
        {
            CustomComboBox *cBox = static_cast<CustomComboBox*>(editor);
            auto userData = index.data(Qt::UserRole);
            if(userData.canConvert<QList<CustomComboBox::ItemId>>())
            {
                auto selectedItems = userData.value<QList<CustomComboBox::ItemId>>();
                cBox->SetCheckedItemsByUserData(selectedItems);
            }
        }
        else if (wEditor.wID == DelegateEditor::DateTime)
        {
            QDateTimeEdit *dtEdit = static_cast<QDateTimeEdit*>(editor);
            dtEdit->setDateTime(QDateTime::fromString(indexData, DateTimeFormat));
        }
        else if(wEditor.wID == DelegateEditor::CustomDateTime)
        {
            CustomDateTimeEdit* dtEdit = static_cast<CustomDateTimeEdit*>(editor);
            auto userData = index.data(Qt::UserRole);
            if(userData.canConvert<QDateTime>())
                dtEdit->setDateTime(userData.toDateTime());
            else if(userData.canConvert<DateTimeCode>())
                dtEdit->setDateTimeCode(userData.value<DateTimeCode>());
        }
        else if (wEditor.wID == DelegateEditor::Time)
        {
            QTimeEdit *tEdit = static_cast<QTimeEdit*>(editor);
            tEdit->setTime(QTime::fromString(indexData, TimeFormat));
        }
        else if (wEditor.wID == DelegateEditor::HotKey)
        {
            HotKeyEditor *hkEditor = static_cast<HotKeyEditor *>(editor);
            hkEditor->setKey(indexData);
        }
        else if (wEditor.wID == DelegateEditor::TextEdit)
        {
            ItemTextEdit *textEdit = static_cast<ItemTextEdit*>(editor);
            if (textEdit)
            {
                textEdit->setText(indexData);
                textEdit->selectAll();
                
                if (wEditor.data.canConvert<TextEditorParams>())
                {
                    auto params = wEditor.data.value<TextEditorParams>();
                    textEdit->setAlignment(params.Alignment);
                    if(params.SizeGrip)
                    {
                        textEdit->setCornerWidget(new QSizeGrip(textEdit));
                        textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                    }
                }
            }
        }
    }
    else
        QStyledItemDelegate::setEditorData(editor, index);
}

/*передача данных из редактора в модель*/
void StyleItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    DelegateEditor wEditor = getEditor(index);

    if (wEditor.wID != DelegateEditor::DefaultWidget)
    {
        QString value;
        if (wEditor.wID == DelegateEditor::TCEditWidget)
        {
            TCEdit *tcEdit = static_cast<TCEdit*>(editor);
            value = TCOperations::getTCAsString(tcEdit->getCurrentTC(), frameRate);
        }
        else if (wEditor.wID == DelegateEditor::LineEdit)
        {
            QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
            value = lineEdit->text();
        }
        else if (wEditor.wID == DelegateEditor::ComboBox)
        {
            QComboBox *cBox = static_cast<QComboBox*>(editor);
            value = cBox->currentText();
        }
        else if (wEditor.wID == DelegateEditor::CustomComboBox)
        {
            CustomComboBox* cBox = static_cast<CustomComboBox*>(editor);
            value = cBox->GetModelText();
            model->setData(index, QVariant::fromValue(cBox->GetCheckedItems()), Qt::UserRole);
        }
        else if (wEditor.wID == DelegateEditor::DateTime)
        {
            QDateTimeEdit *dtEdit = static_cast<QDateTimeEdit*>(editor);            
            value = dtEdit->dateTime().toString(DateTimeFormat);
        }
        else if (wEditor.wID == DelegateEditor::CustomDateTime)
        {
            CustomDateTimeEdit* dtEdit = static_cast<CustomDateTimeEdit*>(editor);
            value = dtEdit->getAsString();
            model->setData(index, QVariant::fromValue(dtEdit->dateTimeCode()), Qt::UserRole);
        }
        else if (wEditor.wID == DelegateEditor::Time)
        {
            QTimeEdit *tEdit = static_cast<QTimeEdit*>(editor);
            value = tEdit->time().toString(TimeFormat);
        }
        else if (wEditor.wID == DelegateEditor::HotKey)
        {
            HotKeyEditor *hkEditor = static_cast<HotKeyEditor *>(editor);
            value = hkEditor->getKey();
        }
        else if (wEditor.wID == DelegateEditor::TextEdit)
        {
            ItemTextEdit *textEdit = static_cast<ItemTextEdit*>(editor);
            if (textEdit)
                value = textEdit->toPlainText();
        }
        model->setData(index, value);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}

/*обновление геометрии редактора*/
void StyleItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect(option.rect);
    
    if(m_firstColumnSelectChekboxesEnabled && index.column() == 0)
    {
        auto tempOption = option;
        initStyleOption(&tempOption, index);
        
        if(tempOption.features.testFlag(QStyleOptionViewItem::HasCheckIndicator))
        {
            auto widget = option.widget;
            auto style = widget ? widget->style() : QApplication::style();
            
            auto cbRect = style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &tempOption, option.widget);
            rect.setLeft(cbRect.right() + 1);
        }
    }
    
    DelegateEditor wEditor = getEditor(index);
    if (wEditor.wID == DelegateEditor::TextEdit)
    {//для редактирования многострочного текста задаем минимальную допустимую высоту редактора
        int minHeight = option.fontMetrics.height() * 3;
        if (rect.height() < minHeight)
            rect.setHeight(minHeight);
    }

    editor->setGeometry(rect);
}

void StyleItemDelegate::setColoumnWidget(int col, DelegateEditor wEditor)
{
    if (wEditor.wID == DelegateEditor::DefaultWidget)
        widgetMap.remove(col);
    else
        widgetMap[col] = wEditor;
}

void StyleItemDelegate::setFirstColumnSelectChekboxesEnabled(bool enabled)
{
    m_firstColumnSelectChekboxesEnabled = enabled;
}

void StyleItemDelegate::setInvisibleSelection(bool invisibleSelection)
{
    m_invisibleSelection = invisibleSelection;
}

bool StyleItemDelegate::drawLines(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const
{
    return false;
}

bool StyleItemDelegate::drawDecorationPixmap(const QVariant &, QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const
{
    return false;
}

bool StyleItemDelegate::drawDefaultPaint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const
{
    return false;
}

DelegateEditor StyleItemDelegate::getEditor(const QModelIndex &ind) const
{
    DelegateEditor wEditor;
    QVariant var;

    var = ind.data(TreeItem::TypeEditorRole);
    //если редактор задан для ячейки
    if (var.isValid())
    {
        wEditor = var.value<DelegateEditor>();
    }
    else if (widgetMap.contains(ind.column()))
    {//если задан для колонки
        wEditor = widgetMap[ind.column()];
    }

    return wEditor;
}

void StyleItemDelegate::updateComboBoxStyle(QComboBox* cBox)
{
    auto index = cBox->currentIndex();
    QString styleSheet;
    
    auto font = cBox->itemData(index, Qt::FontRole);
    if(font.isValid())
    {
        auto f = font.value<QFont>();
        styleSheet += QString("font-family: %1;").arg(f.family());
    }
    
    auto backgroundColor = cBox->itemData(index, Qt::BackgroundRole);
    if(!backgroundColor.isValid()) backgroundColor = cBox->itemData(index, Qt::BackgroundColorRole);
    auto foregroundColor = cBox->itemData(index, Qt::ForegroundRole);
    
    if(backgroundColor.isValid())
    {
        auto c = backgroundColor.value<QColor>();
        styleSheet += QString("background-color:rgb(%1, %2, %3);").arg(c.red()).arg(c.green()).arg(c.blue());
    }
    if(foregroundColor.isValid())
    {
        auto c = foregroundColor.value<QColor>();
        styleSheet += QString("color:rgb(%1, %2, %3);").arg(c.red()).arg(c.green()).arg(c.blue());
    }
    
    cBox->setStyleSheet(styleSheet);
}

void StyleItemDelegate::slot_commitAndCloseEditor()
{
    QWidget *editor = qobject_cast<QWidget *>(sender());
    emit commitData(editor);//записать данные
    emit closeEditor(editor);//закрыть редактор
}

void StyleItemDelegate::slot_commitEditorData()
{
    QWidget *editor = qobject_cast<QWidget *>(sender());
    emit commitData(editor);//записать данные
}
