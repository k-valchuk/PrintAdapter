//-------------------------------------------------------------------------------------------------
// Изменение размера колонки списка через Drag по краю ячейки
//-------------------------------------------------------------------------------------------------
#ifndef TREEVIEWCOLUMNRESIZER_H
#define TREEVIEWCOLUMNRESIZER_H

#include <QTreeView>

class TreeViewColumnResizer : public QObject
{
    Q_OBJECT
public:
    explicit TreeViewColumnResizer(QTreeView *view = 0);
    virtual ~TreeViewColumnResizer();

protected:
    bool eventFilter(QObject* object, QEvent* event) override;

private:
    QTreeView* treeView;

    bool dragInProgress;
    int columnIndex;
    int dragPreviousPos;
    QCursor viewCursor;
};

#endif // TREEVIEWCOLUMNRESIZER_H
