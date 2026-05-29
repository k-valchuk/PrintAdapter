#include "CustomComboBox.h"
#include "qdebug.h"
#include "qscrollbar.h"
#include "qstandarditemmodel.h"
#include "ui_CustomComboBox.h"

#include <QAction>

void CustomComboBoxPopup::updateMask()
{
    QRegion maskedRegion;
    
    auto borderRadius = 5;
    auto sqrect = this->rect();
    sqrect.setTop(qMin(sqrect.bottom(), sqrect.top() + borderRadius));
    sqrect.setBottom(qMax(0, sqrect.bottom() - borderRadius));
    
    maskedRegion += sqrect;
    
    auto sqrect2 = this->rect();
    sqrect2.setLeft (qMin(sqrect2.center().x(), sqrect2.left()  + borderRadius));
    sqrect2.setRight(qMax(sqrect2.center().x(), sqrect2.right() - borderRadius));
    
    maskedRegion += sqrect2;
    
    maskedRegion += QRegion(QRect(this->rect().bottomLeft() - QPoint(0, borderRadius*2), QSize(borderRadius*2, borderRadius*2)), QRegion::Ellipse);
    maskedRegion += QRegion(QRect(this->rect().bottomRight() - QPoint(borderRadius*2, borderRadius*2), QSize(borderRadius*2, borderRadius*2)), QRegion::Ellipse);
    
    maskedRegion += QRegion(QRect(this->rect().topLeft(), QSize(borderRadius*2, borderRadius*2)), QRegion::Ellipse);
    maskedRegion += QRegion(QRect(this->rect().topRight() - QPoint(borderRadius*2, 0), QSize(borderRadius*2, borderRadius*2)), QRegion::Ellipse);
    
    setMask(maskedRegion);
}

void CustomComboBoxPopup::updateSeparators()
{
    
}

CustomComboBoxPopup::CustomComboBoxPopup(QWidget *parent) :
    QFrame(parent, Qt::Tool | Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus),
    ui(new Ui::CustomComboBoxPopup)
{
    ui->setupUi(this);
    
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_WindowPropagation);
    setAttribute(Qt::WA_X11NetWmWindowTypeCombo);
    //setAttribute(Qt::WA_TranslucentBackground);
    
    setObjectName("CustomComboBoxPopup");
}

CustomComboBoxPopup::~CustomComboBoxPopup()
{
    delete ui;
}

void CustomComboBoxPopup::HideWithAnimation()
{
    auto heightAnim = new QPropertyAnimation(this, "geometry", this);
    heightAnim->setDuration(200);
    auto r = geometry();
    heightAnim->setStartValue(QRect(r.topLeft(), QSize(r.width(), 200)));
    heightAnim->setEndValue(QRect(r.topLeft(), QSize(r.width(), 1)));
    heightAnim->setEasingCurve(QEasingCurve::Type::InOutQuad);
    heightAnim->start(QAbstractAnimation::DeleteWhenStopped);
    
    connect(heightAnim, &QPropertyAnimation::finished, this, [this](){
        this->hide();
    });
}

void CustomComboBoxPopup::AddListView(AutoHeightListView* lv)
{
    auto lay = (QVBoxLayout*)ui->scrollAreaWidgetContents->layout();
    lv->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    if(lay->count() > 0)
    {
        if(auto wgt = lay->itemAt(lay->count() - 1)->widget())
            if(wgt->objectName() != "separator")
            {
                auto separator = new QFrame(this);
                separator->setFrameShape(QFrame::Shape::HLine);
                separator->setFrameShadow(QFrame::Shadow::Sunken);
                separator->setObjectName("separator");
                lay->addWidget(separator);
            }
    }
    
    lay->addWidget(lv);
}

void CustomComboBoxPopup::RemoveListView(AutoHeightListView* lv)
{
    auto lay = ui->scrollAreaWidgetContents->layout();
    auto index = lay->indexOf(lv);
    QWidget* separator = nullptr;
    
    if(index + 1 < lay->count())
    {
        auto wgt = lay->takeAt(index + 1)->widget();
        if(wgt && wgt->objectName() == "separator")
            separator = wgt;
    }
    if(!separator && index - 1 >= 0)
    {
        auto wgt = lay->takeAt(index - 1)->widget();
        if(wgt && wgt->objectName() == "separator")
            separator = wgt;
    }
    
    if(separator)
    {
        lay->removeWidget(separator);
        delete separator;
    }
    
    lay->removeWidget(lv);
    delete lv;
}

void CustomComboBoxPopup::ScrollTo(AutoHeightListView* lv, const QModelIndex& index)
{
    auto lvtop = lv->geometry().top();
    auto itemtop = lv->visualRect(index).top();
    
    ui->scrollArea->verticalScrollBar()->setValue(lvtop + itemtop);
}

void CustomComboBoxPopup::showEvent(QShowEvent* ev)
{
    ui->scrollArea->verticalScrollBar()->setValue(0);
    if(m_nextShowAnimationEnabled)
    {
        auto heightAnim = new QPropertyAnimation(this, "geometry", this);
        heightAnim->setDuration(200);
        auto r = geometry();
        heightAnim->setStartValue(QRect(r.topLeft(), QSize(r.width(), 1)));
        heightAnim->setEndValue(QRect(r.topLeft(), QSize(r.width(), 200)));
        heightAnim->setEasingCurve(QEasingCurve::Type::InOutQuad);
        heightAnim->start(QAbstractAnimation::DeleteWhenStopped);
        
        connect(heightAnim, &QPropertyAnimation::finished, this, [this](){
        });
    }
    else
        updateMask();
    
    m_nextShowAnimationEnabled = true;
    
    auto cmb = qobject_cast<CustomComboBox*>(this->parentWidget());
    if(cmb)
    {
        cmb->m_openAct->setIcon(QIcon(":/images/brClosed2"));
        cmb->updateOnDataChanged();
    }
}

void CustomComboBoxPopup::hideEvent(QHideEvent* ev)
{
    auto cmb = qobject_cast<CustomComboBox*>(this->parentWidget());
    if(cmb)
    {
        cmb->m_openAct->setIcon(QIcon(":/images/brOpened"));
        cmb->m_clearAct->setVisible(false);
    }
}

void CustomComboBoxPopup::resizeEvent(QResizeEvent* ev)
{
    updateMask();
}

void CustomComboBox::updatePopupGeometry()
{
    auto editPos = this->rect();
    auto pos = editPos.bottomLeft();
    pos.setY(pos.y());
    m_popup->setFixedWidth(editPos.width());
    m_popup->move(this->mapToGlobal(pos));
}

void CustomComboBox::ShowPopup()
{
    updatePopupGeometry();
    
    if(!m_popup->isVisible())
    {
        updateGroups();
        m_popup->Show(true);
        m_popup->raise();
    }
}

void CustomComboBox::updateGroups()
{
    for(auto lv : m_listViews)
    {
        ((QStandardItemModel*)lv->model())->takeColumn(0);
        m_popup->RemoveListView(lv);
    }
    m_listViews.clear();
    m_orderedListViews.clear();
    
    if(m_checkedItemsSeparate)
    {
        QList<QStandardItem*> checkedItems;
        for(const auto& group : m_model)
        {
            for(const auto item : group.Items)
            {
                if(item->checkState() == Qt::Checked)
                    checkedItems.append(item);
            }
        }
        
        if(m_checkedListView)
        {
            ((QStandardItemModel*)m_checkedListView->model())->takeColumn(0);
            m_popup->RemoveListView(m_checkedListView);
            m_checkedListView = nullptr;
        }
        
        if(checkedItems.size() > 0)
        {
            auto firstLv = getListView("", true);
            auto firstLvModel = (QStandardItemModel*)firstLv->model();
            for(auto item : checkedItems)
                firstLvModel->appendRow(item);
        }
    }
    
    for(const auto& group : m_model)
    {
        if(group.Items.isEmpty())
            continue;
        auto lv = getListView(group.Group);
        auto model = (QStandardItemModel*)lv->model();
        for(const auto item : group.Items)
        {
            //если не надо было выделять чекнутые в первую группу
            //то добавляем все, иначе только неЧекнутые, т.к. чекнутые уже были добавлены в ифе выше
            if(!m_checkedItemsSeparate || item->checkState() != Qt::Checked)
            {
                model->appendRow(item);
            }
        }
    }
}

void CustomComboBox::updateOnDataChanged()
{
    if(m_supressOnDataUpdate) return;
    updateElidedText();
    int checkedCount = 0;
    for(const auto& group : m_model)
    {
        for(const auto item : group.Items)
        {
            if(item->checkState() == Qt::Checked)
                checkedCount++;
        }
    }
    m_clearAct->setVisible(checkedCount != 0);
}

QString CustomComboBox::GetModelText() const
{
    QList<QStandardItem*> checkedItems;
    for(const auto& group : m_model)
    {
        for(const auto item : group.Items)
        {
            if(item->checkState() == Qt::Checked)
                checkedItems.append(item);
        }
    }
    
    QString text;
    for(int i = 0; i < checkedItems.size(); i++)
        text += (i > 0 ? QString(", ") : QString()) + checkedItems[i]->text();
    return text;
}

AutoHeightListView* CustomComboBox::getListView(QVariant group, bool firstLv)
{
    if(firstLv && m_checkedListView)
        return m_checkedListView;
    
    if(!firstLv && m_listViews.contains(group))
        return m_listViews[group];
    
    auto res = new AutoHeightListView(m_popup);
    auto model = new QStandardItemModel(res);
    res->setModel(model);
    
    res->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    
    if(!firstLv)
        m_listViews[group] = res;
    else
        m_checkedListView = res;
    
    m_orderedListViews.append(res);
    
    m_popup->AddListView(res);
    
    connect(res, &QListView::clicked, res, [model](const QModelIndex &index){
        auto item = model->itemFromIndex(index);
        if(!item) return;
        item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
    });
    
    connect(model, &QStandardItemModel::itemChanged, this, [this, model](QStandardItem* item){
        updateOnDataChanged();
    });
    return res;
}

void CustomComboBox::updateElidedText()
{
    auto text = GetModelText();
    int wdth = qMax(0, width() - 55);
    const QFontMetrics fm(fontMetrics());
    QString elidedText = fm.elidedText(text,
                                       Qt::ElideRight,
                                       wdth);
    
    // make sure to show at least the first character
    if (!elidedText.isEmpty())
    {
        const QString showFirstCharacter = text.at(0) + QStringLiteral("...");
        setMinimumWidth(fm.horizontalAdvance(showFirstCharacter) + 1);
    }
    setText(elidedText);
    this->setToolTip(text);
}

bool CustomComboBox::findAndScroll(const QString& text)
{
    //find and scroll
    QList<QPair<QStandardItem*, AutoHeightListView*>> foundItems;
    auto searchStr = text.trimmed().toLower();
    for(auto lv : m_orderedListViews)
    {
        auto model = (QStandardItemModel*)lv->model();
        for(int i = 0; i < model->rowCount(); i++)
        {
            auto item = model->item(i);
            auto itemText = item->text().trimmed().toLower();
            if(itemText.startsWith(searchStr) ||
                itemText.replace(" ", "").startsWith(searchStr))
            {
                foundItems.append({item, lv});
            }
        }
    }
    
    if(foundItems.isEmpty())
        return false;
    
    QPair<QStandardItem*, AutoHeightListView*> itemToHighglight = {nullptr, nullptr};
    if(!m_previouslyFoundItem || m_previouslyFoundStr != text)
        itemToHighglight = foundItems.first();
    else
        for(int i = 0; i < foundItems.size(); i++)
            if(foundItems[i].first == m_previouslyFoundItem)
            {
                if(i + 1 < foundItems.size())
                    itemToHighglight = foundItems[i + 1];
                else
                    itemToHighglight = foundItems.first();
                break;
            }
    if(!itemToHighglight.first)
        itemToHighglight = foundItems.first();
    m_previouslyFoundItem = itemToHighglight.first;
    m_previouslyFoundStr = text;
        
    auto item = itemToHighglight.first;
    auto lv = itemToHighglight.second;
    auto model = (QStandardItemModel*)lv->model();
    m_popup->ScrollTo(lv, model->indexFromItem(item));
    
    if(m_currAnimations.contains(item))
    {
        auto bgAnim = m_currAnimations[item];
        bgAnim->setCurrentTime(0);
    }
    else
    {
        auto oldCol = item->data(Qt::BackgroundRole);
        bool wasValid = oldCol.isValid();
        if(!wasValid)
            oldCol = lv->palette().color(lv->backgroundRole());
        
        auto bgAnim = new QVariantAnimation(this);
        m_currAnimations[item] = bgAnim;
        bgAnim->setDuration(400);
        bgAnim->setStartValue(lv->palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::Highlight));
        bgAnim->setEndValue(oldCol.value<QColor>());
        bgAnim->setEasingCurve(QEasingCurve::Type::InBounce);
        
        connect(bgAnim, &QVariantAnimation::valueChanged, lv, [item, oldCol](const QVariant& val){
            item->setBackground(val.value<QColor>());
        });
        
        connect(bgAnim, &QVariantAnimation::finished, lv, [this, item, oldCol, wasValid](){
            if(!wasValid)
                item->setData(QVariant(), Qt::BackgroundRole);
            else
                item->setBackground(oldCol.value<QColor>());
            
            m_currAnimations.remove(item);
        });
        bgAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    return true;
}

CustomComboBox::CustomComboBox(QWidget* parent) : QLineEdit(parent)
{
    qRegisterMetaType<CustomComboBox::ItemId>();
    qRegisterMetaType<QList<CustomComboBox::ItemId>>();
    
    this->setStyleSheet("icon-size: 11px;");
    this->setReadOnly(true);
    
    m_popup = new CustomComboBoxPopup(this);
    
    connect(qApp, &QApplication::focusChanged, this, [this](QWidget* old, QWidget* now)
            {
                if(m_popup)
                {
                    auto children = m_popup->findChildren<QWidget*>() + QList<QWidget*>{m_popup, this};
                    if(!children.contains(now))
                        m_popup->hide();
                }
            });
    
    QWidget* parentWgtIter = this;
    m_parentChangeWatcher = new _CustomComboBoxParentChangeWatcher(this);
    while(parentWgtIter)
    {
        parentWgtIter->installEventFilter(m_parentChangeWatcher);
        parentWgtIter = parentWgtIter->parentWidget();
    }
    connect(m_parentChangeWatcher, &_CustomComboBoxParentChangeWatcher::parentChanged, this, &CustomComboBox::onTopLevelWidgetChanged);
    
    //-----------------------------------------------
    
    m_clearAct = new QAction(this);
    m_clearAct->setIcon(QIcon(":/CustomComboBox/cancel"));
    m_clearAct->setCheckable(true);
    m_clearAct->setChecked(false);
    
    m_openAct = new QAction(this);
    m_openAct->setIcon(QIcon(":/images/brOpened"));
    m_openAct->setCheckable(true);
    m_openAct->setChecked(false);
    
    this->addAction(m_openAct, QLineEdit::TrailingPosition);
    this->addAction(m_clearAct, QLineEdit::TrailingPosition);
    m_clearAct->setVisible(false);
    
    connect(m_openAct, &QAction::triggered, this, [this](){
        if(m_popup && m_popup->isVisible())
            m_popup->HideWithAnimation();
        else
            ShowPopup();
    });
    connect(m_clearAct, &QAction::triggered, this, [this](){
        m_supressOnDataUpdate = true;
        for(auto& group : m_model)
            for(auto item : group.Items)
                item->setCheckState(Qt::Unchecked);
        m_supressOnDataUpdate = false;
        updateGroups();
        updateOnDataChanged();
    });
    
    connect(&m_searchInputTimer, &QTimer::timeout, this, [this](){
        m_currentSearchString = QString();
    });
}

CustomComboBox::~CustomComboBox()
{
    disconnect(qApp, nullptr, this, nullptr);
    delete m_parentChangeWatcher;
    if(m_popup)
        delete m_popup;
}

void CustomComboBox::AddItem(Item item, ItemId id)
{
    auto group = id.GroupId;
    if(id.Id.isValid())
        item.Data[Qt::UserRole] = id.Id;
    
    auto lv = getListView(group);
    auto model = (QStandardItemModel*)lv->model();
    auto nitem = new QStandardItem();
    for(auto role : item.Data.keys())
        nitem->setData(item.Data[role], role);
    nitem->setCheckable(false);
    nitem->setCheckState(Qt::Unchecked);
    nitem->setEditable(false);
    model->appendRow(nitem);
    
    bool found = false;
    for(auto& gr : m_model)
        if(gr.Group == group)
        {
            gr.Items.append(nitem);
            found = true;
        }
    if(!found){
        m_model.append(ItemGroup{
            group,
            QList<QStandardItem*>{nitem}
        });
    }
}

void CustomComboBox::SetCheckedItemsByUserData(const QList<ItemId>& ids)
{
    m_supressOnDataUpdate = true;
    for(const auto& gr : m_model)
    {
        for(auto item : gr.Items)
        {
            if(ids.contains(ItemId{gr.Group, item->data(Qt::UserRole)}))
                item->setCheckState(Qt::Checked);
            else
                item->setCheckState(Qt::Unchecked);
        }
    }
    m_supressOnDataUpdate = false;
    updateOnDataChanged();
}

QList<CustomComboBox::ItemId> CustomComboBox::GetCheckedItems()
{
    QList<CustomComboBox::ItemId> res;
    for(const auto& gr : m_model)
    {
        for(auto item : gr.Items)
        {
            if(item->checkState() == Qt::Checked)
                res.append(ItemId{gr.Group, item->data(Qt::UserRole)});
        }
    }
    return res;
}

void CustomComboBox::Clear()
{
    for(auto& group : m_model)
    {
        for(auto stItem : group.Items)
            delete stItem;
    }
    m_model.clear();
    updateGroups();
    updateElidedText();
}

void CustomComboBox::SetShowCheckedItemsSeparate(bool separate)
{
    if(separate == m_checkedItemsSeparate)
        return;
    m_checkedItemsSeparate = separate;
    updateGroups();
}

void CustomComboBox::onTopLevelWidgetChanged()
{
    delete m_parentChangeWatcher;
    m_parentChangeWatcher = nullptr;
    
    QWidget* parentWgtIter = this;
    m_parentChangeWatcher = new _CustomComboBoxParentChangeWatcher(this);
    while(parentWgtIter)
    {
        parentWgtIter->installEventFilter(m_parentChangeWatcher);
        parentWgtIter = parentWgtIter->parentWidget();
    }
    
    connect(m_parentChangeWatcher, &_CustomComboBoxParentChangeWatcher::parentChanged, this, &CustomComboBox::onTopLevelWidgetChanged);
}

void CustomComboBox::keyPressEvent(QKeyEvent* event)
{
    if(!event->text().isEmpty() && event->text().isSimpleText())
    {
        m_currentSearchString += event->text();
        
        auto found = findAndScroll(m_currentSearchString);
        
        if(!found)
        {
            m_searchInputTimer.stop();
            m_currentSearchString = event->text();
            found = findAndScroll(m_currentSearchString);
        }
        
        if(found)
            m_searchInputTimer.start(1000);
    }
}



bool CustomComboBox::event(QEvent* event)
{
    if(event->type() == QEvent::Resize || event->type() == QEvent::Move)
        if(m_popup)
            updatePopupGeometry();
    if(event->type() == QEvent::Type::MouseButtonPress ||
        event->type() == QEvent::Type::MouseButtonDblClick)
        ShowPopup();
    if(event->type() == QEvent::Type::MouseButtonPress ||
        event->type() == QEvent::Type::MouseButtonDblClick ||
        event->type() == QEvent::Type::MouseButtonRelease ||
        event->type() == QEvent::Type::MouseMove)
        return true;
    return QLineEdit::event(event);
}

void CustomComboBox::resizeEvent(QResizeEvent* event)
{
    QLineEdit::resizeEvent(event);
    updateElidedText();
}

QSize CustomComboBox::sizeHint() const
{
    auto marginsAndIconsWidth = contentsMargins().left() + contentsMargins().right() +
                                qApp->style()->pixelMetric(QStyle::PM_IconViewIconSize)*2;
    
    auto width = fontMetrics().horizontalAdvance(GetModelText()) + marginsAndIconsWidth;
    
    auto maxTextWidth = 0;
    for(const auto& group : m_model)
        for(const auto& item : group.Items)
            maxTextWidth = qMax(maxTextWidth, fontMetrics().horizontalAdvance(item->text()));
    
    auto minWidth = maxTextWidth + marginsAndIconsWidth;
    
    return QSize(
        qMax(width, minWidth),
        heightForWidth(width)
        );
}

bool _CustomComboBoxParentChangeWatcher::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::ParentChange)
        emit parentChanged();
    
    if((event->type() == QEvent::Move || event->type() == QEvent::Resize) && m_edit->m_popup)
    {
        auto pos = m_edit->pos();
        auto mappedPos = m_edit->mapToGlobal(pos);
        if(mappedPos != m_edit->m_lastPosition || event->type() == QEvent::Resize)
            m_edit->updatePopupGeometry();
        m_edit->m_lastPosition = mappedPos;
    }
    
    return false;
}
