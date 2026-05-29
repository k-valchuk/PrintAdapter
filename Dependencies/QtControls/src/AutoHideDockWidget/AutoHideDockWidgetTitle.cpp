#include "AutoHideDockWidgetTitle.h"
#include "QHBoxLayout"
#include "QPushButton"
#include "QLabel"

AutoHideDockWidgetTitle::AutoHideDockWidgetTitle(bool bHide)
    : QFrame(nullptr), autoHideButton(nullptr)
{
    setObjectName("AutoHideDockWidgetTitle");
	QHBoxLayout* layout = new QHBoxLayout();
	setLayout(layout);
    //отступы
    layout->setContentsMargins(3, 2, 3, 2);

    textLabel = new QLabel();
    textLabel->setStyleSheet("background: transparent;");
    layout->addWidget(textLabel);

    layout->addStretch(1);

    if (bHide)
    {
        autoHideButton = new QPushButton();
        autoHideButton->setObjectName("DockAutoHideButton");
        autoHideButton->setToolTip(tr("Remove from tab"));
        layout->addWidget(autoHideButton);
        connect(autoHideButton, &QPushButton::clicked, this, &AutoHideDockWidgetTitle::sig_autoHideButton_pressed);
    }

    closeButton = new QPushButton();
    closeButton->setObjectName("DockCloseButton");
    closeButton->setToolTip(tr("Close"));
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, this, &AutoHideDockWidgetTitle::sig_closeButton_pressed);
}

AutoHideDockWidgetTitle::~AutoHideDockWidgetTitle()
{
}

void AutoHideDockWidgetTitle::setText(const QString &text)
{
    textLabel->setText(text);
}

void AutoHideDockWidgetTitle::mouseDoubleClickEvent(QMouseEvent *event)
{//когда в закладке - запрещаем переход в состоояние floating по дбл. клик
}
