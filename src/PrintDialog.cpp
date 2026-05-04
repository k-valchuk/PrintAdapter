#include "PrintDialog.h"
#include "PageSetupDialog.h"

#include <QToolBar>
#include <QPushButton>
#include <QDebug>

PrintDialog::PrintDialog(QWidget* pwgt, QString printContent): BaseDialog(pwgt, "Предварительный просмотр") {
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 20, 30);
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setStyleSheet(
        "QToolBar::separator {"
        "   background-color: #1E1E22;"
        "    width: 1px;"                         
        "    margin-left: 5px;"         
        "    margin-right: 5px;"        
        "}"
        "QToolButton {"
        "   background-color: transparent;"       
        "}"
        "QToolBar {"
        "   background-color: #323236;"
        "}"
    );
    QAction* printAction = toolBar->addAction(QIcon(":/icons/print-button.svg"),"Печать");
    toolBar->addSeparator();
    QAction* portraitAction = toolBar->addAction(QIcon(":/icons/portrait-layout-button.svg"),"Книжная ориентация");
    QAction* landscapeAction = toolBar->addAction(QIcon(":/icons/landscape-layout-button.svg"),"Альбомная ориентация");
    toolBar->addSeparator();
    QAction* headersAction = toolBar->addAction(QIcon(":/icons/headers-footers-button.svg"),"Включить колонтитулы / Отключить колонтитулы");
    toolBar->addSeparator();
    QAction* fitWidthAction = toolBar->addAction(QIcon(":/icons/fit-to-width-button.svg"),"Просмотр по ширине страницы");
    QAction* fitPageAction = toolBar->addAction(QIcon(":/icons/fit-to-page-button.svg"),"Просмотр всей страницы");
    QAction* zoomOutAction = toolBar->addAction(QIcon(":/icons/zoom-out-button.svg"),"Масштаб меньше");
    QAction* zoomInAction = toolBar->addAction(QIcon(":/icons/zoom-in-button.svg"),"Масштаб больше");
    toolBar->addSeparator();
    pageShow = new QComboBox(this);
    pageShow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    pageShow->setStyleSheet(
        "QComboBox {"
        "   color: #BABABA;"
        "   border: none;"
        "   background: #222226;"
        "   combobox-popup: 0;"
        "   font: normal normal normal 15/18px Roboto;"
        "   padding-left: 10px;"
        "   height: 28px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #222226;"
        "   font: normal normal normal 15/18px Roboto;"
        "   color: #BABABA;"
        "   selection-background-color: #222226;"
        "   selection-color: #FAFAFA;"
        "   outline: none;"
        "   border: none;"
        "}"
        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: top right;"
        "   width: 20px;"
        "   border: none;"
        "   background: transparent;"
        "}"
        "QComboBox::down-arrow {"
        "   image: url(:/icons/arrow-down.png);"
        "   width: 12px;"
        "   height: 12px;"
        "}"
        "QComboBox::down-arrow:on {"
        "   top: 1px;"
        "   image: url(:/icons/arrow-up.png);"
    "}");

    pageShow->addItem("1 страница", 1);
    pageShow->addItem("2 страницы", 2);
    pageShow->addItem("3 страницы", 3);
    pageShow->addItem("4 страницы", 4);
    pageShow->addItem("6 страниц", 6);
    toolBar->addWidget(pageShow);
    toolBar->addSeparator();
    QAction* settingsAction = toolBar->addAction(QIcon(":/icons/page-setup-button.svg"),"Параметры страницы");


    printReport = new PrintableReport(QPrinter::ScreenResolution, printContent, this);
    
    previewWidget = new QPrintPreviewWidget(printReport->m_printer, this);
    previewWidget->setStyleSheet(
        "QGraphicsView { qproperty-backgroundBrush: #222226; border: none; }"
    );
    previewWidget->setViewMode(QPrintPreviewWidget::SinglePageView);
    
    baseLayout->addWidget(toolBar);
    baseLayout->addWidget(previewWidget, 1);

    QToolBar* pageToolBar = new QToolBar(this);
    pageToolBar->setStyleSheet(
        "QToolButton {"
        "   background-color: transparent;"       
        "}"
        "QToolBar {"
        "   background-color: #323236;"
        "}"
        "QWidget {"
        "   background-color: transparent;" 
        "}"
    );
    QWidget* leftSpacer = new QWidget();
    leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QWidget* rightSpacer = new QWidget();
    rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    pageToolBar->addWidget(leftSpacer);
    QAction* goFirstPage = pageToolBar->addAction(QIcon(":/icons/go-to-first-page-button.svg"),"Перейти в начало");
    QAction* goPreviuosPage = pageToolBar->addAction(QIcon(":/icons/previous-page-button.svg"),"Предыдущая страница");
    pageNumber = new QLineEdit(this);
    pageNumber->setStyleSheet("QLineEdit { background-color: #222226; font: normal normal normal 15px/18px Roboto;  color: #A8A8A8; border: none; margin-left: 10px; margin-right: 5px;}");
    pageNumber->setFixedWidth(40);
    pageNumber->setText("1");
    pageToolBar->addWidget(pageNumber);
    connect(pageNumber, &QLineEdit::textChanged, this, [this](){
        previewWidget->setCurrentPage(pageNumber->text().toInt());
    });
    totalPagesLabel = new QLabel(QString("из %1").arg(printReport->m_document->pageCount()));
    totalPagesLabel->setStyleSheet("QLabel{font: normal normal normal 15px/18px Roboto; color: #838384; margin-left: 5px;}");
    pageToolBar->addWidget(totalPagesLabel);
    QAction* goNextPage = pageToolBar->addAction(QIcon(":/icons/next-page-button.svg"),"Следующая страница");
    QAction* goLasttPage = pageToolBar->addAction(QIcon(":/icons/go-to-last-page-button.svg"),"Перейти в конец");
    pageToolBar->addWidget(rightSpacer);
    baseLayout->addWidget(pageToolBar);

    active_headers = false;
    connect(
        pageShow, QOverload<int>::of(&QComboBox::activated), 
        this, [this](){
            switch (pageShow->currentData().toInt())
            {
            case 1:
                previewWidget->setViewMode(QPrintPreviewWidget::SinglePageView);
                break;
            case 2:
                previewWidget->setViewMode(QPrintPreviewWidget::FacingPagesView);
                break;
            case 3:
            case 4:
            case 6:
                previewWidget->setViewMode(QPrintPreviewWidget::AllPagesView);
                break;
            default:
                break;
            }
        }
    );
    connect(previewWidget, &QPrintPreviewWidget::paintRequested, 
        this, [this](QPrinter* printer){
            printReport->print(printer);
            totalPagesLabel->setText(QString("из %1").arg(printReport->m_document->pageCount()));
        }
    );
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
    connect(settingsAction, &QAction::triggered, previewWidget, [this](){
        PageSetupDialog * dialog = new PageSetupDialog (this, printReport->m_printer, previewWidget, printReport->m_document);
        if (dialog->exec() == QDialog::Accepted) {
            previewWidget->updatePreview();
        }
        delete dialog;
    });

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
    
    QPushButton* chooseButton = new QPushButton("Печать");
    chooseButton->setFixedHeight(30);
    chooseButton->setMinimumWidth(114);
    chooseButton->setStyleSheet(
        "QPushButton{font: normal normal normal 15px/18px Roboto; background: #6F8CB7; color: #FFFFFF; border-radius: 2px; margin-left: 10px;}"
        "QPushButton:hover{background: #9abcff;}"
    );
    connect(chooseButton, SIGNAL(clicked()), previewWidget, SLOT(print()));
    previewWidget->updatePreview();

    button_layout->addWidget(backButton);
    button_layout->addStretch();
    button_layout->addWidget(cancelButton);
    button_layout->addWidget(chooseButton);

    baseLayout->addLayout(button_layout);

    setBaseLayout(baseLayout);

}