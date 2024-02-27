#include "ui/include/BaseGraph.h"

#include <QWidget>
#include <QScrollBar>
#include <QTimer>
#include <QLabel>


#define TOOLBAR_BUTTON_SIZE QSize(30, 30)
#define TOOLBAR_FRAME_WIDTH 30


Gex::Ui::Message::Message(Gex::Ui::UiFeedback feedback, int duration_,
                          int fadeDuration_, QWidget* parent): QDialog(parent)
{
    f = feedback;
    duration = duration_;
    fadeDuration = fadeDuration_;

    setWindowFlag(Qt::FramelessWindowHint);

    std::string stylesheet = ( "QDialog {border-radius: 5px; background-color: " +
            feedback.color.name().toStdString() + ";}");

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    QLabel* label = new QLabel(this);
    label->setText(feedback.message.c_str());
    layout->addWidget(label);

    label->setStyleSheet("QLabel { color: " + feedback.textColor.name() + ";}");

    auto* timer = new QTimer(this);
    auto* fadeTimer = new QTimer(this);

    QObject::connect(timer, &QTimer::timeout,
                     this, &QDialog::hide);

    timer->start();
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
    auto destRect = QRect(x_, r.y(), r.width(), r.height());
    setGeometry(destRect);
}


bool Gex::Ui::Toolbar::IsVisible() const
{
    return visible;
}


void Gex::Ui::Toolbar::Show(bool animate)
{
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
    if (animate)
    {
        animation->setStartValue(QVariant(ViewPoint()));
        animation->setEndValue(QVariant(LeavePoint()));
        animation->start();
    }
    else
    {
        SetPos(QVariant(LeavePoint()));
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


void Gex::Ui::BaseGraphView::SetMainCursor(QCursor c)
{
    mainCursor = c;
    if (!pressed)
    {
        setCursor(c);
    }
}

void Gex::Ui::BaseGraphView::SetPressCursor(QCursor c)
{
    pressCursor = c;

    if (pressed)
        setCursor(c);
}

void Gex::Ui::BaseGraphView::RestoreCursor()
{
    setCursor(mainCursor);
}



Gex::Ui::BaseGraphView::BaseGraphView(QGraphicsScene* scene, QWidget* parent,
                                                bool usetoolbar):
        QGraphicsView(scene, parent)
{
    setRenderHint(QPainter::Antialiasing);

    mainCursor = cursor();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setDragMode(QGraphicsView::RubberBandDrag);

    setContextMenuPolicy(Qt::CustomContextMenu);

//    QObject::connect(this, &QGraphicsView::customContextMenuRequested,
//                     this, &NodeGraphView::ShowCreateNodeMenu);

    if (usetoolbar)
    {
        auto* layout = new QGridLayout();
        setLayout(layout);

        toolbar = new Toolbar(this);
        layout->addWidget(toolbar);

        toolbar->Hide();
    }

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
    pressed = true;

    if (event->modifiers() == Qt::AltModifier && event->button() == Qt::RightButton)
    {
        zooming = true;
        zoomingPos = event->pos();
        SetPressCursor(Qt::SizeBDiagCursor);
    }
    else if (dragMode && event->button() == Qt::LeftButton)
    {
        setCursor(Qt::ClosedHandCursor);
        dragPos = event->pos();
        sceneDragPos = mapToScene(rect()).boundingRect().center();

        QRectF sceneR = mapToScene(rect()).boundingRect();
        scaleX = sceneR.width() / rect().width();
        scaleY = sceneR.height() / rect().height();

        SetPressCursor(Qt::ClosedHandCursor);
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }
}


void Gex::Ui::BaseGraphView::mouseMoveEvent(QMouseEvent* event)
{
    if (zooming)
    {
        zooming = true;

        if (event->pos().x() > zoomingPos.x())
        {
            ZoomOut();
        }
        else if (event->pos().x() < zoomingPos.x())
        {
            ZoomIn();
        }

        zoomingPos = event->pos();
    }
    else if (pressed && dragMode)
    {
        QPointF delta = event->pos() - dragPos;

        delta = {delta.x() * scaleX,
                 delta.y() * scaleY};

        QPointF move = sceneDragPos - delta;
        centerOn(move.x(),move.y());
    }
    else
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
    pressed = false;

    if (zooming)
    {
        zooming = false;
    }
    else
    {
        if (event->button() == Qt::RightButton)
        {
            auto* menu = GetMenu();
            if (menu)
            {
                ShowMenu(menu, event->pos());
            }
        }

        QGraphicsView::mouseReleaseEvent(event);
    }

    RestoreCursor();
}


void Gex::Ui::BaseGraphView::enterEvent(QEnterEvent* event)
{
    if (toolbar)
        toolbar->Show();
}

void Gex::Ui::BaseGraphView::leaveEvent(QEvent* event)
{
    if (toolbar)
        toolbar->Hide();
}



void Gex::Ui::BaseGraphView::ZoomIn()
{
    QPointF sceneCenter = mapToScene(rect().center());

    scale(zoomInFactor, zoomInFactor);

    centerOn(sceneCenter);
}


void Gex::Ui::BaseGraphView::ZoomOut()
{
    QPointF sceneCenter = mapToScene(rect().center());

    scale(zoomOutFactor, zoomOutFactor);

    centerOn(sceneCenter);
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
        dragMode = true;
        SetMainCursor(Qt::OpenHandCursor);
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
        dragMode = false;
        SetMainCursor(Qt::ArrowCursor);
    }
    else if (event->key() == Qt::Key_F)
    {
        FrameSelection();
    }
    else if (event->key() == Qt::Key_A)
    {
        FrameAll();
    }
}


void Gex::Ui::BaseGraphView::resizeEvent(QResizeEvent* event)
{
    if (toolbar)
        toolbar->UpdateVisible();

    QGraphicsView::resizeEvent(event);
}


void Gex::Ui::BaseGraphView::ShowMessage(Gex::Ui::UiFeedback feedback)
{
    if (current)
        current->deleteLater();

    current = new Message(feedback, 10000, 500, this);

    QRect r = geometry();
    QRect mr = current->geometry();

    current->setGeometry(QRect(r.left() + 5, r.bottom() - current->height(),
                               current->height(), current->width()));
    current->show();
}