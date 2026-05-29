#ifndef FILTERROW_H
#define FILTERROW_H

#include <QFrame>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include "IFilterElement.h"
#include "FilterEllipse.h"
#include "FlowLayout.h"
#include "FilterExpressions.h"
#include <QApplication>
#include "FilterRowNode.h"

namespace Ui {
class FilterRow;
}

class BracketLabel : public QLabel
{
    Q_OBJECT
    
public:
    BracketLabel(QString text, QWidget* parent = nullptr) :
        QLabel(parent)
    {
        if(text == "(")
            this->setPixmap(QIcon(":/filter/(").pixmap(30, 30));
        else if(text == ")")
            this->setPixmap(QIcon(":/filter/)").pixmap(30, 30));
        else
            this->setText(text);
        this->setMinimumHeight(30);
        this->setMaximumHeight(30);
    }
};

class OperatorButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(bool myHover READ myHover WRITE setMyHover NOTIFY myHoverChanged)
    
    using Super = QPushButton;
    
    bool m_customHover;
    
    bool m_myHover = false;
public:
    OperatorButton(QString text, bool customHover = true, QWidget* parent = nullptr)
        : Super(parent), m_customHover(customHover)
    {
        this->SetText(text);
        
        this->setMinimumHeight(30);
        this->setMaximumHeight(30);
        this->setMinimumWidth(40);
        this->setMaximumWidth(40);
        
        if(m_customHover)
            this->setCursor(Qt::CursorShape::PointingHandCursor);
    }
    
    bool myHover()
    {
        return m_myHover;
    }
    void setMyHover(bool isHover)
    {
        m_myHover = isHover;
        style()->polish(this);
        emit myHoverChanged(m_myHover);
    }
    void setMyHoverSilent(bool isHover)
    {
        m_myHover = isHover;
        style()->polish(this);
    }
    
    void SetText(QString text)
    {
        if(text == "|")
        {
            auto font = this->font();
            font.setBold(true);
            font.setPixelSize(18);
            this->setFont(font);
        }
        else if(text == "&&")
        {
            auto font = this->font();
            font.setBold(false);
            font.setPixelSize(16);
            this->setFont(font);
        }
        this->setText(text);
    }
public:
    bool event(QEvent* event) override
    {
        if(m_customHover)
        {
            switch (event->type())
            {
            case QEvent::HoverEnter:
                setProperty("myHover", true);
                //return true;
                break;
            case QEvent::HoverLeave:
                setProperty("myHover", false);
                //return true;
                break;
            case QEvent::HoverMove:
                //return true;
                break;
            default:
                break;
            }
        }
        return Super::event(event);
    }
    
signals:
    void myHoverChanged(bool hover);
    
};

class FilterRow : public QFrame
{
    Q_OBJECT
    
public:
    struct FilterRowConfig
    {
        bool AllowSplittingToGroups;
        bool AllowOperatorChange;
        bool IsTogglable;
        bool AddableLineEdit;
        bool OnlyEllipsesVisible;
        
        FilterRowConfig(bool allowSplittingToGroups, bool allowOperatorChange,
                        bool isTogglable = false,
                        bool addableLineEdit = false,
                        bool onlyEllipsesVisible = false)
        {
            AllowSplittingToGroups = allowSplittingToGroups;
            AllowOperatorChange = allowOperatorChange;
            IsTogglable = isTogglable;
            AddableLineEdit = addableLineEdit;
            OnlyEllipsesVisible = onlyEllipsesVisible;
        }
    };
    
private:
    FilterRowConfig m_config;
    bool m_toggled = false, m_wasNull = false;
    
    QList<QSharedPointer<IFilterElement>> m_orderedFilters;
    QMap<QVariant, QSharedPointer<IFilterElement>> m_filters;
    QMap<QVariant, bool> m_favouriteFilters;
    bool m_onlyFavourites = false;
    
    QList<QColor> m_colors; int m_colorIndex = 0;
    
    RootExpression m_root;
    QHash<FilterEllipse*, FilterExpression*> m_widgetToExp;
    QHash<OperatorButton*, IExpression*> m_btnToExp;
    
    FlowLayout* m_elementsContainer;
    
    QVector<QVariant> m_allFieldsAllowedOps;
    QSharedPointer<IFilterElement> m_allFieldsFilter;
    QLineEdit* m_lineEdit;
    QHBoxLayout* m_lineEditLayout = nullptr;
    QList<QWidget*> m_rowLayoutItems;
    bool m_searchOnEnter = true;
    void toLineEdit();
    void toEllipsesRow();
    
    QSharedPointer<IFilterElement> m_deletedBeforeLineEditFilterType = nullptr;
    QVariant m_deletedBeforeLineEditFilterOp = QVariant();
    
    QList<FilterEllipse*> EllipseWidgetsInRow();
    bool isToLineEditAllowed();
    
    FilterEllipse* CreateFilterEllipse();
    OperatorButton* CreateOperatorButton(QString text);
    void setup();
    
    FilterRowNode::FillingFunctors m_fillingFunctors;
public:
    explicit FilterRow(QWidget* parent = nullptr);
    explicit FilterRow(FilterRowConfig config, QWidget* parent = nullptr);
    explicit FilterRow(QList<QSharedPointer<IFilterElement>> filters, QSharedPointer<IFilterElement> allFieldsFilter = nullptr, FilterRowConfig config = FilterRowConfig(true, true), QWidget *parent = nullptr);
    ~FilterRow();
    
    void SetAvailableFiltersList(QList<QSharedPointer<IFilterElement>> filters);
    void SetAllFieldsFilter(QSharedPointer<IFilterElement> allFieldsFilter = nullptr);
    void SetAllFieldsAllowedOps(QVector<QVariant> allFieldsAllowedOps);
    
    FilterRowConfig Config() {return m_config;}
    
    int SizeOfFilters();
    int WholeSize();
    int GetHeightForWidth(int width);
    
    QList<QSharedPointer<IFilterElement>> GetOrderedFilters() {
        return m_orderedFilters;
    }
    QMap<QVariant, QSharedPointer<IFilterElement>> GetFilters() {
        return m_filters;
    }
    QMap<QVariant, bool> GetFavouriteFilters() {
        return m_favouriteFilters;
    }
    void SetFavourite(QVariant filterKey, bool isFavourite) {
        m_favouriteFilters[filterKey] = isFavourite;
    }
    void SetFilterListMode(bool isOnlyFavourites)
    {
        m_onlyFavourites = isOnlyFavourites;
    }
    void SetSearchOnEnter(bool searchOnEnter){m_searchOnEnter = searchOnEnter;}
    bool IsOnlyFavouritesList()
    {
        return m_onlyFavourites;
    }
    
    void AddCustomButton(QWidget* button);
    void RemoveCustomButton(QWidget* button);
    void SetSearchButtonVisible(bool visible);
    
    QSharedPointer<IFilterElement> AllFieldsFielter()
    {
        return m_allFieldsFilter;
    }
    
    QVector<QVariant> AllFieldsAllowedOperators()
    {
        return m_allFieldsAllowedOps;
    }
    
    RootExpression* RootExpr()
    {
        return &m_root;
    }
    
    bool IsEmpty()
    {
        return m_root.GetChild() == nullptr;
    }
    
    QSharedPointer<IExpression> GetCurrentFilter()
    {
        return QSharedPointer<IExpression>(m_root.CopyOfChild());
    }
    
    void restoreFromJson(QJsonObject obj);
    void restoreFromTree(IExpression* exp);
private:
    Ui::FilterRow *ui;
    
    void InitLineEdit();
    void emitFilterChanged();
private slots:
    void onFocusChanged(QWidget* old, QWidget* now);
    void debugPrintTree();
public slots:
    FilterEllipse* addFilterElement();
    void removeFilterElement(FilterEllipse* element);
    void clearFilter();
    void search();
protected:
    void resizeEvent(QResizeEvent* event) override;
    
signals:
    void searchClicked();
    void treeChanged();
    void filterChanged(QSharedPointer<IExpression> exp);
};

#endif // FILTERROW_H
