//-------------------------------------------------------------------------------------------------
// Дерево с файловой моделью
//-------------------------------------------------------------------------------------------------
#ifndef FILEDIALOGTREE_H
#define FILEDIALOGTREE_H

#include "customDragViewList.h"
#include "qfilesystemmodel.h"
#include "QFileSystemWatcher"
#include "thumbnailCreator.h"
#include "QStyledItemDelegate"
#include "QFileIconProvider"

class HeaderView;

/**************Иконки для файлов*****************/
class IconFileProvider : public QFileIconProvider
{
public:
    IconFileProvider(){}
    ~IconFileProvider(){}
    void setHasIcon(bool bIc) {hasIcon = bIc;} // задать наличие иконок

protected:
    virtual QIcon getIcon(const QFileInfo &info) const;

private:
    bool hasIcon = true;//наличие иконок

    QIcon icon(const QFileInfo &info) const;
};
/*************************************************/

//представления списка
enum class VIEW_MENU
{
    LIST,   //список
    TABLE,  //таблица
    IMAGES, //картинки
};

class FileDialogTree;
//отрисовка миниатюры над названием
class ThumbnailFileDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ThumbnailFileDelegate(QWidget *parent = nullptr) : QStyledItemDelegate(parent) {}

signals:
    //сообщить о возможности увеличивать картинку
    void sig_zoomEnabled(bool bZoom) const;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    const int iconHeight = 6;//высота картинки (коэффициент высоты шрифта)
};

//модель файловой системы с возможностью отображать миниатюры
class GraphicsFileSystemModel : public QFileSystemModel
{
public:

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::UserRole && imageList.size() > index.row())
            return imageList.value(index.row());//вернуть миниатюру

        return QFileSystemModel::data(index, role);
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
    {
        if (role == Qt::UserRole && imageList.size() > index.row())
        {
            imageList[index.row()] = value;
            emit dataChanged(index, index, {role});//сообщить о добавлении миниатюры
            return true;
        }
        return QFileSystemModel::setData(index, value ,role);
    }

    void resizeImageList(int newSize)
    {//чистим контейнеры
        imageList.clear();
        imageDataList.clear();
        imageList.resize(newSize);
    }

    void append(const std::shared_ptr<ThumbImageData> &ptrImgData)
    {//добавить данные миниатюры
        imageDataList.append(ptrImgData);
    }

private:

    QVector<QVariant> imageList;                             //список миниатюр
    QVector<std::shared_ptr<ThumbImageData> > imageDataList;      //список данных миниатюр
};

class FileDialogTree : public CustomDragViewList, public ThumbnailReceiver
{
    Q_OBJECT

public:
    explicit FileDialogTree(QWidget *parent = nullptr);
    ~FileDialogTree();

    QFileInfo fileInfo(const QModelIndex &ind) const;    //получить информацию по файлу
    void setRootPath(const QString &path);               //установить корневой путь

    // задать фильтры
    void setFilters(const QStringList &fList);

    //получить состояние заголовка
    void getHeaderState(QByteArray &hState, QByteArray &hGeometry);
    //установить состояние заголовка
    void setHeaderState(const QByteArray &hState, const QByteArray &hGeometry);

    //задать режим представление списка
    void setViewMode(VIEW_MENU view);

    //получить список выбранных файлов
    QStringList getSelectedFiles() const;

    //обновить состояние заголовка
    void updateHeaderState();

    //задать создатель иконок
    void setIconFileProvider(IconFileProvider *iconFP);

protected:
    //миниатюра сформирована
    void thumbReceived(ThumbItem *item, const std::shared_ptr<ThumbImageData> &thumb) override;

signals:
    void sig_currentFilesChanged(const QModelIndexList &indList);    //изменился выбор текущих файлов

private slots:
    void slot_updateDirectory();                         //обновить директорию
    void slot_selectionChanged();                        //изменено выделение
    void slot_directoryLoaded();                         //директория загружена

    void slot_thumbMaxFoldSize(bool bZoom);              //разрешить увеличение картинки

private:
    void createModel();                                  //создать модель
    void hideHeader();                                   //скрыть заголовок
    //сохранить состояние заголовка
    void saveHeaderState(QByteArray &hState, QByteArray &hGeometry) const;


    GraphicsFileSystemModel *fileModel;                  //файловая модель
    QFileSystemWatcher wacher;                           //наблюдатель за обновлением в каталоге
    HeaderView *headerView;                              //заголовок
    QByteArray headerState;                              //состояние заголовка (вид таблица)
    QByteArray headerGeometry;                           //геометрия заголовка (вид таблица)
    bool bRestoreState;                                  //флаг восстановления состояния заголовка
    ThumbnailFileDelegate *thumbDelegate;                //делегат отрисовки миниатюры
    VIEW_MENU viewMode;                                  //режим отображения списка
    IconFileProvider *iconProvider;                      //создатель иконок для файлов (возможность скрыть иконки)
    QString currentPath;                                 //текущий путь
    QStringList filters;                                 //список фильтров
};

#endif // FILEDIALOGTREE_H
