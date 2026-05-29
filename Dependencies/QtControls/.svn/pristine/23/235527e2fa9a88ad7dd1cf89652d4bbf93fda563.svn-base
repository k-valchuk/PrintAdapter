#ifndef FILTERPRESETROW_H
#define FILTERPRESETROW_H

#include <QFrame>
#include "DraggableFrame.h"
#include "qcoreevent.h"
#include <QVariant>
#include "FilterRowToolTip.h"

namespace Ui {
class FilterPresetRow;
}

class FilterPresetListWidget;
class FilterPresetRow : public DraggableFrame, public FilterRowToolTip
{
    Q_OBJECT
    Q_PROPERTY(bool myHover READ myHover WRITE setMyHover NOTIFY myHoverChanged)
    Q_PROPERTY(QColor onHoverColor READ onHoverColor WRITE setOnHoverColor)
    Q_PROPERTY(QColor onHoverTextColor READ onHoverTextColor WRITE setOnHoverTextColor)
    
    FilterPresetListWidget* m_parentList;
    bool m_myHover = false;
    bool m_isInEditMode = false;
    QColor m_onHoverColor, m_onHoverTextColor;
public:
    explicit FilterPresetRow(FilterPresetListWidget *parent = nullptr);
    ~FilterPresetRow();
    
    bool myHover()
    {
        return m_myHover;
    }
    void setMyHover(bool isHover)
    {
        setMyHoverPrivate(isHover);
        emit myHoverChanged(m_myHover);
    }
    void setMyHoverSilent(bool isHover)
    {
        setMyHoverPrivate(isHover);
    }
    
    QColor onHoverColor() { return m_onHoverColor; }
    void setOnHoverColor(QColor color);
    QColor onHoverTextColor() { return m_onHoverTextColor; }
    void setOnHoverTextColor(QColor color);
private:
    Ui::FilterPresetRow *ui;
    
    void setMyHoverPrivate(bool isHover);
public:
    bool event(QEvent* event) override;
    
    void SetEditing(bool isEdit);
public slots:
    void Edit();
    void SetName(QString name);
    void SetPrefix(QString prefix);
signals:
    void myHoverChanged(bool hover);
    void deleteClicked();
    void editClicked();
    void nameEditRequested(QString oldName, QString newName);
};

#endif // FILTERPRESETROW_H
