#include "CustomIconButtonCorner.h"
#include "qdebug.h"

CustomIconButtonCorner::CustomIconButtonCorner(QWidget *parent) :
    QFrame(parent)
{
    m_layout = new QStackedLayout();
    this->setLayout(m_layout);
    
    m_layout->setStackingMode(QStackedLayout::StackingMode::StackAll);
    
    m_button = new QPushButton();
    m_button->setIcon(QIcon(":/filter/star_gray"));
    
    this->SetColor(QColor("gray"));
    
    m_label = new QLabel();
    m_label->setText("0");
    m_label->setStyleSheet("QLabel{"
                           "padding: 0px;"
                           "margin: 0px 0px 25px 27px;"
                           "background-color: palette(window);"
                           "}");
    m_label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_button);
    
    this->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    this->setMinimumSize(QSize(40, 40));
    this->setMaximumSize(QSize(40, 40));
    
    connect(m_button, &QPushButton::clicked, this, [this]()
            {
                emit this->clicked();
            });
}

CustomIconButtonCorner::~CustomIconButtonCorner()
{
    delete m_label;
    delete m_button;
    delete m_layout;
}

void CustomIconButtonCorner::SetText(const QString& text)
{
    m_label->setText(text);
    if(text.isEmpty())
        m_label->setVisible(false);
    else
        m_label->setVisible(true);
    
    auto font = m_label->font();
    QFontMetrics fm(font);
    int width = fm.horizontalAdvance(text);
    
    m_label->setStyleSheet(QString("QLabel{"
                           "padding: 0px 0px 0px 0px;"
                           "margin: 0px 0px 25px %1px;"
                           "background-color: palette(window);"
                                   "}").arg(34 - width/*text.size()*7*/));
}

void CustomIconButtonCorner::SetIcon(const QIcon& icon, double scaleFactor)
{
    m_button->setIcon(icon);
    auto size = this->size();
    size.setWidth(size.width()*scaleFactor);
    size.setHeight(size.height()*scaleFactor);
    m_button->setIconSize(size);
}

void CustomIconButtonCorner::SetColor(QColor color)
{
    auto lighter = color.lighter();
    m_button->setStyleSheet(QString("QPushButton{"
                            "border: 1px solid rgb(%1, %2, %3);"
                            "background-color: transparent;"
                            "padding: 0px;"
                            "margin: 3px;"
                                    "}"
                            "QPushButton:hover{"
                            "border: 1px solid rgb(%4, %5, %6);"
                            "}").arg(color.red()).arg(color.green()).arg(color.blue())
                                .arg(lighter.red()).arg(lighter.green()).arg(lighter.blue()));
}

void CustomIconButtonCorner::SetTextColor(QColor color)
{
    this->setStyleSheet(QString("*{"
                                "color: rgb(%1, %2, %3);"
                                "}").arg(color.red()).arg(color.green()).arg(color.blue()));
}
