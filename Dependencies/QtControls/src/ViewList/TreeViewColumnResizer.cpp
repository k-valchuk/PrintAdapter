#include "TreeViewColumnResizer.h"
#include "QMouseEvent"
#include "QHeaderView"

constexpr int ResizeSensibility = 5;

TreeViewColumnResizer::TreeViewColumnResizer(QTreeView *view) :
  QObject(view), treeView(view), dragInProgress(false), columnIndex(-1)
{
  treeView->viewport()->installEventFilter(this);
  treeView->viewport()->setMouseTracking(true);
}

TreeViewColumnResizer::~TreeViewColumnResizer()
{
}

bool TreeViewColumnResizer::eventFilter(QObject* object, QEvent* event)
{
    if (object == treeView->viewport())
    {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event)
        {
            if (mouse_event->type() == QEvent::MouseMove)
            {
                if (!dragInProgress)
                { // поиск вхождения изменения размера

                    auto colLeft = treeView->columnAt(mouse_event->pos().x() - ResizeSensibility);
                    auto colRight = treeView->columnAt(mouse_event->pos().x() + ResizeSensibility);
                    bool wasOnBoundary = columnIndex != -1;
                    if (colLeft != colRight && treeView->header()->sectionResizeMode(colLeft) == QHeaderView::Interactive)
                    {
                        columnIndex = colLeft;
                    }
                    else
                    {
                        columnIndex = -1;
                    }

                    bool isOnBoundary = columnIndex != -1;
                    if (isOnBoundary != wasOnBoundary)
                    {// обновить курсор
                        if (isOnBoundary)
                        {
                            viewCursor = treeView->viewport()->cursor();
                            treeView->viewport()->setCursor(Qt::SplitHCursor);
                        }
                        else
                        {
                          treeView->viewport()->setCursor(viewCursor);
                        }
                    }
                }
                else
                { // изменить размер

                    int delta = mouse_event->pos().x() - dragPreviousPos;
                    auto header_view = treeView->header();
                    dragPreviousPos = mouse_event->pos().x();
                    header_view->resizeSection(columnIndex, qMax(ResizeSensibility * 2, header_view->sectionSize(columnIndex) + delta));
                    return true;
                }
            }
            else if (mouse_event->type() == QEvent::MouseButtonPress && mouse_event->button() == Qt::LeftButton && !dragInProgress)
            { // начать изменение размера

                if (columnIndex != -1)
                {
                    dragInProgress = true;
                    dragPreviousPos = mouse_event->x();
                    return true;
                }
            }
            else if (mouse_event->type() == QEvent::MouseButtonRelease && mouse_event->button() == Qt::LeftButton && dragInProgress)
            { // остановить изменение размера

                dragInProgress = false;
                return true;
            }
        }
    }

    return false;
}
