#include "BaseDialog.h"
#include <QPushButton>
#include "Config.h"
#include <QLabel>
#include <QMouseEvent>
#include <QSizeGrip>

BaseDialog::BaseDialog(QWidget* pwgt, QString title) : QDialog(pwgt) {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    base_layout = new QVBoxLayout(this);

    titleBar = new QFrame(this);
    titleBar->setObjectName("titleBar");
    titleBar->setContentsMargins(30, 20, 20, 15);
    
    
    titleBar->setStyleSheet("background-color: #2b2d32; box-shadow: 0px 0px 10px #0000005C;");
    setStyleSheet("background-color: #2b2d32;");
    
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);

    QLabel *titleLabel = new QLabel(title, titleBar);
    titleLabel->setStyleSheet("font: normal normal normal 18px/22px Roboto; color: #6F8CB7;");
    QPushButton *closeButton = new QPushButton(titleBar);
    closeButton->setMinimumSize(16, 16);
    closeButton->setIcon(QIcon(":/icons/modal-close-icon.svg"));
    closeButton->setStyleSheet(
        "QPushButton{background: transparent;}"
        "QPushButton:hover{background: rgba(100, 100, 100, 50); margin: -10px;}"
    );

    titleLayout->addWidget(titleLabel); 
    titleLayout->addStretch();         
    titleLayout->addWidget(closeButton); 
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);

    base_layout->addWidget(titleBar);
}

void BaseDialog::setBaseLayout(QLayout* layout) {    
    base_layout->addLayout(layout);
    setLayout(base_layout);
    QSizeGrip* sizeGrip = new QSizeGrip(this);
    base_layout->addWidget(sizeGrip, 0, Qt::AlignRight | Qt::AlignBottom);
}

void BaseDialog::mousePressEvent(QMouseEvent *event) {
    if (event->button() & Qt::LeftButton) {
        
        QWidget* widgetAtPos = childAt(event->pos());
        if (qobject_cast<QSizeGrip*>(widgetAtPos)) {
            return; 
        }
        
        if (widgetAtPos == titleBar || titleBar->findChildren<QWidget*>().contains(widgetAtPos)) {
            m_dragged = true;
            dragCoordinate = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }
}

void BaseDialog::mouseMoveEvent(QMouseEvent *event) {
    if (m_dragged && (event->buttons() & Qt::LeftButton)){
        move(event->globalPos() - dragCoordinate);
        event->accept();
    }
}

void BaseDialog::mouseReleaseEvent(QMouseEvent *event){
    m_dragged = false;
}