#include "PeakMeterWidget.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QColor>
#include <vector>
#include <QGradient>
#include <QPainterPath>

#define THIN_PEAK_METER_COLUMN_PX_WIDTH 2 //in px
#define WIDE_PEAK_METER_COLUMN_PX_WIDTH 4

#define PEAK_METER_SEPARATOR 1 //px
#define MIN_PEAKMETER_VALUE -60
#define PEAK_SHADOW_WIDTH 1
#define BOTTOM_TITLE_HEIGHT 10
#define TOP_PEAK_MARGING 6
#define MAX_CHANNELS 8
#define MAX_PEAK_MARK_HEIGHT 3 //px

#define LEGEND_WIDTH 10

#define WARNING_DB_VALUE -18
#define CRITICAL_DB_VALUE -9

const int logScaleSize = 61;
double logScaleValues[logScaleSize] =
{ -60.000000, -35.065106, -29.294118, -25.857116, -23.401066, -21.488602, -19.922177, -18.595547, -17.444950, -16.429094, -15.519704,
-14.696566/*50*/, -13.944726, -13.252812, -12.611972, -12.015181, -11.456772, -10.932104, -10.437331, -9.969230, -9.525071,
-9.102523, -8.699581, -8.314507, -7.945781, -7.592073, -7.252207, -6.925140, -6.609943, -6.305785, -6.011918, -5.727669,
-5.452429, -5.185642, -4.926807, -4.675462, -4.431186, -4.193593, -3.962326, -3.737057, -3.517483, -3.303324, -3.094317,
-2.890223, -2.690814, -2.495880, -2.305225, -2.118666, -1.936029, -1.757154, -1.581888, -1.410089, -1.241622, -1.076360,
-0.914185,  -0.754982, -0.598644, -0.445071, -0.294166, -0.145838, 0.000000 };
std::vector<double> logScale(logScaleValues, logScaleValues + logScaleSize);

QFont legendFont("Segoe UI", 8);

// Привидение логорифмического значения к шкале c точностью 
int getLogHeight(double value)
{
	std::vector<double>::iterator iter = std::upper_bound(logScale.begin(), logScale.end(), value);

	return iter - logScale.begin();
}

void PeakMeterWidget::setVisibleChannelsNum(int num)
{
	numVisibleChannels = num;
}

PeakMeterWidget::PeakMeterWidget() : QWidget(), numVisibleChannels(2), isActive(false),	alpha(255), isAutoScale(true),
	peakWidth(WIDE_PEAK_METER_COLUMN_PX_WIDTH)
{
	setViewMode(eWideMode);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_maxPeakHeightPerSecond.resize(8);
}

PeakMeterWidget::~PeakMeterWidget()
{

}

void PeakMeterWidget::setViewMode(EViewMode mode)
{
	viewMode = mode;

	switch(viewMode)
	{
	case eThinMode:
		peakWidth = THIN_PEAK_METER_COLUMN_PX_WIDTH;	
		break;
	case eWideMode:
		peakWidth = WIDE_PEAK_METER_COLUMN_PX_WIDTH;
		break;
	}
}

void PeakMeterWidget::setTransparency(int a)
{
	alpha = a;
}

void PeakMeterWidget::paintEvent(QPaintEvent * event)
{
	Q_UNUSED(event);

	QPainter painter(this);

	// Если установлено значение 0 - это соответствует автоматическому кол-ву каналов 
	const int numChannels = !numVisibleChannels ? chValues.size() : numVisibleChannels;

	drawLegendBack(painter, numChannels, rect(), viewMode, isActive);
	drawPeaks(painter, chValues, numChannels, rect(), viewMode, alpha);
	drawLegendLines(painter, numChannels, rect(), viewMode, isAutoScale, isActive);
}

void PeakMeterWidget::setChannelValues(QVector<double> values)
{
	chValues = values;
}

void PeakMeterWidget::setIsActive(bool is)
{
	isActive = is; 
}

void PeakMeterWidget::setIsAutoScale(bool is)
{
	isAutoScale = is;
}

// Для удобства все высоты отрицательные (чтобы откладывать прямоугольники снизу)
int getRealPeakmeterPxHeight(double value, const int maxHeightPx)
{
	return (value - MIN_PEAKMETER_VALUE) *  maxHeightPx / MIN_PEAKMETER_VALUE;
}

void PeakMeterWidget::drawPeaks(QPainter& painter, QVector<double>& chValues, int numVisibleChannels, const QRect& rect,
    EViewMode mode, int alpha, bool needPeaksGradient)
{
	int dx = rect.x();

	QColor greenPeak = Qt::green;
	greenPeak.setAlpha(alpha);

	const int peakWidth = mode == eThinMode ? THIN_PEAK_METER_COLUMN_PX_WIDTH : WIDE_PEAK_METER_COLUMN_PX_WIDTH;

	const int maxHeightPx = rect.height() - TOP_PEAK_MARGING - BOTTOM_TITLE_HEIGHT;
	// Все высоты отрицательные
	const int maxGreenPeakHeight = getRealPeakmeterPxHeight(WARNING_DB_VALUE, maxHeightPx);
	const int maxYeelowPeakHeight = getRealPeakmeterPxHeight(MIN_PEAKMETER_VALUE - WARNING_DB_VALUE + CRITICAL_DB_VALUE, maxHeightPx);
	// Нижние границы прямоугольников
	const int bottomGreenPox = rect.bottom() - BOTTOM_TITLE_HEIGHT;
	const int bottomYellowPos = rect.bottom() - BOTTOM_TITLE_HEIGHT + maxGreenPeakHeight;
	const int bottomRedPos = rect.bottom() - BOTTOM_TITLE_HEIGHT + maxGreenPeakHeight + maxYeelowPeakHeight;

	for (int i = 0; i < chValues.size() && i < numVisibleChannels; i++)
	{
		dx += PEAK_METER_SEPARATOR;

		QRect peackRect;
		peackRect.setX(dx);
		peackRect.setY(bottomGreenPox);
		peackRect.setWidth(peakWidth);        

        if(!needPeaksGradient){
            if (chValues[i] <= WARNING_DB_VALUE) // Если значение в допустимых(зеленых) пределах
            {
                peackRect.setHeight(getRealPeakmeterPxHeight(chValues[i], maxHeightPx));
                painter.fillRect(peackRect, greenPeak);
            }

            if (chValues[i] > WARNING_DB_VALUE) // Если значение в недопустимой области
            {
                peackRect.setHeight(maxGreenPeakHeight);
                painter.fillRect(peackRect, greenPeak);	// Рисуем полностью зеленый столбик

                int warnPeakHeight = 0;
                if (chValues[i] < CRITICAL_DB_VALUE)	// Если значение в пределах желтого значения
                    warnPeakHeight = getRealPeakmeterPxHeight(chValues[i], maxHeightPx) - maxGreenPeakHeight;
                else
                    warnPeakHeight = maxYeelowPeakHeight;

                QColor warnPeak = Qt::yellow;
                warnPeak.setAlpha(alpha);

                peackRect.setY(bottomYellowPos);
                peackRect.setHeight(warnPeakHeight);
                painter.fillRect(peackRect, warnPeak);

                if (chValues[i] > CRITICAL_DB_VALUE) // Если значение в критической области
                {
                    QColor critPeak = Qt::red;
                    critPeak.setAlpha(alpha);

                    peackRect.setY(bottomRedPos);
                    peackRect.setHeight(getRealPeakmeterPxHeight(chValues[i], maxHeightPx) - maxGreenPeakHeight - maxYeelowPeakHeight);
                    painter.fillRect(peackRect, critPeak);
                }
            }
        }
        else {
            QRect gradientRect(peackRect);
            gradientRect.setHeight( - (rect.height() - TOP_PEAK_MARGING - BOTTOM_TITLE_HEIGHT));
            peackRect.setHeight(getRealPeakmeterPxHeight(chValues[i], maxHeightPx));

            QLinearGradient m_gradient(gradientRect.topLeft(),gradientRect.bottomRight());
            m_gradient.setColorAt(0.0, QColor(119 , 164, 31));
            m_gradient.setColorAt(0.36, QColor(101 , 138, 26));
            m_gradient.setColorAt(0.77, QColor(255 , 192, 13));
            m_gradient.setColorAt(0.93, QColor(187 , 32, 32));
            m_gradient.setColorAt(1.0, QColor(180 , 35, 26));

            painter.setRenderHint(QPainter::Antialiasing);
            QPainterPath path;
            path.addRoundedRect(QRectF(peackRect), 2, 2);
            painter.fillPath(path, m_gradient);
        }

		dx += peakWidth + PEAK_METER_SEPARATOR;
	}
}

void PeakMeterWidget::drawMaxPeaks(QPainter& painter, QVector<double>& chValues, int numVisibleChannels, const QRect& rect,
    EViewMode mode, PeakMeterWidget* thisWidget)
{
    int dx = rect.x();
    const int bottomPosY = rect.bottom() - BOTTOM_TITLE_HEIGHT;
    const int maxHeightPx = rect.height() - TOP_PEAK_MARGING - BOTTOM_TITLE_HEIGHT;
    const int peakWidth = mode == eThinMode ? THIN_PEAK_METER_COLUMN_PX_WIDTH : WIDE_PEAK_METER_COLUMN_PX_WIDTH;

    for (int i = 0; i < chValues.size() && i < numVisibleChannels; i++)
    {
        dx += PEAK_METER_SEPARATOR;

        QRect peackRect;
        peackRect.setX(dx);
        peackRect.setY(bottomPosY);
        peackRect.setWidth(peakWidth);
        peackRect.setHeight(getRealPeakmeterPxHeight(chValues[i], maxHeightPx));

        thisWidget->m_maxPeakHeightPerSecond[i].setValue(-peackRect.height());

        if(thisWidget->m_maxPeakHeightPerSecond[i].getValue() > 0)
        {
            QPainterPath pathMaxPeak;
            pathMaxPeak.addRoundedRect(QRectF(QPointF(peackRect.x(), peackRect.y() - thisWidget->m_maxPeakHeightPerSecond[i].getValue()),
                                              QSize(peackRect.width(), MAX_PEAK_MARK_HEIGHT)), 1, 1);
            painter.fillPath(pathMaxPeak, Qt::white);
        }

        dx += peakWidth + PEAK_METER_SEPARATOR;
    }
}

void PeakMeterWidget::drawLegendBack(QPainter& painter, const int numChannels, const QRect& rect,
    EViewMode mode, bool active, int alpha, bool needBackgroundGradient)
{
	int dx = rect.x();
	
	const int peakWidth = mode == eThinMode ? THIN_PEAK_METER_COLUMN_PX_WIDTH : WIDE_PEAK_METER_COLUMN_PX_WIDTH;
	const int height = rect.height() - BOTTOM_TITLE_HEIGHT - TOP_PEAK_MARGING;

    painter.setFont(legendFont);
	for (int i = 0; i < numChannels; i++)
	{
		dx += PEAK_METER_SEPARATOR;

		QRect peackRect;
		peackRect.setX(dx);
		peackRect.setY(TOP_PEAK_MARGING + rect.top());
		peackRect.setWidth(peakWidth + 1);
		peackRect.setHeight(height);

        if(needBackgroundGradient){
            QLinearGradient m_gradient(peackRect.topLeft(),peackRect.bottomRight());
            m_gradient.setColorAt(0.0, QColor(255 , 70, 13, 100/2));
            m_gradient.setColorAt(0.29, QColor(255 , 192, 13, 100/2));
            m_gradient.setColorAt(0.53, QColor(101 , 138, 26, 100/2));
            m_gradient.setColorAt(1.0, QColor(119 , 164, 31 , 100/2));

            painter.setRenderHint(QPainter::Antialiasing);
            QPainterPath path;
            path.addRoundedRect(QRectF(peackRect), 2, 2);
            painter.fillPath(path, m_gradient);
        }
        else
            painter.fillRect(peackRect, QColor(85, 83, 84, alpha));

		painter.setPen(QColor(51, 51, 53, alpha));
		painter.drawLine(peackRect.topLeft(), peackRect.bottomLeft());
		painter.drawLine(peackRect.bottomLeft(), peackRect.bottomRight());
		painter.setPen(QColor(66, 66, 66, alpha));
		painter.drawLine(peackRect.bottomRight(), peackRect.topRight());
		painter.drawLine(peackRect.topRight(), peackRect.topLeft());

		dx += peakWidth + PEAK_METER_SEPARATOR;

		painter.setPen(QPen(active ? QColor(153, 153, 153) : QColor(43, 43, 43)));
		if (!((i + 1) % 2))
			painter.drawText(peackRect.x(), rect.bottom() + 1, QString("%1").arg(i + 1));
	}
}

void PeakMeterWidget::recalculatePeakValues(QVector<double>& chValues, int numVisibleChannels,QRect rect)
{
    for(int i = 0; i < chValues.size() && i < numVisibleChannels; i++)
    {
        const int maxHeightPx = rect.height() - TOP_PEAK_MARGING - BOTTOM_TITLE_HEIGHT;
        m_maxPeakHeightPerSecond[i].setRecalculateVal(-getRealPeakmeterPxHeight(chValues[i], maxHeightPx));
    }
}

int PeakMeterWidget::getWidgetWidth(PeakMeterWidget::EViewMode mode, int numChannels)
{
	if (mode == eThinMode)
	{
		switch (numChannels)
		{
		case 2:	return ePMCh2Thin;
		case 4:	return ePMCh4Thin;
		case 6:	return ePMCh6Thin;
		case 8:	return ePMCh8Thin;
		}
	}
	else
	{
		switch (numChannels)
		{
		case 2:	return ePMCh2Wide;
		case 4:	return ePMCh4Wide;
		case 6:	return ePMCh6Wide;
		case 8:	return ePMCh8Wide;
		}
	}
	return ePMCh8Wide;
}

int PeakMeterWidget::getWidgetWidth(void) const
{
	return getWidgetWidth(viewMode, numVisibleChannels);
}

void PeakMeterWidget::drawLegendLines(QPainter& painter, int numChannels, const QRect& rect, EViewMode mode, bool isAutoScale, 
	bool active, int alpha)
{
	const int peakWidth = mode == eThinMode ? THIN_PEAK_METER_COLUMN_PX_WIDTH : WIDE_PEAK_METER_COLUMN_PX_WIDTH;
	const int dXLegend = rect.x() + (PEAK_METER_SEPARATOR * 2 + peakWidth) * numChannels;

	QColor color(active ? QColor(153, 153, 153, alpha) : QColor(43, 43, 43, alpha)), textColor;
	textColor = color;
	textColor.setAlpha(255);

	painter.setPen(color);
	painter.setFont(legendFont);

	int devider = isAutoScale ? rect.height() / 80 : 1;
	int dbStepValue = 18 / ( devider < 1 ? 1 : devider);
	if (dbStepValue < 1)
		dbStepValue = 1;

	float step = ((float)(rect.height() - TOP_PEAK_MARGING - BOTTOM_TITLE_HEIGHT)) * dbStepValue / (float)-MIN_PEAKMETER_VALUE;

	float dy = TOP_PEAK_MARGING + rect.top();
	for (int i = 0; i > MIN_PEAKMETER_VALUE; i-= dbStepValue)
    {
        painter.drawLine(QLineF(PEAK_METER_SEPARATOR + rect.x(), dy, dXLegend, dy));
		painter.setPen(textColor);
        //Выравниваем 0 с отрицательными числами
        QString finalStr = QString("%1").arg(i).toInt() == 0? QString("%1").arg(i).prepend(" ") : QString("%1").arg(i);
        painter.drawText(dXLegend + 3, dy + 4, finalStr);
		dy += step;
	}
}
