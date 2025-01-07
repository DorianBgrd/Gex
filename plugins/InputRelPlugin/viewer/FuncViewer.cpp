#include "FuncViewer.h"
#include "UiRes/include/uires.h"

#include <QShortcut>


double Gex::InputRel::ToViewX(double value, int decimals)
{
    return value * std::pow(10, decimals);
}


double Gex::InputRel::ToViewY(double value, int decimals)
{
    return -ToViewX(value, decimals);
}


double Gex::InputRel::ToViewLength(double value, int decimals)
{
    return ToViewX(value, decimals);
}


double Gex::InputRel::FromViewX(double value, int decimals)
{
    return value / std::pow(10, decimals);
}


double Gex::InputRel::FromViewY(double value, int decimals)
{
    return -FromViewX(value, decimals);
}


double Gex::InputRel::FromViewLength(double value, int decimals)
{
    return FromViewX(value, decimals);
}


Gex::InputRel::FuncViewerHandle::FuncViewerHandle(
        const CurvePointWk& p,
        const FuncDelegatePtr& d,
        FuncScene* s
        ): QGraphicsObject(nullptr)
{
    point = p;
    delegate = d;
    scene = s;

    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIgnoresTransformations);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
}


void Gex::InputRel::FuncViewerHandle::Initialize()
{
    initialized = true;
}


Gex::InputRel::FuncDelegatePtr Gex::InputRel::FuncViewerHandle::Delegate() const
{
    return delegate;
}


Gex::InputRel::CurvePointWk Gex::InputRel::FuncViewerHandle::Point() const
{
    return point;
}


void Gex::InputRel::FuncViewerHandle::Update()
{
    setPos({ToViewX(point->GetX(), delegate->Decimals()),
            ToViewY(point->GetY(), delegate->Decimals())});
}


void Gex::InputRel::FuncScene::EmitMessage(
        const Gex::Ui::UiFeedback& m)
{
    for (const auto& cb : feedbackCallbacks)
    {
        cb.second(m);
    }
}


QVariant Gex::InputRel::FuncViewerHandle::itemChange(
        GraphicsItemChange change,
        const QVariant &value)
{
    if (initialized && change == QGraphicsItem::ItemPositionChange)
    {
        sourcePos = pos();
        if (!delegate->EditPoint(point, sourcePos, qvariant_cast<QPointF>(value)))
        {
            std::string msg = ("Failed editing point (" + std::to_string(point->GetX()) +
                    ", " + std::to_string(point->GetY()) + ").");
                    scene->EmitMessage(
                    Gex::Ui::MakeFeedback(
                        Gex::Status::Warning,
                        msg)
                    );
        }

        scene->OnSelectedPointsModified();
    }

    return value;
}

void Gex::InputRel::FuncViewerHandle::paintCoordinates(
        QPainter* painter)
{
    painter->save();

    QPen pen(Qt::white);
    painter->setPen(pen);

    painter->drawText(
            boundingRect().bottomRight(),
            "(" + QString::number(Point()->GetX()) +
            ", " + QString::number(Point()->GetY()) + ")"
            );
}


void Gex::InputRel::FuncViewerHandle::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{
    paintHandle(painter, option, widget);

    if (option->state & QStyle::State_Selected)
    {
//        paintCoordinates(painter);
    }
}


//void Gex::InputRel::FuncViewerHandle::mousePressEvent(
//        QGraphicsSceneMouseEvent *event)
//{
//    moved = false;
//    sourcePos = pos();
//    QGraphicsItem::mousePressEvent(event);
//}
//
//void Gex::InputRel::FuncViewerHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
//{
//    if (moved)
//    {
//        delegate->EditPoint(sourcePos, pos());
//    }
//
//    moved = false;
//    QGraphicsItem::mouseReleaseEvent(event);
//}



void Gex::InputRel::FuncViewerDelegate::SetFunction(const std::any& f)
{
    function = f;

    InitFunction(function);
}


std::any Gex::InputRel::FuncViewerDelegate::Function() const
{
    return function;
}


void Gex::InputRel::FuncViewerDelegate::InitFunction(const std::any& f)
{

}


int Gex::InputRel::FuncViewerDelegate::Decimals() const
{
    return decimals;
}


void Gex::InputRel::FuncViewerDelegate::SetDecimals(int d)
{
    decimals = d;
}


Gex::InputRel::FuncViewerHandle*  Gex::InputRel::FuncViewerDelegate::CreateHandle(
        const CurvePointWk& p, FuncScene* scene)
{
    return CreateHandle(p, shared_from_this(), scene);
}


bool Gex::InputRel::FuncViewerDelegate::EditPoint(
        const CurvePointWk& point,
        const QPointF& previous,
        const QPointF& next)
{
    return EditPoint(point, previous, next, decimals);
}


void Gex::InputRel::FuncViewerDelegate::DrawPoints(
        FuncScene* scene, HandleList& outPoints
)
{
    DrawPoints(scene, decimals, outPoints);
}


void Gex::InputRel::FuncViewerDelegate::DrawPoints(
        FuncScene* scene, int dec, HandleList& outPoints
)
{
    for (const auto& point : GetPoints())
    {
        auto* handle = CreateHandle(point, scene);
        handle->setPos(ToViewX(point->GetX(), dec),
                       ToViewY(point->GetY(), dec));
        scene->addItem(handle);

        handle->Initialize();

        outPoints.append(handle);
    }
}


void Gex::InputRel::FuncViewerDelegate::DrawCurve(
        QPainterPath& path,
        FuncScene* scene
) const
{
    DrawCurve(path, scene, decimals);
}


Gex::InputRel::FuncDelegateFactory* Gex::InputRel::FuncDelegateFactory::instance;


Gex::InputRel::FuncDelegateCreatorPtr Gex::InputRel::FuncDelegateFactory::GetFactory(size_t hash)
{
    if (!instance)
        instance = new FuncDelegateFactory();

    auto iter = instance->delegates.find(hash);
    if (iter == instance->delegates.end())
        return {};

    return iter->second;
}


Gex::InputRel::FuncDelegatePtr Gex::InputRel::FuncDelegateFactory::GetDelegate(
        const std::any& func)
{
    auto delegate = GetFactory(func)->CreateDelegate();
    if (delegate)
    {
        delegate->SetFunction(func);
    }

    return delegate;
}


void  Gex::InputRel::FuncDelegateFactory::RegisterDelegate(
        size_t hash, const FuncDelegateCreatorPtr& delegate)
{
    if (!instance)
        instance = new FuncDelegateFactory();

    instance->delegates[hash] = delegate;
}


Gex::InputRel::FuncScene::FuncScene(
        qreal x, qreal y, qreal width,
        qreal height, QObject* parent):
        QGraphicsScene(x, y, width, height, parent)
{

}


unsigned int Gex::InputRel::FuncScene::AddFeedbackCallback(
        const UiFeedbackCb& cb)
{
    feedbackIndex++;
    feedbackCallbacks[feedbackIndex] = cb;

    return feedbackIndex;
}


bool Gex::InputRel::FuncScene::RemoveFeedbackCallback(
        unsigned int index)
{
    auto iter = feedbackCallbacks.find(index);
    if (iter == feedbackCallbacks.end())
        return false;

    feedbackCallbacks.erase(iter);
    return true;
}


Gex::InputRel::FuncScene::~FuncScene()
{

}


bool Gex::InputRel::FuncScene::CreateMode() const
{
    return createMode;
}


void Gex::InputRel::FuncScene::SetCreateMode(bool mode)
{
    createMode = mode;
}


int Gex::InputRel::FuncScene::Decimals() const
{
    return decimals;
}


void Gex::InputRel::FuncScene::SetDecimals(int d)
{
    decimals = d;
}


void Gex::InputRel::FuncScene::mouseReleaseEvent(
        QGraphicsSceneMouseEvent *event
)
{
    if (createMode)
    {
        Q_EMIT PointRequested(event->scenePos());
        createMode = false;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}


QList<Gex::InputRel::FuncViewerHandlePtr> Gex::InputRel::FuncScene::SelectedHandles() const
{
    QList<FuncViewerHandlePtr> sel;
    for (auto* item : selectedItems())
    {
        if (auto* handle = dynamic_cast<FuncViewerHandle*>(item))
        {
            sel.append(handle);
        }
    }

    return sel;
}


void Gex::InputRel::FuncScene::OnSelectedPointsModified()
{
    Q_EMIT PointsModified(SelectedHandles());
}


Gex::InputRel::FuncView::FuncView(FuncScene* scene, QWidget *parent, bool toolbar):
    BaseGraphView(scene, parent, toolbar)
{
    setBackgroundBrush(QColor(0, 0, 0, 0));

    setViewportUpdateMode(FullViewportUpdate);
    funcScene = scene;

//    curveItem = new QGraphicsPathItem();
//    curveItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

//    QPen curvePen(QColor(232, 168, 55));
//    curvePen.setWidth(10000);
//    curveItem->setPen(curvePen);

//    funcScene->addItem(curveItem);

    QObject::connect(funcScene, &FuncScene::PointRequested,
                     this, &FuncView::AddPoint);

    QObject::connect(funcScene, &QGraphicsScene::selectionChanged,
                     this, &FuncView::OnSceneSelection);

    QObject::connect(funcScene, &FuncScene::PointsModified,
                     this, &FuncView::OnPointsModified);

    QRectF viewRect(ToViewX(-100, decimals),
                    ToViewY(100, decimals),
                    ToViewLength(200, decimals),
                    ToViewLength(200, decimals)
                    );
    fitInView(viewRect, Qt::KeepAspectRatio);
}

Gex::InputRel::FuncView::~FuncView()
{

}


void Gex::InputRel::FuncView::OnSceneSelection()
{
    QList<FuncViewerHandlePtr> selectedPoints;

    for (auto* selected : scene()->selectedItems())
    {
        auto* casted = dynamic_cast<FuncViewerHandle*>(selected);
        if (!casted)
        {
            continue;
        }

        selectedPoints.append(casted);
    }

    Q_EMIT PointsSelected(selectedPoints);
}


void Gex::InputRel::FuncView::OnPointsModified(
        QList<FuncViewerHandlePtr> pnts)
{
    DrawCurve();

    Q_EMIT PointsModified(std::move(pnts));
}


double Gex::InputRel::FuncView::Tick() const
{
    return tick;
}


void Gex::InputRel::FuncView::SetTick(double t)
{
    tick = t;

    update();
}


void Gex::InputRel::FuncView::AddPoint(QPointF point)
{
    delegate->AddPoint(FromViewX(point.x(), decimals),
                       FromViewY(point.y(), decimals));

    if (createMode)
    {
        SetCreateMode(false);
    }

    Draw();
}


void Gex::InputRel::FuncView::SetFunction(const std::any& func)
{
    delegate = FuncDelegateFactory::GetDelegate(func);

    Draw();
}


bool Gex::InputRel::FuncView::CreateMode() const
{
    return createMode;
}


void Gex::InputRel::FuncView::SetCreateMode(bool mode)
{
    createMode = mode;

    if (mode)
    {
        setCursor(Qt::CrossCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }

    funcScene->SetCreateMode(createMode);
}


void Gex::InputRel::FuncView::ActivateCreateMode()
{
    SetCreateMode(true);
}


void Gex::InputRel::FuncView::Draw()
{
    DrawPoints();
    DrawCurve();
}


void Gex::InputRel::FuncView::DrawPoints()
{
    for (auto* handle : handles)
    {
        delete handle;
    }

    handles.clear();

    if (delegate)
    {
        delegate->DrawPoints(funcScene, handles);
    }
}


void Gex::InputRel::FuncView::DrawCurve()
{
    if (!delegate)
        return;

    path.clear();
    delegate->DrawCurve(path, funcScene, decimals);

//    curveItem->setPath(path);
}


void Gex::InputRel::FuncView::AutoFrame()
{
    auto points = delegate->GetPoints();
    if (points.empty())
    {
        QRectF viewRect(ToViewX(-100, decimals),
                        ToViewY(100, decimals),
                        ToViewLength(200, decimals),
                        ToViewLength(200, decimals)
        );

        fitInView(viewRect, Qt::KeepAspectRatio);
        return;
    }

    if (points.size() == 1)
    {
        auto onlyPoint = *points.begin();
        fitInView(QRectF(
                          ToViewX(onlyPoint->GetX() - 10, decimals),
                          ToViewY(onlyPoint->GetY() - 10, decimals),
                          ToViewX(10, decimals),
                          ToViewX(10, decimals)
                  ), Qt::KeepAspectRatio
        );
    }

    else
    {
        auto firstPoint = *points.begin();
        auto lastPoint = *(--points.end());

        auto minY = static_cast<double>(MAXINT_PTR);
        double maxY = -minY;

        for (const auto& point : points)
        {
            if (point->GetY() < minY)
            {
                minY = point->GetY();
            }
            else if (point->GetY() > maxY)
            {
                maxY = point->GetY();
            }
        }

        fitInView(
                QRectF(
                        ToViewX(firstPoint->GetX() - 5, decimals),
                        ToViewY(minY - 5, decimals),
                        ToViewX(lastPoint->GetX() - firstPoint->GetX() + 10, decimals),
                        ToViewX(maxY - minY + 10, decimals)
                        ), Qt::KeepAspectRatio
                );
    }
}


void Gex::InputRel::FuncView::paintEvent(QPaintEvent* event)
{
    QGraphicsView::paintEvent(event);
}


void Gex::InputRel::FuncView::DrawAdaptativeGrid(QPainter* painter)
{
    auto sceneTopLeft = mapToScene(viewport()->geometry().topLeft());
    auto sceneBottomRight = mapToScene(viewport()->geometry().bottomRight());

    QRectF rect(sceneTopLeft, sceneBottomRight);
    double scale = rect.width() / viewport()->rect().width();

    QPen linePen(QColor(50, 50, 50));
    linePen.setWidth(0);

    painter->setPen(linePen);
    painter->drawLine(rect.left(), 0, rect.right(), 0);

    double increment = ToViewLength(1, decimals);

    // Get the number of line that should be drawn.
    // Max number of line is 10, so multiply the increment by 10 until
    // a fine number of line is reached.
    while ((std::ceil(rect.height() / increment)) > 10)
    {
        increment *= 2;
    }

//    double fontSize = ToViewLength(8, decimals);
    double fontSize = 10 * scale;
    painter->setFont(QFont("sans", fontSize));

    // The base draw is 10 to 10 on round numbers.
    double top = rect.top() - std::fmod(rect.top(), increment);

    double y = top;
    while (y < rect.bottom())
    {
        painter->drawLine(rect.left(), y, rect.right(), y);
        painter->drawText(rect.left(), y + fontSize - 2, QString::number(FromViewY(y, decimals)));
        y += increment;
    }

    double left = rect.left()  - std::fmod(rect.left(), increment);;

    double x = left;
    while (x < rect.right())
    {
        painter->drawLine(x, rect.top(), x, rect.bottom());
        painter->drawText(x + scale, rect.top() + fontSize, QString::number(FromViewX(x, decimals)));
        x += increment;
    }
}


void Gex::InputRel::FuncView::drawBackground(
        QPainter *painter,
        const QRectF &)
{
    DrawAdaptativeGrid(painter);

    painter->setRenderHint(QPainter::Antialiasing);

    QPen curvePen(QColor(220, 119, 18));
    curvePen.setWidth(0);
    painter->setPen(curvePen);

    painter->drawPath(path);
}


void Gex::InputRel::FuncView::drawForeground(QPainter *painter, const QRectF &r)
{
    auto sceneTopLeft = mapToScene(viewport()->geometry().topLeft());
    auto sceneBottomRight = mapToScene(viewport()->geometry().bottomRight());

    QRectF rect(sceneTopLeft, sceneBottomRight);
    double scale = rect.width() / viewport()->rect().width();

    QPen tickPen(Qt::red);
    tickPen.setWidth(scale);

    painter->setPen(tickPen);

    painter->drawLine(ToViewX(tick, decimals), rect.top(),
                      ToViewX(tick, decimals), rect.bottom());
}


Gex::InputRel::FuncDelegatePtr Gex::InputRel::FuncView::Delegate() const
{
    return delegate;
}


Gex::InputRel::FuncViewWidget::~FuncViewWidget()
{
        QObject::disconnect(view, &FuncView::PointsSelected,
                            this, &FuncViewWidget::OnPointSelected);
}


Gex::InputRel::FuncViewWidget::FuncViewWidget(QWidget* parent):
    QWidget(parent)
{
    auto* layout = new QVBoxLayout();
    setLayout(layout);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignLeft);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(buttonLayout);

    scene = new FuncScene(ToViewLength(-10000000, decimals),
                          ToViewLength(-10000000, decimals),
                          ToViewLength(20000000, decimals),
                          ToViewLength(20000000, decimals),
                          this);

    view = new FuncView(scene, this, false);
    layout->addWidget(view);

    auto cb = [this](const Gex::Ui::UiFeedback& f)
    {
        this->view->ShowMessage(f);
    };

    scene->AddFeedbackCallback(cb);

    QObject::connect(view, &FuncView::PointsSelected,
                     this, &FuncViewWidget::OnPointSelected);

    addPointBtn = new QPushButton(this);
    addPointBtn->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(
            fa::fa_solid, fa::fa_add));
    buttonLayout->addWidget(addPointBtn);

    QObject::connect(addPointBtn, &QPushButton::clicked,
                     view, &FuncView::ActivateCreateMode);

    delPointBtn = new QPushButton(this);
    delPointBtn->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(
            fa::fa_solid, fa::fa_minus));
    buttonLayout->addWidget(delPointBtn);

    auto* xLabel = new QLabel(this);
    xLabel->setText("x :");
    buttonLayout->addWidget(xLabel);

    xValue = new QDoubleSpinBox(this);
    xValue->setMinimum(-99999999);
    xValue->setMaximum(99999999);
    xValue->setButtonSymbols(QDoubleSpinBox::NoButtons);
    buttonLayout->addWidget(xValue);

    auto* yLabel = new QLabel(this);
    yLabel->setText("y :");
    buttonLayout->addWidget(yLabel);

    yValue = new QDoubleSpinBox(this);
    yValue->setMinimum(-99999999);
    yValue->setMaximum(99999999);
    yValue->setButtonSymbols(QDoubleSpinBox::NoButtons);
    buttonLayout->addWidget(yValue);
}


void Gex::InputRel::FuncViewWidget::SetFunction(const std::any& func)
{
    view->SetFunction(func);
}


void Gex::InputRel::FuncViewWidget::OnPointSelected(
        const QList<FuncViewerHandlePtr>& pnts)
{
    for (const FuncViewerHandlePtr pnt : pnts)
    {
        if (!pnt)
            continue;

        xValue->setValue(pnt->Point()->GetX());
        yValue->setValue(pnt->Point()->GetY());
    }

    editing = pnts;
}


double Gex::InputRel::FuncViewWidget::Tick() const
{
    return view->Tick();
}

void Gex::InputRel::FuncViewWidget::SetTick(double t)
{
    view->SetTick(t);
}


void Gex::InputRel::FuncViewWidget::showEvent(QShowEvent* event)
{
    if (!initialized)
    {
        view->AutoFrame();
        initialized = true;
    }

    QWidget::showEvent(event);
}


void Gex::InputRel::FuncViewWidget::EditPoints(double y)
{
    bool success = true;
    std::string msg = "Failed editing points :\n";
    for (auto& pnt : editing)
    {
        bool r = view->Delegate()->EditPoint(
                pnt->Point(), pnt->pos(),
                QPointF(pnt->pos().x(),
                        ToViewY(y, decimals)
                        )
                );

        if (!r)
        {
            auto p = pnt->Point();
            msg += "Point (" + std::to_string(p->GetX()) + ", "
                    + std::to_string(p->GetY()) +
                    ") could not be edited.";
            success = false;
        }
    }

    if (!success)
    {
        auto fdbk = Gex::Ui::MakeFeedback(
                Gex::Status::Warning, msg);

        view->ShowMessage(fdbk);
    }
}
