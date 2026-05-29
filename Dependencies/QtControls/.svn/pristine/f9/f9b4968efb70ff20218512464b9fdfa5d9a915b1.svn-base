#include "Spoiler.h"
#include <QPropertyAnimation>

Spoiler::Spoiler(const QString & title, const int animationDuration, QWidget *parent) : QWidget(parent), animationDuration(animationDuration)
{
    constructorCommonFunc(title);

    toggleButton->setArrowType(Qt::ArrowType::RightArrow);

    QObject::connect(toggleButton, &QToolButton::clicked, [this](const bool checked) {
        toggleButton->setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
        toggleAnimation.setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
        toggleAnimation.start();
    });

    QObject::connect(toggleButton, SIGNAL(clicked()), this, SLOT(showContentArea()));

    connect(&toggleAnimation, SIGNAL(finished()), this, SLOT(comletedAnimation()));
}

Spoiler::Spoiler(QString startArrowUrl, const  QString & title, const int animationDuration, QWidget *parent)  : QWidget(parent), animationDuration(animationDuration)
{
    constructorCommonFunc(title);
    toggleButton->setArrowType(Qt::ArrowType::NoArrow);
    toggleButton->setIcon(QIcon(startArrowUrl));

    connect(&hideContentTimer, &QTimer::timeout, [this]()
            {
                contentArea.hide();
                if(needLeftSideLine)
                    leftSideLine->hide();
                spacerToolButton->changeSize(32, defaultToolButonSize, QSizePolicy::Fixed, QSizePolicy::Fixed);
                hideContentTimer.stop();
            });

    QObject::connect(toggleButton, &SpoilerToolButton::sigChangeHoverState, [this](const bool hovered) {
        hoveredState = hovered;
        changeToolButtonIcon();
    });

    QObject::connect(toggleButton, &QToolButton::clicked, [this](const bool checked) {
        if(!checked)
        {
            hideContentTimer.start(180);
        }
        else
        {
            spacerToolButton->changeSize(32, defaultSpacerSize, QSizePolicy::Fixed, QSizePolicy::Fixed);
            if(needLeftSideLine)
                leftSideLine->show();
            contentArea.show();
        }

        checkedState = checked;
        changeToolButtonIcon();
        toggleAnimation.setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
        toggleAnimation.start();
    });

    QObject::connect(toggleButton, SIGNAL(clicked()), this, SLOT(showContentArea()));

    connect(&toggleAnimation, SIGNAL(finished()), this, SLOT(comletedAnimation()));
}

void Spoiler::constructorCommonFunc(const QString & title)
{
    toggleButton = new SpoilerToolButton(this);
    toggleButton->setObjectName("toggleButton");
    toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setTitle(title);
    toggleButton->setCheckable(true);
    toggleButton->setChecked(false);

    headerLine.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    headerLine.setObjectName("headerLine");

    leftSideLine = new QFrame();
    leftSideLine->setObjectName(QString::fromUtf8("leftSideLine"));
    leftSideLine->setStyleSheet(QString::fromUtf8("QFrame#leftSideLine {color: #6F8CB7;}"));
    leftSideLine->setFrameShadow(QFrame::Plain);
    leftSideLine->setLineWidth(2);
    leftSideLine->setFrameShape(QFrame::VLine);
    leftSideLine->hide();
    lineLayout = new QHBoxLayout();
    lineLayout->addWidget(leftSideLine);
    lineLayout->addWidget(&contentArea);
    lineLayout->setContentsMargins(0,0,0,0);

    contentArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    contentArea.setFocusPolicy(Qt::ClickFocus);
    toggleButton->setFocusPolicy(Qt::ClickFocus);
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
    spacerToolButton = new QSpacerItem(32,32, QSizePolicy::Fixed, QSizePolicy::Fixed);
    mainLayout.addItem(spacerToolButton, row, 0, 1, 1, Qt::AlignLeft);
    mainLayout.addWidget(&headerLine, row++, 2, 1, 1);
    mainLayout.addLayout(lineLayout, row, 0, 1, 3, Qt::AlignLeft);
    setLayout(&mainLayout);
}

void Spoiler::setSpacerHeight(int height)
{
    if(height < defaultToolButonSize)
        return;

    defaultSpacerSize = height;
    spacerToolButton->changeSize(32,height, QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void Spoiler::changeToolButtonIcon()
{
    if(checkedState && hoveredState)
        toggleButton->setIcon(QIcon(arrowDownHoverUrl));

    if(checkedState && !hoveredState)
        toggleButton->setIcon(QIcon(arrowDownUrl));

    if(!checkedState && hoveredState)
        toggleButton->setIcon(QIcon(arrowRightHoverUrl));

    if(!checkedState && !hoveredState)
        toggleButton->setIcon(QIcon(arrowRightUrl));
}

void Spoiler::setArrowIconUrls(QString rightHoverArrow, QString downHoverArrow, QString rightArrow, QString downArrow)
{
    arrowRightHoverUrl = rightHoverArrow;
    arrowDownHoverUrl = downHoverArrow;
    arrowRightUrl = rightArrow;
    arrowDownUrl = downArrow;
}

void Spoiler::setContentLayout(QLayout & contentLayout)
{
    delete contentArea.layout();
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

void Spoiler::changeCurrentLayoutSize(int currMaxHeightIn)
{
    int currMaxHeight = currMaxHeightIn + contentArea.layout()->contentsMargins().top() + contentArea.layout()->contentsMargins().bottom();

    contentArea.setMaximumHeight(currMaxHeight);
    auto contentHeight = currMaxHeight;

    for (int i = 0; i < toggleAnimation.animationCount() - 1; ++i) {
        QPropertyAnimation * spoilerAnimation = static_cast<QPropertyAnimation *>(toggleAnimation.animationAt(i));
        spoilerAnimation->setStartValue(defaultSpacerSize);
        spoilerAnimation->setEndValue(defaultSpacerSize + contentHeight);

        spoilerAnimation->setDuration(defaultSpacerSize + contentHeight < 100? animationDuration/2 : animationDuration);
    }
    QPropertyAnimation * contentAnimation = static_cast<QPropertyAnimation *>(toggleAnimation.animationAt(toggleAnimation.animationCount() - 1));
    contentAnimation->setDuration(defaultSpacerSize + contentHeight < 100? animationDuration*1/3 : animationDuration*2/3);
    contentAnimation->setEndValue(contentHeight);
    contentAnimation->setStartValue(0);

    if(!toggleButton->isChecked())
    {
        contentArea.setFixedHeight(0);
        contentArea.resize(contentArea.width(), 0);

        this->setFixedHeight(contentArea.maximumHeight() + defaultToolButonSize);
        this->resize(contentArea.maximumHeight() + defaultToolButonSize,this->width());

        toggleButton->updateGeometry();
    }
    else
    {
        contentArea.setFixedHeight(contentArea.maximumHeight());
        contentArea.resize(contentArea.width(), contentArea.maximumHeight());

        this->setFixedHeight(contentArea.maximumHeight() + defaultSpacerSize);
        this->resize(contentArea.maximumHeight() + defaultSpacerSize,this->width());
    }
}

void Spoiler::setLeftSideLine()
{
    lineLayout->setContentsMargins(10,0,0,0);
    lineLayout->setSpacing(6);
    mainLayout.setHorizontalSpacing(0);
    leftSideLine->show();
    needLeftSideLine = true;
}

void Spoiler::setTitleFontStyle(QFont font)
{
    toggleButton->setFont(font);
}

void Spoiler::setHeaderLineColor(QString hexColor)
{
    headerLine.setStyleSheet("QFrame#headerLine {background-color:"+hexColor+"}");
}

void Spoiler::setToolButtonProperty(QString propName, QVariant prop)
{
    toggleButton->setProperty(propName.toStdString().c_str(), prop);
}

void Spoiler::setBackgroundColor(QString hexColor)
{
    contentArea.setStyleSheet("QScrollArea { border: none; background-color:"+hexColor+"}");
}

void Spoiler::setTitle(QString Title)
{
    int curWidth = toggleButton->fontMetrics().width(Title);
    toggleButton->setFixedWidth(curWidth + 25);
    toggleButton->resize(curWidth + 25, defaultToolButonSize);
    toggleButton->setText(Title);
}

QLayout* Spoiler::findParentLayout(QLayout* w, QLayout* topLevelLayout)
{
  for (QObject* qo: topLevelLayout->children())
  {
     QLayout* layout = qobject_cast<QLayout*>(qo);
     if (layout != nullptr)
     {
        if (layout->indexOf(w) > -1)
          return layout;
        else if (!layout->children().isEmpty())
        {
          layout = findParentLayout(w, layout);
          if (layout != nullptr)
            return layout;
        }
     }
  }
  return nullptr;
}

QLayout* Spoiler::findParentLayout(QLayout* w)
{
    if (w->parentWidget() != nullptr)
        if (w->parentWidget()->layout() != nullptr)
            return findParentLayout(w, w->parentWidget()->layout());
    return nullptr;
}

void Spoiler::HideSomeLayoutUnderSpoiler(QLayout *contentLayout)
{
    auto parentLayout = findParentLayout(contentLayout);
    if(parentLayout)
        if(auto parentBoxLayout = qobject_cast<QBoxLayout*>(parentLayout))
        {
            auto index = parentBoxLayout->indexOf(contentLayout);
            parentBoxLayout->removeItem(contentLayout);
            setContentLayout(*contentLayout);
            parentBoxLayout->insertWidget(index, this);
        }
}

void Spoiler::showContentArea()
{
    if(toggleButton->isChecked())
    {
        contentArea.setFixedHeight(contentArea.maximumHeight());
        contentArea.resize(contentArea.width(), contentArea.maximumHeight());

        this->setFixedHeight(contentArea.maximumHeight() + defaultSpacerSize);
        this->resize(contentArea.maximumHeight() + defaultSpacerSize,this->width());
    }
}

void Spoiler::comletedAnimation()
{
    if(!toggleButton->isChecked())
    {
        contentArea.setFixedHeight(0);
        contentArea.resize(0,contentArea.width());

        this->setFixedHeight(contentArea.maximumHeight() + defaultToolButonSize);
        this->resize(contentArea.maximumHeight() + defaultToolButonSize,this->width());
    }
}

Spoiler::~Spoiler()
{
    delete leftSideLine;
    delete lineLayout;
}
