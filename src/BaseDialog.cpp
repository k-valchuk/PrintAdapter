#include "BaseDialog.h"
#include <QPushButton>
#include "Config.h"
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QSizeGrip>

BaseDialog::BaseDialog(QWidget* pwgt) : QDialog(pwgt) {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    base_layout = new QVBoxLayout(this);

    base_layout->setContentsMargins(0, 0, 0, 0); 

    QFrame *titleBar = new QFrame(this);
    titleBar->setObjectName("titleBar");
    
    
    titleBar->setFixedHeight(35);
    titleBar->setStyleSheet("background-color: #2b2d32; color: #8e9ec5;");

    
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(10, 0, 5, 0); 

    QLabel *titleLabel = new QLabel("Мой Заголовок", titleBar);
    QPushButton *closeButton = new QPushButton("×", titleBar);
    closeButton->setFixedSize(30, 30);
    closeButton->setStyleSheet("QPushButton {color: #838385; border: none; font-size: 18px; } "
                               "QPushButton:hover { background-color: #3c4353; }");

    titleLayout->addWidget(titleLabel); 
    titleLayout->addStretch();         
    titleLayout->addWidget(closeButton); 
     connect(closeButton, &QPushButton::clicked, this, &QDialog::close);

    base_layout->addWidget(titleBar);
}

void BaseDialog::setBaseLayout(QLayout* layout) {    
    base_layout->addLayout(layout);
    setLayout(base_layout);
    QSizeGrip * sizeGrip = new QSizeGrip(this);
    base_layout->addWidget(sizeGrip, 0, Qt::AlignRight | Qt::AlignBottom);
}

void BaseDialog::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragCoordinate = event->globalPos() - this->geometry().topLeft();
        event->accept();
    }
}

void BaseDialog::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragCoordinate);
        event->accept();
    }
}