//-----------------------------------------------------------------------------------------------------------------------
//Компоновка с учётом соотношения сторон
//-----------------------------------------------------------------------------------------------------------------------
#ifndef ASPECTRATIOSINGLEITEMLAYOUT_H
#define ASPECTRATIOSINGLEITEMLAYOUT_H

#include <QLayout>

namespace AspectRatio
{
    constexpr float aspectRatio_16_9 = 16.0f / 9.0f;
    constexpr float aspectRatio_4_3 = 4.0f / 3.0f;
}


class AspectRatioSingleItemLayout : public QLayout
{

public:

    AspectRatioSingleItemLayout(QWidget *parent = nullptr, double aspR = AspectRatio::aspectRatio_16_9);
    ~AspectRatioSingleItemLayout();

    void setAspectRatio(double aspR);                        //установить соотношение сторон

    //------------------------------------override-----------------------------------------------------//

    virtual int count() const override;
    virtual QLayoutItem *itemAt(int i) const override;
    virtual QLayoutItem *takeAt(int) override;
    virtual Qt::Orientations expandingDirections() const override;
    virtual bool hasHeightForWidth() const override;
    virtual int heightForWidth(int width) const override;
    virtual void setGeometry(const QRect &rect) override;
    virtual QSize sizeHint() const override;
    virtual QSize minimumSize() const override;
    void addItem(QLayoutItem *item) override;

private:

    QLayoutItem *layItem;

    double aspectRatio;
};

#endif // ASPECTRATIOSINGLEITEMLAYOUT_H
