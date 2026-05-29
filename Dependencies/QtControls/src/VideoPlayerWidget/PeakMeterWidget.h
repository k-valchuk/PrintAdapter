#ifndef _PEAKMETERWIDGET_H_
#define _PEAKMETERWIDGET_H_

#include <QWidget>
#include <QVector>
#include "MaxPeakValue.h"

class PeakMeterWidget : public QWidget
{
	Q_OBJECT
private:
	QVector<double> chValues;	// Значения аудиоканалов 
	int numVisibleChannels;
	bool isActive, isAutoScale;
	int alpha;
	int peakWidth;
    QVector<MaxPeakValue> m_maxPeakHeightPerSecond;
public:
	enum EPeakmeterLength
	{
		ePMCh2Thin = 32, ePMCh2Wide = 42,
		ePMCh4Thin = 42, ePMCh4Wide = 53,
		ePMCh6Thin = 50, ePMCh6Wide = 63,
		ePMCh8Thin = 60, ePMCh8Wide = 75
	};

	enum EViewMode
	{
		eThinMode, 
		eWideMode
	}viewMode;

	PeakMeterWidget();
	~PeakMeterWidget();

	void setViewMode(EViewMode mode);
	void setTransparency(int);
	void setIsActive(bool);
	void setIsAutoScale(bool);
	void setVisibleChannelsNum(int);
	int getVisibleChannelsCount(void) const { return numVisibleChannels; }

	int getWidgetWidth(void) const;
	static int getWidgetWidth(PeakMeterWidget::EViewMode mode, int numChannels);

	static void drawLegendLines(QPainter& painter, int numChannels, const QRect& rect,
		EViewMode mode, bool isAutoScale, bool active = true, int alpha = 255);
	static void drawLegendBack(QPainter& painter, int numChannels, const QRect& rect,
        EViewMode mode, bool active = true, int alpha = 255, bool needBackgroundGradient = false);
    static void drawPeaks(QPainter& painter, QVector<double>& values, int numChannels, const QRect& rect,
        EViewMode mode, int alpha = 255, bool needPeaksGradient = false);
    static void drawMaxPeaks(QPainter& painter, QVector<double>& values, int numChannels, const QRect& rect,
                             EViewMode mode, PeakMeterWidget* thisWidget);
	
	void setChannelValues(QVector<double>);
	void paintEvent(QPaintEvent * event);

    // Пересчет позиций отображений максимальных пиков при изменении размера пикметра
    void recalculatePeakValues(QVector<double>& chValues, int numVisibleChannels, QRect rect);
};

#ifdef TESTING
int getLogHeight(double value);
#endif //TESTING

#endif //_PEAKMETERWIDGET_H_
