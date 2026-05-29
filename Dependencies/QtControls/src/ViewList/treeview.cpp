#include "treeview.h"
#include "QHeaderView"
#include "QModelIndex"
#include "QKeyEvent"
#include "QClipboard"
#include "QApplication"
#include "QSettings"
#include "QPainter"
#include "TreeViewColumnResizer.h"
#include "qmath.h"

TreeView::TreeView(int columnCount, QWidget *parent):TreeView(QVector<QVariant>(columnCount), false, nullptr, parent)
{    
    headerView->setHidden(true);
}

TreeView::TreeView(const QVector<QVariant> &headerList, bool hEditable, const QString &setLocation, const QString &objName, StyleItemDelegate *delegate, QWidget *parent) :
    TreeView(headerList, hEditable, delegate, parent)
{
    settingsFile = setLocation;
    bAutoSaveSettings = true;
    if (!settingsFile.isEmpty() && !settingsFile.endsWith("/"))
    {
        settingsFile.append("/");
    }
    settingsFile += "ListSettings";
    setObjectName(objName);
    loadSettings();//загружаем настройки
}

TreeView::TreeView(const QVector<QVariant> &headerList, bool hEditable, StyleItemDelegate *delegate, QWidget *parent) :
    TreeView(new QSortFilterProxyModel(), headerList, hEditable, delegate, parent)
{
    sortFilterModel->setParent(this);
}

TreeView::TreeView(QSortFilterProxyModel* proxyModel, const QVector<QVariant>& headerList, bool hEditable, StyleItemDelegate* delegate, QWidget* parent) :
    CustomDragViewList(parent), styleItemDelegate(delegate)
{
    headerView = new HeaderView(Qt::Horizontal, hEditable, this);
    
    QTreeView::setHeader(headerView);
    sourceModel = new TreeModel(headerList);
    sortFilterModel = proxyModel;
    if(sortFilterModel)
    {
        sortFilterModel->setSourceModel(sourceModel);
        sortFilterModel->setDynamicSortFilter(false);
    }
    setModel(sortFilterModel ? (QAbstractItemModel*)sortFilterModel : sourceModel);
    
    headerView->createContextMenu(headerDataList());//создаем контекстное
    
    //множественное построчное выделение
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    setSortingEnabled(true);//разрешить сортировку
    //drag and drop
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    setAcceptDrops(true);
    
    dropIndicatorStyle = new DropIndicatorProxyStyle(QColor("#EEBA3F"), style());
    setStyle(dropIndicatorStyle);
    
    if (!styleItemDelegate)
        styleItemDelegate = new StyleItemDelegate(this);
    setItemDelegate(styleItemDelegate);//применить делегат стилей
    
    setFocusPolicy(Qt::StrongFocus);
    
    //изменение заголовка
    connect(headerView, SIGNAL(sig_editHeader(int, QString)), this, SLOT(slot_editHeader(int, QString)));
    //выделение изменено
    connect(this->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SIGNAL(sig_selectionChanged()));
    //данные в ячейке изменены
    connect(styleItemDelegate, SIGNAL(sig_textEdited(QString)), this, SLOT(slot_textEdited(QString)));
    //завершение редактирования ячейки
    connect(styleItemDelegate, SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)), this, SLOT(slot_dataEditingFinished(QWidget*, QAbstractItemDelegate::EndEditHint)));
    //ячейка редактируется
    connect(styleItemDelegate, SIGNAL(sig_itemEdit(QModelIndex)), this, SIGNAL(sig_itemEdit(QModelIndex)));
    
    connect(model(), &QAbstractItemModel::dataChanged, this, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()){
        if(!m_firstColumnSelectChekboxesEnabled)
            return;
        
        if(topLeft != bottomRight)
            return;
        if(topLeft.column() != 0)
            return;
        if(!roles.contains(Qt::ItemDataRole::CheckStateRole))
            return;
        
        
        auto nval = data(topLeft, Qt::ItemDataRole::CheckStateRole) == Qt::Checked ? true : false;
        if(selectionMode() == QAbstractItemView::SingleSelection && !nval)
        {
            this->setData(topLeft, Qt::Checked, Qt::CheckStateRole);
            return;
        }
        selectionModel()->setCurrentIndex(topLeft,
                                          (nval ? QItemSelectionModel::SelectionFlag::Select : QItemSelectionModel::SelectionFlag::Deselect) | QItemSelectionModel::SelectionFlag::Rows
                                              | (selectionMode() == QAbstractItemView::SingleSelection ? QItemSelectionModel::Clear : (QItemSelectionModel::SelectionFlag)0) );
    });
    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection &selected, const QItemSelection &deselected){
        if(!m_firstColumnSelectChekboxesEnabled)
            return;
        
        QSignalBlocker bl{model()};
        for(const auto& index : selected.indexes())
        {
            if(index.column() == 0 && model()->flags(index).testFlag(Qt::ItemIsUserCheckable))
                setData(index, Qt::Checked, Qt::ItemDataRole::CheckStateRole);
        }
        
        for(const auto& index : deselected.indexes())
        {
            if(index.column() == 0 && model()->flags(index).testFlag(Qt::ItemIsUserCheckable))
                setData(index, Qt::Unchecked, Qt::ItemDataRole::CheckStateRole);
        }
        
        auto firstLevelRows = rows(QModelIndex());
        auto selrows = selectedRows().toSet();
        int notSelectedRowsCount = firstLevelRows.count();
        for(const auto& row : firstLevelRows)
        {
            auto fl = model()->flags(row);
            if((fl.testFlag(Qt::ItemIsSelectable) && fl.testFlag(Qt::ItemIsUserCheckable)) == false || selrows.contains(row))
                notSelectedRowsCount--;
        }
        Qt::CheckState chState = Qt::PartiallyChecked;
        if(notSelectedRowsCount == 0)
            chState = Qt::Checked;
        else if(notSelectedRowsCount == firstLevelRows.count())
            chState = Qt::Unchecked;
        headerView->setFirstColumnCheckboxChecked(chState);
    });
    connect(headerView, &HeaderView::sig_selectAll, this, [this](bool select){
        if(!m_firstColumnSelectChekboxesEnabled)
            return;
        
        if(select)
            selectAll();
        else
            selectionModel()->clearSelection();
    });
}

TreeView::~TreeView()
{
    if (bAutoSaveSettings)
        saveSettings();

    if (styleItemDelegate)
        styleItemDelegate->deleteLater();
    delete sourceModel;
    delete sortFilterModel;
}

bool TreeView::insertRow(int position, const QModelIndex &parentIndex, const QVector<QVariant> *data)
{
    if (position < 0 )//вставка в конец
        position = getModel()->rowCount(parentIndex);

     if (!getModel()->insertRow(position, parentIndex))
         return false;

     if (data)//вставка данных
     {
         for (int i = 0; i < data->size(); ++i)
            setData(index(position, i, parentIndex), data->at(i));
     }

     //если есть разбиение на страницы
     if (rPageCount > 0)
         hideRow(position, parentIndex);//скрыть строку если не принадлежит странице

     return true;
}

bool TreeView::insertRows(int count, int position, const QModelIndex &parentIndex)
{
    if (position < 0 )//вставка в конец
        position = getModel()->rowCount(parentIndex);

     if (!getModel()->insertRows(position, count, parentIndex))
         return false;

     //если есть разбиение на страницы
     if (rPageCount > 0)
     {
         for (int i = 0; i < count; ++i)
            hideRow(position + i, parentIndex);//скрыть строку если не принадлежит странице
     }

     return true;
}

bool TreeView::removeRow(int row, const QModelIndex &parent)
{
    bool success = false;

    success = getModel()->removeRow(row, parent);
    if (success)
    {
        if (rPageCount > 0)//если есть разбиение
            showRow(parent.row(), parent.parent());
    }

    return success;
}

bool TreeView::removeRows(QModelIndexList &indexList)
{
    bool bRemove = false;
    TreeModel::sortIndexes(indexList);//сортируем по убыванию
    foreach (QModelIndex ind, indexList)
    {
        bRemove = removeRow(ind.row(), ind.parent());
    }
    return bRemove;
}

bool TreeView::insertColumn(int position, const QVariant &value)
{
    if (position < 0)
        position = getModel()->columnCount();
    if (!getModel()->insertColumn(position))
        return false;
    if (headerView)
        headerView->addContextAction(position, value.toString());
    return getModel()->setHeaderData(position, Qt::Horizontal, value);
}

bool TreeView::removeColumn(int position)
{
    if (coloumnCount() == 0)
        return false;

    if (headerView)
        headerView->removeContextAction(position);
    return  getModel()->removeColumn(position);

}

bool TreeView::setData(const QModelIndex &index, const QVariant& value, int role)
{
    return getModel()->setData(index, value, role);
}

bool TreeView::setDataRow(int row, const QModelIndex &parent, const QVector<QVariant> &vecData, int role)
{
    bool res = false;
    QModelIndex ind;
    if (coloumnCount() >= vecData.size())
    {
        for (int col = 0; col < vecData.size(); ++col)
        {
            ind = index(row, col, parent);
            res = setData(ind, vecData.at(col), role);
        }
    }

    return res;
}

QVariant TreeView::data(const QModelIndex &index, int role) const
{
    return getModel()->data(index, role);
}

QVector<QVariant> TreeView::dataRow(const QModelIndex &ind, int role) const
{
    QVector<QVariant> dataVec;
    if (ind.isValid())
        for (int i = 0; i < coloumnCount(); ++i)
            dataVec.append(data(index(ind.row(), i, ind.parent()), role));

    return dataVec;
}

bool TreeView::setHeaderData(int section, const QVariant &value, int role)
{
    return getModel()->setHeaderData(section, Qt::Horizontal, value, role);
}

void TreeView::setHeaderDataList(const QVariantList &list, int role)
{
    for ( int i = 0; i < list.size(); ++i)
        setHeaderData(i, list[i], role);
    headerView->createContextMenu(headerDataList());
}

QVariant TreeView::headerData(int section, int role) const
{
    return getModel()->headerData(section, Qt::Horizontal, role);
}

QVariantList TreeView::headerDataList() const
{
    QVariantList list;
    for (int i = 0; i < coloumnCount(); ++i)
        list<<headerData(i);

    return list;
}

QModelIndex TreeView::index(int row, int col, const QModelIndex &parent) const
{
    if (row < 0)
        row = getModel()->rowCount(parent) - 1;
    return getModel()->index(row, col, parent);
}

int TreeView::coloumnCount() const
{
    return getModel()->columnCount();
}

int TreeView::rowCount(const QModelIndex &index) const
{
    return getModel()->rowCount(index);
}

QModelIndex TreeView::currentItemIndex() const
{
    return getModel()->index(currentIndex().row(), 0, currentIndex().parent());
}

void TreeView::setSorting(bool bSort)
{
    setSortingEnabled(bSort);
    if (!bSort && headerView)
        headerView->setSectionsClickable(true);
}

void TreeView::setTableMode()
{
    setRootIsDecorated(false);
}

void TreeView::setEditHeader(bool bEd)
{
    if (headerView)
        headerView->setHeaderEditorVisible(bEd);
}

void TreeView::keyPressEvent(QKeyEvent *event)
{
    if(event->matches(QKeySequence::Copy) )//копировать в буфер
    {
        copyToBuffer();
    }
    else if (event->matches(QKeySequence::Cut))//вырезать
    {
        cutToBuffer();
    }
    else if(event->matches(QKeySequence::Paste) )//вставить из буфера
    {
        pasteFromBuffer();
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}

void TreeView::dragEnterEvent(QDragEnterEvent *event)
{
    TreeView *sourceView = qobject_cast<TreeView*>(event->source());//источник переноса
    QVariantList dragList;
    if (event->mimeData()->hasUrls()) //urls
    {
        QList<QUrl> urlList = event->mimeData()->urls();
        foreach (auto url, urlList)
            dragList<<url;
    }
    else
    {
        bool foundCustomHandledMimeType = false;
        for(const auto& customMimeDataFormat : customMimeDataTypesHandlers.keys())
        {
            if(event->mimeData()->hasFormat(customMimeDataFormat))
            {
                auto handler = customMimeDataTypesHandlers[customMimeDataFormat];
                dragList << handler(event->mimeData());
                foundCustomHandledMimeType = true;
            }
        }

        if(!foundCustomHandledMimeType && event->mimeData()->hasFormat(mimeType) && sourceView)
        {
            QByteArray mimeBA = event->mimeData()->data(mimeType);
            QModelIndexList dragIndexList = TreeModel::restoreIndexes(mimeBA, sourceView->getSourceModel());//список переносимых индексов

            foreach (auto ind, dragIndexList)
                dragList<<ind;
        }
    }

    
    QModelIndex dropIndex = sortFilterModel ? sortFilterModel->mapToSource(indexAt(event->pos())) : indexAt(event->pos());//куда drop
    if (!acceptDrop(dragList, event->source(), dropIndex, dropIndicatorPosition()))
        event->setDropAction(Qt::IgnoreAction);//игнорировать вставку
    QTreeView::dragEnterEvent(event);
}

void TreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    QTreeView::drawBranches(painter, rect, index);
    
    //цвет фона ветки, если задан
    QVariant varColor = index.siblingAtColumn(treePosition()).data(Qt::BackgroundColorRole);
    bool isSelected = selectionModel()->isSelected(index);
    if (varColor.isValid() && !isSelected)
    {
        painter->fillRect(rect, varColor.value<QColor>());
    }
    
    //--------------------------------------------
    // переделка кода из приватной реализации QTreeView через публичные возможности
    // чтобы отрисовать треугольник поверх заливки
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
    
    auto hasChildren = [this](const QModelIndex& idx)->bool
    {
        return idx.model()->hasChildren(idx) && this->isExpanded(idx.siblingAtColumn(0));
    };
    
    auto fillRectWithLines = [](const QRect& rect, bool inverse) -> QVector<QLineF>{
        QVector<QLineF> res;
        
        auto ys = rect.top();
        auto yf = rect.bottom();
        auto xs = rect.left();
        auto xf = rect.right();
        
        qreal step = 5;
        qreal offset = 6;
        
        qreal height = rect.height();
        qreal countf = height/step;
        offset = ((countf - (int)(countf)) * step)/2.0;
        
        qreal startOffset = step*qCeil((xf - xs)/step);
        for(auto y = ys - startOffset + offset; y <= yf + (xf - xs); y += step)
        {
            qreal x1 = xs;
            qreal y1 = y;
            
            qreal x2 = xf;
            qreal y2 = inverse ? y1 + (x2 - x1) : y1 - (x2 - x1);
            
            res.append(QLineF(QPointF(x1, y1), QPointF(x2, y2)));
        }
        
        return res;
    };
    
    int level = -1; //определение уровня вложенности (потому что без приватного d мы его не знаем)
    bool grColorOnTop = index.siblingAtColumn(0).data(TreeItem::GroupBgOnTopRole).toBool();
    auto parentIndex = index;
    while(parentIndex.isValid())
    {
        level += 1;
        
        //рисование цветных бренчей со штриховкой (если есть цвет в роли для закрашивания справа-налево до бренчей)
        QVariant grColor = parentIndex.siblingAtColumn(0).data(TreeItem::GroupBackgroundColorRole);
        if (grColor.isValid() && !isSelected && (/*level > 0 || */grColorOnTop || !varColor.isValid()))
        {
            QRect oneBranchRect = rect;
            oneBranchRect.setWidth(this->indentation() - 1);
            oneBranchRect.moveRight(rect.right() - this->indentation()*level);
            painter->fillRect(oneBranchRect, grColor.value<QColor>());
            
            int partOfWidth = 3;
            
            if(parentIndex != index)
            {
                painter->save();
                
                painter->setRenderHint(QPainter::Antialiasing, true);
                auto lightness = grColor.value<QColor>().lightness();
                auto penColor = lightness < 180 ? QColor::fromRgb(255, 255, 255, 70) : QColor::fromRgb(0, 0, 0, 40);
                painter->setPen(QPen(penColor, 2.0, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap));
                
                QRect leftBorder = oneBranchRect;
                leftBorder.setWidth(this->indentation()/partOfWidth);
                leftBorder.moveLeft(oneBranchRect.left());
                painter->setClipRect(leftBorder);
                painter->drawLines(fillRectWithLines(leftBorder, false));
                
                QRect rightBorder = oneBranchRect;
                rightBorder.setWidth(this->indentation()/partOfWidth);
                rightBorder.moveRight(oneBranchRect.right());
 
                painter->setClipRect(rightBorder);
                painter->drawLines(fillRectWithLines(rightBorder, true));
                
                if(isLastInParent(index.siblingAtColumn(0), parentIndex.siblingAtColumn(0)) && !hasChildren(index.siblingAtColumn(0)))
                {
                    QRect bottomBorder = oneBranchRect;
                    bottomBorder.setHeight(3);
                    bottomBorder.moveBottom(oneBranchRect.bottom());
                    
                    bottomBorder.setLeft(bottomBorder.left() + this->indentation()/partOfWidth);
                    bottomBorder.setRight(bottomBorder.right() - this->indentation()/partOfWidth);
                    painter->setClipRect(bottomBorder);
                    painter->drawLines(fillRectWithLines(bottomBorder, false));
                }
                
                painter->restore();
            }
        }
        
        parentIndex = parentIndex.parent();
    }
    
    const bool reverse = isRightToLeft();
    const int indent = this->indentation();
    const int outer = this->rootIsDecorated() ? 0 : 1;
    QRect primitive(reverse ? rect.left() : rect.right() + 1, rect.top(), indent, rect.height());
    
    QStyleOptionViewItem opt = viewOptions();
    QStyle::State extraFlags = QStyle::State_None;
    if (isEnabled())
        extraFlags |= QStyle::State_Enabled;
    if (hasFocus())
        extraFlags |= QStyle::State_Active;
    QPoint oldBO = painter->brushOrigin();
    if (verticalScrollMode() == QAbstractItemView::ScrollPerPixel)
        painter->setBrushOrigin(QPoint(0, verticalOffset()));
    
    if (isSelected && !m_invisibleSelection)
        extraFlags |= QStyle::State_Selected;
    
    if (level >= outer) {
        // start with the innermost branch
        primitive.moveLeft(reverse ? primitive.left() : primitive.left() - indent);
        opt.rect = primitive;
    
        const bool expanded = isExpanded(index);
        const bool children = model()->hasChildren(index);
        
        opt.state = QStyle::State_Item | extraFlags
                    | (children ? QStyle::State_Children : QStyle::State_None)
                    | (expanded ? QStyle::State_Open : QStyle::State_None);
        
        style()->drawPrimitive(QStyle::PE_IndicatorBranch, &opt, painter, this);
    }
    //здесь дальше должен быть for по level для дорисовки элементов веток левее (такие как палочки и тд)
    //но пока не нужно (в базовом случае только треугольнички для раскрытия) и поэтому не сделано
    painter->setBrushOrigin(oldBO);
}

void TreeView::dropEvent(QDropEvent *event)
{
    //игнорировать drop по умолчанию
    Qt::DropAction dropAction = event->dropAction();

    event->setDropAction(Qt::IgnoreAction);
    QTreeView::dropEvent(event);

    //получаем индексы переносимых строк из mimeData
    TreeView *sourceView = qobject_cast<TreeView*>(event->source());
    QVariantList dropList;
    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> urlList = event->mimeData()->urls();
        for (QUrl &url: urlList)
            dropList<<url;
    }
    else
    {
        bool foundCustomHandledMimeType = false;

        for(const auto& customMimeDataFormat : customMimeDataTypesHandlers.keys())
        {
            if(event->mimeData()->hasFormat(customMimeDataFormat))
            {
                auto handler = customMimeDataTypesHandlers[customMimeDataFormat];
                dropList << handler(event->mimeData());
                foundCustomHandledMimeType = true;
            }
        }

        if (!foundCustomHandledMimeType && event->mimeData()->hasFormat(mimeType) && sourceView)
        {
            QModelIndexList dragIndexList;
            QByteArray mimeBA = event->mimeData()->data(mimeType);
            dragIndexList = sourceModel->restoreIndexes(mimeBA, sourceView->sourceModel);
            dragIndexList = sourceView->indexListFromSource(dragIndexList);//переводим в индексы сорт модели
            for (QModelIndex &ind: dragIndexList)
                dropList<<ind;
        }
    }

    QModelIndex dropIndex = indexAt(event->pos());
    dropIndex = index(dropIndex.row(), 0, dropIndex.parent());

    //сигнал о переносе
    if (!dropList.isEmpty())
    {
        emit sig_pasteItems(dropList, event->source(),
                        dropIndex, static_cast< dropIndicator >(dropIndicatorPosition()), dropAction, false);
    }
}

void TreeView::slot_editHeader(int pos, const QString &hData)
{
    setHeaderData(pos, hData);
}

void TreeView::slot_textEdited(const QString &text)
{
    emit sig_textChanged(currentIndex(), text);
}

void TreeView::slot_dataEditingFinished(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    emit sig_dataEditingFinished(currentIndex());
    if(hint != QAbstractItemDelegate::RevertModelCache)
        emit sig_dataEditingAccepted(currentIndex());
}

void TreeView::copyToBuffer()
{
    setDataToBuffer();
}

void TreeView::pasteFromBuffer()
{
    if (!acceptDrops())//если вставка не разрешена
        return;

    QClipboard *cb = QApplication::clipboard();//буфер обмена
    if (cb)
    {
        const TreeMimeData *md = qobject_cast<const TreeMimeData*>(cb->mimeData());
        if (!md)
        {
            return;
        }

        // читаем из буфера обмена
        QVariantList dropList;
        TreeView *tView = qobject_cast<TreeView *>(md->parent());
        if (tView)
        {
            for (QModelIndex &ind: md->getIndexList())
            {
                if (tView->index(ind.row(), ind.column(), ind.parent()).isValid())//если существует индекс
                {
                    dropList<<ind;
                }
            }

            QModelIndex dropIndex;
            if (selectedRows().size() > 0)
            {
                dropIndex = currentItemIndex();
            }

            if (!dropList.isEmpty())
                emit sig_pasteItems(dropList, md->parent(), dropIndex, BelowItem, md->getDropAction());//сообщаем о вставке

            //очистить буфер при перемещении
            if (md->getDropAction() == Qt::MoveAction)
                cb->clear();
        }
    }
}

void TreeView::cutToBuffer()
{
    setDataToBuffer(Qt::MoveAction);
}

void TreeView::setHeader(QHeaderView *header)
{
    QTreeView::setHeader(header);
    if (headerView)
        headerView = nullptr;
}

void TreeView::setColumnResizer(bool bEnabled)
{
    if (bEnabled)
    {
        columnResizer = std::make_unique<TreeViewColumnResizer>(this);
    }
    else
    {
        columnResizer.reset();
    }
}

void TreeView::setCustomMimeDataTypesHandlers(QMap<QString, std::function<QVariantList (const QMimeData*)> > customMimeDataTypesHandlers)
{
    this->customMimeDataTypesHandlers = customMimeDataTypesHandlers;
    this->sourceModel->setCustomMimeTypes(customMimeDataTypesHandlers.keys());
}

void TreeView::setFirstColumnSelectChekboxesEnabled(bool enabled, bool selectAllCheckBoxInHeader)
{
    m_firstColumnSelectChekboxesEnabled = enabled;
    this->sourceModel->setFirstColumnSelectChekboxesEnabled(enabled);
    this->styleItemDelegate->setFirstColumnSelectChekboxesEnabled(enabled);
    this->headerView->setFirstColumnSelectChekboxesEnabled(selectAllCheckBoxInHeader && enabled);
    this->update();
}

void TreeView::setInvisibleSelection(bool invisibleSelection)
{
    m_invisibleSelection = invisibleSelection;
    this->styleItemDelegate->setInvisibleSelection(invisibleSelection);
    QString correctedStyleSheet = this->styleSheet();
    static const QString transparentSelectionStyle = "QTreeView::branch:selected, QTreeView::branch:selected:alternate{background:transparent;}";
    if(invisibleSelection)
    {
        if(!correctedStyleSheet.contains(transparentSelectionStyle))
            correctedStyleSheet += transparentSelectionStyle;
    }
    else
        correctedStyleSheet = correctedStyleSheet.remove(transparentSelectionStyle);
    this->setStyleSheet(correctedStyleSheet);
    this->update();
}

QModelIndexList TreeView::rows(QModelIndex parent)
{
    QList<QModelIndex> result;
    for(int i = 0; i < this->rowCount(parent); i++)
        result.append(this->index(i, 0, parent));
    return result;
}

QModelIndexList TreeView::allRows(QModelIndex parent)
{
    QList<QModelIndex> result;
    
    if(parent.isValid())
        result.append(parent);
    
    for(auto row : rows(parent))
        result.append(allRows(row));
    
    return result;
}

QModelIndexList TreeView::indexListFromSource(const QModelIndexList &sourceIndexList) const
{
    if(!sortFilterModel)
        return sourceIndexList;
    QModelIndexList proxyList;
    for (QModelIndex ind : sourceIndexList)
        proxyList.append(sortFilterModel->mapFromSource(ind));
    return proxyList;
}

void TreeView::hideRow(int row, const QModelIndex &parentIndex)
{
    if (!parentIndex.isValid())//если уровень вложенности = 0
    {
        int lastRow = currentPage * rPageCount;
        if (row < (currentPage - 1) * rPageCount || row > lastRow)//если строка не на странице
                setRowHidden(row, parentIndex, true);
        else if (index(lastRow, 0, QModelIndex()).isValid())//скрыть последнюю если произошла вставка
            setRowHidden(lastRow, QModelIndex(), true);
    }
}

void TreeView::showRow(int row, const QModelIndex &parentIndex)
{
    if (!parentIndex.isValid())//если уровень вложенности = 0
    {
        int lastRow = currentPage * rPageCount;
        if (row >= (currentPage - 1) * rPageCount || row < lastRow)//если строка на странице
            setRowHidden(--lastRow, QModelIndex(), false);//показать последнюю строку на странице
    }
}

void TreeView::saveSettings() const
{
    QSettings MySetting(settingsFile, QSettings::IniFormat);
    MySetting.beginGroup(objectName());
//    if (headerView->editable())
//        MySetting.setValue("HeaderList", headerDataList());
    MySetting.setValue("HeaderState", header()->saveState());
    MySetting.setValue("HeaderGeometry", header()->saveGeometry());
    MySetting.setValue("FontSize", font().pointSize());
    MySetting.endGroup();
}

void TreeView::loadSettings()
{
    QFont curFont = font();
    QSettings MySetting(settingsFile, QSettings::IniFormat);
    MySetting.beginGroup(objectName());
//    if (headerView->editable())//если заголовок редактируется, то загрузить актуальный список
//        setHeaderDataList(MySetting.value("HeaderList").toList());
    header()->restoreState(MySetting.value("HeaderState", "").toByteArray());
    header()->restoreGeometry(MySetting.value("HeaderGeometry", "").toByteArray());
    curFont.setPointSize(MySetting.value("FontSize", 10).toInt());
    MySetting.endGroup();

    setFont(curFont);

}

void TreeView::rowsHide(int startRow, int count, bool bHide)
{
    for (int i = startRow; i < startRow + count; ++i)
        setRowHidden(i, QModelIndex(), bHide);
}

void TreeView::setDataToBuffer(Qt::DropAction act)
{
    //заполняем буфер обмена
    QClipboard *cb = QApplication::clipboard();
    if (cb)
    {
        TreeMimeData *md = new TreeMimeData;
        auto selList = selectedRows();
        md->setIndexList(selList);
        md->setParent(this);
        md->setDropAction(act);
        cb->setMimeData(md);

        // оповещение о копировании
        emit sig_itemsCopiedToBuffer(selList);
    }
}

bool TreeView::copyTreeIndex(const TreeView* sourceTree, const QModelIndex &sourceIndex, const QModelIndex &drIndParent, int dstRow, Qt::DropAction drAct)
{
    bool success;
    QModelIndex curIndex;

    auto vecData = sourceTree->dataRow(sourceIndex);
    success = insertRow(dstRow, drIndParent, &vecData);//вставить новую строку

    QModelIndex indS = sourceIndex;
    if (this == sourceTree && drIndParent == sourceIndex.parent() && dstRow <= sourceIndex.row())//берем следующий индекс, если вставка была до
        indS = sourceTree->index(indS.row() + 1, 0, drIndParent);

    //если перемещаем
    if ( success)
    {
        setIndexRowStyle(index(dstRow, 0, drIndParent), data(indS, Qt::TextColorRole).value<QColor>(), data(indS, Qt::BackgroundColorRole).value<QColor>(),
                         data(indS, TreeItem::HighlightColorRole).value<QColor>(), data(indS, Qt::FontRole).value<QFont>(),  data(indS, TreeItem::HighlightFontRole).value<QFont>());

        QModelIndex ind;
        for (int col = 0; col < coloumnCount(); ++col)
        {
            ind = index(dstRow, col, drIndParent);
            setData(ind, data(indS, TreeItem::FlagsRole), TreeItem::FlagsRole);
        }
    }

    for (int i = 0; i < sourceTree->rowCount(indS); ++i)//копирование дочерних итемов
    {
        curIndex = sourceTree->index(i, 0, indS);
        success = copyTreeIndex(sourceTree, curIndex, index(dstRow, 0, drIndParent), i, drAct);
    }

    return success;
}

void TreeView::dragDrop(const QModelIndexList &indList, const QModelIndex &dropIndex, const QModelIndex &dropIndParent, TreeView *sourceTree, int pos, Qt::DropAction drAct)
{
    QModelIndex ind;
    for (int i = 0; i < indList.size(); ++i)
    {
        ind = indList.at(i);
        if (dropIndex.parent() == ind.parent() && dropIndex.row() < ind.row())
            ind = sourceTree->index(ind.row() + i, 0,ind.parent());
        copyTreeIndex(sourceTree, ind, dropIndParent, pos++, drAct);
    }
    if (drAct == Qt::MoveAction)
    {
        for (int i = indList.size() - 1; i >= 0; --i)
        {
            ind = indList.at(i);
            if (dropIndex.parent() == ind.parent() && dropIndex.row() < ind.row())
            {
                ind = sourceTree->index(ind.row() + indList.size(), 0,ind.parent());
            }
            sourceTree->removeRow(ind.row(), ind.parent());
        }
    }
}

void TreeView::setRowPageCount(int rpCount)
{
    if (rpCount == rPageCount)
        return;
    if (rpCount <= 0)//отключить разбиение
        rowsHide(0, rowCount(QModelIndex()), false);
    else
    {
        int startRp = (currentPage - 1) * rpCount;//начало страницы
        rowsHide(0, startRp, true);//скрываем до страницы
        rowsHide(startRp, rpCount,false);//показываем страницу
        startRp+=rpCount;
        rowsHide(startRp, rowCount(QModelIndex()) - startRp, true);//скрываем после страницы
    }

    rPageCount = rpCount;
}

void TreeView::setCurrentPage(int curPage)
{
    if (rPageCount < 1)
        return;

    rowsHide(rPageCount * (currentPage - 1), rPageCount, true);//скрываем старую страницу

    rowsHide(rPageCount * (curPage - 1), rPageCount, false);//показываем новую страницу

    currentPage = curPage;
}

int TreeView::getPageCount() const
{
    int pCount = rowCount(QModelIndex()) / rPageCount;
    if (rowCount(QModelIndex()) % rPageCount > 0)
        ++pCount;
    return (pCount < 1) ? 1 : pCount;
}

QModelIndexList TreeView::selectedRows() const
{
    QModelIndexList indexList;
    for (QModelIndex ind : selectedIndexes())
    {
        if (ind.column() == 0)
            indexList<<ind;
    }
    //qSort(indexList.begin(), indexList.end(), qLess<QModelIndex>()); //сортировка по убыванию
    return indexList;
}

void TreeView::clear()
{
    getModel()->removeRows(0, rowCount(QModelIndex()), QModelIndex());
}

void TreeView::setIndexRowStyle(const QModelIndex &ind, const QColor &colorText, const QColor &colorBackground, const QColor &highlightTextColor, const QFont &font, const QFont &highlightFont)
{
    for (int i = 0; i < coloumnCount(); ++i)
    {
        QModelIndex curInd = index(ind.row(), i, ind.parent());
        setIndexStyle(curInd, colorText, colorBackground, highlightTextColor, font, highlightFont);
    }
}

void TreeView::setIndexColoumnStyle(const QModelIndex &ind, const QColor &colorText, const QColor &colorBackground, const QColor &highlightTextColor, const QFont &font, const QFont &highlightFont)
{
    for (int i = 0; i < rowCount(ind.parent()); ++i)
    {
        QModelIndex curInd = index(i, ind.column(), ind.parent());
        setIndexStyle(curInd, colorText, colorBackground, highlightTextColor, font, highlightFont);
    }
}

void TreeView::setIndexStyle(const QModelIndex &index, const QColor &colorText, const QColor &colorBackground, const QColor &highlightTextColor, const QFont &font, const QFont &highlightFont)
{
    if (colorBackground.isValid())
        setData(index, colorBackground, Qt::BackgroundColorRole);
    if (colorText.isValid())
        setData(index, colorText, Qt::TextColorRole);
    if (font != qApp->font())//если задан новый шрифт
        setData(index, font, Qt::FontRole);
    if (highlightTextColor.isValid())
        setData(index, highlightTextColor, TreeItem::HighlightColorRole);
    if (highlightFont != qApp->font())
        setData(index, highlightFont, TreeItem::HighlightFontRole);
}

void TreeView::setRowBranchGroupStyle(const QModelIndex& index, const QColor& colorBackground, const QColor& colorText, bool thisColorOnTop)
{
    auto firstIdx = index.siblingAtColumn(0);
    if(colorBackground.isValid())
        setData(firstIdx, colorBackground, TreeItem::GroupBackgroundColorRole);
    if(colorText.isValid())
        setData(firstIdx, colorText, TreeItem::GroupTextColorRole);
    setData(firstIdx, thisColorOnTop, TreeItem::GroupBgOnTopRole);
}

void TreeView::setDefaultIndexStyle(const QModelIndex &index)
{
    setData(index, QVariant(), Qt::BackgroundColorRole);
    setData(index, QVariant(), Qt::TextColorRole);
    setData(index, QVariant(), Qt::FontRole);
    setData(index, QVariant(), TreeItem::HighlightColorRole);
    setData(index, QVariant(), TreeItem::HighlightFontRole);
}

void TreeView::setDefaultRowStyle(const QModelIndex &ind)
{
    QModelIndex curInd;
    for (int i = 0; i < coloumnCount(); ++i)
    {
        curInd = index(ind.row(), i, ind.parent());
        setDefaultIndexStyle(curInd);
    }
}

void TreeView::setDefalutColoumnStyle(QModelIndex &ind)
{
    QModelIndex curInd;
    int rowC = rowCount(ind.parent());
    for (int i = 0; i < rowC; ++i)
    {
        curInd = index(i, ind.row(), ind.parent());
        setDefaultIndexStyle(curInd);
    }
}

void TreeView::setDefaultRowBranchGroupStyle(const QModelIndex& index)
{
    auto firstIdx = index.siblingAtColumn(0);
    setData(firstIdx, QVariant(), TreeItem::GroupBackgroundColorRole);
    setData(firstIdx, QVariant(), TreeItem::GroupTextColorRole);
    setData(firstIdx, true,       TreeItem::GroupBgOnTopRole);
}

void TreeView::setColoumnWidgetDelegate(int col, DelegateEditor::WidgetID wID, const QVariant &value)
{
    DelegateEditor dEditor;
    dEditor.wID = wID;
    dEditor.data = value;
    styleItemDelegate->setColoumnWidget(col, dEditor);
}

void TreeView::setFilterKeyColumn(int key)
{
    if(sortFilterModel)
        sortFilterModel->setFilterKeyColumn(key);
}

void TreeView::setFilterRegExp(const QString &pattern)
{
    if(sortFilterModel)
        sortFilterModel->setFilterRegExp(pattern);
}

QString TreeView::getFilterRegExp() const
{
    if(sortFilterModel)
        return sortFilterModel->filterRegExp().pattern();
    return QString();
}

void TreeView::insertEmptyRow(int row, const QModelIndex &parent)
{
    insertRow(row, parent);
    setFirstColumnSpanned(row, parent, true);//объединить итемы
}

void TreeView::setDataTimeFormat(const QString &format)
{
    styleItemDelegate->setDateTimeFormat(format);
}

void TreeView::setTimeFormat(const QString &format)
{
    styleItemDelegate->setTimeFormat(format);
}

void TreeView::setDropIndicatorColor(const QColor &color)
{
    dropIndicatorStyle->setDropIndocatorColor(color);
}

void TreeView::setTCFrameRate(int fR)
{
    styleItemDelegate->setFrameRate(fR);
}
