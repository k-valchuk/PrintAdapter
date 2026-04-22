#include "PrintDialog.h"
#include <QPrintPreviewWidget>
#include <QPushButton>

PrintDialog::PrintDialog(QWidget* pwgt, QPrinter* printer): BaseDialog(pwgt, "Предварительный просмотр") {
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 20, 30);

    QPrintPreviewWidget* previewWidget = new QPrintPreviewWidget(printer, this);
    baseLayout->addWidget(previewWidget);


    

    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setContentsMargins(0, 21, 0, 0);
    QPushButton* backButton = new QPushButton("Назад");
    backButton->setFixedHeight(30);
    backButton->setMinimumWidth(93);
    backButton->setStyleSheet(
        "QPushButton {border: 1px solid #494949; border-radius: 2px; font: normal normal normal 15px/18px Roboto; color: #6F8CB7; background-color: transparent;}"
        "QPushButton:hover{border: 1px solid #6F8CB7;}"
    );
    
    connect(
        backButton, SIGNAL(clicked()),
        this, SLOT(close())
    );
    QPushButton* cancelButton = new QPushButton("Отмена");
    cancelButton->setFixedHeight(30);
    cancelButton->setMinimumWidth(93);
    cancelButton->setStyleSheet(
        "QPushButton {border: 1px solid #494949; border-radius: 2px; font: normal normal normal 15px/18px Roboto; color: #6F8CB7; background-color: transparent; margin-right: 10px;}"
        "QPushButton:hover{border: 1px solid #6F8CB7;}"
    );
    
    connect(
        cancelButton, SIGNAL(clicked()),
        this, SLOT(close())
    );
    
    QPushButton* chooseButton = new QPushButton("Выбрать");
    chooseButton->setFixedHeight(30);
    chooseButton->setMinimumWidth(114);
    chooseButton->setStyleSheet(
        "QPushButton{font: normal normal normal 15px/18px Roboto; background: #6F8CB7; color: #FFFFFF; border-radius: 2px; margin-left: 10px;}"
        "QPushButton:hover{background: #9abcff;}"
    );
    connect(previewWidget, &QPrintPreviewWidget::paintRequested,
            this, &PrintDialog::onPaintRequested);
    previewWidget->updatePreview();

    button_layout->addWidget(backButton);
    button_layout->addStretch();
    button_layout->addWidget(cancelButton);
    button_layout->addWidget(chooseButton);

    baseLayout->addLayout(button_layout);

    setBaseLayout(baseLayout);

}

void PrintDialog::onPaintRequested(QPrinter* printer){
    emit paintRequested(printer);
}