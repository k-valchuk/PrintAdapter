#include "DockWidgetTabButton.h"
#include <QStylePainter>
#include "QStyleOptionButton"
#include "qmenu.h"

//мин. ширина кнопки
constexpr int MINIMUM_WIDTH = 15;
//макс. ширина кнопки
constexpr int MAXIMUM_WIDTH = 120;
//отступ текста
constexpr int MARGIN_SIZE = 6;
//высота кнопки
constexpr int FIX_HEIGHT = 25;

DockWidgetTabButton::DockWidgetTabButton(const QString& text, Qt::Orientation orient)
	: QPushButton(text, nullptr)
    , tabAction(nullptr), tabOrientation(orient)
{
    setToolTip(text);

    setFocusPolicy(Qt::NoFocus);//для корректной обработки focusOut в dockWidget

    //задать размер кнопки с учетом ширины текста
    int fw = fontMetrics().width(text) + MARGIN_SIZE * 2;

    if (fw < MINIMUM_WIDTH)
        fw = MINIMUM_WIDTH;
    else if (fw > MAXIMUM_WIDTH)
        fw = MAXIMUM_WIDTH;

    if(tabOrientation == Qt::Vertical)
        setFixedSize(FIX_HEIGHT, fw);
    else if(tabOrientation == Qt::Horizontal)
        setFixedSize(fw, FIX_HEIGHT);

}

DockWidgetTabButton::~DockWidgetTabButton()
{
}

void DockWidgetTabButton::setTabText(const QString& text)
{
    //вместить текст в кнопку
    int aw = (tabOrientation == Qt::Horizontal) ? width() - MARGIN_SIZE : height() - MARGIN_SIZE;
    setText(fontMetrics().elidedText(text, Qt::ElideRight, aw));
}

QSize DockWidgetTabButton::sizeHint() const
{//для вертикальной кнопки перевернем размер
	QSize size = QPushButton::sizeHint();
    if(tabOrientation == Qt::Vertical)
		size.transpose();

	return size;
}

void DockWidgetTabButton::paintEvent(QPaintEvent* )
{
	QStylePainter painter(this);
    if (tabOrientation == Qt::Vertical)
    {//для вертикальной перевернуть систему координат
        painter.rotate(90);
        painter.translate(0, -width());
    }
    //рисуем кнопку
	painter.drawControl(QStyle::CE_PushButton, getStyleOption());
}

QStyleOptionButton DockWidgetTabButton::getStyleOption() const
{
    //собираем заданные фичи и состояние кнопки
    QStyleOptionButton opt;
	opt.initFrom(this);

    if(tabOrientation == Qt::Vertical)
	{
		QSize size = opt.rect.size();
		size.transpose();
		opt.rect.setSize(size);
	}

	opt.features = QStyleOptionButton::None;

    if(isFlat())
		opt.features |= QStyleOptionButton::Flat;	
    if(menu())
        opt.features |= QStyleOptionButton::HasMenu;
    if(autoDefault() || isDefault())
        opt.features |= QStyleOptionButton::AutoDefaultButton;
    if(isDefault())
		opt.features |= QStyleOptionButton::DefaultButton;	

    if(isDown() || (menu() && menu()->isVisible()))
        opt.state |= QStyle::State_Sunken;
    if(isChecked())
		opt.state |= QStyle::State_On;	
    if(!isFlat() && !isDown())
		opt.state |= QStyle::State_Raised;	

	opt.text = text();
	opt.icon = icon();
	opt.iconSize = iconSize();

	return opt;
}

void DockWidgetTabButton::resizeEvent(QResizeEvent* )
{//перерисовать текст при изменении размера
    setTabText(text());
}
