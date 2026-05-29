//----------------------------------------------------------------------
//Итем списка
//----------------------------------------------------------------------
#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include "QColor"
#include "QFont"

class TreeItem
{
public:
    //пользовательские роли
    enum UserRoles{
        HighlightColorRole = Qt::UserRole + 1,      //цвет текста выбранного итема
        HighlightFontRole,                          //шрифт выбранного итема
        FlagsRole,                                  //флаги (ItemsFlags)
        TypeEditorRole,                             //тип редактора
        DecorationPixmapRole,                       //масштабируемая картинка (QIcon | QPixmap)
        DecorationAlignmentRole,                    //выравнивание DecorationRole, данные типа QVariantList(QStyleOptionViewItem::Position, Qt::Alignment),
        GroupBackgroundColorRole,                   //цвет фона только до tree position
        GroupTextColorRole,                         //цвет текста только до tree position
        GroupBgOnTopRole,                           //bool - заливается ли предыдущий цвет поверх обычного BackgroundColorRole или под ним
        DisableSelectionCheckboxRole,               //true чтобы явно отключить selectionCheckbox если такой режим включен для TreeView
        RowSpannedRole,                             //кастомный механизм цельной spanned-строки, потому что КутешНый кривой (не рисует, если например 0й столбец скрыт)
        RowSpannedDataColumn                        //int - из какой колонки брать данные для кастомной заспанненой строки
    };

    explicit TreeItem(const QVector<QVariant> &data, TreeItem *parent = 0);
    ~TreeItem();

    TreeItem *child(int number);                                                              //дочерний итем
    int childCount() const;                                                                   //количество дочерних итемов
    int childNumber() const;                                                                  //номер итема относительно родителя
    int columnCount() const;                                                                  //количество столбцов в итеме
    QVariant data(int column, int role = Qt::DisplayRole) const;                              //данные итема
    bool insertChildren(int position, int rowCount, int colCount);                            //вставить дочерние итемы
    bool insertColumns(int position, int colCount);                                           //вставить столбцы
    TreeItem *parent();                                                                       //родительский итем
    bool removeChildren(int position, int rowCount);                                          //удалить дочерние итемы
    bool removeColumns(int position, int colCount);                                           //удалить столбцы
    bool setData(int column, const QVariant &value, int role = Qt::DisplayRole);              //задать данные итема

    //флаги по умолчанию
    static Qt::ItemFlags defaultFlags();

private:
    QList<TreeItem*> childItems;                //список дочерних итемов

    QVector<QVariant> itemData;                 //данные итема
    QVector<QVariant> userData;                 //данные пользователя
    QVector<QVariant> colorTextData;            //данные цвета текста
    QVector<QVariant> colorBackgroundData;      //данные цвета фона
    QVector<QVariant> fontData;                 //данные шрифта
    QVector<QVariant> highlightColorData;       //данные цвета текста выделенных строк
    QVector<QVariant> highlightFontData;        //данные шрифта выделенных строк
    QVector<QVariant> flagsData;                //флаги
    QVector<QVariant> alignmentTextData;        //выравнивание текста по умолчанию AlignLeft
    QVector<QVariant> decorationData;           //данные с иконками
    QVector<QVariant> editorTypeData;           //тип редактора данных
    QVector<QVariant> decorationPixData;        //картинки
    QVector<QVariant> decorationAlignmentData;  //выравнивание decorationRole
    QVector<QVariant> checkableData;            //данные checkbox
    QVector<QVariant> toolTipData;              //данные подсказки
    QVariant groupBgColorData;                  //данные фона для итемов до treePosition
    QVariant groupTextColorData;                //данные текста для итемов до treePosition
    bool groupBgOnTopData = true;               //заливается ли предыдущий цвет поверх обычного BgRole или под ним
    bool disableSelectionCheckbox = false;
    bool isSpanned = false;
    int  spannedDataColumn = 0;

    TreeItem *parentItem;                       //родительский итем
};

#endif // TREEITEM_H
