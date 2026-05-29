//-----------------------------------------------------------------------------------------------------------------------
//Список с кастомным отображением при перетаскивании
//-----------------------------------------------------------------------------------------------------------------------
//Изменение размера шрифта списка по Ctrl + MouseWheel
//-----------------------------------------------------------------------------------------------------------------------

#ifndef CUSTOMDRAGVIEWLIST_H
#define CUSTOMDRAGVIEWLIST_H
#include "QTreeView"

class CustomDragViewList : public QTreeView
{
    Q_OBJECT
public:
    explicit CustomDragViewList(QWidget *parent);
    void setDragColumn(int col){dragColumn = col;}        //задать колонку с текстом для отображения
    //отрисовать элемент преретаскивания
    static void drawDragObjects(QObject *src, const QModelIndexList &indexes, QAbstractItemModel *model, int col, Qt::DropActions supportedActions);
    //разрешить увеличивать текст
    void setZoomEnabled(bool bEn) {bZoomEnabled = bEn;}

protected:
    virtual void startDrag(Qt::DropActions supportedActions) override;      //начало переноса
    virtual void wheelEvent(QWheelEvent *event) override;                   //прокрутка колесика мышки (для изменения шрифта)
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override final;  //двойное нажатие мыши

private slots:
    void slot_expanded(const QModelIndex &ind);         //ветка открыта
    void slot_collapsed(const QModelIndex &ind);        //ветка закрыта
    void slot_mousePress(const QModelIndex &);          //нажата клавиша мыши

private:
    //объект для отслеживания двойного нажатия на ветку списка
    struct DblClickBranch
    {
        //операции
        enum Operation
        {
            NoOperation,
            Expand,
            Collapse
        };

        Operation op = NoOperation;
        QModelIndex branchInd;              //индекс ветки
    }dbClickBranch;

    int dragColumn = 0;             //колонка, откуда тянуть текст для отображения
    bool bZoomEnabled = true;       //увеличение текста

    void zoomText(int sign);                                                          //изменить размер шрифта
};
#endif // CUSTOMDRAGVIEWLIST_H
