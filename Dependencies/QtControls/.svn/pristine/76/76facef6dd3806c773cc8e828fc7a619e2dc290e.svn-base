#include "treemodel.h"
#include "QMimeData"
#include "QDataStream"
#include "QStack"
#include <functional>

TreeModel::TreeModel(const QVector<QVariant> &horizontalHeaderData,QObject *parent) : QAbstractItemModel(parent)
{
    rootItem = new TreeItem(horizontalHeaderData);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex & ) const
{
    return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if(m_dataGetter)
    {
        auto res = m_dataGetter(index, role);
        if(res.first)
            return res.second;
    }
    
    QVariant value;
    switch (role)
    {
        case Qt::TextAlignmentRole:
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::UserRole:
        case Qt::BackgroundColorRole:
        case Qt::TextColorRole:
        case Qt::FontRole:
        case Qt::DecorationRole:
        case Qt::ToolTipRole:
        case Qt::CheckStateRole:
        case TreeItem::HighlightColorRole:
        case TreeItem::HighlightFontRole:
        case TreeItem::FlagsRole:
        case TreeItem::TypeEditorRole:
        case TreeItem::DecorationPixmapRole:
        case TreeItem::DecorationAlignmentRole:
        case TreeItem::GroupBackgroundColorRole:
        case TreeItem::GroupTextColorRole:
        case TreeItem::GroupBgOnTopRole:
        case TreeItem::DisableSelectionCheckboxRole:
        TreeItem *item = getItem(index);
        value = item->data(index.column(), role);
    }
        
    if(m_firstColumnSelectChekboxesEnabled && role == Qt::CheckStateRole && index.column() == 0 && !value.isValid())
    {
        auto fl = flags(index);
        if(fl.testFlag(Qt::ItemIsSelectable) && fl.testFlag(Qt::ItemIsUserCheckable))
            return Qt::Unchecked;
    }
    
    return value;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    QVariant var = data(index, TreeItem::FlagsRole);
    if (var.isValid())//если есть установленные флаги
        defaultFlags = Qt::ItemFlags(var.toInt());
    else
        defaultFlags |= Qt::ItemIsDropEnabled;
    
    if(m_firstColumnSelectChekboxesEnabled && index.column() == 0 && defaultFlags.testFlag(Qt::ItemIsSelectable))
    {
        bool explicitlyDisabled = data(index, TreeItem::DisableSelectionCheckboxRole).toBool();
        if(!explicitlyDisabled)
            defaultFlags |= Qt::ItemIsUserCheckable;
    }
    
    return defaultFlags;
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        switch (role)
        {
            case Qt::DisplayRole:
            case Qt::UserRole:
            case Qt::DecorationRole:
            case Qt::TextAlignmentRole:
            case Qt::ToolTipRole:
            return rootItem->data(section, role);
        default: break;
        }
    }
    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TreeItem *parentItem = getItem(parent);

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TreeModel::hasChildren(const QModelIndex& parent) const
{
    if(!customHasChildrenHandler)
        return QAbstractItemModel::hasChildren(parent);
    else
        return customHasChildrenHandler(parent);
}

bool TreeModel::canFetchMore(const QModelIndex& parent) const
{
    if(!customCanFetchMoreHandler)
        return QAbstractItemModel::canFetchMore(parent);
    else
        return customCanFetchMoreHandler(parent);
}

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    if(columns <= 0)
        return false;
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    if(rows <= 0)
        return false;
    TreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

Qt::DropActions TreeModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList TreeModel::mimeTypes() const
{
    return QStringList{mimeType, "text/uri-list"} + customMimeTypes;
}

QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *result = customMimeDataGetter ? customMimeDataGetter(indexes) : new QMimeData();    
    result->setData(mimeType, saveIndexes(indexes));
    return result;
}

bool TreeModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int , int , const QModelIndex &) const
{
    bool hasAllowedMimeFormats = data->hasFormat(mimeType) || data->hasUrls();
    if(!hasAllowedMimeFormats)
    {
        for(const auto& customMimeFormat : customMimeTypes)
            if(data->hasFormat(customMimeFormat))
            {
                hasAllowedMimeFormats = true;
                break;
            }
    }
    
    if (
        (action != Qt::MoveAction && action!= Qt::CopyAction) ||            //верный action
        (!hasAllowedMimeFormats)                                            //формат
       )
    {
        return false;
    }

    return true;
}

bool TreeModel::dropMimeData(const QMimeData *, Qt::DropAction action, int , int , const QModelIndex &)
{
    if (action == Qt::CopyAction)
        return false;
    return true;//true удалит исходные строки
}

QByteArray TreeModel::saveIndexes(const QModelIndexList &indexes)
{
    QByteArray result;
    QDataStream stream(&result, QIODevice::WriteOnly);

    for (const QModelIndex &index: indexes)//проходим по перетаскиваемым индексам
    {
        QModelIndex localIndex = index;
        if (localIndex.column() > 0)
            continue;
        QStack<int> indexParentStack;
        while (localIndex.isValid())//запоминаем номера строк в глубину
        {
            indexParentStack << localIndex.row();
            localIndex = localIndex.parent();
        }

        stream << indexParentStack.size();//глубина индекса
        while (!indexParentStack.isEmpty())
        {
            stream << indexParentStack.pop();
        }
    }
    return result;
}

QModelIndexList TreeModel::restoreIndexes(QByteArray &data, const TreeModel *model)
{
    QModelIndexList result;
    QDataStream stream(&data, QIODevice::ReadOnly);

    while(!stream.atEnd())
    {
        int childDepth = 0;
        stream >> childDepth;

        QModelIndex currentIndex;
        for (int i = 0; i < childDepth; ++i)
        {
            int row = 0;
            stream >> row;
            currentIndex = model->index(row, 0, currentIndex);
        }
        result << currentIndex;
    }

    return result;
}

void TreeModel::sortIndexes(QModelIndexList &indexes, SortingOrder order)
{
    bool bResult = false;
    if (order == LessOrder)//если сортировка по возрастанию
        bResult = true;
    std::sort(indexes.begin(), indexes.end(), [&bResult](const QModelIndex &left, const QModelIndex &right)
    {
        int leftDeep = getDeepIndex(left);
        int rightDeep = getDeepIndex(right);
        if (leftDeep < rightDeep)
        {
            return bResult;
        }
        else if (rightDeep < leftDeep)
        {
            return !bResult;
        }
        else if (left.parent() < right.parent())
        {
            return bResult;
        }
        else if (right.parent() < left.parent())
        {
            return !bResult;
        }
        else
        {
            return bResult == (left < right);
        }
    }
    );
}

void TreeModel::setCustomMimeTypes(const QStringList& mimeTypes)
{
    customMimeTypes = mimeTypes;
}

void TreeModel::setCustomMimeGetter(std::function<QMimeData* (const QModelIndexList&)> customMimeGetter)
{
    customMimeDataGetter = customMimeGetter;
}

void TreeModel::setCustomHasChildren(std::function<bool (const QModelIndex&)> hasChildrenHandler,
                                     std::function<bool(const QModelIndex&)> canFetchMoreHandler)
{
    beginResetModel();
    customHasChildrenHandler = hasChildrenHandler;
    customCanFetchMoreHandler  = canFetchMoreHandler;
    endResetModel();
}

void TreeModel::setCustomDataHandler(std::function<QPair<bool, QVariant> (const QModelIndex&, int)> dataGetter)
{
    beginResetModel();
    m_dataGetter = dataGetter;
    endResetModel();
}

void TreeModel::setFirstColumnSelectChekboxesEnabled(bool enabled)
{
    beginResetModel();
    m_firstColumnSelectChekboxesEnabled = enabled;
    endResetModel();
}

bool TreeModel::copyTreeItem(TreeItem *sourceItem, const QModelIndex &parent, int dstRow)
{
    bool success;
    QModelIndex curIndex;
    TreeItem *item;
    success = insertRow(dstRow, parent);//вставить новую строку

    for (int col = 0; col < columnCount(); ++col)//заполнить данными новую строку
    {
        curIndex = index(dstRow, col, parent);
        success  = setData(curIndex, sourceItem->data(col));
    }

    for (int i = 0; i < sourceItem->childCount(); ++i)//копирование дочерних итемов
    {
        item = sourceItem->child(i);
        success = copyTreeItem(item, index(dstRow, 0, parent), item->childNumber());
    }

    return success;
}

bool TreeModel::isOneLevelItems(const QModelIndexList &indexList, const QModelIndex &parent) const
{
    //узнаем глубину индекса
    std::function< int(const QModelIndex &) > searchDepth = [&searchDepth](const QModelIndex &index)
    {
        int depth = 0;
        if (index.isValid())
        {
            ++depth;
            depth += searchDepth(index.parent());
        }
        return depth;
    };

    int dropDepth = searchDepth(parent);//глубина итема куда drop
    for (const QModelIndex &index : indexList)
    {
        if (dropDepth != searchDepth(index.parent()))//если разная глубина
            return false;
    }
    return true;
}

int TreeModel::getDeepIndex(const QModelIndex &ind)
{
    int deep = 0;
    QModelIndex deepIndex = ind.parent();

    while (deepIndex != QModelIndex())
    {
        ++deep;
        deepIndex = deepIndex.parent();
    }

    return deep;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    TreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value, role);

    QVector<int> rolesVec{role};

    if (result)
        emit dataChanged(index, index, rolesVec);

    return result;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    bool result;

    result = rootItem->setData(section, value, role);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}
