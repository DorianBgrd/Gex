#include "BezierFunc.h"
#include <cmath>
#include <algorithm>


Gex::InputRel::Point::Point(double x_, double y_)
{
    x = x_;
    y = y_;
}


void Gex::InputRel::Point::operator=(Point other)
{
    Swap(other);
}


void Gex::InputRel::Point::Swap(const Point& other)
{
    x = other.x;
    y = other.y;
}


Gex::InputRel::Point Gex::InputRel::operator*(double f,
        const Gex::InputRel::Point& p)
{
    return {f * p.GetX(), f* p.GetY()};
}


Gex::InputRel::Point Gex::InputRel::operator+(
        const Gex::InputRel::Point& p1,
        const Gex::InputRel::Point& p2)
{
    return {p1.GetX() + p2.GetX(),  p1.GetY() + p2.GetY()};
}


bool Gex::InputRel::operator==(const Gex::InputRel::Point& p1,
        const Gex::InputRel::Point& p2)
{
    return (p1.GetX() == p2.GetX() && p1.GetY() == p2.GetY());
}


double Gex::InputRel::Point::GetX() const
{
    return x;
}


void Gex::InputRel::Point::SetX(double x_)
{
    x = x_;
}


double Gex::InputRel::Point::GetY() const
{
    return y;
}


void Gex::InputRel::Point::SetY(double y_)
{
    y = y_;
}


void Gex::InputRel::Point::Move(double dx, double dy)
{
    x += dx;
    y += dy;
}


Gex::InputRel::PointPtr Gex::InputRel::Point::NewPoint(double x, double y)
{
    return std::make_shared<Point>(x, y);
}






Gex::InputRel::BezierPoint::BezierPoint(
        double x_, double y_, double lx_, double ly_,
        double rx_, double ry_): Point(x_, y_)
{
    leftHandle = NewPoint(lx_, ly_);
    rightHandle = NewPoint(rx_, ry_);
}


Gex::InputRel::PointRef Gex::InputRel::BezierPoint::LeftHandle() const
{
    return leftHandle;
}


Gex::InputRel::PointRef Gex::InputRel::BezierPoint::RightHandle() const
{
    return rightHandle;
}


Gex::InputRel::BezierPointPtr Gex::InputRel::BezierPoint::NewBezierPoint(
        double x, double y, double lx, double ly,
        double rx, double ry)
{
    return std::make_shared<BezierPoint>(x, y, lx, ly, rx, ry);
}



Gex::InputRel::BezierFunc::BezierFunc(): Func()
{
    AddPoint(0);
    AddPoint(100);
}


Gex::InputRel::BezierPoints Gex::InputRel::BezierFunc::CurvePoints() const
{
    return points;
}


Gex::InputRel::BezierPointRefs Gex::InputRel::BezierFunc::CurvePointRefs() const
{
    return {points.begin(), points.end()};
}


class PointFinder
{
    double searchx;

public:
    PointFinder(double x)
    {
        searchx = x;
    };

    bool operator()(const Gex::InputRel::BezierPointPtr& point) const
    {
        return point->GetX() == searchx;
    }
};


Gex::InputRel::BezierPointRef Gex::InputRel::BezierFunc::PointAt(double x)
{
    auto found = std::find_if(points.begin(), points.end(), PointFinder(x));
    if (found == points.end())
        return {};

    return *found;
}


//Gex::InputRel::ControlPoints Gex::InputRel::BezierCurve::ControlPoints(
//        const Point& point) const
//{
//    return points.at(point);
//}


Gex::InputRel::BezierPointRef Gex::InputRel::BezierFunc::AddPoint(double x)
{
    double value = y(x);

    bool hasPrevious = false;
    bool hasNext = false;
    auto adjacent = AdjacentPoints(x, hasPrevious, hasNext);

    double lx = x - 1;
    double ly = value;
    double rx = x + 1;
    double ry = value;

    BezierPointPtr previous;
    BezierPointPtr next;

    auto iter = adjacent.begin();
    if (hasPrevious)
    {
        previous = *iter;
        lx = previous->GetX() + (x - previous->GetX()) / 2;

        std::advance(iter, 1);
    }
    if (hasNext)
    {
        next = *iter;
        rx = x + (next->GetX() - x) / 2;
    }

    auto newPoint = BezierPoint::NewBezierPoint(x, value, lx, ly, rx, ry);
    points.insert(newPoint);

    return newPoint;
}


void Gex::InputRel::BezierFunc::RemovePoint(double x)
{
    auto p = PointAt(x);
    if (p.expired())
        return;

    points.erase(p.lock());
}


Gex::InputRel::BezierPoints Gex::InputRel::BezierFunc::AdjacentPoints(
        const double& x, bool& foundPrev, bool& foundPost) const
{

    BezierPointPtr prev;
    double pvd = 1;

    BezierPointPtr post;
    double psd = -1;

    for (auto p : points)
    {
        double distx = x - p->GetX();
        if (distx < 0 && pvd < 0 && distx > pvd)
        {
            pvd = distx;
            prev = p;
        }
        else if (distx > 0 && psd > 1 && distx < psd)
        {
            psd = distx;
            post = p;
        }
        else if (distx == 0)
        {
            pvd = distx;
            prev = p;
        }
    }

    foundPrev = false;
    foundPost = false;

    BezierPoints adjacent;
    if (pvd <= 0)
    {
        adjacent.insert(prev);
        foundPrev = true;
    }
    if (psd >= 0)
    {
        adjacent.insert(post);
        foundPost = true;
    }

    return adjacent;
}


double Gex::InputRel::BezierFunc::y(double x) const
{
    bool previous = false;
    bool next = false;
    BezierPoints adjacent = AdjacentPoints(x, previous, next);

    if (adjacent.empty())
    {
        return 0;
    }

    if (adjacent.size() == 1)
    {
        return (*adjacent.begin())->GetY();
    }
    //        |
    // x -- y -- x -- y -- x -- y -- x

    auto iter = adjacent.begin();
    BezierPointPtr previousPoint = (*iter);

    std::advance(iter, 1);
    BezierPointPtr nextPoint = (*iter);

    PointRef leftHandle = previousPoint->RightHandle();
    PointRef rightHandle = nextPoint->LeftHandle();

    double u = x / (nextPoint->GetX() - previousPoint->GetY());

    return (std::pow(1-u, 3) * previousPoint->GetY() +
            u * leftHandle.lock()->GetY() * (3 * std::pow(1-u, 2)) +
            rightHandle.lock()->GetY() * (3 * (1-u) * std::pow(u, 2)) +
            nextPoint->GetY() * std::pow(u, 3));
}


#define POINTS_K "points"

bool Gex::InputRel::BezierFunc::Serialize(rapidjson::Value& dict,
               rapidjson::Document& json) const
{
    rapidjson::Value& k = rapidjson::Value().SetString(
            POINTS_K, json.GetAllocator());

    rapidjson::Value& l = rapidjson::Value().SetArray();
    for (auto p : points)
    {
        rapidjson::Value& pa = rapidjson::Value().SetArray();
        pa.PushBack(p->GetX(), json.GetAllocator());
        pa.PushBack(p->GetY(), json.GetAllocator());
        pa.PushBack(p->LeftHandle().lock()->GetX(), json.GetAllocator());
        pa.PushBack(p->LeftHandle().lock()->GetY(), json.GetAllocator());
        pa.PushBack(p->RightHandle().lock()->GetX(), json.GetAllocator());
        pa.PushBack(p->RightHandle().lock()->GetY(), json.GetAllocator());

        l.PushBack(pa.Move(), json.GetAllocator());
    }

    dict.AddMember(k.Move(), l.Move(), json.GetAllocator());
    return true;
}


bool Gex::InputRel::BezierFunc::Deserialize(rapidjson::Value& dict)
{
    if (dict.HasMember(POINTS_K))
    {
        rapidjson::Value& l = dict[POINTS_K];

        double x = l[0].GetDouble();
        double y = l[1].GetDouble();
        double lx = l[2].GetDouble();
        double ly = l[3].GetDouble();
        double rx = l[4].GetDouble();
        double ry = l[5].GetDouble();

        auto p = AddPoint(x).lock();
        p->SetX(x);
        p->SetY(y);

        auto lh = p->LeftHandle().lock();
        lh->SetX(lx);
        lh->SetY(ly);

        auto rh = p->RightHandle().lock();
        rh->SetX(rx);
        rh->SetY(ry);

        return true;
    }

    return false;
}


