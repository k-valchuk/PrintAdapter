#ifndef FILTERSLISTLISTVIEW_H
#define FILTERSLISTLISTVIEW_H

#include <QListView>
#include <QKeyEvent>

class FiltersListListView : public QListView
{
    Q_OBJECT
    
public:
    FiltersListListView(QWidget* parent = nullptr)
        : QListView(parent) {}
    
protected:
    void keyPressEvent(QKeyEvent* event) override
    {
        auto indexes = this->selectedIndexes();
        
        if(event->key() == Qt::Key_Up && indexes.size() == 1 && indexes.first().row() == 0)
        {
            emit moveToEdit();
            return;
        }
        
        QListView::keyPressEvent(event);
        
        indexes = this->selectedIndexes();
        
        if((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) && indexes.size() == 1)
        {
            emit clicked(indexes.first());
            return;
        }
    }
    
signals:
    void moveToEdit();
};

#endif // FILTERSLISTLISTVIEW_H
