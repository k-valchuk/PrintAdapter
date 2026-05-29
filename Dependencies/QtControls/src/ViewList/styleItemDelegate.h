//----------------------------------------------------------------------
//Делегат стиля итемов
//----------------------------------------------------------------------
#ifndef STYLEITEMDELEGATE_H
#define STYLEITEMDELEGATE_H

#include "qstyleditemdelegate.h"
#include "itemTextEdit.h"

//виджет делегата при редактировании итема
struct DelegateEditor
{
    enum WidgetID{
        DefaultWidget,
        LineEdit,
        TCEditWidget,
        ComboBox,
        Time,
        DateTime,
        Date,
        Image,
        HotKey,
        CheckBox,
        Offset,
        DblSpinBox,
        TextEdit,
        CustomComboBox,
        CustomDateTime
    };
    WidgetID wID = DefaultWidget;
    QVariant data;
};
Q_DECLARE_METATYPE(DelegateEditor)

using PtrQStringList = const QStringList*;
Q_DECLARE_METATYPE(PtrQStringList)

using ItemList = QList<QMap<Qt::ItemDataRole, QVariant>>;
Q_DECLARE_METATYPE(ItemList)

using PtrItemList = const QList<QMap<Qt::ItemDataRole, QVariant>>*;
Q_DECLARE_METATYPE(PtrItemList)

struct GroupedItems {
    QVariant GroupKey;
    ItemList Items;
};
Q_DECLARE_METATYPE(GroupedItems)
using GroupedItemList = QList<GroupedItems>;
Q_DECLARE_METATYPE(GroupedItemList)

using PtrGroupedItemList = const QList<GroupedItems>*;
Q_DECLARE_METATYPE(PtrGroupedItemList)

using PtrIGetTextEditParams = IGetTextEditParams*;
Q_DECLARE_METATYPE(PtrIGetTextEditParams)

struct TextEditorParams{
    Qt::Alignment Alignment;
    bool SizeGrip = false;
};
Q_DECLARE_METATYPE(TextEditorParams)

class QComboBox;
class StyleItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:

    StyleItemDelegate(QWidget *parent = 0);
    virtual ~StyleItemDelegate(){}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setColoumnWidget(int col, DelegateEditor wEditor);                                         //задать виджет для столбца
    void setDateTimeFormat(const QString &format){DateTimeFormat = format;}                         //задать формат отображения дата/время
    void setTimeFormat(const QString &format){TimeFormat = format;}                                 //задать формат отображения времени
    void setFrameRate(int fR) {frameRate = fR;}                                                     //задать частоту кадров для TCEdit

    void setDrawSideLines(bool bDraw) {bDrawSideLines = bDraw;}                                     //задать рисование боковых бордюров ячейки
    void setFirstColumnSelectChekboxesEnabled(bool enabled);
    void setInvisibleSelection(bool invisibleSelection);
signals:
    void sig_textEdited(const QString&);                     //сигнал об изменении текста
    void sig_itemEdit(const QModelIndex&)const;              //итем редактируется

public slots:
    void slot_commitAndCloseEditor();                        //записать данные и закрыть редактор
    void slot_commitEditorData();                            //записать данные редактора

protected:
    //кастомная отрисовка разделителя строк
    virtual bool drawLines(QPainter *painter, const QStyleOptionViewItem &, const QModelIndex &) const;
    //кастомная отрисовка DecorationPixmapRole
    virtual bool drawDecorationPixmap(const QVariant &, QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const;
    //кастомная отрисовка базового функционала делегата
    virtual bool drawDefaultPaint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    DelegateEditor getEditor(const QModelIndex &ind) const;  //получить тип редактора по индексу
    
    static void updateComboBoxStyle(QComboBox* cBox);

protected:
    QMap<int, DelegateEditor> widgetMap;                     //список заданных виджетов по столбцам
    QString DateTimeFormat = "dd.MM.yyyy hh:mm:ss";          //формат дата/время
    QString TimeFormat = "hh:mm:ss";                         //формат время
    int frameRate = 25;                                      //частота кадров
    bool bDrawSideLines;                                     //рисовать боковые бордюры у ячейки
    bool m_firstColumnSelectChekboxesEnabled = false;
    bool m_invisibleSelection = false;                      //не раскрашивать выделенную строку в хайлайт-цвет
};

#endif // STYLEITEMDELEGATE_H
