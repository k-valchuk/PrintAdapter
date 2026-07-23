#include "PageSetupDialog.h"
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QLocale>
#include <QPainter>

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

    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 20, 5);

    QHBoxLayout* pageLayout = new QHBoxLayout(this);
    QVBoxLayout* baseSettingsLayout = new QVBoxLayout(this);
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
    }
    fieldGroupBox->setObjectName("pageSetupGroupBox");
    fieldGroupBox->setMaximumWidth(327);
    fieldGroupBox->setLayout(field_form_layout);
    baseSettingsLayout->addStretch();
    baseSettingsLayout->addWidget(fieldGroupBox);
    pageLayout->addLayout(baseSettingsLayout);

    QGroupBox* previewGroupBox = new QGroupBox(this);
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
    previewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout* previewLayout = new QVBoxLayout(this);
    previewLayout->addStretch();
    previewLayout->addWidget(previewLabel, 1);
    previewLayout->addStretch();
    previewGroupBox->setLayout(previewLayout);
    pageLayout->addWidget(previewGroupBox);


    baseLayout->addLayout(pageLayout);

    QTimer::singleShot(0, this, [this]() {
        updateThumbnail();
    });
    int j = 0;
    for (QString title : {"Верхний колонтитул", "Нижний колонтитул"}){
        QGroupBox* headerGroupBox = new QGroupBox(title, this);
        QHBoxLayout *pform_layout = new QHBoxLayout(this);
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
    

    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->addStretch();
    button_layout->setContentsMargins(0, 90, 0, 0);

    QPushButton* cancelButton = new QPushButton("Отмена", this);
    cancelButton->setObjectName("cancelButton");
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    QPushButton* applyButton = new QPushButton("Применить");
    applyButton->setObjectName("applyButton");
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
    QSize labelSize = previewLabel->size();
    if (labelSize.isEmpty()) return;
    auto originOrientation = printer->orientation();
    auto originPageSize = printer->paperSize();
    printer->setPageOrientation((QPageLayout::Orientation)orientationComboBox->currentData().toInt());
    printer->setPaperSize((QPrinter::PaperSize)paperComboBox->currentData().toInt());

    QRectF pageRect = printer->pageLayout().fullRectPoints();

    printer->setPaperSize(originPageSize);
    printer->setOrientation(originOrientation);
   
    double scale = qMin((double)labelSize.width() / pageRect.width(), 
                        (double)labelSize.height() / pageRect.height()) * 0.9; 

    QSize pixmapSize = (pageRect.size() * scale).toSize();
    

    QPixmap pixmap(pixmapSize);
    pixmap.fill(Qt::white);
    
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);
    painter.scale(scale, scale);


    
    //{"Верхнее", "Нижнее", "Левое", "Правое"}
    double mmToPx = 2.8346; 
    double marginLeft = marginEdits[2]->text().toDouble() * mmToPx;
    double marginTop = marginEdits[0]->text().toDouble() * mmToPx;
    double marginRight = marginEdits[3]->text().toDouble() * mmToPx;
    double marginBottom = marginEdits[1]->text().toDouble() * mmToPx;

    QTextDocument* doc_clone = doc->clone();
    doc_clone->setPageSize(QSize(pageRect.width() - marginLeft -marginRight, pageRect.height() - marginTop - marginBottom));
    doc_clone->setDocumentMargin(0);

    painter.save();
    painter.translate(marginLeft, marginTop);
    QRectF contentRect(0, 0, pageRect.width() - marginLeft -marginRight, pageRect.height() - marginTop - marginBottom);
    painter.setClipRect(contentRect); 

    doc_clone->drawContents(&painter, contentRect);
    painter.restore();

    QRectF marginRect = QRectF(0,0, pageRect.width(), pageRect.height()).adjusted(marginLeft, marginTop, -marginRight, -marginBottom);
    QPen marginPen(QColor("#39393C"), 1, Qt::DashLine);
    marginPen.setCosmetic(true);
    painter.setPen(marginPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(marginRect);

    
    painter.end();


    previewLabel->setPixmap(pixmap);
}
