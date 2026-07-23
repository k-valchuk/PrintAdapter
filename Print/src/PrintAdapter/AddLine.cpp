#include "AddLine.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QToolTip>

AddLine::AddLine(QWidget* pwgt, QString title): QGroupBox(pwgt) {
    setObjectName("addGroupBox");
    QHBoxLayout *addLineLayout = new QHBoxLayout(this);
    addLineLayout->setContentsMargins(12, 0, 14, 0);
    QLabel* rundownLabel = new QLabel(title, this);
    rundownLabel->setObjectName("baseLabel");
    addLineLayout->addWidget(rundownLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);

    addButton = new QPushButton(this);
    addButton->setObjectName("addButton");
    addButton->setMouseTracking(true); 
    addButton->installEventFilter(this); 
    connect(addButton, &QPushButton::clicked, this, [this](){
        emit addSignal();
    });
    addLineLayout->addWidget(addButton, 0, Qt::AlignRight | Qt::AlignVCenter);

}

bool AddLine::eventFilter(QObject *watched, QEvent *event) {
    if (watched == addButton && event->type() == QEvent::ToolTip) {
        QToolTip::showText(QCursor::pos(), "Создать шаблон", addButton);
        return true;
    }
    return QGroupBox::eventFilter(watched, event);
}