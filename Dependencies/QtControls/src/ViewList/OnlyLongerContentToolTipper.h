#ifndef ONLYLONGERCONTENTTOOLTIPPER_H
#define ONLYLONGERCONTENTTOOLTIPPER_H

#include <QObject>

class OnlyLongerContentToolTipper : public QObject {
    
public:
    
    explicit OnlyLongerContentToolTipper(QObject* parent = NULL);
protected:
    
    bool eventFilter(QObject* obj, QEvent* event);
};

class HeaderViewOnlyLongerContentToolTipper : public QObject {
    
public:
    
    explicit HeaderViewOnlyLongerContentToolTipper(QObject* parent = NULL);
protected:
    
    bool eventFilter(QObject* obj, QEvent* event);
};


#endif // ONLYLONGERCONTENTTOOLTIPPER_H
