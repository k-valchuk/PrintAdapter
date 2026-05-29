#ifndef QFCATEGORYBOX_H
#define QFCATEGORYBOX_H

#include <QFrame>
#include "QuickFilterItem.h"
#include "DraggableFrame.h"

namespace Ui {
class QFCategoryBox;
}

class QStandardItemModel;
class QFSpoiler;
class QFCategoryBox : public DraggableFrame
{
    Q_OBJECT
    
    QFSpoiler* m_spoiler = nullptr;
    QStandardItemModel* m_model = nullptr;
    QList<QuickFilterPreset*> m_items;
    QuickFilterCategory* m_cat = nullptr;
    bool m_loadedAll = false;
    int treeViewContentHeight();
    bool Filter(QString text);
    void updateList();
    void updateName();
public:
    explicit QFCategoryBox(bool draggingEnabled, QWidget *parent = nullptr);
    ~QFCategoryBox();
    
    void SetNode(QuickFilterCategory* cat);
    void Expand(bool expand);
    QuickFilterCategory* GetNode() { return m_cat; }
    
    void ClearSelection();
private:
    Ui::QFCategoryBox *ui;
    
signals:
    void ShowMoreOrHideBtnClicked();
};

#endif // QFCATEGORYBOX_H
