#include "RemoveConfirmDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

RemoveConfirmDialog::RemoveConfirmDialog(QWidget* pwgt, QString templateName): BaseDialog(pwgt, "Удалить шаблон печати") {
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Вы действительно хотите удалить шаблон печати \"<b>" + templateName + "</b>\"?", this);
    label->setObjectName("baseLabel");
    baseLayout->addWidget(label);
    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setContentsMargins(160, 90, 0, 0);
    QPushButton* cancelButton = new QPushButton("Отмена", this);
    cancelButton->setObjectName("cancelButton");
    
    connect(
        cancelButton, SIGNAL(clicked()),
        this, SLOT(close())
    );
    
    QPushButton* chooseButton = new QPushButton("Удалить");
    chooseButton->setObjectName("applyButton");

    connect(
        chooseButton, SIGNAL(clicked()),
        this, SLOT(accept())
    );

    button_layout->addWidget(cancelButton);
    button_layout->addWidget(chooseButton);
    baseLayout->addLayout(button_layout);

    setBaseLayout(baseLayout);

}