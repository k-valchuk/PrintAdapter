#pragma once

#include <QObject>
#include <QtPrintSupport/QPrinter>
#include <QPrintPreviewDialog>

class QTextDocument;

extern QString standartFooter1, standartFooter2;

class PrintableReport : public QObject
{
Q_OBJECT

protected:
	QPrinter *m_printer;
	QTextDocument *m_document;

	double m_spacing;

	double m_headerSize;
	double m_headerRule;
	QString m_headerText;
	double m_footerSize;
	double m_footerRule;
	QString m_footerText;

	QString m_dateFormat;

public:
	PrintableReport(QPrinter::PrinterMode printerMode, QObject *parent = NULL);
	PrintableReport(QPrinter::PrinterMode printerMode, const QTextDocument &document, QObject *parent = NULL);
	PrintableReport(QPrinter::PrinterMode printerMode, const QString &content, QObject *parent = NULL);

	virtual ~PrintableReport();

	void print(QWidget *parent, const QString &title);
	void preview(QWidget *parent, const QString &title);



	inline QPrinter::PageSize pageSize() const
	{
		return m_printer->pageSize();
	}
	inline QPrinter::Orientation orientation() const
	{
		return m_printer->orientation();
	}
	inline void setPageSize(QPrinter::PageSize size)
	{
		m_printer->setPageSize(size);
	}
	inline void setOrientation(QPrinter::Orientation orientation)
	{
		m_printer->setOrientation(orientation);
	}
	inline void setMargins(qreal left, qreal top, qreal right, qreal bottom, QPrinter::Unit unit)
	{
		m_printer->setPageMargins(left, top, right, bottom, unit);
	}

	inline double spacing() const
	{
		return m_spacing;
	}
	inline void setSpacing(double value)
	{
		if((value > 0) && (value <= m_printer->paperRect().height() / 8))
			m_spacing = value;
	}

	inline double headerSize() const
	{
		return m_headerSize;
	}
	inline double headerRule() const
	{
		return m_headerRule;
	}
	inline QString headerText() const
	{
		return m_headerText;
	}
	inline void setHeaderSize(double size)
	{
		if((size > 0) && (size <= m_printer->paperRect().height() / 8))
			m_headerSize = size;
	}
	inline void setHeaderRule(double pointsize)
	{
		m_headerRule = qMax(0.0, pointsize);
	}
	inline void setHeaderText(const QString &text)
	{
		m_headerText = text;
	}

	inline double footerSize() const
	{
		return m_footerSize;
	}
	inline double footerRule() const
	{
		return m_footerRule;
	}
	inline QString footerText() const
	{
		return m_footerText;
	}
	inline void setFooterSize(double size)
	{
		if ((size > 0) && (size <= m_printer->paperRect().height() / 8))
			m_footerSize = size;
	}
	inline void setFooterRule(double pointsize)
	{
		m_footerRule = qMax(0.0, pointsize);
	}
	inline void setFooterText(const QString &text)
	{
		m_footerText = text;
	}

	inline QString dateFormat() const
	{
		return m_dateFormat;
	}
	inline void setDateFormat(const QString &format)
	{
		m_dateFormat = format;
	}

protected:
	inline void initPrinter(QPrinter::PrinterMode printerMode)
	{
		m_printer = new QPrinter(printerMode);
		m_printer->setPageSize(QPrinter::A4);
		m_printer->setOrientation(QPrinter::Portrait);
		m_printer->setPageMargins(15.0, 15.0, 15.0, 15.0, QPrinter::Millimeter);
	}

	QRectF contentRect(QPainter *painter);
	QRectF headerRect(QPainter *painter);
	QRectF footerRect(QPainter *painter);

	void paintPage(QPainter *painter, int pagenum);

protected slots:
	void print(QPrinter *printer);
};
