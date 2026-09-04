#include "PageSetupDialog.h"
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QLocale>
#include <QPainter>
#include <QCoreApplication>

void PageSetupDialog::setComboBoxStyle(QComboBox* cbox, const QStringList elements, int distance) {
    cbox->setObjectName("PageComboBox");
    cbox->addItems(elements);
    cbox->setFixedSize(140, 28);
}


void PageSetupDialog::addParam(const QString label, QFormLayout* layout, QWidget* pwgt, int distance){
    QLabel* pLabel = new QLabel(label, this);
    pLabel->setStyleSheet(QString("QLabel {color: #838384; font: normal normal normal 15px Roboto; margin-right: %1 px;}").arg(distance));
    layout->addRow(pLabel, pwgt);
}

PageSetupDialog::PageSetupDialog(QWidget* pwgt, QPrinter* printer_, QPrintPreviewWidget* previewWidget_, QTextDocument *doc_, QString headerTitle_, QList<QList<Headers>> headersValues): BaseDialog(pwgt, "Параметры страницы"), headerTitle(headerTitle_) {
    printer = printer_;
    previewWidget = previewWidget_;
    doc = doc_;
    setMinimumSize(400, 300);

    QVBoxLayout* baseLayout = new QVBoxLayout();
    baseLayout->setContentsMargins(10, 10, 20, 5);

    QHBoxLayout* pageLayout = new QHBoxLayout();
    QVBoxLayout* baseSettingsLayout = new QVBoxLayout();
    QGroupBox* pageGroupBox = new QGroupBox("Параметры страницы", this);
    pageGroupBox->setObjectName("pageSetupGroupBox");
    pageGroupBox->setMaximumSize(327, 114);
    QFormLayout* page_form_layout = new QFormLayout();
    paperComboBox = new QComboBox(this);
    setComboBoxStyle(paperComboBox, {}, 16);
    paperComboBox->addItem("A4", QPrinter::A4);
    paperComboBox->addItem("A3", QPrinter::A3);
    paperComboBox->addItem("A5", QPrinter::A5);
    paperComboBox->setCurrentIndex(paperComboBox->findData(printer->paperSize()));
    connect(paperComboBox, QOverload<int>::of(&QComboBox::activated), this, &PageSetupDialog::updateThumbnail);
    addParam("Размер бумаги", page_form_layout, paperComboBox, 16);
    orientationComboBox = new QComboBox(this);
    setComboBoxStyle(orientationComboBox, {}, 16);
    orientationComboBox->addItem("Книжная", QPrinter::Portrait);
    orientationComboBox->addItem("Альбомная", QPrinter::Landscape);
    orientationComboBox->setCurrentIndex(orientationComboBox->findData(printer->orientation()));
    connect(orientationComboBox, QOverload<int>::of(&QComboBox::activated), this, &PageSetupDialog::updateThumbnail);
    addParam("Ориентация", page_form_layout, orientationComboBox, 16);
    pageGroupBox->setLayout(page_form_layout);

    baseSettingsLayout->addWidget(pageGroupBox);
    

    QGroupBox* fieldGroupBox = new QGroupBox("Поля (мм)", this);
    QFormLayout *field_form_layout = new QFormLayout();
    QMarginsF margins = printer->pageLayout().margins();
    QList<QPair<QString, qreal>> data;
    data.append({"Верхнее", margins.top()});
    data.append({"Нижнее", margins.bottom()});
    data.append({"Левое", margins.left()});
    data.append({"Правое", margins.right()});
    for (auto pair : data){
        QLineEdit* pLineEdit = new QLineEdit(this);
        pLineEdit->setText(QString::number(pair.second, 'f', 1));
        pLineEdit->setObjectName("pageFieldInput");
        QDoubleValidator* validator = new QDoubleValidator(0.0, 999, 2, pLineEdit);
        validator->setNotation(QDoubleValidator::StandardNotation);
        validator->setLocale(QLocale::C);
        pLineEdit->setValidator(validator);
        
        addParam(pair.first, field_form_layout, pLineEdit, 40);
        marginEdits.append(pLineEdit);
        connect(pLineEdit, &QLineEdit::editingFinished, this, &PageSetupDialog::updateThumbnail);
        connect(pLineEdit, &QLineEdit::returnPressed, this, [pLineEdit]() {
            pLineEdit->clearFocus();
        });
    }
    fieldGroupBox->setObjectName("pageSetupGroupBox");
    fieldGroupBox->setMaximumWidth(327);
    fieldGroupBox->setLayout(field_form_layout);
    baseSettingsLayout->addStretch();
    baseSettingsLayout->addWidget(fieldGroupBox);
    pageLayout->addLayout(baseSettingsLayout);

    previewGroupBox = new QGroupBox(this);
    previewGroupBox->setMinimumWidth(100);
    previewGroupBox->setStyleSheet(
        "QGroupBox {"
        "   margin-top: 10px;"
        "   border: 1px solid #494949;"
        "   border-radius: 2px;"
        "}"
    );
    previewGroupBox->setContentsMargins(10, 14, 10, 10);
    
    previewLabel = new QLabel(this);
    previewLabel->setStyleSheet("background-color: #313135;");
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored); 
    QVBoxLayout* previewLayout = new QVBoxLayout();
    previewLayout->addWidget(previewLabel);
    previewGroupBox->setLayout(previewLayout);
    pageLayout->addWidget(previewGroupBox, 1);


    baseLayout->addLayout(pageLayout);

    resizeTimer = new QTimer(this);
    resizeTimer->setSingleShot(true);

    connect(resizeTimer, &QTimer::timeout, this, &PageSetupDialog::renderThumbnail);

    QTimer::singleShot(0, this, [this]() {
        updateThumbnail();
    });
    int j = 0;
    for (QString title : {"Верхний колонтитул", "Нижний колонтитул"}){
        QGroupBox* headerGroupBox = new QGroupBox(title, this);
        QHBoxLayout *pform_layout = new QHBoxLayout();
        headerGroupBox->setObjectName("pageSetupGroupBox");
        for (int i = 0; i < 3; i++) {
            QComboBox *pcomboBox = new QComboBox(this);
            for (const auto& pair : {
                    QPair<QString, int>("Пусто", static_cast<int>(Headers::EMPTY)),
                    QPair<QString, int>("Заголовок", static_cast<int>(Headers::TITLE)), 
                    QPair<QString, int>("Номер страницы", static_cast<int>(Headers::PAGE_NUMBER)), 
                    QPair<QString, int>("Стр. № из общего количества", static_cast<int>(Headers::PAGE_NUMBER_TOTAL)), 
                    QPair<QString, int>("Всего страниц", static_cast<int>(Headers::COUNT_PAGES)), 
                    QPair<QString, int>("Дата в кратком формате", static_cast<int>(Headers::LONG_DATE)), 
                    QPair<QString, int>("Дата в длинном формате", static_cast<int>(Headers::SHORT_DATE)), 
                    QPair<QString, int>("Время", static_cast<int>(Headers::TIME))
                }) 
            {
                pcomboBox->addItem(QString(pair.first), pair.second);
            }
            pcomboBox->setCurrentIndex(pcomboBox->findData(static_cast<int>(headersValues[j][i])));
            pcomboBox->setObjectName("PageComboBox");
                
            
            pform_layout->addWidget(pcomboBox);
            headerComboBoxes.append(pcomboBox);
        }
        headerGroupBox->setLayout(pform_layout);
        baseLayout->addWidget(headerGroupBox);
        j++;
        
    }
    

    QHBoxLayout* button_layout = new QHBoxLayout();
    button_layout->addStretch();
    button_layout->setContentsMargins(0, 90, 0, 0);

    QPushButton* cancelButton = new QPushButton("Отмена", this);
    cancelButton->setObjectName("cancelButton");
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    QPushButton* applyButton = new QPushButton("Применить");
    applyButton->setObjectName("applyButton");
    cancelButton->setAutoDefault(false);
    cancelButton->setDefault(false);

    applyButton->setAutoDefault(false);
    applyButton->setDefault(false);
    connect(
        applyButton, &QPushButton::clicked, 
        this, [this](){
            printer->setPageOrientation((QPageLayout::Orientation)orientationComboBox->currentData().toInt());
            printer->setPaperSize((QPrinter::PaperSize)paperComboBox->currentData().toInt());
            QPageLayout layout = printer->pageLayout();
            double left = marginEdits[2]->text().toDouble();
            double top = marginEdits[0]->text().toDouble();
            double right = marginEdits[3]->text().toDouble();
            double bottom = marginEdits[1]->text().toDouble();
            layout.setMargins(QMarginsF(left, top, right, bottom));
            printer->setPageLayout(layout);

            computeHeaders();

            accept();
        }
    );


    button_layout->addWidget(cancelButton);
    button_layout->addWidget(applyButton);
    baseLayout->addLayout(button_layout);
    setBaseLayout(baseLayout);
}

void PageSetupDialog::computeHeaders() {
    QStringList headers = QStringList();
    QList<Headers> newHeadersValues;
    for (QComboBox* comboBox : headerComboBoxes) {
        Headers temp_data = static_cast<Headers>(comboBox->currentData().toInt());
        newHeadersValues.append(temp_data);
        QLocale russianLocale(QLocale::Russian);
        switch (temp_data)
        {
        case Headers::TITLE:
            headers.append(headerTitle);
            break;
        case Headers::PAGE_NUMBER:
            headers.append(QString("&page;"));
            break;
        case Headers::PAGE_NUMBER_TOTAL:
            headers.append(QString("&page; из &totalpages;"));
            break;
        case Headers::COUNT_PAGES:
            headers.append(QString("&totalpages;"));
            break;
        case Headers::LONG_DATE:
            headers.append(russianLocale.toString(QDate::currentDate(), "dd MMMM yyyy"));
            break;
        case Headers::SHORT_DATE:
            headers.append(QDate::currentDate().toString("dd.MM.yyyy"));
            break;
        case Headers::TIME:
            headers.append(QTime::currentTime().toString("hh:mm:ss"));
            break;
        case Headers::EMPTY:
            headers.append("");
            break;
        }
    }
    emit headersChanged(headers.mid(0, 3), headers.mid(3, 3), {newHeadersValues.mid(0, 3),newHeadersValues.mid(3, 3)});

}


void PageSetupDialog::updateThumbnail() {
    cachedPreviewPixmap = QPixmap(); 
    renderThumbnail();
}

void PageSetupDialog::renderThumbnail() {
    if (marginEdits.size() < 4) return; 

    previewGroupBox->setMinimumWidth(100);
    previewGroupBox->setMaximumWidth(16777215);

    QCoreApplication::processEvents();

    QSize viewSize = previewLabel->size();
    if (viewSize.width() <= 0 || viewSize.height() <= 0) return;

    auto originOrientation = printer->orientation();
    auto originPageSize = printer->paperSize();
    printer->setPageOrientation((QPageLayout::Orientation)orientationComboBox->currentData().toInt());
    printer->setPaperSize((QPrinter::PaperSize)paperComboBox->currentData().toInt());

    QRectF pageRect = printer->pageLayout().fullRectPoints();

    printer->setPaperSize(originPageSize);
    printer->setOrientation(originOrientation);

    if (pageRect.isEmpty()) return;
    
    double scale = qMin((double)viewSize.width() / pageRect.width(), 
                        (double)viewSize.height() / pageRect.height()) * 0.95;
    
    QSize pixmapSize(qRound(pageRect.width() * scale), qRound(pageRect.height() * scale));
    if (pixmapSize.width() <= 0 || pixmapSize.height() <= 0) return;
    QPixmap pixmap(pixmapSize);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);

    painter.scale(scale, scale);

    double mmToPx = 2.8346; 
    double marginLeft   = marginEdits[2]->text().toDouble() * mmToPx;
    double marginTop    = marginEdits[0]->text().toDouble() * mmToPx;
    double marginRight   = marginEdits[3]->text().toDouble() * mmToPx;
    double marginBottom = marginEdits[1]->text().toDouble() * mmToPx;

    QScopedPointer<QTextDocument> doc_clone(doc->clone());
    if (doc_clone) {
        doc_clone->setPageSize(QSize(pageRect.width() - marginLeft - marginRight, pageRect.height() - marginTop - marginBottom));
        doc_clone->setDocumentMargin(0);

        painter.save();
        painter.translate(marginLeft, marginTop);
        QRectF contentRect(0, 0, pageRect.width() - marginLeft - marginRight, pageRect.height() - marginTop - marginBottom);
        painter.setClipRect(contentRect); 

        doc_clone->drawContents(&painter, contentRect);
        painter.restore();
    }

    QRectF marginRect = QRectF(0, 0, pageRect.width(), pageRect.height()).adjusted(marginLeft, marginTop, -marginRight, -marginBottom);
    QPen marginPen(QColor("#39393C"), 1, Qt::DashLine);
    marginPen.setCosmetic(true);
    painter.setPen(marginPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(marginRect);
    
    painter.end();


    cachedPreviewPixmap = pixmap;

    previewLabel->setPixmap(cachedPreviewPixmap);
}


void PageSetupDialog::resizeEvent(QResizeEvent* event) {
    BaseDialog::resizeEvent(event); 

    previewGroupBox->setFixedWidth(previewGroupBox->width());

    if (!cachedPreviewPixmap.isNull()) {
        scaleCachedThumbnail();
    }

    resizeTimer->start(150); 
}

void PageSetupDialog::scaleCachedThumbnail() {
    if (cachedPreviewPixmap.isNull()) return;

    previewLabel->setPixmap(cachedPreviewPixmap);
}