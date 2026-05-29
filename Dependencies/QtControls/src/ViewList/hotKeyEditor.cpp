#include "hotKeyEditor.h"
#include "QHBoxLayout"
#include "QPushButton"
#include "QLineEdit"
#include "QKeyEvent"
HotKeyEditor::HotKeyEditor(QWidget *parent) : QWidget(parent)
{
    setObjectName("ViewEditor");
    //текстовое поле
    hkEdit = new QLineEdit();
    hkEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    hkEdit->installEventFilter(this);
    QFont editFont(font().family(), font().pointSize());
    hkEdit->setFont(editFont);
    hkEdit->setContextMenuPolicy(Qt::NoContextMenu);

    //завершить редактирование
    connect(hkEdit, SIGNAL(editingFinished()),this, SIGNAL(sig_editingFinished()));

    //кнопка очистить
    clearBP = new QPushButton();
    clearBP->setIcon(QIcon(":/images/close"));
    clearBP->setFlat(true);
    clearBP->setMaximumSize(24, 24);
    clearBP->setFocusPolicy(Qt::NoFocus);
    clearBP->setObjectName("no_hover");
    //очистить текст по кнопке
    connect(clearBP, SIGNAL(clicked(bool)), hkEdit, SLOT(clear()));

    //компоновщик
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(hkEdit);
    mainLayout->addWidget(clearBP);
    setLayout(mainLayout);

    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(hkEdit);
}

void HotKeyEditor::setKey(const QString &key)
{
    hkEdit->setText(key);
}

QString HotKeyEditor::getKey() const
{
    return hkEdit->text();
}

bool HotKeyEditor::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == hkEdit && event->type() == QEvent::KeyPress)//отловить нажатие клавиш
    {
        QKeyEvent *keyEvent = (QKeyEvent *)event;
        Qt::Key key = static_cast<Qt::Key>(keyEvent->key());
        int resKey = keyEvent->nativeVirtualKey();
        //перевод в латиницу
        switch (resKey)
        {
            case Qt::Key_Agrave:
                                        resKey = Qt::Key_QuoteLeft;
                                        break;
            case Qt::Key_Ucircumflex:
                                        resKey = Qt::Key_BracketLeft;
                                        break;
            case Qt::Key_Yacute:
                                        resKey = Qt::Key_BracketRight;
                                        break;
            case Qt::Key_masculine:
                                        resKey = Qt::Key_Semicolon;
                                        break;
            case Qt::Key_THORN:
                                        resKey = Qt::Key_Apostrophe;
                                        break;
            case Qt::Key_questiondown:
                                        resKey = Qt::Key_Slash;
                                        break;
            case Qt::Key_onequarter:
                                        resKey = Qt::Key_Comma;
                                        break;
            case Qt::Key_threequarters:
                                        resKey = Qt::Key_Period;
                                        break;
            default:
                                        if (!(resKey >= (int)Qt::Key_A && resKey <= (int)Qt::Key_Z))
                                            resKey = keyEvent->key();
        }

        //отсеить модификаторы без клавиш
        if(!(key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt || key == Qt::Key_Meta || key == Qt::Key_unknown))
            hkEdit->setText(QKeySequence(resKey + keyEvent->modifiers()).toString());

        return true;
    }

    return QWidget::eventFilter(obj, event);
}

void HotKeyEditor::resizeEvent(QResizeEvent *)
{
    clearBP->setIconSize(clearBP->size());//изменить размер иконки
}
