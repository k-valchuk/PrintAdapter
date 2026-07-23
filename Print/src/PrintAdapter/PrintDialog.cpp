#include "PrintDialog.h"

#include <QPushButton>
#include <QShortcut>
#include <QGraphicsView>
#include <QDebug>
#include <QMouseEvent>
#include <QMenu>

PrintDialog::PrintDialog(QWidget* pwgt, QString printContent, QString rundownTitle): BaseDialog(pwgt, "Предварительный просмотр"), headerTitle(rundownTitle) {
    headersValues = {{Headers::EMPTY,Headers::EMPTY,Headers::EMPTY}, {Headers::EMPTY,Headers::EMPTY,Headers::EMPTY}};

    this->installEventFilter(this);
    
    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(20, 10, 20, 5);
    toolBar = new QToolBar(this);
    toolBar->setObjectName("PrintToolBar");
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
    pageShow->setObjectName("PageComboBox");

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
    previewWidget->setZoomMode(QPrintPreviewWidget::FitInView);
    previewWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    baseLayout->addWidget(toolBar);
    baseLayout->addWidget(previewWidget);
    toolBar->installEventFilter(this);
    QList<QWidget*> previewChildren = previewWidget->findChildren<QWidget*>();
    for (QWidget* child : previewChildren) {
        child->installEventFilter(this);
    }

    QToolBar* pageToolBar = new QToolBar(this);
    pageToolBar->setObjectName("PageFlipperToolBar");
    QWidget* leftSpacer = new QWidget();
    leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QWidget* rightSpacer = new QWidget();
    rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    pageToolBar->addWidget(leftSpacer);
    QAction* goFirstPage = pageToolBar->addAction(QIcon(":/icons/go-to-first-page-button.svg"),"Перейти в начало");
    QAction* goPreviuosPage = pageToolBar->addAction(QIcon(":/icons/previous-page-button.svg"),"Предыдущая страница");
    pageNumber = new QLineEdit(this);
    pageNumber->setText("1");
    pageNumber->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    pageToolBar->addWidget(pageNumber);
    connect(pageNumber, &QLineEdit::textChanged, this, [this](){
        previewWidget->setCurrentPage(pageNumber->text().toInt());
    });
    totalPagesLabel = new QLabel(QString("из %1").arg(printReport->m_document->pageCount()));
    pageToolBar->addWidget(totalPagesLabel);
    QAction* goNextPage = pageToolBar->addAction(QIcon(":/icons/next-page-button.svg"),"Следующая страница");
    QAction* goLastPage = pageToolBar->addAction(QIcon(":/icons/go-to-last-page-button.svg"),"Перейти в конец");
    pageToolBar->addWidget(rightSpacer);

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
        PageSetupDialog * dialog = new PageSetupDialog (this, printReport->m_printer, previewWidget, printReport->m_document, headerTitle, headersValues);
        connect(dialog, &PageSetupDialog::headersChanged, this, &PrintDialog::updateHeaders);
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

    lower_toolbar = new QWidget(this);
    QHBoxLayout* button_layout = new QHBoxLayout(lower_toolbar);
    button_layout->setMargin(0);
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

    button_layout->addWidget(cancelButton);
    button_layout->addWidget(pageToolBar);
    button_layout->addWidget(chooseButton);

    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+H"), this);
    connect(shortcut, &QShortcut::activated, this, [this](){
        bool isVisible = toolBar->isVisible();
        toolBar->setVisible(!isVisible);
        lower_toolbar->setVisible(!isVisible);
    });

    
    baseLayout->addWidget(lower_toolbar);
    lower_toolbar->installEventFilter(this);

    setBaseLayout(baseLayout);

}

bool PrintDialog::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            
            QMenu menu(this);
            QAction* printAction = menu.addAction("Печать");
            menu.addSeparator();
            QMenu* scaleMenu = menu.addMenu("Масштаб");
            menu.addSeparator();
            scaleMenu->setObjectName("ToggleMenu");
            QAction* fitWidthAction = scaleMenu->addAction("По ширине страницы");
            QAction* fitSizeAction = scaleMenu->addAction("По размеру страницы");
            fitWidthAction->setCheckable(true);
            fitSizeAction->setCheckable(true);
            QActionGroup* scaleGroup = new QActionGroup(this);
            scaleGroup->addAction(fitWidthAction);
            scaleGroup->addAction(fitSizeAction);
            scaleGroup->setExclusive(true); 

            QPrintPreviewWidget::ZoomMode currentMode = previewWidget->zoomMode();

            if (currentMode == QPrintPreviewWidget::FitInView) {
                fitSizeAction->setIcon(QIcon(":/icons/done.svg"));
                fitWidthAction->setIcon(QIcon()); 
                fitSizeAction->setChecked(true);
            } 
            else if (currentMode == QPrintPreviewWidget::FitToWidth) {
                fitWidthAction->setIcon(QIcon(":/icons/done.svg"));
                fitSizeAction->setIcon(QIcon()); 
                fitWidthAction->setChecked(true);
            } 
            else {
                fitWidthAction->setIcon(QIcon());
                fitSizeAction->setIcon(QIcon());
            }

            QMenu* hideTools = menu.addMenu("Панель инструментов");
            hideTools->setObjectName("ToggleMenu");
            QAction* showAction = hideTools->addAction("Показать");
            QAction* hideAction = hideTools->addAction("Скрыть");

            showAction->setCheckable(true);
            hideAction->setCheckable(true);

            QActionGroup* toolsGroup = new QActionGroup(this);
            toolsGroup->addAction(showAction);
            toolsGroup->addAction(hideAction);

            toolsGroup->setExclusive(true); 

            if (toolBar->isVisible()) {
                showAction->setChecked(true);
                showAction->setIcon(QIcon(":/icons/done.svg"));
                hideAction->setIcon(QIcon());
            } else {
                hideAction->setChecked(true);
                showAction->setIcon(QIcon());
                hideAction->setIcon(QIcon(":/icons/done.svg"));
            }

            connect(showAction, &QAction::triggered, this, [this]() {
                toolBar->show();
                lower_toolbar->show();
            });

            connect(hideAction, &QAction::triggered, this, [this]() {
                toolBar->hide();
                lower_toolbar->hide();
            });


            connect(fitWidthAction, &QAction::triggered, this, [this]() {
                previewWidget->fitToWidth();
            });

            connect(fitSizeAction, &QAction::triggered, this, [this]() {
                previewWidget->fitInView();
            });

            
            connect(printAction, &QAction::triggered, this, [this]() {
                previewWidget->print();
            });

            QAction* selected = menu.exec(QCursor::pos());
            
            return true;
        }

    }
    return QWidget::eventFilter(obj, event);

}

void PrintDialog::updateHeaders(QStringList headers, QStringList footers, QList<QList<Headers>> newHeadersValues){
    QString html = QString(
    "<table width='100%' style='border-collapse: collapse;'>"
    "  <tr>"
    "    <td width='33.3%' align='left'>%1</td>"
    "    <td width='33.3%' align='center'>%2</td>"
    "    <td width='33.3%' align='right'>%3</td>"
    "  </tr>"
    "</table>");
    headersValues = newHeadersValues;
    printReport->setHeaderText(html.arg(headers.at(0), headers.at(1), headers.at(2)));
    printReport->setFooterText(html.arg(footers.at(0), footers.at(1), footers.at(2)));

}