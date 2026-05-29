//--------------------------------------------------------------------------------
//Заголовок списка
//--------------------------------------------------------------------------------
#ifndef HEADERVIEW_H
#define HEADERVIEW_H
#include "QHeaderView"
#include "QProxyStyle"

//---------------------------------------
//Стиль заголовка
//
// - позиция иконки
//----------------------------------------
class HeaderProxyStyle : public QProxyStyle
{
public:
    HeaderProxyStyle(Qt::Alignment align = Qt::AlignCenter, QStyle *style = 0);
    void drawControl(ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = nullptr) const override;

private:
    Qt::Alignment iconAlignment;
};

class QCheckBox;
class HeaderView : public QHeaderView
{
    Q_OBJECT
    
    bool m_customSeparatorsEnabled = false;
    QColor m_customSeparatorsColor = QColor::fromRgba(qRgba(0, 0, 0, 0));
    qreal m_customSeparatorsOpacity = 0.6;
    qreal m_customSeparatorsWidth = 1.5;
    Q_PROPERTY(bool EnableCustomSeparators WRITE setEnableCustomSeparators READ isCustomSeparatorsEnabled);
    Q_PROPERTY(QColor CustomSeparatorsColor WRITE setCustomSeparatorsColor READ getCustomSeparatorsColor);
    Q_PROPERTY(qreal CustomSeparatorsOpacity WRITE setCustomSeparatorsOpacity READ getCustomSeparatorsOpacity);
    Q_PROPERTY(qreal CustomSeparatorsWidth WRITE setCustomSeparatorsWidth READ getCustomSeparatorsWidth);
public:
    HeaderView(Qt::Orientation orientation, bool edit, QWidget *parent = nullptr);
    ~HeaderView();

    //создать контекстное меню
    void createContextMenu(const QVariantList &headerList, const QVector<int> &idlist = {});
    //добавить итем контекстного меню
    void addContextAction(int pos, const QString &nameAct, int idCol = -1);

    void removeContextAction(int pos);                          //удалить итем контекстного меню
    bool clearContextActionList();                              //очистить список контекстного меню
    void setHeaderEditorVisible(bool bV);                       //установить возможность редактирования заголовка
    bool editable(){return bEditable;}                          //разрешено ли редактирование
    void updateContextActionsState();                           //обновить состояние контекстного меню (выбранные ячейки)
    int getCurrentSectionPos()const {return currentSectionPos;} //текущий выбранный заголовок
    void setFirstColumnSelectChekboxesEnabled(bool enabled);
    void setFirstColumnCheckboxChecked(Qt::CheckState checked);
    
    void setEnableCustomSeparators(bool enable);
    bool isCustomSeparatorsEnabled();
    void setCustomSeparatorsColor(QColor color);
    QColor getCustomSeparatorsColor();
    void setCustomSeparatorsOpacity(qreal opacity);
    qreal getCustomSeparatorsOpacity();
    void setCustomSeparatorsWidth(qreal width);
    qreal getCustomSeparatorsWidth();
    
signals:
    void sig_editHeader(int pos, const QString &hData);         //заголовок был изменен пользователем
    void sig_sectionHidden(int index, bool isHide);             //столбец скрыт/отображен
    
    void sig_selectAll(bool selectOrDeselect);
protected:
    void mousePressEvent(QMouseEvent *e) override;              //отловить вызов контекстного меню
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    void mouseMoveEvent(QMouseEvent* e) override;
private slots:
    void slot_actionTriggered(bool checked);                    //нажатие на итем контекстного меню
    void slot_editHeader();                                     //изменение заголовка пользователем
    void slot_showContextMenu(const QPoint &pos);               //показать контекстное меню

private:
    int currentSectionPos;                                      //позиция выбранного столбца заголовка
    QAction *actSeparator;                                      //разделитель в контекстном меню
    QAction *actHeaderEditor;                                   //редактор заголовка в контекстном меню
    bool bEditable;                                             //возможность редактировать заголовок
    QMenu *contextMenu;                                         //контекстное меню
    bool m_firstColumnSelectChekboxesEnabled = false;           //рисовать ли чекбокс для выделить/отменить всё в 1 столбце
    Qt::CheckState m_isChecked = Qt::Unchecked;                 //состояние чекбокса для выделения/отмены выделения всех
    bool m_cursorWasOnCheckbox = false;
    
    void setCurrentSectionPos(int pos);                         //установить позицию текущего столбца (для изменения заголовка)
    void initCheckboxStyleOption(QStyleOptionViewItem* option) const;
};

#endif // HEADERVIEW_H
