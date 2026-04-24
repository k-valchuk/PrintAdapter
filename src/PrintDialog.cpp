#include "PrintDialog.h"
#include <QToolBar>
#include <QPushButton>
#include <QDebug>

PrintDialog::PrintDialog(QWidget* pwgt, QString printContent): BaseDialog(pwgt, "Предварительный просмотр") {
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 20, 30);

    QToolBar* toolBar = new QToolBar(this);
    QAction* printAction = toolBar->addAction(QIcon(":/icons/print-button.svg"),"Печать");
    QAction* portraitAction = toolBar->addAction(QIcon(":/icons/portrait-layout-button.svg"),"Книжная ориентация");
    QAction* landscapeAction = toolBar->addAction(QIcon(":/icons/landscape-layout-button.svg"),"Альбомная ориентация");
    QAction* headersAction = toolBar->addAction(QIcon(":/icons/headers-footers-button.svg"),"Включить колонтитулы / Отключить колонтитулы");
    QAction* fitWidthAction = toolBar->addAction(QIcon(":/icons/fit-to-width-button.svg"),"Просмотр по ширине страницы");
    QAction* fitPageAction = toolBar->addAction(QIcon(":/icons/fit-to-page-button.svg"),"Просмотр всей страницы");
    QAction* zoomOutAction = toolBar->addAction(QIcon(":/icons/zoom-out-button.svg"),"Масштаб меньше");
    QAction* zoomInAction = toolBar->addAction(QIcon(":/icons/zoom-in-button.svg"),"Масштаб больше");
    QAction* settingsAction = toolBar->addAction(QIcon(":/icons/page-setup-button.svg"),"Параметры страницы");


    printReport = new PrintableReport(QPrinter::ScreenResolution, printContent, this);
    
    previewWidget = new QPrintPreviewWidget(printReport->m_printer, this);
    previewWidget->setStyleSheet(
        "QGraphicsView { qproperty-backgroundBrush: #222226; border: none; }"
    );
    
    baseLayout->addWidget(toolBar);
    baseLayout->addWidget(previewWidget);
    active_headers = false;

    connect(previewWidget, SIGNAL(paintRequested(QPrinter *)), printReport, SLOT(print(QPrinter *)));
    connect(printAction, &QAction::triggered, previewWidget, &QPrintPreviewWidget::print);
    connect(portraitAction, &QAction::triggered, previewWidget, &QPrintPreviewWidget::setPortraitOrientation);
    connect(landscapeAction, &QAction::triggered, previewWidget, &QPrintPreviewWidget::setLandscapeOrientation);
    connect(headersAction, &QAction::triggered, previewWidget, [this](){
        if (active_headers) {
            active_headers = false;
            printReport->disabelHeaderSize();
            printReport->disabelFooterSize();
        } else {
            active_headers = true;
            printReport->setHeaderSize(10);
            printReport->setFooterSize(10);
        }
        previewWidget->updatePreview(); 
    });
    connect(fitWidthAction, &QAction::triggered, previewWidget, &QPrintPreviewWidget::fitToWidth);
    connect(fitPageAction, &QAction::triggered, previewWidget, &QPrintPreviewWidget::fitInView);

    connect(zoomOutAction, SIGNAL(triggered()), previewWidget, SLOT(zoomOut()));
    connect(zoomInAction, SIGNAL(triggered()), previewWidget,  SLOT(zoomIn()));
    //connect(settingsAction, &QAction::triggered, previewWidget, &QPrintPreviewWidget::zoomOut);



    

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
        backButton, &QPushButton::clicked,
        this, [this](){
            qDebug() << "Кнопка 'Назад' реально нажата"; 
            emit backButtonClicked();
            this->close();
        }
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
    previewWidget->updatePreview();

    button_layout->addWidget(backButton);
    button_layout->addStretch();
    button_layout->addWidget(cancelButton);
    button_layout->addWidget(chooseButton);

    baseLayout->addLayout(button_layout);

    setBaseLayout(baseLayout);

}