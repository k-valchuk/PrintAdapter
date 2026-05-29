#include "Switcher.h"

#include "QPropertyAnimation"
#include "qdebug.h"
#include "qevent.h"
#include "qpainter.h"
#include "qpainterpath.h"

class SwitcherEllipseFrame : public QFrame
{
    Q_OBJECT
    
    QRect m_position;
public:
    SwitcherEllipseFrame(QWidget* parent = nullptr) : QFrame(parent) {}
    
    Q_PROPERTY(QRect position READ position WRITE setPosition)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    
    QRect position() const;
    void setPosition(const QRect& newPosition);
    QColor color() const;
    void setColor(const QColor& newColor);
    
protected:
    void paintEvent(QPaintEvent* event);
private:
    QColor m_color;
};

#include "Switcher.moc"

Switcher::Switcher(QWidget* parent)
    : QFrame(parent),
    m_container(this)
{
    m_ellipseContainer = new SwitcherEllipseFrame(this);
    m_stackedLayout = new QStackedLayout(this);
    m_hLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
    
    this->setLayout(m_stackedLayout);
    m_stackedLayout->addWidget(&m_container);
    m_stackedLayout->addWidget(m_ellipseContainer);
    m_stackedLayout->setStackingMode(QStackedLayout::StackAll);
    m_container.setLayout(m_hLayout);
    
    m_container.setStyleSheet("background-color: transparent; border: none;");
    m_ellipseContainer->setStyleSheet("background-color: transparent; border: none;");
    
    m_container.setContentsMargins(0, 0, 0, 0);
    m_ellipseContainer->setContentsMargins(0, 0, 0, 0);
    this->setContentsMargins(0, 0, 0, 0);
    m_stackedLayout->setContentsMargins(0, 0, 0, 0);
    m_hLayout->setContentsMargins(2, 2, 2, 2);
    
    this->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
    this->setLineWidth(0);
}

Switcher::~Switcher()
{
    delete m_ellipseContainer;
}

void Switcher::UpdateEllipse(bool animation)
{
    auto borderradius = this->height()/2;
    if(m_buttons.size())
        borderradius = m_buttons.first()->height()/2 + m_hLayout->contentsMargins().left()/2;
    this->setStyleSheet(QString("border-radius: %1px;").arg(borderradius));
    
    if(m_currentIndex < 0 || m_currentIndex >= m_items.size())
    {
        m_ellipseContainer->setColor(QColor(0, 0, 0, 0));
        return;
    }
    
    auto item = m_items[m_currentIndex];
    auto btn = m_buttons[m_currentIndex];
    
    double time = 0;
    if(m_animationTime > 0)
    {
        auto speed = qMax(this->width()/(double)m_animationTime, 0.0001);
        if(m_hLayout->direction() == QBoxLayout::LeftToRight || m_hLayout->direction() == QBoxLayout::RightToLeft)
            time = qAbs(m_ellipseContainer->position().center().x() - btn->geometry().center().x())/speed;
        else
            time = qAbs(m_ellipseContainer->position().center().y() - btn->geometry().center().y())/speed;
    }
    auto geomAnim = new QPropertyAnimation(m_ellipseContainer, "position", this);
    geomAnim->setDuration((animation && m_prevBtn) ? time : 0);
    geomAnim->setStartValue(m_ellipseContainer->position());
    geomAnim->setEndValue(btn->geometry());
    geomAnim->setEasingCurve(m_easingCurve);
    geomAnim->start(QAbstractAnimation::DeleteWhenStopped);
    
    
    auto colAnim = new QPropertyAnimation(m_ellipseContainer, "color", this);
    colAnim->setDuration((animation && m_prevBtn) ? time : 0);
    colAnim->setStartValue(m_ellipseContainer->color());
    colAnim->setEndValue(item.bgColor);
    colAnim->setEasingCurve(m_easingCurve);
    connect(colAnim, &QPropertyAnimation::finished, m_ellipseContainer, [this](){
        m_ellipseContainer->repaint();
    });
    colAnim->start(QAbstractAnimation::DeleteWhenStopped);
    
    if(m_prevBtn)
        m_prevBtn->setStyleSheet("background-color: transparent; border: none;");
    btn->setStyleSheet(QString("background-color: transparent; border: none;"
                               "color: rgba(%1, %2, %3, %4);")
                           .arg(item.textColor.red())
                           .arg(item.textColor.green())
                           .arg(item.textColor.blue())
                           .arg(item.textColor.alpha()));
    m_prevBtn = btn;
}

void Switcher::ModifyButton(QPushButton* btn, Item item)
{
    btn->setText(item.text);
    btn->setStyleSheet("background-color: transparent; border: none;");
    btn->setCursor(Qt::PointingHandCursor);
}

void Switcher::resizeEvent(QResizeEvent* event)
{
    UpdateEllipse(false);
    QFrame::resizeEvent(event);
}

void Switcher::showEvent(QShowEvent* event)
{
    UpdateEllipse(false);
    QFrame::showEvent(event);
    
    auto nparent = this->parentWidget();
    if(nparent != m_installedOn)
    {
        if(m_installedOn) m_installedOn->removeEventFilter(this);
        if(nparent) {
            nparent->installEventFilter(this);
            connect(nparent, &QObject::destroyed, this, [this](){
                    m_installedOn = nullptr;
                }, Qt::DirectConnection);
        }
        m_installedOn = nparent;
    }
}

Switcher::Item Switcher::ItemAt(int index)
{
    if(index < m_items.size())
        return m_items[index];
    else return Item();
}

void Switcher::InsertItem(int index, Item item)
{
    if(index < 0 || index > m_items.size())
        index = m_items.size();
    
    m_items.insert(index, item);
    
    auto btn = new QPushButton(&m_container);
    ModifyButton(btn, item);
    m_buttons.insert(index, btn);
    
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(btn, &QPushButton::clicked, this, [this, btn](){
        auto index = m_buttons.indexOf(btn);
        emit this->changeRequested(index);
        if(m_onlyProgrammaticalChange)
            return;
        this->SetCurrentIndex(index);
    });
    
    m_hLayout->insertWidget(index, btn);
    
    if(index <= m_currentIndex)
    {
        m_currentIndex++;
        UpdateEllipse();
    }
}

void Switcher::RemoveItemAt(int index)
{
    if(index >= m_items.size())
        return;
    
    m_items.removeAt(index);
    auto btn = m_buttons.takeAt(index);
    m_hLayout->removeWidget(btn);
    if(m_prevBtn == btn)
        m_prevBtn = nullptr;
    delete btn;
    
    if(index <= m_currentIndex)
    {
        m_currentIndex--;
        if(m_currentIndex < 0) m_currentIndex = 0;
        UpdateEllipse();
    }
}

void Switcher::EditItemAt(int index, Item nItem)
{
    if(index < 0 || index >= m_items.size())
        return;
    
    m_items[index] = nItem;
    auto btn = m_buttons[index];
    ModifyButton(btn, nItem);
    UpdateEllipse();
}

void Switcher::SetCurrentIndex(int index)
{
    if(index < 0 || index >= m_items.size())
        return;
    
    if(m_currentIndex != index)
    {
        m_currentIndex = index;
        emit currentIndexChanged(index);
    }
    
    UpdateEllipse();
}

void Switcher::SetCurrentItemByData(const QVariant& data)
{
    for(int i = 0; i < m_items.size(); i++)
        if(m_items[i].data == data)
        {
            SetCurrentIndex(i);
            return;
        }
}


//----------------------------------
QRect SwitcherEllipseFrame::position() const
{
    return m_position;
}

void SwitcherEllipseFrame::setPosition(const QRect& newPosition)
{
    if (m_position == newPosition)
        return;
    m_position = newPosition;
    repaint();
}

QColor SwitcherEllipseFrame::color() const
{
    return m_color;
}

void SwitcherEllipseFrame::setColor(const QColor& newColor)
{
    if (m_color == newColor)
        return;
    m_color = newColor;
    //repaint();
}

void SwitcherEllipseFrame::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QPainterPath path;
    path.addRoundRect(m_position, 100); //100 это проценты а не пиксели
    painter.fillPath(path, m_color);
    
    QFrame::paintEvent(event);
}


bool Switcher::event(QEvent* event)
{
    if(event->type() == QEvent::ParentChange)
    {
        auto nparent = this->parentWidget();
        if(nparent != m_installedOn)
        {
            if(m_installedOn) m_installedOn->removeEventFilter(this);
            if(nparent) {
                nparent->installEventFilter(this);
                connect(nparent, &QObject::destroyed, this, [this](){
                        m_installedOn = nullptr;
                    }, Qt::DirectConnection);
            }
            m_installedOn = nparent;
        }
    }
    
    return QFrame::event(event);
}


bool Switcher::eventFilter(QObject* watched, QEvent* event)
{
    auto parent = this->parentWidget();
    if(event->type() == QEvent::Resize && parent)
    {
        auto rect = parent->rect();
        int left, top, right, bottom;
        parent->getContentsMargins(&left, &top, &right, &bottom);
        if(parent->layout()) {
            int ileft, itop, iright, ibottom;
            parent->layout()->getContentsMargins(&ileft, &itop, &iright, &ibottom);
            left += ileft; top += itop; right += iright; bottom += ibottom;
        }
        rect.adjust(+left, +top, -right, -bottom);
        rect.adjust(+1, +1, -1, -1);
        
        int contentsWidth = 0, contentsHeight = 0;
        for(int i = 0; i < m_hLayout->count(); i++)
        {
            auto item = m_hLayout->itemAt(i);
            auto size = item->minimumSize();
            
            contentsWidth  += (i > 0 ? m_hLayout->spacing() : 0) + size.width();
            contentsHeight += (i > 0 ? m_hLayout->spacing() : 0) + size.height();
        }
        m_hLayout->getContentsMargins(&left, &top, &right, &bottom);
        contentsWidth  += left + right;
        contentsHeight += top + bottom;
        
        if(rect.width() <= contentsWidth && rect.height() > contentsHeight)
        {
            if(m_hLayout->direction() != QBoxLayout::TopToBottom)
                m_hLayout->setDirection(QBoxLayout::TopToBottom);
        }
        else
        {
            if(m_hLayout->direction() != QBoxLayout::LeftToRight)
                m_hLayout->setDirection(QBoxLayout::LeftToRight);
        }
        return true;
    }
    return false;
}
