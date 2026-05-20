#include "AddLine.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>

AddLine::AddLine(QWidget* pwgt, QString title): QGroupBox(pwgt) {
    setObjectName("addGroupBox");
    QHBoxLayout *addLineLayout = new QHBoxLayout(this);
    QLabel* rundownLabel = new QLabel(title, this);
    rundownLabel->setObjectName("baseLabel");
    addLineLayout->addWidget(rundownLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);

    QPushButton* addButton = new QPushButton(this);
    addButton->setIcon(QIcon(":/icons/add-button.svg"));
    addButton->setObjectName("addButton");
    connect(addButton, &QPushButton::clicked, this, [this](){
        emit addSignal();
    });
    addLineLayout->addWidget(addButton, 0, Qt::AlignRight | Qt::AlignVCenter);

}