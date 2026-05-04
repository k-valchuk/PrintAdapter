#include "PrintDialog.h"
#include "PageSetupDialog.h"

#include <QToolBar>
#include <QPushButton>
#include <QDebug>

PrintDialog::PrintDialog(QWidget* pwgt, QString printContent): BaseDialog(pwgt, "Предварительный просмотр") {
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 20, 30);
    QToolBar* toolBar = new QToolBar(this);
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
    previewWidget->setViewMode(QPrintPreviewWidget::SinglePageView);
    
    baseLayout->addWidget(toolBar);
    baseLayout->addWidget(previewWidget, 1);

    QToolBar* pageToolBar = new QToolBar(this);
    QWidget* leftSpacer = new QWidget();
    leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QWidget* rightSpacer = new QWidget();
    rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    pageToolBar->addWidget(leftSpacer);
    QAction* goFirstPage = pageToolBar->addAction(QIcon(":/icons/go-to-first-page-button.svg"),"Перейти в начало");
    QAction* goPreviuosPage = pageToolBar->addAction(QIcon(":/icons/previous-page-button.svg"),"Предыдущая страница");
    pageNumber = new QLineEdit(this);
    pageNumber->setText("1");
    pageToolBar->addWidget(pageNumber);
    connect(pageNumber, &QLineEdit::textChanged, this, [this](){
        previewWidget->setCurrentPage(pageNumber->text().toInt());
    });
    totalPagesLabel = new QLabel(QString("из %1").arg(printReport->m_document->pageCount()));
    pageToolBar->addWidget(totalPagesLabel);
    QAction* goNextPage = pageToolBar->addAction(QIcon(":/icons/next-page-button.svg"),"Следующая страница");
    QAction* goLastPage = pageToolBar->addAction(QIcon(":/icons/go-to-last-page-button.svg"),"Перейти в конец");
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

    connect(goFirstPage, &QAction::triggered, this, [this](){
        previewWidget->setCurrentPage(1);
        pageNumber->setText(QString::number(previewWidget->currentPage()));
    });
    connect(goPreviuosPage, &QAction::triggered, this, [this](){
        previewWidget->setCurrentPage(previewWidget->currentPage() - 1);
        pageNumber->setText(QString::number(previewWidget->currentPage()));
    });
    connect(goNextPage, &QAction::triggered, this, [this](){
        previewWidget->setCurrentPage(previewWidget->currentPage() + 1);
        pageNumber->setText(QString::number(previewWidget->currentPage()));
    });
    connect(goLastPage, &QAction::triggered, this, [this](){
        previewWidget->setCurrentPage(previewWidget->pageCount());
        pageNumber->setText(QString::number(previewWidget->currentPage()));
    });

    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setContentsMargins(0, 21, 0, 0);
    QPushButton* backButton = new QPushButton("Назад");
    backButton->setObjectName("cancelButton");
    
    connect(
        backButton, &QPushButton::clicked,
        this, [this](){
            qDebug() << "Кнопка 'Назад' реально нажата"; 
            emit backButtonClicked();
            this->close();
        }
    );
    QPushButton* cancelButton = new QPushButton("Отмена");
    cancelButton->setObjectName("cancelButton");
    
    connect(
        cancelButton, SIGNAL(clicked()),
        this, SLOT(close())
    );
    
    QPushButton* chooseButton = new QPushButton("Печать");
    chooseButton->setObjectName("applyButton");
    connect(chooseButton, SIGNAL(clicked()), previewWidget, SLOT(print()));
    previewWidget->updatePreview();

    button_layout->addWidget(backButton);
    button_layout->addStretch();
    button_layout->addWidget(cancelButton);
    button_layout->addWidget(chooseButton);

    baseLayout->addLayout(button_layout);

    setBaseLayout(baseLayout);

}