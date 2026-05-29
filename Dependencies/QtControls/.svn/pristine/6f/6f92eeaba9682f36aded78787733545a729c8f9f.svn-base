#include "FilterRowToolTip.h"

#include <QApplication>
#include "FilterRow.h"
#include "qbuffer.h"
#include "qpainter.h"
#include "qscreen.h"
#include <QToolTip>

void FilterRowToolTip::SetExpGetter(std::function<QSharedPointer<IExpression>()> getter,
                                    std::function<QSharedPointer<IFilterElement>()> allFiltGetter,
                                    std::function<QList<QSharedPointer<IFilterElement>>()> filtsGetter)
{
    m_expGetter = getter;
    m_allFiltGetter = allFiltGetter;
    m_filtsGetter = filtsGetter;
}

void FilterRowToolTip::ShowTooltip(QPoint pos, QWidget* parent)
{
    if(!m_expGetter)
        return;
    
    auto exp = m_expGetter();
    if(!exp)
    {
        QToolTip::showText(pos, QApplication::tr("Empty filter"), parent);
        return;
    }
    
    //auto wgt = new QLabel("test tooltip");
    //wgt->setWindowFlag(Qt::ToolTip, true);
    //wgt->setWindowModality(Qt::NonModal);
    //wgt->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    //wgt->move(((QHelpEvent*)ev)->globalPos());
    //wgt->show();
    
    // instantiate a generic tooltip widget
    QWidget toolTipWidget;
    toolTipWidget.setWindowFlags(Qt::ToolTip);
    toolTipWidget.setAutoFillBackground(true);
    toolTipWidget.setBackgroundRole(QPalette::Base);
    toolTipWidget.setContentsMargins(0, 0, 0, 0);
    //... here add whatever content to the widget you want, any child widgets, layouts etc.
    
    QVBoxLayout lay;
    FilterRow filterRow(FilterRow::FilterRowConfig{false, false, false, false, true});
    if(m_allFiltGetter)
        filterRow.SetAllFieldsFilter(m_allFiltGetter());
    if(m_filtsGetter)
        filterRow.SetAvailableFiltersList(m_filtsGetter());
    filterRow.restoreFromTree(exp->Copy());
    lay.addWidget(&filterRow);
    toolTipWidget.setLayout(&lay);
    lay.setSpacing(0);
    lay.setContentsMargins(0, 0, 0, 0);
    
    auto filtersWidth = filterRow.SizeOfFilters();
    auto width = filterRow.WholeSize();
    auto btnsSize = width - filtersWidth;
    
    auto screen = qApp->screenAt(pos);
    auto monitorWidth = screen ? screen->availableSize().width() : 1000;
    width = qMin(width, qRound(monitorWidth*0.95));
    
    filterRow.setFixedWidth(width);
    filterRow.setFixedHeight(filterRow.GetHeightForWidth(width - btnsSize));
    
    // render the widget to pixmap and delete the widget
    QSize size = toolTipWidget.sizeHint();
    if(size.width() <= 0 || size.height() <= 0)
        return;
    
    qreal dpr = parent->parentWidget()->devicePixelRatioF(); // parent is the widget for which we show the tooltip; or use the screen device pixel ratio if widget not available
    QPixmap pixmap(size * dpr);
    pixmap.setDevicePixelRatio(dpr);
    QPainter painter(&pixmap);
    toolTipWidget.render(&painter);
    
    // generate in-memory PNG from the pixmap and wrap it in <img/> element
    QByteArray data;
    QBuffer buffer(&data);
    pixmap.save(&buffer, "PNG");
    QString html = QStringLiteral("<img src='data:image/png;base64, %1' width='%2' height='%3'/>").arg(QString::fromLatin1(data.toBase64())).arg(size.width()).arg(size.height());
    
    QToolTip::showText(pos, html, parent);
}
