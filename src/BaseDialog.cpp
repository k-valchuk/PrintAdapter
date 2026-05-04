#include "BaseDialog.h"
#include <QPushButton>
#include "Config.h"
#include <QLabel>
#include <QDirIterator>
#include <QMouseEvent>
#include <QSizeGrip>

BaseDialog::BaseDialog(QWidget* pwgt, QString title) : QDialog(pwgt) {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    
    base_layout = new QVBoxLayout(this);

    titleBar = new QFrame(this);
    titleBar->setObjectName("titleBar");
    titleBar->setContentsMargins(30, 20, 20, 15);
    
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);

    QLabel *titleLabel = new QLabel(title, titleBar);
    titleLabel->setProperty("class", "title");
    
    QPushButton *closeButton = new QPushButton(titleBar);
    closeButton->setObjectName("closeButton");

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
    QDirIterator it(":/styles", QStringList() << "*.qss", QDir::Files);
    QString styles;
    while (it.hasNext()){
        QFile file(it.next());
        if (file.open(QFile::ReadOnly)){
            styles += file.readAll() + "\n";
        }
    }
    setStyleSheet(styles);
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