#include "BaseDialog.h"
#include <QPushButton>


BaseDialog::BaseDialog(QWidget* pwgt) : QDialog(pwgt) {
}

void BaseDialog::setBaseLayout(QLayout* layout) {
    QHBoxLayout* button_layout = new QHBoxLayout;

    QPushButton* acceptButton = new QPushButton("Ок");
    QPushButton* rejectButton = new QPushButton("Отмена");

    button_layout->addWidget(acceptButton);
    button_layout->addWidget(rejectButton);

    connect(acceptButton, SIGNAL(clicked()), SLOT(accept()));
    connect(rejectButton, SIGNAL(clicked()), SLOT(reject()));

    base_layout = new QVBoxLayout(this);
    base_layout->addLayout(layout);
    base_layout->addLayout(button_layout);
    setLayout(base_layout);

}