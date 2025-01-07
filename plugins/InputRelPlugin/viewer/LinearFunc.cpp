#include "LinearFunc.h"


QRectF Gex::InputRel::LinearFuncHandle::boundingRect() const
{
    return QRect(-4, -4, 8, 8);
}


void Gex::InputRel::LinearFuncHandle::paintHandle(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    painter->setBrush(Qt::yellow);

    painter->setPen(Qt::NoPen);
    if (option->state & QStyle::State_Selected)
    {
        painter->setBrush(Qt::white);
    }

    painter->drawRect(boundingRect());
}


void Gex::InputRel::LinearFuncDelegate::AddPoint(double x, double y)
{
    func->AddPoint(x, y);
}


void Gex::InputRel::LinearFuncDelegate::InitFunction(
        const std::any& f)
{
    func = std::any_cast<LinearFuncPtr>(f);
}


Gex::InputRel::PointsSet Gex::InputRel::LinearFuncDelegate::GetPoints() const
{
    return func->Points();
}


Gex::InputRel::FuncViewerHandle* Gex::InputRel::LinearFuncDelegate::CreateHandle(
        const CurvePointWk& p, const FuncDelegatePtr& d, FuncScene* scene)
{
    return new LinearFuncHandle(p, d, scene);
}


bool Gex::InputRel::LinearFuncDelegate::EditPoint(
        const CurvePointWk& point,
        const QPointF& previous,
        const QPointF& next,
        int decimals
)
{
    PointDelta delta;
    delta.SetDeltaX(FromViewX(next.x() - previous.x(), decimals));
    delta.SetDeltaY(FromViewY(next.y() - previous.y(), decimals));

    return func->EditPoint(point->GetX(), &delta);
}


void Gex::InputRel::LinearFuncDelegate::DrawCurve(
        QPainterPath& path,
        FuncScene* scene,
        int decimals
) const
{
    bool init = false;
    for (const auto& point : func->Points())
    {
        QPointF qp(ToViewX(point->GetX(), decimals),
                   ToViewY(point->GetY(), decimals));
        if (init)
            path.lineTo(qp);
        else
            init = true;

        path.moveTo(qp);
    }
}