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

class QFSpoiler : public QWidget {
    Q_OBJECT
private:
    QGridLayout mainLayout;
    QToolButton toggleButton;
    QFrame headerLine;
    QParallelAnimationGroup toggleAnimation;
    QScrollArea contentArea;
    int animationDuration{300};
    const int defaultToolButonSize = 21;
    bool m_expanded = false;
public:
    explicit QFSpoiler(const QString & title = "", const int animationDuration = 300, QWidget *parent = 0);
    ~QFSpoiler();
    void setContentLayout(QLayout & contentLayout);
    
    void HideSomeLayoutUnderSpoiler(QLayout* contentLayout);
    void RecalcSize();

    void setTitleFontStyle(QFont font);
    void setTitleColor(QString hexColor);
    void setTitle(QString Title);
    
    void expand(bool expanded, bool withAnimation = true);
signals:
    void aboutToBeExpanded();
};

#endif // SPOILER_H
