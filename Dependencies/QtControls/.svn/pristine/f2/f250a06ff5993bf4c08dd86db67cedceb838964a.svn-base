#include "treeitem.h"
TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
{
    parentItem = parent;
    itemData = data;
    userData.resize(data.size());
    colorBackgroundData.resize(data.size());
    colorTextData.resize(data.size());
    fontData.resize(data.size());
    highlightColorData.resize(data.size());
    highlightFontData.resize(data.size());
    decorationData.resize(data.size());
    editorTypeData.resize(data.size());
    decorationPixData.resize(data.size());
    alignmentTextData.fill(QVariant(Qt::AlignLeft), data.size());    
    flagsData.fill(QVariant(defaultFlags()), data.size());
    decorationAlignmentData.resize(data.size());
    checkableData.resize(data.size());
    toolTipData.resize(data.size());
}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

TreeItem *TreeItem::child(int number)
{
    return childItems.value(number);
}

int TreeItem::childCount() const
{
    return childItems.count();
}

int TreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

int TreeItem::columnCount() const
{
    return itemData.count();
}

QVariant TreeItem::data(int column, int role) const
{
    QVariant value;
    if (role == Qt::UserRole)
        value = userData.value(column);
    else if (role == Qt::BackgroundColorRole)
        value = colorBackgroundData.value(column);
    else if (role == Qt::TextColorRole)
        value = colorTextData.value(column);
    else if (role == Qt::FontRole)
        value = fontData.value(column);
    else if (role == HighlightColorRole)
        value = highlightColorData.value(column);
    else if (role == HighlightFontRole)
        value = highlightFontData.value(column);
    else if (role == FlagsRole)
        value = flagsData.value(column);
    else if (role == Qt::TextAlignmentRole)
        value = alignmentTextData.value(column);
    else if (role == Qt::DecorationRole)
        value = decorationData.value(column);
    else if (role == TypeEditorRole)
        value = editorTypeData.value(column);
    else if (role == DecorationPixmapRole)
        value = decorationPixData.value(column);
    else if (role == DecorationAlignmentRole)
        value = decorationAlignmentData.value(column);
    else if (role == Qt::CheckStateRole)
        value = checkableData.value(column);
    else if (role == Qt::ToolTipRole && toolTipData.value(column).isValid())
        value = toolTipData.value(column);
    else if (role == GroupBackgroundColorRole)
        value = groupBgColorData;
    else if (role == GroupTextColorRole)
        value = groupTextColorData;
    else if (role == GroupBgOnTopRole)
        value = groupBgOnTopData;
    else if (role == DisableSelectionCheckboxRole)
        value = disableSelectionCheckbox;
    else if (role == RowSpannedRole)
        value = isSpanned;
    else if (role == RowSpannedDataColumn)
        value = spannedDataColumn;
    else
        value = itemData.value(column);

    return value;
}

bool TreeItem::insertChildren(int position, int rowCount, int colCount)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < rowCount; ++row)
    {
        QVector<QVariant> data(colCount);
        TreeItem *item = new TreeItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}

bool TreeItem::insertColumns(int position, int colCount)
{
    if (position < 0 || position > itemData.size())
        return false;


    for (int column = 0; column < colCount; ++column)
    {
        itemData.insert(position, QVariant());
        userData.insert(position, QVariant());        
        colorBackgroundData.insert(position, colorBackgroundData.last());
        colorTextData.insert(position, colorTextData.last());
        fontData.insert(position, fontData.last());
        highlightColorData.insert(position, highlightColorData.last());
        highlightFontData.insert(position, highlightFontData.last());
        flagsData.insert(position, QVariant(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled));
        alignmentTextData.insert(position, QVariant(Qt::AlignLeft));
        decorationData.insert(position, QVariant());
        editorTypeData.insert(position, QVariant());
        decorationPixData.insert(position, QVariant());
        decorationAlignmentData.insert(position, QVariant());
        checkableData.insert(position, QVariant());
        toolTipData.insert(position, QVariant());
    }

    foreach (TreeItem *child, childItems)
        child->insertColumns(position, colCount);

    return true;
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

bool TreeItem::removeChildren(int position, int rowCount)
{
    if (position < 0 || position + rowCount > childItems.size())
        return false;

    for (int row = 0; row < rowCount; ++row)
        delete childItems.takeAt(position);

    return true;
}


bool TreeItem::removeColumns(int position, int colCount)
{
    if (position < 0 || position + colCount > itemData.size())
        return false;

    for (int column = 0; column < colCount; ++column)
    {
        itemData.remove(position);
        userData.remove(position);
        colorBackgroundData.remove(position);
        colorTextData.remove(position);
        fontData.remove(position);
        highlightColorData.remove(position);
        highlightFontData.remove(position);
        flagsData.remove(position);
        alignmentTextData.remove(position);
        decorationData.remove(position);
        editorTypeData.remove(position);
        decorationPixData.remove(position);
        decorationAlignmentData.remove(position);
        checkableData.remove(position);
        toolTipData.remove(position);
    }

    foreach (TreeItem *child, childItems)
        child->removeColumns(position, colCount);

    return true;
}

bool TreeItem::setData(int column, const QVariant &value, int role)
{
    if (column < 0 || column >= itemData.size())
        return false;

    if (role == Qt::UserRole)
        userData[column] = value;
    else if (role == Qt::BackgroundColorRole)
        colorBackgroundData[column] = value;
    else if (role == Qt::TextColorRole)
        colorTextData[column] = value;
    else if (role == Qt::FontRole)
        fontData[column] = value;
    else if (role == HighlightColorRole)
        highlightColorData[column] = value;
    else if (role == HighlightFontRole)
        highlightFontData[column] = value;
    else if (role == FlagsRole)
        flagsData[column] = value;
    else if (role == Qt::TextAlignmentRole)
        alignmentTextData[column] = value;
    else if (role == Qt::DecorationRole)
        decorationData[column] = value;
    else if (role == TypeEditorRole)
        editorTypeData[column] = value;
    else if (role == DecorationPixmapRole)
        decorationPixData[column] = value;
    else if (role == DecorationAlignmentRole)
        decorationAlignmentData[column] = value;
    else if (role == Qt::CheckStateRole)
        checkableData[column] = value;
    else if (role == Qt::ToolTipRole)
        toolTipData[column] = value;
    else if (role == GroupBackgroundColorRole)
        groupBgColorData = value;
    else if (role == GroupTextColorRole)
        groupTextColorData = value;
    else if (role == GroupBgOnTopRole)
        groupBgOnTopData = value.isValid() ? value.toBool() : false;
    else if (role == DisableSelectionCheckboxRole)
        disableSelectionCheckbox = value.isValid() ? value.toBool() : false;
    else if (role == RowSpannedRole)
        isSpanned = value.isValid() ? value.toBool() : false;
    else if (role == RowSpannedDataColumn)
        spannedDataColumn = value.isValid() ? value.toInt() : 0;
    else
        itemData[column] = value;

    return true;
}

Qt::ItemFlags TreeItem::defaultFlags()
{
    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
}
