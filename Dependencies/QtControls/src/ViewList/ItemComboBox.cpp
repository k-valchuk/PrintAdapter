#include "ItemComboBox.h"
#include "qabstractitemview.h"
#include "QKeyEvent"

ItemComboBox::ItemComboBox(QWidget *parent) : QComboBox(parent)
{
    view()->installEventFilter(this);
    view()->viewport()->installEventFilter(this);
}

bool ItemComboBox::eventFilter(QObject *object, QEvent *event)
{
    if (object == view())
    {
        switch (event->type())
        {
            case QEvent::ShortcutOverride:
                switch (static_cast<QKeyEvent*>(event)->key())
                {
                    case Qt::Key_Enter:
                    case Qt::Key_Return:
                    setCurrentIndex(view()->currentIndex().row());
                    emit sig_activated(currentIndex());
                    return false;
                }
            break;

            case QEvent::FocusOut:
                emit sig_hideView();
            return false;
        }
    }
    else if (object == view()->viewport() && event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *m = static_cast<QMouseEvent *>(event);
        if (view()->rect().contains(m->pos()))
        {
            setCurrentIndex(view()->currentIndex().row());
            emit sig_activated(currentIndex());
            return false;
        }
    }
    return QComboBox::eventFilter(object, event);
}
