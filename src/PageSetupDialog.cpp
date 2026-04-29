#include "PageSetupDialog.h"
#include <QTimer>
#include <QDebug>

#include <QPainter>

void PageSetupDialog::setCancelButtonStyle(QPushButton* btn){
    btn->setFixedHeight(30);
    btn->setMinimumWidth(93);
    btn->setStyleSheet(
        "QPushButton {border: 1px solid #494949; border-radius: 2px; font: normal normal normal 15px/18px Roboto; color: #6F8CB7; background-color: transparent; margin-right: 10px;}"
        "QPushButton:hover{border: 1px solid #6F8CB7;}"
    );

}

void PageSetupDialog::setApplyButtonStyle(QPushButton* btn) {
    btn->setFixedHeight(30);
    btn->setMinimumWidth(114);
    btn->setStyleSheet(
        "QPushButton{font: normal normal normal 15px/18px Roboto; background: #6F8CB7; color: #FFFFFF; border-radius: 2px; margin-left: 10px;}"
        "QPushButton:hover{background: #9abcff;}"
    );
}

void PageSetupDialog::setComboBoxStyle(QComboBox* cbox, const QStringList elements, int distance) {
    cbox->addItems(elements);
    cbox->setFixedSize(140, 28);
    cbox->setStyleSheet(
        QString("QComboBox {"
        "   color: #BABABA;"
        "   border: none;"
        "   background: #222226;"
        "   combobox-popup: 0;"
        "   font: normal normal normal 15/18px Roboto;"
        "   padding-left: 10px;"
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
    "}").arg(distance)
);
}

void PageSetupDialog::setGroupBoxStyle(QGroupBox* gbox){

    gbox->setStyleSheet(
        "QGroupBox {"
        "   border: 1px solid #494949;"
        "   border-radius: 2px;"
        "   margin-top: 10px;"
        "   font: normal normal normal 15/18px Roboto;"
        "}"
        "QGroupBox::title {"
        "   color: #BABABA;"
        "subcontrol-origin: margin;"
        "subcontrol-position: top left;" 
        "top: 2px;"
        "left: 10px;"
        "padding: 0 3px;"
        "}"
    );
}

void PageSetupDialog::setLineEditStyle(QLineEdit* lineEdit){
    lineEdit->setFixedSize(140, 28);
    lineEdit->setStyleSheet(
            "QLineEdit{"
            "   color: #BABABA;"
            "   font: normal normal normal 15px/18px Roboto;"
            "   background-color: #222226;"
            "   border: none;"
            "   padding-left: 10px;"
            "   margin-left: 40px;"
            "}"
    );
}

void PageSetupDialog::addParam(const QString label, QFormLayout* layout, QWidget* pwgt, int distance){
    QLabel* pLabel = new QLabel(label, this);
    pLabel->setStyleSheet(QString("QLabel {color: #838384; font: normal normal normal 15px/18px Roboto; margin-right: %1 px;}").arg(distance));
    layout->addRow(pLabel, pwgt);
}

PageSetupDialog::PageSetupDialog(QWidget* pwgt, QPrinter* printer_, QPrintPreviewWidget* previewWidget_, QTextDocument *doc_): BaseDialog(pwgt, "Параметры страницы") {
    printer = printer_;
    previewWidget = previewWidget_;
    doc = doc_;

    QVBoxLayout* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 20, 30);

    QHBoxLayout* pageLayout = new QHBoxLayout(this);
    QVBoxLayout* baseSettingsLayout = new QVBoxLayout(this);
    QGroupBox* pageGroupBox = new QGroupBox("Параметры страницы", this);
    setGroupBoxStyle(pageGroupBox);
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
    QMap<QString, qreal> data;
    data.insert("Верхнее", margins.top());
    data.insert("Нижнее", margins.bottom());
    data.insert("Левое", margins.left());
    data.insert("Правое", margins.right());
    QMapIterator<QString, qreal> item(data);
    for (;item.hasNext();){
        item.next();
        QLineEdit* pLineEdit = new QLineEdit(this);
        pLineEdit->setText(QString::number(item.value(), 'f', 1));
        setLineEditStyle(pLineEdit);
        QDoubleValidator* validator = new QDoubleValidator(0.0, 999, 2, pLineEdit);
        validator->setNotation(QDoubleValidator::StandardNotation);
        validator->setLocale(QLocale::C);
        pLineEdit->setValidator(validator);
        
        addParam(item.key(), field_form_layout, pLineEdit, 40);
        marginEdits.append(pLineEdit);
        connect(pLineEdit, &QLineEdit::editingFinished, this, &PageSetupDialog::updateThumbnail);
    }
    setGroupBoxStyle(fieldGroupBox);
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
    previewLabel->setStyleSheet("background-color: #313135; none;");
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->installEventFilter(this);
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

    for (QString title : {"Верхний колонтитул", "Нижний колонтитул"}){
        QGroupBox* headerGroupBox = new QGroupBox(title, this);
        QHBoxLayout *pform_layout = new QHBoxLayout(this);
        setGroupBoxStyle(headerGroupBox);
        for (int i = 0; i < 3; i++) {
            QComboBox *pcomboBox = new QComboBox(this);
            setComboBoxStyle(
                pcomboBox, 
                {
                    "Заголовок", 
                    "URL-адрес", 
                    "Номер страницы", 
                    "Стр. № из общего количества", 
                    "Всего страниц", 
                    "Дата в кратком формате", 
                    "Дата в длинном формате", 
                    "Время"
                }, 
                0
            );
            pform_layout->addWidget(pcomboBox);
        }
        headerGroupBox->setLayout(pform_layout);
        baseLayout->addWidget(headerGroupBox);
        
    }
    

    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setContentsMargins(343, 90, 0, 0);

    QPushButton* cancelButton = new QPushButton("Отмена", this);
    setCancelButtonStyle(cancelButton);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    QPushButton* applyButton = new QPushButton("Применить");
    setApplyButtonStyle(applyButton);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(accept()));


    button_layout->addWidget(cancelButton);
    button_layout->addWidget(applyButton);
    baseLayout->addLayout(button_layout);
    setBaseLayout(baseLayout);
}

bool PageSetupDialog::eventFilter(QObject *watched, QEvent *event){
    if (watched == previewLabel && event->type() == QEvent::Resize) {
        updateThumbnail();
    }
    return QDialog::eventFilter(watched, event);
}

void PageSetupDialog::updateThumbnail() {
    QSize labelSize = previewLabel->size();
    if (labelSize.isEmpty()) return;
    printer->setPageOrientation((QPageLayout::Orientation)orientationComboBox->currentData().toInt());
    printer->setPaperSize((QPrinter::PaperSize)paperComboBox->currentData().toInt());
    qDebug() << paperComboBox->currentData();

    QRectF pageRect = printer->pageLayout().fullRectPoints();
    
   
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
    double left = marginEdits[2]->text().toDouble();
    double top = marginEdits[0]->text().toDouble();
    double right = marginEdits[3]->text().toDouble();
    double bottom = marginEdits[1]->text().toDouble();
    double marginLeft = left * mmToPx;
    double marginTop = top * mmToPx;
    double marginRight = right * mmToPx;
    double marginBottom = bottom * mmToPx;

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
    QPageLayout layout = printer->pageLayout();
    layout.setMargins(QMarginsF(left, top, right, bottom));
    printer->setPageLayout(layout);


    previewLabel->setPixmap(pixmap);
}