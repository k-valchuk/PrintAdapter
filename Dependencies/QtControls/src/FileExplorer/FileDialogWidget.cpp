#include "FileDialogWidget.h"
#include "ui_FileDialogWidget.h"
#include "QSettings"
#include "QtConcurrent/QtConcurrentRun"
#include "QMessageBox"

const int defaultFontSize = 10;//размер шрифта по умолчанию

FileDialogWidget::FileDialogWidget(FilesViews viewFlags, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FileDialogWidget)
{
    ui->setupUi(this);
    ui->add_pushButton->setObjectName("ShadowButton");
    ui->insert_pushButton->setObjectName("ShadowButton");
    ui->replace_pushButton->setObjectName("ShadowButton");

    bAvailableRootPath = false;
    createViewMenu(viewFlags);
    pathWatcher = new QFutureWatcher<bool>(this);
    pathFuture = new QFuture<bool>;

    ui->fileTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

    //завершение проверки пути
    connect(pathWatcher, SIGNAL(finished()), SLOT(slot_pathWatcherFinished()));
    //выбор файла
    connect(ui->fileTreeView, SIGNAL(sig_currentFilesChanged(QModelIndexList)), this, SLOT(slot_currentFilesChanged(QModelIndexList)));
    //выбор каталога
    connect(ui->dirTreeView, SIGNAL(sig_selectPathChanged(QString)), this, SLOT(slot_currentDirChanged(QString)));
    //изменен путь
    connect(ui->pathLineEdit, SIGNAL(editingFinished()), this, SLOT(slot_pathLineEdit_editingFinished()));
    //обновить
    connect(ui->updatePath_pushButton, SIGNAL(clicked(bool)), SLOT(slot_updatePath()));
    //на каталог вверх
    connect(ui->upDir_pushButton, SIGNAL(clicked(bool)), SLOT(slot_upDir()));
    //контекстное меню
    connect(ui->fileTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SIGNAL(sig_filesCustomContextMenuRequested(QPoint)));
}

FileDialogWidget::~FileDialogWidget()
{
    delete ui;
}

void FileDialogWidget::setRootPath(const QString &path)
{
    QString tmpPath = path;
    if (tmpPath.endsWith('/') || tmpPath.endsWith('\\'))
        tmpPath.chop(1);

    checkPathSlash(tmpPath);
    if (currentRootPath == tmpPath)
        return;

    currentRootPath = tmpPath;
    ui->rootPath_label->setText(currentRootPath); //отображаем корневой каталог
    //очистить модели
    ui->dirTreeView->setRootPath("");
    ui->dirTreeView->setCurrentDir("");
    checkPath(currentRootPath);
}

void FileDialogWidget::saveStateSettings(QSettings &set) const
{
    QByteArray hState, hGeometry;
    ui->fileTreeView->getHeaderState(hState, hGeometry);
    set.beginGroup("FileDialog_Settings");
    set.setValue("header_state", hState);
    set.setValue("header_geometry", hGeometry);
    set.setValue("fileTree_fontSize", ui->fileTreeView->font().pointSize());
    set.setValue("dirTree_fontSize", ui->dirTreeView->font().pointSize());
    set.setValue("view", actViewGroup->checkedAction()->property("id").toInt());
    set.setValue("splitter", actSplitterGroup->checkedAction()->property("id").toInt());
    set.setValue("spliter_state", ui->splitter->saveState());
    set.endGroup();
}

void FileDialogWidget::loadStateSettings(QSettings &set)
{
    set.beginGroup("FileDialog_Settings");
    QByteArray hState = set.value("header_state", QByteArray()).toByteArray();
    QByteArray hGeometry = set.value("header_geometry", QByteArray()).toByteArray();
    QFont fileTreeFont = ui->fileTreeView->font();
    QFont dirTreeFont = ui->dirTreeView->font();
    fileTreeFont.setPointSize(set.value("fileTree_fontSize", defaultFontSize).toInt());
    dirTreeFont.setPointSize(set.value("dirTree_fontSize", defaultFontSize).toInt());
    setViewMode(set.value("view", (int)VIEW_MENU::TABLE).toInt());
    setSplitterMode(set.value("splitter", Qt::Horizontal).toInt());
    ui->splitter->restoreState(set.value("spliter_state", "").toByteArray());
    set.endGroup();

    ui->fileTreeView->setHeaderState(hState, hGeometry);
    ui->fileTreeView->setFont(fileTreeFont);
    ui->dirTreeView->setFont(dirTreeFont);
}

void FileDialogWidget::setThumbCreator(ThumbnailCreator *thumbCreator)
{
    ui->fileTreeView->setThumbCreator(thumbCreator);
}

void FileDialogWidget::clearFilesSelection()
{
    ui->fileTreeView->clearSelection();
}

QString FileDialogWidget::getCurrentDirPath() const
{
    return ui->dirTreeView->currentPath();
}

void FileDialogWidget::setCurrentDirPath(const QString &dirPath)
{
    QString dirPathSlash(dirPath);
    checkPathSlash(dirPathSlash);

    if (dirPathSlash.startsWith(currentRootPath))
        ui->dirTreeView->setCurrentDir(dirPathSlash);
}

QVector<QFileInfo> FileDialogWidget::getSelectedFiles() const
{
    QVector<QFileInfo> files;

    if (ui->fileTreeView->selectionModel())
    {
        for (auto &ind : ui->fileTreeView->selectionModel()->selectedRows())
        {
            files.append(ui->fileTreeView->fileInfo(ind));
        }
    }
    return files;
}

void FileDialogWidget::setFileFilters(const QStringList &filters)
{
    ui->fileTreeView->setFilters(filters);
}

void FileDialogWidget::setThumbFormatId(int formatId)
{
    ui->fileTreeView->setThumbFormatId(formatId);
}

void FileDialogWidget::setIconFileProvider(IconFileProvider *iconFP)
{
    ui->fileTreeView->setIconFileProvider(iconFP);
}

void FileDialogWidget::setButtonsFrameVisible(bool bVis)
{
    ui->buttonsFrame->setVisible(bVis);
}

void FileDialogWidget::checkPathSlash(QString &path)
{
#ifdef WIN32
    path.replace("/", "\\");
#else
    path.replace("\\", "/");
#endif
}

void FileDialogWidget::slot_pathLineEdit_editingFinished()
{
    if (bAvailableRootPath && pathWatcher->isFinished() && pathWatcher->result())//если путь доступен
    {
        QString path = ui->pathLineEdit->text();
        QDir dir(currentRootPath + path);
        if (dir != QDir(ui->dirTreeView->currentPath()))//если поменялся путь
        {
            ui->dirTreeView->setCurrentDir(dir.path());//обновляем путь
        }
    }
}

void FileDialogWidget::slot_currentFilesChanged(const QModelIndexList &indList)
{
    QVector<QFileInfo> files;

    for (auto &ind : indList)
    {
        files.append(ui->fileTreeView->fileInfo(ind));
    }

    emit sig_currentFilesChanged(files);
}

void FileDialogWidget::slot_currentDirChanged(const QString &path)
{
    QString p;
    if (!path.isEmpty())
    {
        // отделяем корень
        p = QDir(path).path();
        QString rPath = QDir(ui->rootPath_label->text()).path();
        rPath.remove(0, 1);// при монтировании
        int startRootPos = p.indexOf(rPath);
        if (startRootPos != -1)
            p = p.right(p.size() - rPath.size() - startRootPos);
    }
    checkPathSlash(p);
    ui->pathLineEdit->setText(p);
    ui->fileTreeView->setRootPath(path);
}

void FileDialogWidget::slot_updatePath()
{
    checkPath(currentRootPath);
}

void FileDialogWidget::slot_upDir()
{
    QString path = ui->pathLineEdit->text();

    if (!path.isEmpty() && bAvailableRootPath && pathWatcher->isFinished() && pathWatcher->result())
    {
        //стираем путь с конца до "/" и переходим
        path = path.left(path.lastIndexOf(QRegExp("\\\\|/")));
        ui->pathLineEdit->setText(path);
        ui->dirTreeView->setCurrentDir(QDir(currentRootPath + path).path());
    }
}

void FileDialogWidget::slot_pathWatcherFinished()
{
    if (pathFuture->result())
    {
        ui->dirTreeView->setRootPath(currentRootPath);                          //устанавливаем корневой каталог
        QString p = QDir(currentRootPath + ui->pathLineEdit->text()).path();
        ui->dirTreeView->setCurrentDir(p);                                      //отображаем файлы из корневого
        bAvailableRootPath = true;
    }
    else
    {
        bAvailableRootPath = false;
        QMessageBox::warning(0, windowTitle(), tr("Path: %1 not found!").arg(currentRootPath));
    }
}

void FileDialogWidget::checkPath(const QString &path)
{    
    auto chPath = [](QString p) ->bool
    {
        if (p.isEmpty())
            return false;
        QDir dir(p);
        return dir.exists();
    };
    //запускаем проверку пути в отдельном потоке
    bAvailableRootPath = false;
    *pathFuture = QtConcurrent::run(chPath, path);
    pathWatcher->setFuture(*pathFuture);
}

void FileDialogWidget::createViewMenu(FilesViews viewFlags)
{
    viewMenu.setContextMenuPolicy(Qt::ActionsContextMenu);
    actViewGroup = new QActionGroup(this);
    actSplitterGroup = new QActionGroup(this);
    actViewGroup->setExclusive(true);
    actSplitterGroup->setExclusive(true);

    //добавить пункт меню
    auto addMenuAction = [&](QAction *act, int id, QActionGroup *actGroup)
    {
        act->setCheckable(true);
        act->setProperty("id", id);
        actGroup->addAction(act);
    };

    //вид
    QAction *act = nullptr;
    if (viewFlags.testFlag(FilesView::List))
    {
        act = viewMenu.addAction(tr("List"), this, SLOT(slot_viewChanged()));
        addMenuAction(act, (int)VIEW_MENU::LIST, actViewGroup);
    }
    if (viewFlags.testFlag(FilesView::Table))
    {
        act = viewMenu.addAction(tr("Table"), this, SLOT(slot_viewChanged()));
        addMenuAction(act, (int)VIEW_MENU::TABLE, actViewGroup);
        act->setChecked(true);
    }
    if (viewFlags.testFlag(FilesView::Image))
    {
        act = viewMenu.addAction(tr("Images"), this, SLOT(slot_viewChanged()));
        addMenuAction(act, (int)VIEW_MENU::IMAGES, actViewGroup);
    }

    viewMenu.addSeparator();

    //компоновка
    act = viewMenu.addAction(tr("Vertical"), this, SLOT(slot_splitterChanged()));
    addMenuAction(act, Qt::Vertical, actSplitterGroup);
    act = viewMenu.addAction(tr("Horizontal"), this, SLOT(slot_splitterChanged()));
    addMenuAction(act, Qt::Horizontal, actSplitterGroup);
    act->setChecked(true);
}

void FileDialogWidget::setViewMode(int id)
{
    for (QAction *act : actViewGroup->actions())
    {
        if (act->property("id").toInt() == id)
        {
            act->setChecked(true);
            break;
        }
    }
    ui->fileTreeView->setViewMode(VIEW_MENU(id));
}

void FileDialogWidget::setSplitterMode(int id)
{
    for (QAction *act : actSplitterGroup->actions())
    {
        if (act->property("id").toInt() == id)
        {
            act->setChecked(true);
            break;
        }
    }
    ui->splitter->setOrientation((Qt::Orientation)id);
}

void FileDialogWidget::on_view_pushButton_clicked()
{
    //позиция меню
    QPoint p = ui->view_pushButton->pos();
    p.ry() += ui->view_pushButton->height();

    viewMenu.exec(mapToGlobal(p));
}

void FileDialogWidget::slot_viewChanged()
{
    VIEW_MENU view = (VIEW_MENU)sender()->property("id").toInt();
    ui->fileTreeView->setViewMode(view);
    ui->fileTreeView->updateHeaderState();
}

void FileDialogWidget::slot_splitterChanged()
{
    Qt::Orientation orient = (Qt::Orientation)sender()->property("id").toInt();
    ui->splitter->setOrientation(orient);
}

void FileDialogWidget::on_add_pushButton_clicked()
{
    emit sig_addFiles(ui->fileTreeView->getSelectedFiles());
}

void FileDialogWidget::on_insert_pushButton_clicked()
{
    emit sig_insertFiles(ui->fileTreeView->getSelectedFiles());
}

void FileDialogWidget::on_replace_pushButton_clicked()
{
    emit sig_replaceFiles(ui->fileTreeView->getSelectedFiles());
}
