#ifndef SPOILER_H
#define SPOILER_H
#pragma once
#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>
#include <QGroupBox>
#include <QPropertyAnimation>
#include "SpoilerToolButton.h"

class Spoiler : public QWidget {
    Q_OBJECT
private:
    QGridLayout mainLayout;
    SpoilerToolButton* toggleButton;
    QFrame headerLine;
    QParallelAnimationGroup toggleAnimation;
    QScrollArea contentArea;
    int animationDuration{300};
    const int defaultToolButonSize = 21;
    int defaultSpacerSize = 32;
    QFrame* leftSideLine;
    QHBoxLayout* lineLayout;

    bool hoveredState = false;
    bool checkedState = false;

    QString arrowRightHoverUrl;
    QString arrowDownHoverUrl;
    QString arrowRightUrl;
    QString arrowDownUrl;
    
    static QLayout* findParentLayout(QLayout* w, QLayout* topLevelLayout);
    static QLayout* findParentLayout(QLayout* w);

    void changeToolButtonIcon();

    void constructorCommonFunc(const QString & title);

    QTimer hideContentTimer;
    QSpacerItem* spacerToolButton;

    bool needLeftSideLine = false;
public:
    explicit Spoiler(const QString & title = "", const int animationDuration = 300, QWidget *parent = 0);
    explicit Spoiler(QString startArrowUrl, const  QString & title = "", const int animationDuration = 300, QWidget *parent = 0);
    ~Spoiler();
    void setContentLayout(QLayout & contentLayout);
    
    void HideSomeLayoutUnderSpoiler(QLayout* contentLayout);

    void changeCurrentLayoutSize(int currMaxHeight);

    void setSpacerHeight(int height);

    void setLeftSideLine();
    void setTitleFontStyle(QFont font);
    void setHeaderLineColor(QString hexColor);
    void setToolButtonProperty(QString propName, QVariant prop);
    void setBackgroundColor(QString hexColor);
    void setTitle(QString Title);
    void setArrowIconUrls(QString rightHoverArrow, QString downHoverArrow, QString rightArrow, QString downArrow);

public slots:
    void showContentArea();
    void comletedAnimation();
};

#endif // SPOILER_H
