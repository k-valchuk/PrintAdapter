#ifndef QUICKFILTERPANEL_H
#define QUICKFILTERPANEL_H

#include <QFrame>
#include "QuickFilterItem.h"
#include "QuickFilterTabButtons.h"
#include <QStandardItemModel>
#include <QSplitter>
#include "FilterExpressions.h"
#include "FilterPresetItem.h"
#include "ReordarableVLayout.h"
#include "FilterPresetRow.h"
#include "QFCategoryBox.h"

namespace Ui {
class QuickFilterPanel;
}

class QuickFilterPanelSplitter : public QSplitter
{
    Q_OBJECT
    
public:
    QuickFilterPanelSplitter(QWidget* parent = nullptr)
        : QSplitter(parent)
    {
        
    }
    
    void MoveSplitter(int pos, int index)
    {
        this->moveSplitter(pos, index);
    }
};

class QuickFilterTabButtons;

class QuickFilterPanel : public QFrame
{
    Q_OBJECT
    
    QuickFilterRoot m_favRoot;
    QHash<QuickFilterCategory*, QFCategoryBox*> m_catToExpander;
    QuickFilterTabButtons* m_tabWgt = nullptr;
    QJsonObject m_quickFilterCache;
    bool m_connectedToSplitter = false;
    ReordarableVLayout* m_quickFilterVLayout;
    
    QuickFilterPanelSplitter* m_splitter = nullptr;
    int m_lastNonZeroSplitterPosL = 0;
    int m_lastNonZeroSplitterPosR = 0;
    bool m_wasCollapsed = false;
    
    int m_selectedFavouritesCount = 0;
    
    FilterPresetGroup m_presetsRoot;
    
    FilterPresetGroup m_historyRoot;
    
    static void forAllRows(QStandardItem* parent, std::function<void(QStandardItem*)> action)
    {
        action(parent);
        for(int row = 0; row < parent->rowCount(); row++)
            forAllRows(parent->child(row), action);
    }
    
    QSharedPointer<IExpression> GetFilterFormedOnlyFromCategories(bool all = true, const QSet<QuickFilterCategory*>& parentCategories = {});
    
    FilterPreset* m_selectedPreset = nullptr;
public:
    explicit QuickFilterPanel(QWidget *parent = nullptr);
    ~QuickFilterPanel();
    
    void SetOtherQuickFilterTabWgt(QuickFilterTabButtons* tabWgt);
    
    QuickFilterRoot* FavouritesRoot()
    {
        return &m_favRoot;
    }
    FilterPresetGroup* PresetsRoot()
    {
        return &m_presetsRoot;
    }
    
    FilterPresetGroup* HistoryRoot()
    {
        return &m_historyRoot;
    }
    
    void SetVisibility(bool quick, bool presets, bool history);
    
    //формирует дерево только из тех узлов фильтра, которые
    //лежат под переданными в аргумент группами фильтров
    //это может быть полезно чтобы отделить, например, ту часть фильтра
    //которая должна обрабатываться сервером по другой логике (buckets из elasticsearch)
    QSharedPointer<IExpression> GetFilterFormedOnlyFromCategories(const QSet<QuickFilterCategory*>& parentCategories);
private:
    Ui::QuickFilterPanel *ui;
    
private slots:
    void onItemChanged(bool supressFilterUpdate);
    void onPageChanged(int i);
    
signals:
    void QuickFilterChanged(QSharedPointer<IExpression> exp);
    
    void FilterPresetApplied(QSharedPointer<IExpression> expForFilterRow);
    void FilterPresetDeleteRequested(IFilterPresetItem* item);
    void FilterPresetsOrderChanged();
    void FilterPresetEdited(FilterPreset* item, QString newName, QSharedPointer<IExpression> newExp);
    void SavePresetClicked(QSharedPointer<IExpression> exp);
    void QFCategoryShowOrHideButtonPressed(QuickFilterCategory* cat);
protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
};


#endif // QUICKFILTERPANEL_H
