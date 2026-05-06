#include "BaseDialog.h"
#include <QPushButton>
#include "Config.h"
#include <QLabel>
#include <QDirIterator>
#include <QMouseEvent>
#include <QSizeGrip>

BaseDialog::BaseDialog(QWidget* pwgt, QString title) : QDialog(pwgt, Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint) {
    setWindowTitle(title);
    setWindowIcon(QIcon(":/icons/bramspace_icon.ico"));
    base_layout = new QVBoxLayout(this);
}

void BaseDialog::setBaseLayout(QLayout* layout) {    
    base_layout->addLayout(layout);
    setLayout(base_layout);
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