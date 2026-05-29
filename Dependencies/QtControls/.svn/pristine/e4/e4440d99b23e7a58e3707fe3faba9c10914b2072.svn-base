#include "customDragViewList.h"
#include "QPainter"
#include "QDrag"
#include "QWheelEvent"
#include "functional"
#include "qheaderview.h"

constexpr int OFFSET = 10;          //отступ
constexpr int MIN_WIDTH = 200;      //минимальная ширина отображения
constexpr int MIN_FONT_HEIGHT = 8;  //минимальная высота шрифта

CustomDragViewList::CustomDragViewList(QWidget *parent) : QTreeView(parent)
{
    setExpandsOnDoubleClick(false);
    connect(this, SIGNAL(expanded(QModelIndex)),SLOT(slot_expanded(QModelIndex)));
    connect(this, SIGNAL(collapsed(QModelIndex)), SLOT(slot_collapsed(QModelIndex)));
    connect(this, SIGNAL(clicked(QModelIndex)), SLOT(slot_mousePress(QModelIndex)));
}

void CustomDragViewList::drawDragObjects(QObject *src, const QModelIndexList &indexes, QAbstractItemModel *model, int col,Qt::DropActions supportedActions)
{
    if (!indexes.isEmpty() && col < model->columnCount())
    {
        //qSort(indexes);
        QMimeData *data = model->mimeData(indexes);
        if (!data)
            return;

        int count = indexes.size()/model->columnCount();//количество элементов
        //шрифт и метрики
        QFont f("Calibri", 12);
        if (auto *wid = qobject_cast<QWidget*>(src))//шрифт источника
            f = wid->font();
        QFontMetrics fm(f);

        //первая и последняя строки
        QString textFirst = indexes.at(col).data().toString();
        QString textLast;
        if (count > 1)
        {
            textLast = indexes.at(indexes.size() - model->columnCount() + col).data().toString();
        }

        //параметры рамки и начало для текста
        int resWidth = qMax(fm.width(textFirst), fm.width(textLast)) + OFFSET * 2;
        int resHeight = fm.height();
        int pStartText = OFFSET;
        if (resWidth < MIN_WIDTH)
        {
            pStartText = (MIN_WIDTH + OFFSET * 2 - resWidth) / 2;
            resWidth = MIN_WIDTH;
        }
        if (count > 1)
        {
            resHeight = 3 * resHeight;
        }

        //отрисовка
        QPixmap pixmap(resWidth, resHeight);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QColor("#79B6E1"));
        painter.setBrush(QColor(47, 54, 58, 200));
        painter.drawRect(pixmap.rect());
        painter.setFont(f);
        painter.setPen(Qt::white);
        painter.drawText(pStartText, fm.ascent(), textFirst);
        if (count > 1)//отображаем множественное перемещение
        {
            painter.drawText(resWidth / 2, fm.ascent() * 2, "...");
            painter.drawText(pStartText, fm.ascent() * 3, textLast);
        }
        painter.end();

        QDrag *drag = new QDrag(src);
        drag->setPixmap(pixmap);
        drag->setMimeData(data);
        drag->setHotSpot(pixmap.rect().center());
        drag->exec(supportedActions);
    }
}

void CustomDragViewList::startDrag(Qt::DropActions supportedActions)
{
    drawDragObjects(this, selectionModel()->selectedIndexes(), model(), dragColumn, supportedActions);
}

void CustomDragViewList::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)//если нажат CTRL
    {
        int sign = 1;//знак
        if (event->angleDelta().y() < 0)
            sign = -1;
        zoomText(sign);
    }
    else
        QTreeView::wheelEvent(event);
}

void CustomDragViewList::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex ind = indexAt(event->pos());
    if (ind == dbClickBranch.branchInd && ind.column() == 0)
    {//если двойное нажатие на ветке
        if (dbClickBranch.op == DblClickBranch::Expand)//свернуть
            collapse(ind);
        else if (dbClickBranch.op == DblClickBranch::Collapse)//раскрыть
            expand(ind);
    }
    dbClickBranch.op = DblClickBranch::NoOperation;

    QTreeView::mouseDoubleClickEvent(event);
}

void CustomDragViewList::slot_expanded(const QModelIndex &ind)
{
    dbClickBranch.branchInd = ind;
    dbClickBranch.op = DblClickBranch::Expand;
}

void CustomDragViewList::slot_collapsed(const QModelIndex &ind)
{
    dbClickBranch.branchInd = ind;
    dbClickBranch.op = DblClickBranch::Collapse;
}

void CustomDragViewList::slot_mousePress(const QModelIndex &)
{//сбросить по клику оперцию раскрытия/скрытия ветки (не будет вызван при нажатии на ветку)
    dbClickBranch.op = DblClickBranch::NoOperation;
}

void CustomDragViewList::zoomText(int sign)
{
    if (!bZoomEnabled && sign > 0)//запрет на увеличение
        return;

    QFont fontText = this->font();
    int textSize = fontText.pointSize() + sign;//новый размер
    auto *listModel = model();
    //размер шрифта для заданных индексов
    std::function<void(const QModelIndex&)> fontFunc = [&](const QModelIndex& parentInd)
    {
        QModelIndex ind;
        QFont fontT;
        for (int i = 0; i < listModel->rowCount(parentInd); ++i)
        {
            ind = listModel->index(i, 0, parentInd);
            if (ind.isValid())
                fontFunc(ind);

            for (int j = 0; j < listModel->columnCount(); ++j)
            {
                ind = listModel->index(i, j, parentInd);
                if (ind.data(Qt::FontRole).isValid())//если шрифт задан
                {
                    fontT = ind.data(Qt::FontRole).value<QFont>();
                    fontT.setPointSize(textSize);
                    listModel->setData(ind, fontT, Qt::FontRole);
                }
            }
        }
    };

    if (textSize >= MIN_FONT_HEIGHT && listModel)//если новый размер не меньше минимального то изменяем
    {
        QFont headerFont = this->header()->font();
        headerFont.setPointSize(textSize);
        this->header()->setFont(headerFont);
        this->header()->style()->polish(this->header());
        fontText.setPointSize(textSize);
        this->setFont(fontText);
        fontFunc(QModelIndex());

        auto icon_size = iconSize();
        if (icon_size.isValid())
        {// Увеличить иконку, если размер был задан
            icon_size.setWidth(icon_size.width() + sign);
            icon_size.setHeight(icon_size.height() + sign);
            setIconSize(icon_size);
        }
    }
}
