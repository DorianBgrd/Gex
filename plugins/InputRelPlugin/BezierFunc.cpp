#include "BezierFunc.h"


Gex::InputRel::BezierPoint::BezierPoint(): CurvePoint()
{
    leftHandle = std::make_shared<BezierHandle>();
    rightHandle = std::make_shared<BezierHandle>();
}


Gex::InputRel::BezierPoint::BezierPoint(Func* f): CurvePoint(f)
{
    leftHandle = std::make_shared<BezierHandle>();
    rightHandle = std::make_shared<BezierHandle>();
}


Gex::InputRel::BezierPoint::BezierPoint(
        double x, double y,
        double lhx, double lhy,
        double rhx, double rhy,
        Func* f
): CurvePoint(x, y, f)
{
    leftHandle = std::make_shared<BezierHandle>(lhx, lhy);
    rightHandle = std::make_shared<BezierHandle>(rhx, rhy);
}


Gex::InputRel::BezierPoint::BezierHandleWk Gex::InputRel::BezierPoint::LeftHandle() const
{
    return leftHandle;
}


Gex::InputRel::BezierPoint::BezierHandleWk Gex::InputRel::BezierPoint::RightHandle() const
{
    return rightHandle;
}


Gex::InputRel::CurvePointPtr
Gex::InputRel::BezierFunc::CreatePoint(double x, double y)
{
    return std::make_shared<BezierPoint>(x, y, -10, 0, 10, 0, this);
}


double Gex::InputRel::BezierFunc::Interpolate(
        double x, const CurvePointPtr& previous,
        const CurvePointPtr& next) const
{
    return previous->GetY();
}