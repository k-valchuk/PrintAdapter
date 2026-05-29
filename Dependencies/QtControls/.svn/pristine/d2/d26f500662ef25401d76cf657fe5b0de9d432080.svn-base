#ifndef CUSTOMCOMBOBOX_H
#define CUSTOMCOMBOBOX_H

#include "qaction.h"
#include "qevent.h"
#include "qlistview.h"
#include "qstandarditemmodel.h"
#include "qtimer.h"
#include "qtreeview.h"
#include <QFrame>
#include <QLineEdit>
#include <QPropertyAnimation>

namespace Ui {
class CustomComboBoxPopup;
}

class AutoHeightListView : public QListView
{
    Q_OBJECT
public:
    AutoHeightListView(QWidget* parent = nullptr) : QListView(parent) {
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setSizeAdjustPolicy(QAbstractItemView::SizeAdjustPolicy::AdjustToContents);
    }
    
    QSize minimumSizeHint() const override {
        return sizeHint();
    }
    
    QSize viewportSizeHint() const override
    {
        using T = QListView;
        
        if (QAbstractItemView::sizeAdjustPolicy() != QAbstractScrollArea::AdjustToContents)
            return T::viewportSizeHint();
        
        if (std::is_same<T, QListView>::value)
        {
            if (model() == nullptr)
                return QSize(0, 0);
            if (model()->rowCount() == 0 || model()->columnCount() == 0)
                return QSize(0, 0);
        }
        
        if (std::is_same<T, QListView>::value)
        {
            const int rowCount = model()->rowCount();
            int height = 0;
            for (int i = 0; i < rowCount; i++) {
                height += T::sizeHintForRow(i);
            }
            return QSize(T::viewportSizeHint().width(), height);
        }
        
        return T::viewportSizeHint();
    }
};

class CustomComboBoxPopup : public QFrame
{
    Q_OBJECT
    
    bool m_nextShowAnimationEnabled = true;
    void updateMask();
    void updateSeparators();
public:
    explicit CustomComboBoxPopup(QWidget *parent = nullptr);
    ~CustomComboBoxPopup();
    
    void Show(bool animation = true) {if(!this->isVisible()) {m_nextShowAnimationEnabled = animation; this->show();}}
    void HideWithAnimation();
    
    void AddListView(AutoHeightListView* lv);
    void RemoveListView(AutoHeightListView* lv);
    
    void ScrollTo(AutoHeightListView* lv, const QModelIndex& index);
protected:
    void showEvent(QShowEvent* ev) override;
    void hideEvent(QHideEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;
private:
    Ui::CustomComboBoxPopup *ui;
};

class _CustomComboBoxParentChangeWatcher;
class CustomComboBox : public QLineEdit
{
    Q_OBJECT
    
    friend class CustomComboBoxPopup;
    friend class _CustomComboBoxParentChangeWatcher;
    
    struct ItemGroup
    {
        QVariant Group;
        QList<QStandardItem*> Items;
    };
    QList<ItemGroup> m_model;
    
    QAction* m_clearAct, *m_openAct;
    CustomComboBoxPopup* m_popup = nullptr;
    _CustomComboBoxParentChangeWatcher* m_parentChangeWatcher = nullptr;
    QPoint m_lastPosition;
    void updatePopupGeometry();
    void updateGroups();
    bool m_supressOnDataUpdate = false;
    void updateOnDataChanged();
    
    AutoHeightListView* m_checkedListView = nullptr;
    QList<AutoHeightListView*> m_orderedListViews;
    QMap<QVariant, AutoHeightListView*> m_listViews;
    
    AutoHeightListView* getListView(QVariant group, bool firstLv = false);
    void updateElidedText();
    
    QTimer  m_searchInputTimer;
    QString m_currentSearchString;
    QStandardItem* m_previouslyFoundItem = nullptr; QString m_previouslyFoundStr;
    QMap<QStandardItem*, QVariantAnimation*> m_currAnimations;
    bool findAndScroll(const QString& text);
    
    bool m_checkedItemsSeparate = true;
public:
    struct Item
    {
        QMap<Qt::ItemDataRole, QVariant> Data;
    };
    
    struct ItemId
    {
        QVariant GroupId;
        QVariant Id;
        
        bool operator==(const ItemId& other)
        {
            if(GroupId == other.GroupId && Id == other.Id)
                return true;
            return false;
        }
    };
    
    CustomComboBox(QWidget* parent = nullptr);
    ~CustomComboBox();
    
    void AddItem(Item item, ItemId id = ItemId());
    void SetCheckedItemsByUserData(const QList<ItemId>& ids);
    QList<ItemId> GetCheckedItems();
    //void RemoveItemByUserData(ItemId id);
    void Clear();
    
    void SetShowCheckedItemsSeparate(bool separate);
    
    QString GetModelText() const;
public slots:
    void ShowPopup();
private slots:
    void onTopLevelWidgetChanged();
    
protected:
    void keyPressEvent(QKeyEvent* event) override;
    
public:
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    QSize sizeHint() const override;
};

Q_DECLARE_METATYPE(CustomComboBox::ItemId)
Q_DECLARE_METATYPE(QList<CustomComboBox::ItemId>)

class _CustomComboBoxParentChangeWatcher : public QObject
{
    Q_OBJECT
    
    CustomComboBox* m_edit;
public:
    _CustomComboBoxParentChangeWatcher(CustomComboBox* edit) : m_edit(edit) {}
    
    bool eventFilter(QObject* watched, QEvent* event) override;
signals:
    void parentChanged();
};

#endif // CUSTOMCOMBOBOX_H
