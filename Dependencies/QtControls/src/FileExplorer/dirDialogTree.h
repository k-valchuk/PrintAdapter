//-------------------------------------------------------------------------------------------------
// Дерево с файловой моделью с фильтрами по директориям
//-------------------------------------------------------------------------------------------------
#ifndef DIRDIALOGTREE_H
#define DIRDIALOGTREE_H
#include "customDragViewList.h"
#include "qfilesystemmodel.h"

class IconProvider;
class DirDialogTree : public CustomDragViewList
{
    Q_OBJECT
public:
    explicit DirDialogTree(QWidget *parent = nullptr);
    ~DirDialogTree();
    void setRootPath(const QString &path);                //установить корневой каталог
    QString currentPath();                                //текущий выбранный путь
    void setCurrentDir(const QString &path);              //выбрать путь

private:
    void createModel();                                   //создать модель
    void updateRootPath();								  //обновить корень
	
    QFileSystemModel *model;                              //файловая модель
    IconProvider *iconProvider;                           //создатель иконок для директорий

private slots:
    void slot_selectionChanged();                         //изменено выделение

signals:
    void sig_selectPathChanged(const QString &);          //выбран новый каталог

};

#endif // DIRDIALOGTREE_H
