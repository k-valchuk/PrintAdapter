//-------------------------------------------------------------------------------------------------
// Виджет отображения директорий с файлами
//-------------------------------------------------------------------------------------------------
#ifndef FILEDIALOGWIDGET_H
#define FILEDIALOGWIDGET_H

#include <QFrame>
#include "QFutureWatcher"
#include "QMenu"
#include "thumbnailCreator.h"
#include "QModelIndexList"
#include "QFileInfo"
#include "fileDialogTree.h"

namespace Ui {
class FileDialogWidget;
}
class QSettings;
class FileDialogWidget : public QFrame
{
    Q_OBJECT

public:

    // Флаги представления списка файлов
    enum FilesView {
        List = 1,
        Table = 2,
        Image = 4
    };
    Q_DECLARE_FLAGS(FilesViews, FilesView)

    explicit FileDialogWidget(FilesViews viewFlags = FilesViews(FilesView::List | FilesView::Table), QWidget *parent = nullptr);
    ~FileDialogWidget();

    void setRootPath(const QString& path);                  //установить корневой каталог   
    void setFileFilters(const QStringList &filters);        //задать фильтры для файлов (например {"*.tpj", "*.tga", "*.avi"})

    void saveStateSettings(QSettings &set) const;           //сохранить настройки
    void loadStateSettings(QSettings &set);                 //загрузить настройки

    void setThumbCreator(ThumbnailCreator *thumbCreator);   //задать создатель миниатюр

    void clearFilesSelection();                             //очистить выделение файлов

    QString getCurrentDirPath() const;                      //получить текущую выбранную директорию
    void setCurrentDirPath(const QString &dirPath);         //установить текущую директорию

    QVector<QFileInfo> getSelectedFiles() const;            //получить выбранные файлы

    void setThumbFormatId(int formatId);                    //id формата для создания миниатюр файлов (если отличный от создателя миниатюр)

    void setIconFileProvider(IconFileProvider *iconFP);    //задать создатель иконок

    void setButtonsFrameVisible(bool bVis);                //видимость панелеи кнопок


    static void checkPathSlash(QString &path);              //проверка слеша (win - linux)

signals:
    //передача команд
    void sig_addFiles(const QStringList &grFiles);          //добавить файлы
    void sig_insertFiles(const QStringList &grFiles);       //вставить файлы
    void sig_replaceFiles(const QStringList &grFiles);      //заменить на файлы

    //изменен список выбранных файлов
    void sig_currentFilesChanged (const QVector<QFileInfo> &files);

    //вызов контекстного меню для списка файлов
    void sig_filesCustomContextMenuRequested(const QPoint &);

private slots:
    void slot_pathLineEdit_editingFinished();                        //редактирование пути завершено
    void slot_currentFilesChanged(const QModelIndexList &indList);   //текущие файлы изменились
    void slot_currentDirChanged(const QString &path);                //текущий путь изменился
    void slot_updatePath();                                          //обновить путь
    void slot_upDir();                                               //перейти вверх на каталог
    void slot_pathWatcherFinished();                                 //завершена проверка пути

    void on_view_pushButton_clicked();                      //открыть меню выбора представления
    void slot_viewChanged();                                //изменено представление
    void slot_splitterChanged();                            //изменена компоновка

    void on_add_pushButton_clicked();                       //добавить выбранные файлы
    void on_insert_pushButton_clicked();                    //вставить выбранные файлы
    void on_replace_pushButton_clicked();                   //заменить на выбранные файлы

private:
    void checkPath(const QString &path);                    //запустить проверку пути
    void createViewMenu(FilesViews viewFlags);              //создать меню представления
    void setViewMode(int id);                               //установить режим представления списка файлов
    void setSplitterMode(int id);                           //установить режим компоновки

    Ui::FileDialogWidget *ui;
    QString currentRootPath;                                //текущий корневой каталог

    //для проверки пути в отдельном потоке
    QFutureWatcher<bool> *pathWatcher;
    QFuture<bool> *pathFuture;

    bool bAvailableRootPath;                                //доступность корневого пути
    QMenu viewMenu;                                         //меню выбора представления окна
    QActionGroup *actViewGroup;                             //список представлений
    QActionGroup *actSplitterGroup;                         //список компоновки    
};

#endif // FILEDIALOGWIDGET_H
