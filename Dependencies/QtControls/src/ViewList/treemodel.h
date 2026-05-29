//--------------------------------------------------------------------
//Модель списка
//--------------------------------------------------------------------
#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <functional>
#include "treeitem.h"
const QString mimeType = "TreeItem";
enum SortingOrder
{
    GreaterOrder,
    LessOrder
};

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(const QVector<QVariant> &horizontalHeaderData, QObject *parent = 0);
    ~TreeModel();

    /*перегруженные методы QAbstractItemModel*/
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;
    bool canFetchMore(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;    

    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int, const QModelIndex &) const override;
    bool dropMimeData(const QMimeData *, Qt::DropAction action, int, int column, const QModelIndex &) override;

    /*--------------------------------------------*/
    static QModelIndexList restoreIndexes(QByteArray &data, const TreeModel *model);        //восстановить индексы из mimeData
    static void sortIndexes(QModelIndexList &indexes, SortingOrder order = GreaterOrder);   //сортировка индексов (по умолчанию - по убыванию)
    
    /*--------------------------------------------*/
    void setCustomMimeTypes(const QStringList& mimeTypes);
    void setCustomMimeGetter(std::function<QMimeData*(const QModelIndexList&)> customMimeGetter);
    void setCustomHasChildren(std::function<bool(const QModelIndex&)> hasChildrenHandler,
                              std::function<bool(const QModelIndex&)> canFetchMoreHandler);
    void setCustomDataHandler(std::function<QPair<bool, QVariant>(const QModelIndex&, int)> dataGetter);
    void setFirstColumnSelectChekboxesEnabled(bool enabled);
private:
    static QByteArray saveIndexes(const QModelIndexList &indexes);                          //сохранить индексы для mimeData
    bool copyTreeItem(TreeItem *sourceItem, const QModelIndex &parent, int dstRow);         //копировать итем списка *
    bool isOneLevelItems(const QModelIndexList &indexList, const QModelIndex &parent)const; //проверка уровня расположения итемов (для drop) *
    static int getDeepIndex(const QModelIndex &ind);                                        //получить глубину индекса

    TreeItem *getItem(const QModelIndex &index) const;  //получить итем списка по индексу
    TreeItem *rootItem;                                 //корневой итем
    QStringList customMimeTypes;
    std::function<bool(const QModelIndex&)> customHasChildrenHandler = nullptr;
    std::function<bool(const QModelIndex&)> customCanFetchMoreHandler = nullptr;
    std::function<QMimeData*(const QModelIndexList&)> customMimeDataGetter = nullptr;
    std::function<QPair<bool, QVariant>(const QModelIndex&, int)> m_dataGetter = nullptr;
    bool m_firstColumnSelectChekboxesEnabled = false;
};


#endif // TREEMODEL_H
