#include "PageSetupDialog.h"
#include <QTimer>
#include <QLineEdit>
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
    QFormLayout *page_form_layout = new QFormLayout();
    QComboBox *paperComboBox = new QComboBox(this);
    setComboBoxStyle(paperComboBox, {"A4", "A3", "A5"}, 16);
    addParam("Размер бумаги", page_form_layout, paperComboBox, 16);
    QComboBox *orientationComboBox = new QComboBox(this);
    setComboBoxStyle(orientationComboBox, {"Книжная", "Альбомная"}, 16);
    addParam("Ориентация", page_form_layout, orientationComboBox, 16);
    pageGroupBox->setLayout(page_form_layout);

    baseSettingsLayout->addWidget(pageGroupBox);
    

    QGroupBox* fieldGroupBox = new QGroupBox("Поля (мм)", this);
    QFormLayout *field_form_layout = new QFormLayout();
    for (QString label : {"Верхнее", "Нижнее", "Левое", "Правое"}){
        QLineEdit* pLineEdeit = new QLineEdit("20", this);
        pLineEdeit->setFixedSize(140, 28);
        pLineEdeit->setStyleSheet(
            "QLineEdit{"
            "   color: #BABABA;"
            "   font: normal normal normal 15px/18px Roboto;"
            "   background-color: #222226;"
            "   border: none;"
            "   padding-left: 10px;"
            "   margin-left: 40px;"
            "}"
        );
        addParam(label, field_form_layout, pLineEdeit, 40);
    }
    setGroupBoxStyle(fieldGroupBox);
    fieldGroupBox->setLayout(field_form_layout);
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
    previewLabel->setStyleSheet("background-color: #313135; border: none; margin: -24px -55px;");
    previewLabel->setMinimumSize(136, 186);
    QVBoxLayout* previewLayout = new QVBoxLayout(this);
    previewLayout->addWidget(previewLabel, 24, Qt::AlignCenter);
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

    QPushButton* applyButton = new QPushButton("Применить");
    setApplyButtonStyle(applyButton);


    button_layout->addWidget(cancelButton);
    button_layout->addWidget(applyButton);
    baseLayout->addLayout(button_layout);
    setBaseLayout(baseLayout);
}

void PageSetupDialog::updateThumbnail() {
    QSize labelSize = previewLabel->size();
    if (labelSize.isEmpty()) return;


    QRectF pageRect = printer->pageLayout().fullRectPoints();
    
   
    double scale = qMin((double)labelSize.width() / pageRect.width(), 
                        (double)labelSize.height() / pageRect.height()) * 0.9; 

    QSize pixmapSize = (pageRect.size() * scale).toSize();
    

    QPixmap pixmap(pixmapSize);
    pixmap.fill(Qt::white);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.scale(scale, scale);


    doc->setPageSize(pageRect.size()); 
    doc->drawContents(&painter);
    
    painter.end();


    previewLabel->setPixmap(pixmap);
}