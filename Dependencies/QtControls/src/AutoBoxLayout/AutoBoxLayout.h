
#ifndef AUTOBOXLAYOUT_H
#define AUTOBOXLAYOUT_H

#include <QLayout>
#include <QBoxLayout>
#include <QRect>
#include <QStyle>


class AutoBoxLayout : public QLayout
{
public:
    enum class Alignment {
        Start,
        Center,
        End
    };
    enum class SideAlignment {
        Start,
        Center,
        End,
        Width,
        WidthAround
    };
    
    void addItem(QLayoutItem *item) override;
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int) const override;
    int heightForKnownWidth(int width);
    int count() const override;
    QLayoutItem *itemAt(int index) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect &rect) override;
    QSize sizeHint() const override;
    QLayoutItem *takeAt(int index) override;
    
    void setAlignmentWhenHorizontal(Alignment main,
                                    Alignment baseline,
                                    SideAlignment side);
    void setAlignmentWhenVertical(Alignment main,
                                    Alignment baseline,
                                    SideAlignment side);
    void setSpacing(int hSpacing, int vSpacing);
    void setPriorityOrientation(bool horizontal);
    
private:
    QList<QLayoutItem*> m_items;
    bool m_horizontalPriority = true;
    int m_hSpacing = 4, m_vSpacing = 4;
    
    struct AlignmentState{
        Alignment main;
        Alignment baseline;
        SideAlignment side;
    }
    m_alignmentWhenH{Alignment::Center, Alignment::Center, SideAlignment::Width},
    m_alignmentWhenV{Alignment::Center, Alignment::Center, SideAlignment::WidthAround};
    
    bool shouldBeHorizontal(QRect rect) const;
    int doLayout(QRect rect, bool testOnly = true) const;
    int smartSpacing(QStyle::PixelMetric pm) const;
};

#endif // AUTOBOXLAYOUT_H
