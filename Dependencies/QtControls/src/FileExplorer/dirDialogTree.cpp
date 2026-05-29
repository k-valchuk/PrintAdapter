#include "dirDialogTree.h"
#include "QHeaderView"
#include "QFileIconProvider"
#include "QScrollBar"
/**************Создатель иконок каталога для дерева директорий*****************/
class IconProvider : public QFileIconProvider
{
    QIcon icon(const QFileInfo &info) const
    {
        if (info.isDir())
        {
            QIcon folderIcon;
            folderIcon.addFile(":/FileDialog_Icons/folderClose", QSize(), QIcon::Normal, QIcon::Off);
            folderIcon.addFile(":/FileDialog_Icons/folderOpen", QSize(), QIcon::Normal, QIcon::On);
            return folderIcon;
        }
        return QFileIconProvider::icon(info);
    }
};
/*************************************************/

DirDialogTree::DirDialogTree(QWidget *parent): CustomDragViewList(parent)
{
    model = nullptr;
    setSelectionMode(QAbstractItemView::SingleSelection);//одиночный выбор
    iconProvider = new IconProvider();
    setVerticalScrollBar(new QScrollBar(Qt::Vertical));
}

DirDialogTree::~DirDialogTree()
{
    delete model;
}

void DirDialogTree::setRootPath(const QString &path)
{
    if (!path.isEmpty() && QDir(path).exists())//если путь существует
    {
        createModel();
        setRootIndex(model->setRootPath(path));

        //скрыть ненужные столбцы
        header()->hideSection(1);
        header()->hideSection(2);
        header()->hideSection(3);
        //скрыть заголовок
        header()->hide();
    }
    else
        setModel(nullptr);
}

QString DirDialogTree::currentPath()
{
    if (model)
        return model->filePath(currentIndex());
    return QString();
}

void DirDialogTree::setCurrentDir(const QString &path)
{
    if (model)
        setCurrentIndex(model->index(path));    

    emit sig_selectPathChanged(path);
}

void DirDialogTree::createModel()
{
    if (model)
        delete model;

    model = new QFileSystemModel;
    model->setIconProvider(iconProvider);
    model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);//отображать только директории
    setModel(model);
    //изменено выделение в списке
    connect(selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(slot_selectionChanged()));
	//обновление модели при ложном изменении корня
    connect(model, &QFileSystemModel::rowsRemoved, this, &DirDialogTree::updateRootPath, Qt::QueuedConnection);
}

void DirDialogTree::updateRootPath()
{
    if (model)
    {
        QString path = model->rootPath();
        setRootPath(path);
        setCurrentDir(path);
    }
}

void DirDialogTree::slot_selectionChanged()
{
    QModelIndex ind;
    QModelIndexList indLIst = selectionModel()->selectedRows();
    if (indLIst.size() == 1)
        ind = indLIst.last();

    emit sig_selectPathChanged(model->fileInfo(ind).absoluteFilePath());
}
