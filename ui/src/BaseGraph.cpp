#include "ui/include/BaseGraph.h"

#include <QWidget>
#include <QScrollBar>
#include <QTimer>
#include <QLabel>


#define TOOLBAR_BUTTON_SIZE QSize(30, 30)
#define TOOLBAR_FRAME_WIDTH 30


Gex::Ui::Message::Message(QWidget* parent): QFrame(parent)
{
    setAutoFillBackground(true);

    animation = new QVariantAnimation(this);
    animation->setStartValue(255);
    animation->setEndValue(0);

    QObject::connect(animation, &QVariantAnimation::valueChanged,
                     this, &Message::SetOpacity);

    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(5, 5, 5, 5);
    setLayout(layout);

    label = new QLabel(this);
    label->setStyleSheet("QLabel{background-color: transparent;}");
    layout->addWidget(label);

    setWindowFlag(Qt::FramelessWindowHint);
}


std::string ColorToStr(QColor color, int alpha=255)
{
    return ("rgba(" + std::to_string(color.red()) + ", " +
            std::to_string(color.green()) + ", " +
            std::to_string(color.blue()) + ", " +
            std::to_string(alpha) + ")");
}


void Gex::Ui::Message::SetFeedback(Gex::Ui::UiFeedback feedback)
{
    f = feedback;

    std::string stylesheet_ = ("QFrame {border-radius: 5px; background-color: " +
            ColorToStr(feedback.color) + ";}");
    setStyleSheet(stylesheet_.c_str());

    std::string labelStylesheet = ("QLabel {color: " +
            ColorToStr(feedback.textColor) + ";}");
    label->setStyleSheet(labelStylesheet.c_str());

    label->setText(feedback.message.c_str());
}


void Gex::Ui::Message::SetOpacity(QVariant opacity)
{
    double o = opacity.toInt();

    std::string stylesheet_ = ("QFrame {border-radius: 5px; background-color: " +
                               ColorToStr(f.color, o) + ";}");
    setStyleSheet(stylesheet_.c_str());

    std::string labelStylesheet = ("QLabel {color: " +
            ColorToStr(f.textColor, o) +
            ";background-color: transparent;}");
    label->setStyleSheet(labelStylesheet.c_str());

    if (o < 2)
    {
        setVisible(false);
    }
}


void Gex::Ui::Message::Hide(int duration)
{
    animation->setDuration(duration);
    animation->start();
}


void Gex::Ui::Message::Reset()
{
    animation->stop();
    SetOpacity(255);
}


Gex::Ui::Toolbar::Toolbar(QWidget* parent): QFrame(parent)
{
    QVBoxLayout* l = new QVBoxLayout();
    l->setAlignment(Qt::AlignVCenter);
    l->setContentsMargins(10, 0, 0, 0);
    setLayout(l);

    setObjectName("TransparentFrame");

    toolbar = new QFrame(this);
    toolbar->setObjectName("TransparentFrame");
    toolbarLayout = new QVBoxLayout();
    toolbarLayout->setAlignment(Qt::AlignCenter);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    toolbar->setLayout(toolbarLayout);
    l->addWidget(toolbar);

    animation = new QVariantAnimation(this);
    animation->setDuration(200);

    QObject::connect(animation, &QPropertyAnimation::valueChanged,
                     this, &Toolbar::SetPos);
    QObject::connect(animation, &QVariantAnimation::finished,
                     this, &Toolbar::AnimationFinished);

    setFixedWidth(TOOLBAR_BUTTON_SIZE.width() + 10);
//    AdjustSize();
}


int Gex::Ui::Toolbar::LeavePoint() const
{
    return -size().width();
}


int Gex::Ui::Toolbar::ViewPoint() const
{
    return 0;
}


void Gex::Ui::Toolbar::resizeEvent(QResizeEvent* event)
{
//    ResizeToolbar();
}

QPushButton* Gex::Ui::Toolbar::NewButton()
{
    auto* button = new QPushButton(toolbar);
    button->setObjectName("ViewToolbarButton");
    button->setFixedSize(TOOLBAR_BUTTON_SIZE);
    toolbarLayout->addWidget(button);

//    AdjustSize();

    return button;
}


void Gex::Ui::Toolbar::AddWidget(QWidget* widget)
{
    widget->setParent(this);
    toolbarLayout->addWidget(widget);
}


QFrame* Gex::Ui::Toolbar::AddSeparator()
{
    auto* sep = new QFrame(toolbar);
    sep->setObjectName("ToolbarSeparator");
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(1);
    sep->setFixedWidth(TOOLBAR_BUTTON_SIZE.width());
    toolbarLayout->addWidget(sep, Qt::AlignCenter);
    return sep;
}


QSize Gex::Ui::Toolbar::Size() const{
    return QSize(TOOLBAR_BUTTON_SIZE.width(),
            toolbarLayout->count() * TOOLBAR_BUTTON_SIZE.height()
            + toolbarLayout->spacing() * toolbarLayout->count() - 1
            + toolbarLayout->contentsMargins().top()
            + toolbarLayout->contentsMargins().bottom());
}


void Gex::Ui::Toolbar::SetPos(QVariant x)
{
    auto r = geometry();
    int x_ = qvariant_cast<int>(x);
    auto destRect = QRect(x_, 0, r.width(), r.height());

    toolbar->setGeometry(destRect);
}


bool Gex::Ui::Toolbar::IsVisible() const
{
    return visible;
}


bool Gex::Ui::Toolbar::IsHiding() const
{
    return hiding;
}


void  Gex::Ui::Toolbar::AnimationFinished()
{
    if (hiding)
        setVisible(false);
}


void Gex::Ui::Toolbar::Show(bool animate)
{
    hiding = false;

    show();
    if (animate)
    {
        animation->setStartValue(QVariant(LeavePoint()));
        animation->setEndValue(QVariant(ViewPoint()));
        animation->start();
    }
    else
    {
        SetPos(QVariant(ViewPoint()));
    }

    visible = true;
}


void Gex::Ui::Toolbar::Hide(bool animate)
{
    hiding = true;
    if (animate)
    {
        animation->setStartValue(QVariant(ViewPoint()));
        animation->setEndValue(QVariant(LeavePoint()));
        animation->start();
    }
    else
    {
        SetPos(QVariant(LeavePoint()));
        hide();
    }

    visible = false;
}


void Gex::Ui::Toolbar::UpdateVisible()
{
    if (IsVisible())
        Show(false);
    else
        Hide(false);
}


bool Gex::Ui::MoveContext::AcceptBaseEvents() const
{
    return true;
}

void Gex::Ui::MoveContext::Setup(BaseGraphView* view)
{
    CurrentView()->setCursor(Qt::OpenHandCursor);
    policy = CurrentView()->contextMenuPolicy();

    CurrentView()->setContextMenuPolicy(Qt::NoContextMenu);
}


void Gex::Ui::MoveContext::Finalize(BaseGraphView *scene)
{
    scene->unsetCursor();
    CurrentView()->setContextMenuPolicy(policy);
}


void Gex::Ui::MoveContext::OnPressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        CurrentView()->setDragMode(BaseGraphView::ScrollHandDrag);
    }
    else if (event->button() == Qt::RightButton)
    {
        zooming = true;
        zoomingPos = event->pos();
        CurrentView()->setDragMode(BaseGraphView::NoDrag);
        CurrentView()->setCursor(Qt::SizeBDiagCursor);
    }
}

void Gex::Ui::MoveContext::OnMoveEvent(QMouseEvent* event)
{
    if (zooming)
    {
        zooming = true;

        if (event->pos().x() > zoomingPos.x())
        {
            CurrentView()->ZoomOut();
        }
        else if (event->pos().x() < zoomingPos.x())
        {
            CurrentView()->ZoomIn();
        }

        zoomingPos = event->pos();
    }
}

void Gex::Ui::MoveContext::OnReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        CurrentView()->unsetCursor();
    }

    zooming = false;
}


bool Gex::Ui::SelectContext::AcceptBaseEvents() const
{
    return true;
}

void Gex::Ui::SelectContext::Setup(BaseGraphView* view)
{
    CurrentView()->setDragMode(QGraphicsView::RubberBandDrag);
}

void Gex::Ui::SelectContext::OnPressEvent(QMouseEvent* event)
{

}

void Gex::Ui::SelectContext::OnMoveEvent(QMouseEvent* event)
{

}

void Gex::Ui::SelectContext::OnReleaseEvent(QMouseEvent* event)
{

}



Gex::Ui::BaseGraphView::BaseGraphView(QGraphicsScene* scene, QWidget* parent,
                                      bool usetoolbar):
        QGraphicsView(scene, parent)
{
//    installEventFilter(new MoveEventFilter(this)),

    contexts = new ViewContexts(this);
    contexts->RegisterContext<SelectContext>("Select");
    contexts->RegisterContext<MoveContext>("Move");

    contexts->SetCurrentContext("Select");

    setRenderHint(QPainter::Antialiasing);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setDragMode(QGraphicsView::RubberBandDrag);

    setContextMenuPolicy(Qt::CustomContextMenu);

    auto* layout = new QGridLayout();
    layout->setAlignment(Qt::AlignLeft);
    setLayout(layout);

    auto menuCallback = [this](const QPoint& pos)
    {
        this->ShowMenu(GetMenu(), pos);
    };

    QObject::connect(this, &QGraphicsView::customContextMenuRequested,
                     menuCallback);

    message = new Message(this);
    auto policy = message->sizePolicy();
    policy.setRetainSizeWhenHidden(true);
    message->setSizePolicy(policy);
    messageTimer = new QTimer(this);

    QObject::connect(messageTimer, &QTimer::timeout,
                     this, &BaseGraphView::HideMessage);

    if (usetoolbar)
    {
        toolbar = new Toolbar(this);
        layout->addWidget(toolbar, 0, 0, 9, 1, Qt::AlignLeft | Qt::AlignVCenter);

        toolbar->Hide();
    }

    layout->addWidget(message, 10, 0, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
}


Gex::Ui::BaseGraphView::~BaseGraphView()
{
    delete contexts;
}


Gex::Ui::ViewContexts* Gex::Ui::BaseGraphView::GetEditors() const
{
    return contexts;
}


Gex::Ui::Toolbar* Gex::Ui::BaseGraphView::GetToolbar() const
{
    return toolbar;
}



QMenu* Gex::Ui::BaseGraphView::GetMenu()
{
    return nullptr;
}


bool Gex::Ui::BaseGraphView::DeleteMenuOnHide() const
{
    return true;
}


QRect ItemsRect(QList<QGraphicsItem*> items)
{
    qreal minx = items[0]->sceneBoundingRect().topLeft().x();
    qreal miny = items[0]->sceneBoundingRect().topLeft().y();
    qreal maxx = items[0]->sceneBoundingRect().bottomRight().x();
    qreal maxy = items[0]->sceneBoundingRect().bottomRight().y();
    QList<QRectF> rects;
    for (int i = 1; i < items.length(); i++)
    {
        QRectF boundingRect = items[i]->sceneBoundingRect();
        qreal minx_ = boundingRect.topLeft().x();
        qreal miny_ = boundingRect.topLeft().y();
        qreal maxx_ = boundingRect.bottomRight().x();
        qreal maxy_ = boundingRect.bottomRight().y();
        if (minx_ < minx)
            minx = minx_;
        if (miny_ < miny)
            miny = miny_;
        if (maxx_ > maxx)
            maxx = maxx_;
        if (maxy_ > maxy)
            maxy = maxy_;
    }

    return QRect(minx, miny, maxx - minx, maxy - miny);
}


void Gex::Ui::BaseGraphView::ViewUsedSceneRect()
{
    QRect usedSceneRect;

    QList<QGraphicsItem*> items_ = scene()->items();
    if (items_.empty())
    {
        return;
    }

    fitInView(ItemsRect(FilterSelectedItems(items_)), Qt::KeepAspectRatio);
}


void Gex::Ui::BaseGraphView::ViewSelectionSceneRect()
{
    QList<QGraphicsItem*> selection = scene()->selectedItems();

    if (selection.empty())
    {
        return;
    }

    fitInView(ItemsRect(FilterSelectedItems(selection)), Qt::KeepAspectRatio);
}


void Gex::Ui::BaseGraphView::mousePressEvent(QMouseEvent* event)
{
    contexts->OnPressEvent(event);

    if (contexts->AcceptBaseEvents())
    {
        QGraphicsView::mousePressEvent(event);
    }
}


void Gex::Ui::BaseGraphView::mouseMoveEvent(QMouseEvent* event)
{
    contexts->OnMoveEvent(event);

    if (contexts->AcceptBaseEvents())
    {
        QGraphicsView::mouseMoveEvent(event);
    }
}


void Gex::Ui::BaseGraphView::ShowMenu(QMenu* menu, const QPoint& pos)
{
    menu->exec(mapToGlobal(pos));
}


QList<QGraphicsItem*> Gex::Ui::BaseGraphView::FilterSelectedItems(
        QList<QGraphicsItem*> items) const
{
    return items;
}


void Gex::Ui::BaseGraphView::mouseReleaseEvent(QMouseEvent* event)
{
//    pressed = false;
//
//    if (zooming)
//    {
//        zooming = false;
//    }
//    else
//    {
//        if (event->button() == Qt::RightButton)
//        {
//            auto* menu = GetMenu();
//            if (menu)
//            {
//                ShowMenu(menu, event->pos());
//            }
//        }
//
//        QGraphicsView::mouseReleaseEvent(event);
//    }
//
//    RestoreCursor();
    contexts->OnReleaseEvent(event);

    if (contexts->AcceptBaseEvents())
    {
        QGraphicsView::mouseReleaseEvent(event);
    }
}


void Gex::Ui::BaseGraphView::enterEvent(QEnterEvent* event)
{
    if (toolbar && !toolbar->IsVisible())
        toolbar->Show();

    QGraphicsView::leaveEvent(event);
}

void Gex::Ui::BaseGraphView::leaveEvent(QEvent* event)
{
    if (toolbar && toolbar->IsVisible())
        toolbar->Hide();

    QGraphicsView::leaveEvent(event);
}



void Gex::Ui::BaseGraphView::ZoomIn()
{
    QTransform currTsfm = transform();
    QTransform scaledTransform(
            currTsfm.m11() / zoomFactor, currTsfm.m12(), currTsfm.m13(),
            currTsfm.m21(), currTsfm.m22() / zoomFactor, currTsfm.m23(),
            currTsfm.m31(), currTsfm.m32(), currTsfm.m33());

    setTransform(scaledTransform);
}


void Gex::Ui::BaseGraphView::ZoomOut()
{
    QTransform currTsfm = transform();
    QTransform scaledTransform(
            currTsfm.m11() * zoomFactor, currTsfm.m12(), currTsfm.m13(),
            currTsfm.m21(), currTsfm.m22() * zoomFactor, currTsfm.m23(),
            currTsfm.m31(), currTsfm.m32(), currTsfm.m33());

    setTransform(scaledTransform);
}


QRectF UnitedRect(QList<QRectF> rects)
{
    QRectF rect = rects[0];

    for (int i = 1; i < rects.count(); i++)
    {
        rect = rect.united(rects[i]);
    }

    return rect;
}


void Gex::Ui::BaseGraphView::FrameSelection()
{
    QList<QRectF> rects;

    for (const auto* item : scene()->selectedItems())
    {
        rects.append(item->sceneBoundingRect());
    }

    if (rects.empty())
        return;

    auto frameRect = UnitedRect(rects);
    fitInView(frameRect, Qt::KeepAspectRatio);
}


void Gex::Ui::BaseGraphView::FrameAll()
{
    QList<QRectF> rects;

    for (const auto* item : scene()->items())
    {
        rects.append(item->sceneBoundingRect());
    }

    if (rects.empty())
        return;

    auto frameRect = UnitedRect(rects);
    fitInView(frameRect, Qt::KeepAspectRatio);
}




void Gex::Ui::BaseGraphView::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0)
    {
        ZoomIn();
    }
    else
    {
        ZoomOut();
    }
}


void Gex::Ui::BaseGraphView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Alt)
    {
        if (!event->isAutoRepeat())
        {
            contexts->SetCurrentContext("Move");
            grabKeyboard();
        }
        event->accept();
    }
    else
    {
        QGraphicsView::keyPressEvent(event);
    }
}

void Gex::Ui::BaseGraphView::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Alt)
    {
        if (!event->isAutoRepeat())
        {
            contexts->UnsetCurrentContext();
            releaseKeyboard();
        }
        event->accept();
    }
    else if (event->key() == Qt::Key_F)
    {
        FrameSelection();
        event->accept();
    }
    else if (event->key() == Qt::Key_A)
    {
        FrameAll();
        event->accept();
    }
    else
    {
        QGraphicsView::keyReleaseEvent(event);
    }
}


bool Gex::Ui::BaseGraphView::event(QEvent *event)
{
    return QGraphicsView::event(event);
}


void Gex::Ui::BaseGraphView::resizeEvent(QResizeEvent* event)
{
    if (toolbar)
        toolbar->UpdateVisible();

    QGraphicsView::resizeEvent(event);
}


void Gex::Ui::BaseGraphView::ShowMessage(Gex::Ui::UiFeedback feedback,
                                         int duration)
{
    messageTimer->setInterval(duration);
    messageTimer->start();

    message->SetFeedback(feedback);
    message->Reset();
    message->adjustSize();

    QRect r = rect();
    QRect mr = message->geometry();

    QRect geometry = QRect(5, r.height() - mr.height() - 5,
                           mr.width(), mr.height());

    message->setGeometry(geometry);
    message->setVisible(true);
}


void Gex::Ui::BaseGraphView::HideMessage()
{
    message->Hide(2000);
}