#include <QTextDocument>
#include <QTextFrame>
#include <QAbstractTextDocumentLayout>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QPainter>
#include <QDate>
#include <QApplication>
#include "PrintableReport.h"
#include "PrintDialog.h"
#include <QDebug>

QString standartFooter1 = "<p align=\"right\"><strong>&page;</strong></p>";
QString standartFooter2 = "<p align=\"right\"><strong>&page;/&totalpages;</strong></p>";

static inline double mmToInches(double mm)
{
	return mm * 0.0393700787;
}

PrintableReport::PrintableReport(QPrinter::PrinterMode printerMode, QObject *parent) : QObject(parent),
		m_spacing(5.0),
		m_headerSize(0.0), m_headerRule(0.5), m_headerText(QString()),
		m_footerSize(0.0), m_footerRule(0.5), m_footerText(QString()),
		m_dateFormat()
{
	initPrinter(printerMode);

	m_document = new QTextDocument();
}

PrintableReport::PrintableReport(QPrinter::PrinterMode printerMode, const QTextDocument &document, QObject *parent) : QObject(parent),
		m_spacing(5.0),
		m_headerSize(0.0), m_headerRule(0.5), m_headerText(QString()),
		m_footerSize(0.0), m_footerRule(0.5), m_footerText(QString()),
		m_dateFormat()
{
	initPrinter(printerMode);

	m_document = document.clone();
}

PrintableReport::PrintableReport(QPrinter::PrinterMode printerMode, const QString &content, QObject *parent) : QObject(parent),
		m_spacing(5.0),
		m_headerSize(0.0), m_headerRule(0.5), m_headerText(QString()),
		m_footerSize(0.0), m_footerRule(0.5), m_footerText(QString()),
		m_dateFormat()
{
	initPrinter(printerMode);

	if(Qt::mightBeRichText(content)) {
		m_document = new QTextDocument();
		m_document->setHtml(content);
	}
	else {
		m_document = new QTextDocument(content);
	}
}

PrintableReport::~PrintableReport()
{
	delete m_printer;
	delete m_document;
}

void PrintableReport::print(QWidget *parent, const QString &title)
{
	// setup printer
	m_printer->setOutputFormat(QPrinter::NativeFormat);
	m_printer->setOutputFileName(QString());

	// show print dialog
	QPrintDialog dialog(m_printer, parent);
	dialog.setWindowTitle(title);
	if(dialog.exec() == QDialog::Rejected)
		return;

	// print it
	print(m_printer);
}

void PrintableReport::preview(QWidget *parent, const QString &title)
{
	//PrintDialog *dialog = new PrintDialog(parent, this, m_printer);

	//connect(dialog, SIGNAL(paintRequested(QPrinter *)), this, SLOT(print(QPrinter *)));
	
	//dialog->exec();

	//delete dialog;
}

///////////////////////////////////////////////////////////////////////////////
// private methods
///////////////////////////////////////////////////////////////////////////////

QRectF PrintableReport::contentRect(QPainter *painter)
{
	QRectF rect = painter->window();

	if(m_headerSize > 0)
		rect.adjust(0, mmToInches(m_headerSize + m_spacing) * m_printer->resolution(), 0, 0);
	if(m_footerSize > 0)
		rect.adjust(0, 0, 0, -mmToInches(m_footerSize + m_spacing) * m_printer->resolution());

	return rect;
}

QRectF PrintableReport::headerRect(QPainter *painter)
{
	QRectF rect = painter->window();
	rect.setBottom(rect.top() + mmToInches(m_headerSize) * m_printer->resolution());

	return rect;
}

QRectF PrintableReport::footerRect(QPainter *painter)
{
	QRectF rect = painter->window();
	rect.setTop(rect.bottom() - mmToInches(m_footerSize) * m_printer->resolution());

	return rect;
}

void PrintableReport::print(QPrinter *printer)
{

	QPainter painter(printer);

	QRectF pr = printer->pageRect(QPrinter::Inch);
	QRect adjustedViewport(0, 0, 0, 0);
	adjustedViewport.setWidth(pr.width() * printer->resolution());
	adjustedViewport.setHeight(pr.height() * printer->resolution());
	painter.setViewport(adjustedViewport);


	m_document->setUseDesignMetrics(true);
	m_document->documentLayout()->setPaintDevice(printer);
	m_document->setPageSize(contentRect(&painter).size());
	// dump existing margin (if any)
	QTextFrameFormat fmt = m_document->rootFrame()->frameFormat();
	fmt.setMargin(0);
	m_document->rootFrame()->setFrameFormat(fmt);

	// to iterate through pages we have to worry about
	// copies, collation, page range, and print order

	// get num copies
	int doccopies;
	int pagecopies;
	if(printer->collateCopies()) {
		doccopies = 1;
		pagecopies = printer->numCopies();
	}
	else {
		doccopies = printer->numCopies();
		pagecopies = 1;
	}

	// get page range
	int firstpage = printer->fromPage();
	int lastpage = printer->toPage();
	if(firstpage == 0 && lastpage == 0) { // all pages
		firstpage = 1;
		lastpage = m_document->pageCount();
	}

	// print order
	bool ascending = true;
	if(printer->pageOrder() == QPrinter::LastPageFirst) {
		int tmp = firstpage;
		firstpage = lastpage;
		lastpage = tmp;
		ascending = false;
	}

	// loop through and print pages
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);
	for(int dc = 0; dc < doccopies; dc++) {
		int pagenum = firstpage;
		while(true) {
			for(int pc = 0; pc < pagecopies; pc++) {
				if(printer->printerState() == QPrinter::Aborted || printer->printerState() == QPrinter::Error)
					return;
                // print page
				paintPage(&painter, pagenum);
				QApplication::processEvents();
				if(pc < pagecopies - 1)
					printer->newPage();
			}
			if(pagenum == lastpage)
				break;
			if(ascending)
				pagenum++;
            else
				pagenum--;
			printer->newPage();
		}

		if(dc < doccopies - 1)
			printer->newPage();
	}
}

void PrintableReport::paintPage(QPainter *painter, int pagenum)
{
	QRectF rect;

	// header
	if(m_headerSize > 0) {
		rect = headerRect(painter);
		if(m_headerRule > 0.0) {
			painter->save();
			// allow space between rule and header
			//painter->translate(0, onepoint + (m_headerRule * onepoint / 2.0));
			//painter->setPen(QPen(Qt::black, m_headerRule * onepoint));
				//painter->translate(0, (1 + m_headerRule) * m_printer->resolution() / 2 );
				painter->setBrush(Qt::black);
			painter->drawLine(rect.bottomLeft(), rect.bottomRight());
			painter->restore();
		}

        // replace page variables
        QString header = m_headerText;
		header.replace("&page;", QString::number(pagenum));
		header.replace("&totalpages;", QString::number(m_document->pageCount()));
		if(m_dateFormat.isEmpty())
			header.replace("&date;", QDate::currentDate().toString());
		else
			header.replace("&date;", QDate::currentDate().toString(m_dateFormat));

		painter->save();
		painter->translate(rect.left(), rect.top());
		QRectF clip(0, 0, rect.width(), rect.height());
		QTextDocument doc;
		doc.setUseDesignMetrics(true);
		doc.setHtml(header);
		doc.documentLayout()->setPaintDevice(painter->device());
		doc.setPageSize(rect.size());

		// align text to bottom
		double newtop = clip.bottom() - doc.size().height();
		clip.setHeight(doc.size().height());
		painter->translate(0, newtop);

		doc.drawContents(painter, clip);
		painter->restore();
	}

	// footer
	if(m_footerSize > 0) {
		rect = footerRect(painter);
		if (m_footerRule > 0.0) {
			painter->save();
			// allow space between rule and footer
			//painter->translate(0, -onepoint + (-m_footerRule * onepoint / 2.0));
			//painter->setPen(QPen(Qt::black, m_footerRule * onepoint));
				//painter->translate(0, -(1 + m_footerRule) * m_printer->resolution() / 2 );
				painter->setBrush(Qt::black);
			painter->drawLine(rect.topLeft(), rect.topRight());
			painter->restore();
		}

		// replace page variables
		QString footer = m_footerText;
		footer.replace("&page;", QString::number(pagenum));
		footer.replace("&totalpages;", QString::number(m_document->pageCount()));
		if(m_dateFormat.isEmpty())
			footer.replace("&date;", QDate::currentDate().toString());
		else
			footer.replace("&date;", QDate::currentDate().toString(m_dateFormat));

		painter->save();
		painter->translate(rect.left(), rect.top());
		QRectF clip(0, 0, rect.width(), rect.height());
		QTextDocument doc;
		doc.setUseDesignMetrics(true);
		doc.setHtml(footer);
		doc.documentLayout()->setPaintDevice(painter->device());
		doc.setTextWidth(m_printer->pageRect().width());
		doc.setPageSize(rect.size());
		doc.drawContents(painter, clip);
		painter->restore();
	}

	// content
	painter->save();

	rect = contentRect(painter);
	painter->translate(rect.left(), rect.top() - (pagenum - 1) * rect.height());
	QRectF clip(0, (pagenum - 1) * rect.height(), rect.width(), rect.height());

	m_document->drawContents(painter, clip);

	painter->restore();
}
