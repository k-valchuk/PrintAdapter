#include "fileDialogTree.h"
#include "QHeaderView"
#include "headerview.h"
#include "QPainter"

FileDialogTree::FileDialogTree(QWidget *parent): CustomDragViewList(parent), bRestoreState(false), viewMode(VIEW_MENU::TABLE)
{
    fileModel = nullptr;
    setSelectionMode(QAbstractItemView::ExtendedSelection);//множественный выбор
    setDragEnabled(true);//разрешить перетаскивание
    setDragDropMode(QAbstractItemView::DragDrop);
    setSortingEnabled(true);//разрешить сортировку

    //заголовок с контекстным меню
    headerView = new HeaderView(Qt::Horizontal, false, this);
    setHeader(headerView);
    headerView->setStretchLastSection(true);    
    headerView->createContextMenu({tr("Name"), tr("Size"), tr("Type"), tr("Date Modified")});
    sortByColumn(0, Qt::AscendingOrder);//сортировка по умолчанию

    thumbDelegate = new ThumbnailFileDelegate(this);
    //разрешение на увеличение картинки
    connect(thumbDelegate, &ThumbnailFileDelegate::sig_zoomEnabled, this, &FileDialogTree::slot_thumbMaxFoldSize);

    iconProvider = new IconFileProvider();

    //следить за обновлением в директории
    connect(&wacher, SIGNAL(directoryChanged(QString)), this, SLOT(slot_updateDirectory()));
    connect(&wacher, SIGNAL(fileChanged(QString)), this, SLOT(slot_updateDirectory()));    
}

FileDialogTree::~FileDialogTree()
{
    if (thumbDelegate)
        thumbDelegate->deleteLater();

    if (fileModel)
        delete fileModel;

    if (iconProvider)
        delete iconProvider;
}

void FileDialogTree::setRootPath(const QString &path)
{
    if (currentPath == path)
        return;

    currentPath = path;

    //сохраняем состояние заголовка, чтобы пустая модель не затерла настройки заголовка
    saveHeaderState(headerState, headerGeometry);

    if (!currentPath.isEmpty() && QDir(currentPath).exists())//если путь существует
    {
        createModel();

        fileModel->setRootPath(currentPath);
        setRootIndex(fileModel->index(currentPath));
        if (!wacher.directories().isEmpty())
            wacher.removePaths(wacher.directories());
        wacher.addPath(currentPath);
    }
    else
    {
        bRestoreState = true;//пустая модель - нужно будет восстановить заголовок у новой модели
        setModel(nullptr);
    }
}

void FileDialogTree::setFilters(const QStringList &fList)
{
    filters = fList;

    if (!currentPath.isEmpty())
    {
        QString path = currentPath;
        currentPath.clear();
        setRootPath(path);
    }
}

void FileDialogTree::getHeaderState(QByteArray &hState, QByteArray &hGeometry)
{
    saveHeaderState(hState, hGeometry);
}

void FileDialogTree::setHeaderState(const QByteArray &hState, const QByteArray &hGeometry)
{
    headerState = hState;
    headerGeometry = hGeometry;

    if (!model())
    {//файловая модель не определена, применим настройки заголовка когда создадим новую модель
       bRestoreState = true;
    }
    else
    {//обновить заголовок
        updateHeaderState();
    }
}

void FileDialogTree::setViewMode(VIEW_MENU view)
{    
    //сохранить заголовок таблицы перед изменением вида списка
    saveHeaderState(headerState, headerGeometry);

    viewMode = view;

    switch(viewMode)
    {
        case VIEW_MENU::LIST://список
            hideHeader();
            setItemDelegateForColumn(0, nullptr);//без делегата
            iconProvider->setHasIcon(true);//с иконками
            setZoomEnabled(true);
        break;

        case VIEW_MENU::TABLE://таблица
            //показать заголовок
            header()->show();
            header()->showSection(1);
            header()->showSection(2);
            header()->showSection(3);
            resizeColumnToContents(0);
            setItemDelegateForColumn(0, nullptr);//без делегата
            iconProvider->setHasIcon(true);//с иконками
            updateHeaderState();
            setZoomEnabled(true);
        break;

        case VIEW_MENU::IMAGES://картинки
            hideHeader();
            setItemDelegateForColumn(0, thumbDelegate);//делегат отрисовки миниатюр
            iconProvider->setHasIcon(false);//без иконок
            slot_directoryLoaded();//загружаем миниатюры            
        break;
    }

    setRootIsDecorated(viewMode != VIEW_MENU::IMAGES);   

    //обновить отображение иконок
    if (fileModel)
        fileModel->setIconProvider(iconProvider);

}

QStringList FileDialogTree::getSelectedFiles() const
{
    QStringList files;
    int row = -1;
    for (auto &ind : selectedIndexes())
    {
        if (ind.row() != row)
        {
            files.append(fileInfo(ind).filePath());
            row = ind.row();
        }
    }

    return files;
}

void FileDialogTree::thumbReceived(ThumbItem *item, const std::shared_ptr<ThumbImageData> &thumb)
{
    if (fileModel && item)
    {
        QModelIndex ind = fileModel->index(item->id, 0, rootIndex());
        if (ind.isValid())
        {//добавить подготовленную миниатюру
            QImage image((uchar*)thumb->data, thumb->width, thumb->height, QImage::Format_ARGB32_Premultiplied);
            fileModel->setData(ind, image, Qt::UserRole);
            fileModel->append(thumb);
        }
    }
}

QFileInfo FileDialogTree::fileInfo(const QModelIndex &ind) const
{
    if (fileModel)
        return fileModel->fileInfo(ind);
    return QFileInfo();
}

void FileDialogTree::createModel()
{
    if (fileModel)
    {
        delete fileModel;
    }
    fileModel = new GraphicsFileSystemModel;
    fileModel->setIconProvider(iconProvider);//задать создателя иконок
    fileModel->setNameFilters(filters);//установить фильтры
    fileModel->setNameFilterDisables(false);//не показывать файлы, не соответствующие фильтрам
    fileModel->setFilter(QDir::Files);//показывать только файлы
    setModel(fileModel);
    //изменено выделение в списке
    connect(selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(slot_selectionChanged()));
    //пути в текущей директории загружены
    connect(fileModel, SIGNAL(directoryLoaded(QString)), this, SLOT(slot_directoryLoaded()));

    //восстанавливаем состояние заголовка
    if (bRestoreState)
    {
        updateHeaderState();
        bRestoreState = false;//не требуется восстанавливать заголовок пока не удалится модель или обновятся настройки заголовка(чтение шаблона)
    }
}

void FileDialogTree::updateHeaderState()
{
    if (viewMode == VIEW_MENU::TABLE)
    {//для таблицы обновить ностройки заголовка
        if (!headerState.isEmpty() && !headerGeometry.isEmpty())
        {
            header()->restoreState(headerState);
            header()->restoreGeometry(headerGeometry);
        }
        headerView->updateContextActionsState();//обновить контекстное меню
    }
    else
    {//для остальных - скрыть заголовок
        hideHeader();
    }
}

void FileDialogTree::setIconFileProvider(IconFileProvider *iconFP)
{
    if (iconProvider)
        delete iconProvider;

    iconProvider = iconFP;
}

void FileDialogTree::hideHeader()
{
    headerView->hideSection(1);
    headerView->hideSection(2);
    headerView->hideSection(3);
    headerView->hide();
}

void FileDialogTree::saveHeaderState(QByteArray &hState, QByteArray &hGeometry) const
{
    if (model() && viewMode == VIEW_MENU::TABLE && isVisible())
    {//сохраняем состояние заголовка только видимой таблицы
        hState = headerView->saveState();
        hGeometry = headerView->saveGeometry();
    }
}

void FileDialogTree::slot_updateDirectory()
{
    QString path = fileModel->rootPath();
    fileModel->setRootPath("");
    fileModel->setRootPath(path);
}

void FileDialogTree::slot_selectionChanged()
{
    emit sig_currentFilesChanged(selectionModel()->selectedRows());
}

void FileDialogTree::slot_directoryLoaded()
{    //после загрузки директории

    if (viewMode == VIEW_MENU::IMAGES && fileModel)
    {//для отображения миниатюр

        //обновить сортировку для модели
        fileModel->sort(header()->sortIndicatorSection(), header()->sortIndicatorOrder());
        clearThumbQueue();//очистить старую очередь
        int filesCount = fileModel->rowCount(rootIndex());
        fileModel->resizeImageList(filesCount);

        for (int i = 0; i < filesCount; ++i)
        {//запрос на создание миниатюры
            auto thItem = std::make_shared<ThumbItem>();
            thItem->id = i;
            thItem->filePath = fileModel->fileInfo(fileModel->index(i, 0, rootIndex())).filePath().toStdWString();
            createThumb(thItem);
        }
    }
}

void FileDialogTree::slot_thumbMaxFoldSize(bool bZoom)
{
    setZoomEnabled(bZoom);
}


/*********      ThumbnailFileDelegate       *********/
/*********                                  *********/


void ThumbnailFileDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //выравнивание текста и иконки
    QStyleOptionViewItem opt(option);
    opt.displayAlignment = Qt::AlignBottom | Qt::AlignHCenter;
    opt.decorationAlignment = Qt::AlignCenter;
    opt.decorationPosition = QStyleOptionViewItem::Top;

    //отрисовка по умолчанию
    QStyledItemDelegate::paint(painter, opt, index);

    //рисуем миниатюру
    auto varImg = index.data(Qt::UserRole);
    if (varImg.isValid() && varImg.type() == QVariant::Image)
    {
        QRect imgRect(option.rect);
        imgRect.setHeight(option.fontMetrics.height() * iconHeight);
        //сохраняем соотношение сторон
        QImage img = qvariant_cast<QImage>(varImg).scaled(imgRect.size(), Qt::KeepAspectRatio);
        //координата отрисовки
        int x0 = imgRect.x() + (imgRect.width() - img.width()) / 2;
        int y0 = imgRect.y() + (imgRect.height() - img.height()) / 2;

        //отрисовка
        painter->drawImage(x0, y0, img);
    }
}

QSize ThumbnailFileDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize sizeH = QStyledItemDelegate::sizeHint(option, index);

    auto varImg = index.data(Qt::UserRole);
    if (varImg.isValid() && varImg.type() == QVariant::Image)
    {//если есть изображение - рассчитать область
        if (index.row() == 0)//достаточно первой картинки
        {
            //берем увеличенную метрику
            QFont zoomFont(option.font);
            zoomFont.setPointSize(zoomFont.pointSize() + 2);
            QFontMetrics zoomMetr(zoomFont);

            QSize imgSize(option.widget->width(), zoomMetr.height() * iconHeight);//максимальный размер изображения по видимой области
            imgSize = qvariant_cast<QImage>(varImg).size().scaled(imgSize, Qt::KeepAspectRatio);//сохраняем соотношение сторон картинки
            int maxFontSize = imgSize.height() / iconHeight;//максимально возможная высота шрифта

            //разрешить/запретить zoom
            emit sig_zoomEnabled(zoomMetr.height() <= maxFontSize);
        }

        //высота миниатюры + высота шрифта
        sizeH.setHeight(option.fontMetrics.height() * (iconHeight + 1));
    }

    return sizeH;
}


/**************Иконки для файлов*****************/

QIcon IconFileProvider::getIcon(const QFileInfo &info) const
{
    return QFileIconProvider::icon(info);
}

QIcon IconFileProvider::icon(const QFileInfo &info) const
{
    if (!hasIcon)
    {//без иконки
        QIcon fileIcon;
        QPixmap px(1,1);
        px.fill(Qt::transparent);
        fileIcon.addPixmap(px);
        return fileIcon;
    }
    //иконка
    return getIcon(info);
}
