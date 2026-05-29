//-----------------------------------------------------------------------------------------------------------------------
//Представление списка
//-----------------------------------------------------------------------------------------------------------------------
// - По умолчанию Drop разрешен на все итемы.
// Для ограничения отпускания итемов, необходимо переопределить метод "acceptDrop" (принять или отклонить перенос).
//
//- При вставке итемов (Drop или Ctrl+V) вызывается сигнал "sig_pasteItems". Необходимо реализовать свой алгоритм вставки.
//- Включить полосатость setAlternatingRowColors(bool enable); Цвет полосатости задается в стилях.
//- Установить виджет в определенную ячейку setIndexWidget(const QModelIndex &, QWidget *).
//
//------------------------------------------------------------------------------------------------------------------------

#ifndef TREEVIEW_H
#define TREEVIEW_H
#include "customDragViewList.h"
#include "treemodel.h"
#include "headerview.h"
#include "QSortFilterProxyModel"
#include "QShortcut"
#include "QMimeData"
#include "styleItemDelegate.h"
#include "dropIndicatorProxyStyle.h"
#include <memory>
#include <QMap>
#include <functional>

class TreeViewColumnResizer;
class TreeView : public CustomDragViewList
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QTreeView)
public:

    //расположение индикатора при вставке
    enum dropIndicator
    {
        OnItem,
        AboveItem,
        BelowItem,
        OnViewport
    };

    //список без заголовка
    TreeView(int columnCount, QWidget *parent = 0);
    //список с заголовком с автосохранением/автозагрузкой настроек заголовка
    TreeView(const QVector<QVariant> &headerList, bool hEditable,
             const QString &setLocation, const QString &objName,  StyleItemDelegate *delegate= nullptr, QWidget *parent = 0);
    //список с заголовком
    TreeView(const QVector<QVariant> &headerList, bool hEditable = true, StyleItemDelegate *delegate= nullptr, QWidget *parent = 0);
    TreeView(QSortFilterProxyModel* proxyModel, const QVector<QVariant> &headerList, bool hEditable = true, StyleItemDelegate *delegate= nullptr, QWidget *parent = 0);

    virtual ~TreeView();
    bool insertRow(int position = -1, const QModelIndex &parentIndex = QModelIndex(), const QVector<QVariant> *data = nullptr);//вставить строку
    bool insertRows(int count, int position = -1, const QModelIndex &parentIndex = QModelIndex());                             //вставить строки
    bool removeRow(int row, const QModelIndex &parent);                                                                        //удалить строку
    bool removeRows(QModelIndexList &indexList);                                                                               //удалить строки
    bool insertColumn(int position = -1, const QVariant &value = QVariant());                                                  //добавить столбец
    bool removeColumn(int position);                                                                                           //удалить столбец
    bool setData(const QModelIndex &index, const QVariant& value, int role = Qt::EditRole);                                    //вставить данные в ячейку итема
    bool setDataRow(int row, const QModelIndex &parent, const QVector<QVariant> &vecData, int role = Qt::DisplayRole);         //вставить данные в строку
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;                                                 //получить данные
    QVector<QVariant> dataRow(const QModelIndex &ind, int role = Qt::DisplayRole) const;                                       //данные всех столбцов строки
    bool setHeaderData(int section, const QVariant &value, int role = Qt::EditRole);                                           //изменить данные заголовка
    void setHeaderDataList(const QVariantList &list, int role = Qt::EditRole);                                                 //
    QVariant headerData(int section, int role = Qt::DisplayRole) const;                                                        //получить данные заголовка
    QVariantList headerDataList()const;                                                                                        //список данных заголовка
    int coloumnCount()const;                                                                                                   //количество столбцов
    int rowCount(const QModelIndex &index)const;                                                                               //количество строк по индексу родителя
    QModelIndex index(int row, int col, const QModelIndex &parent) const;                                                      //получить индекс ячейки итема
    QModelIndex currentItemIndex() const;                                                                                      //индекс текущей строки
    void setSorting(bool bSort);                                                                                               //вкл/выкл сортировку
    void setTableMode();                                                                                                       //убрать отступ первой колонки (для таблицы)
    void setEditHeader(bool bEd);                                                                                              //установить возможность редактировать заголовок
    bool copyTreeIndex(const TreeView *sourceTree, const QModelIndex &sourceIndex,
                       const QModelIndex &drIndParent, int dstRow, Qt::DropAction drAct = Qt::CopyAction);                     //копировать индекс вместе с дочерними
    void dragDrop(const QModelIndexList &indList, const QModelIndex &dropIndex,
                  const QModelIndex &dropIndParent, TreeView *sourceTree, int pos, Qt::DropAction drAct);                      //перемещение индексов
    void setRowPageCount(int rpCount);                                                                                         //задать количество строк на странице
    void setCurrentPage(int curPage);                                                                                          //вывести текущую страницу
    int getPageCount() const;                                                                                                  //количество страниц
    int getCurrentPage() const{return currentPage;}                                                                            //получить текущую страницу
    QModelIndexList selectedRows()const;                                                                                       //список индексов выбранных строк    
    void clear();                                                                                                              //очистить список
    TreeModel* getSourceModel(){return sourceModel;}                                                                           //получить модель данных

    /******************Стили***************************/
    //после перерисовки веток нужен rePaint() для корректного отображения (если вкл. alternatingRowColors)
    //TODO: реализовать свои alternatingRowColors

    //задать параметры стиля для строки
    void setIndexRowStyle(const QModelIndex &index, const QColor &colorText,
                          const QColor &colorBackground, const QColor &highlightTextColor = QColor(),
                          const QFont &font = QFont(),const QFont &highlightFont = QFont());
    //задать параметры стиля для столбца
    void setIndexColoumnStyle(const QModelIndex &index, const QColor &colorText,
                              const QColor &colorBackground, const QColor &highlightTextColor = QColor(),
                              const QFont &font = QFont(),const QFont &highlightFont = QFont());
    //задать параметры стиля для индекса
    void setIndexStyle(const QModelIndex &index, const QColor &colorText,
                       const QColor &colorBackground, const QColor &highlightTextColor = QColor(),
                       const QFont &font = QFont(),const QFont &highlightFont = QFont());
    void setRowBranchGroupStyle(const QModelIndex& index, const QColor& colorBackground, const QColor& colorText, bool thisColorOnTop = true);

    void setDefaultIndexStyle(const QModelIndex &index);            //сбросить стиль индекса по умолчанию
    void setDefaultRowStyle(const QModelIndex &ind);                //сбросить стиль строки
    void setDefalutColoumnStyle(QModelIndex &index);                //сбросить стиль колонки
    void setDefaultRowBranchGroupStyle(const QModelIndex& index);

    /*************************************************/

    void insertEmptyRow(int row, const QModelIndex &parent);        //вставить пустую строку (разделитель/комментарий)
    void setDataTimeFormat(const QString &format);                  //задать формат отображения дата/время
    void setTimeFormat(const QString &format);                      //задать формат отобарежния времени
    void setDropIndicatorColor(const QColor &color);                //задать цвет drop индикатора
    void setTCFrameRate(int fR);                                    //задать частоту кадров для ввода TC

    void setColoumnWidgetDelegate(int col, DelegateEditor::WidgetID wID, const QVariant &value = QVariant()); //задать виджет делегат для колонки

    //фильтр
    void setFilterKeyColumn(int key);                   //задать колонку для фильтра
    void setFilterRegExp(const QString &pattern);       //задать фильтр регулярным выражение
    QString getFilterRegExp() const;                    //фильтр регулярного выражения

    void copyToBuffer();                                                              //операция копирования
    void pasteFromBuffer();                                                           //операция вставки
    void cutToBuffer();                                                               //операция вырезать

    void setHeader(QHeaderView *header);                                              //установить свой заголовок

    void setColumnResizer(bool bEnabled);                                             //вкл/выкл изменение колонок без заголовка (выкл - по умолчанию)
    void setCustomMimeDataTypesHandlers(QMap<QString, std::function<QVariantList(const QMimeData*)>> customMimeDataTypesHandlers);
    
    void setFirstColumnSelectChekboxesEnabled(bool enabled, bool selectAllCheckBoxInHeader = true);
    void setInvisibleSelection(bool invisibleSelection);
    
    QModelIndexList rows(QModelIndex parent);       //получить все строки от родителя
    QModelIndexList allRows(QModelIndex parent = QModelIndex());    //получить все строки от родителя рекурсивно
protected:
    virtual void keyPressEvent(QKeyEvent* event) override;                                                                  //нажатие клавиши
    virtual void dropEvent(QDropEvent *event) override;                                                                     //событие при отпускании итема
    virtual void dragEnterEvent(QDragEnterEvent *event) override;                                                           //событие при пересечении границы виджета
    virtual bool acceptDrop(const QVariantList &, QObject *, const QModelIndex &, DropIndicatorPosition){return true;}      //разрешение на ставку
    //перерисовка веток
    virtual void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const override;

    HeaderView *headerView;                                      //заголовок

private:

    QModelIndexList indexListFromSource(const QModelIndexList &sourceIndexList) const;//перевод индексов модели источника к индексам proxy модели
    void hideRow(int row, const QModelIndex &parentIndex);                            //скрыть строку
    void showRow(int row, const QModelIndex &parentIndex);                            //показать строку
    void saveSettings()const;                                                         //сохранить настройки
    void loadSettings();                                                              //загрузить настройки
    void rowsHide(int startRow, int count, bool bHide);                               //скрыть строки
    void setDataToBuffer(Qt::DropAction act = Qt::CopyAction);                        //установить данные в буфер с флагом перемещения
    QAbstractItemModel* getModel() const {return sortFilterModel ? (QAbstractItemModel*)sortFilterModel : sourceModel;}

    TreeModel *sourceModel;                                      //модель данных
    QSortFilterProxyModel *sortFilterModel;                      //модель для сортировки
    int currentPage = 1;                                         //текущая страница
    int rPageCount = -1;                                         //количество строк на странице, (-1 не разбивать на страницы)
    StyleItemDelegate *styleItemDelegate;                        //делегат стилей
    DropIndicatorProxyStyle *dropIndicatorStyle;                 //стиль drop индикатора
    QString settingsFile;                                        //файл настроек
    bool bAutoSaveSettings = false;                              //автосохранение настроек
    std::unique_ptr<TreeViewColumnResizer> columnResizer;        //объект для изменения размера колонок
    QMap<QString, std::function<QVariantList(const QMimeData*)>>
                                    customMimeDataTypesHandlers; //обработчики кастомных типов mimeData, ключ - тип mimeData
    bool m_firstColumnSelectChekboxesEnabled = false;            //рисование доп-чекбоксов для выделения строк в первом столбике
    bool m_invisibleSelection = false;                           //не раскрашивать выделенную строку в хайлайт-цвет
private slots:
    void slot_editHeader(int pos, const QString &hData);         //изменение заголовка пользователем
    void slot_textEdited(const QString &text);                   //изменен текст в делегате
    void slot_dataEditingFinished(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);  //данные изменены пользователем
signals:
    //сигнал о вставке итемов
    void sig_pasteItems(const QVariantList& indexList,            //индексы строк, котрые переносим
                        QObject *source,                          //источник переноса
                        const QModelIndex& dropIndex,             //индекс, куда отпускаем
                        TreeView::dropIndicator drPos = BelowItem,//позиция индикатора для вставки
                        Qt::DropAction drAct = Qt::CopyAction,    //действие
                        bool bFromClipboard = true);              //вставка из буфера

    void sig_dataEditingFinished(const QModelIndex &);              //данные ячейки изменены
    void sig_dataEditingAccepted(const QModelIndex &);              //данные ячейки изменены и подтвержедены (предыдущий сигнал выдается даже если был нажат esc, отменяющий редактирование)
    void sig_textChanged(const QModelIndex &, const QString &);     //изменен текст в делегате
    void sig_selectionChanged();                                    //сигнал об изменении выделения
    void sig_itemEdit(const QModelIndex&);                          //итем редактируется
    void sig_itemsCopiedToBuffer(const QModelIndexList &indList);   //итемы скопированы в буфер
};

//Данные для буфера обмена (copy/paste)
class TreeMimeData : public QMimeData
{
    Q_OBJECT
    QModelIndexList indexList;
    Qt::DropAction dropAction = Qt::CopyAction;
public:
    void setIndexList(const QModelIndexList& itL) {indexList = itL;}
    QModelIndexList getIndexList()const {return indexList;}

    void setDropAction(Qt::DropAction act) {dropAction = act;}
    Qt::DropAction getDropAction() const {return dropAction;}
};

#endif // TREEVIEW_H
