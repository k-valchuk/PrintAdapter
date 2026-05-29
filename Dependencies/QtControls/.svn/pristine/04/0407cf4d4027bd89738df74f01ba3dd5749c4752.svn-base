#include "QFSpoiler.h"
#include "qapplication.h"
#include "qdebug.h"
#include <QPropertyAnimation>

QFSpoiler::QFSpoiler(const QString & title, const int animationDuration, QWidget *parent) : QWidget(parent), animationDuration(animationDuration)
{
    toggleButton.setObjectName("toggleButton");
    toggleButton.setStyleSheet("QToolButton#toggleButton { border: none; }");
    toggleButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggleButton.setArrowType(Qt::ArrowType::RightArrow);
    toggleButton.setText(title);
    toggleButton.setCheckable(true);
    toggleButton.setChecked(false);

    headerLine.setFrameShape(QFrame::HLine);
    headerLine.setFrameShadow(QFrame::Sunken);
    headerLine.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    headerLine.setVisible(false);
    
    contentArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    contentArea.setFocusPolicy(Qt::ClickFocus);
    toggleButton.setFocusPolicy(Qt::ClickFocus);
    // start out collapsed
    contentArea.setMaximumHeight(0);
    contentArea.setMinimumHeight(0);
    // let the entire widget grow and shrink with its content
    toggleAnimation.addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    toggleAnimation.addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    toggleAnimation.addAnimation(new QPropertyAnimation(&contentArea, "maximumHeight"));
    // don't waste space
    mainLayout.setVerticalSpacing(0);
    mainLayout.setContentsMargins(0, 0, 0, 0);
    int row = 0;
    mainLayout.addWidget(&toggleButton, row, 0, 1, 1, Qt::AlignLeft);
    mainLayout.addWidget(&headerLine, row++, 2, 1, 1);
    mainLayout.addWidget(&contentArea, row, 0, 1, 3);
    setLayout(&mainLayout);

    QObject::connect(&toggleButton, &QToolButton::clicked, [this](const bool checked) {
        toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
        toggleAnimation.setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
        if(checked){
            emit aboutToBeExpanded();
            setContentLayout(*contentArea.layout());
        }
        toggleAnimation.start();
        m_expanded = checked;
    });
}

void QFSpoiler::setContentLayout(QLayout & contentLayout)
{
    contentArea.setLayout(&contentLayout);
    const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight();
    auto contentHeight = contentLayout.sizeHint().height();
    for (int i = 0; i < toggleAnimation.animationCount() - 1; ++i) {
        QPropertyAnimation * spoilerAnimation = static_cast<QPropertyAnimation *>(toggleAnimation.animationAt(i));
        spoilerAnimation->setDuration(animationDuration);
        spoilerAnimation->setStartValue(collapsedHeight);
        spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
    }
    QPropertyAnimation * contentAnimation = static_cast<QPropertyAnimation *>(toggleAnimation.animationAt(toggleAnimation.animationCount() - 1));
    contentAnimation->setDuration(animationDuration);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);
}

void QFSpoiler::setTitleFontStyle(QFont font)
{
    toggleButton.setFont(font);
}

void QFSpoiler::setTitle(QString Title)
{
    toggleButton.setText(Title);
}

void QFSpoiler::expand(bool expanded, bool withAnimation)
{
    toggleButton.setChecked(expanded);
    toggleButton.setArrowType(expanded ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    toggleAnimation.setDirection(expanded ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    if(expanded){
        emit aboutToBeExpanded();
        setContentLayout(*contentArea.layout());
    }
    
    if(withAnimation)
        toggleAnimation.start();
    else
    {
        const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight();
        auto contentHeight = contentArea.layout()->sizeHint().height();
        
        if(expanded)
        {
            this->setMinimumHeight(collapsedHeight + contentHeight);
            this->setMaximumHeight(collapsedHeight + contentHeight);
            contentArea.setMaximumHeight(contentHeight);
        }
        else
        {
            this->setMinimumHeight(collapsedHeight);
            this->setMaximumHeight(collapsedHeight);
            contentArea.setMaximumHeight(0);
        }
    }
    
    m_expanded = expanded;
}

void QFSpoiler::HideSomeLayoutUnderSpoiler(QLayout *contentLayout)
{
    auto parentWgt = contentLayout->parentWidget();
    if(parentWgt)
    {
        setContentLayout(*contentLayout);
        
        auto nlayout = new QVBoxLayout(parentWgt);
        parentWgt->setLayout(nlayout);
        nlayout->addWidget(this);
    }
}

void QFSpoiler::RecalcSize()
{
    if(m_expanded)
    {
        auto collapsedHeight = qMax(0, sizeHint().height() - contentArea.maximumHeight());
        auto contentHeight = qMax(0, contentArea.layout()->sizeHint().height());
        
        this->setMinimumHeight(collapsedHeight);
        this->setMaximumHeight(collapsedHeight);
        contentArea.setMaximumHeight(0);
        
        collapsedHeight = qMax(0, sizeHint().height() - contentArea.maximumHeight());
        contentHeight = qMax(0, contentArea.layout()->sizeHint().height());
        
        this->setMinimumHeight(collapsedHeight + contentHeight);
        this->setMaximumHeight(collapsedHeight + contentHeight);
        contentArea.setMaximumHeight(contentHeight);
    }
}

QFSpoiler::~QFSpoiler()
{
    
}
