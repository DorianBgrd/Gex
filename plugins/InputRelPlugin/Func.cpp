#include "Func.h"
#include <cmath>


Gex::InputRel::CurvePoint::CurvePoint(Func* f)
{
    x = 0;
    y = 0;
    func = f;
}


Gex::InputRel::CurvePoint::CurvePoint(
        double px, double py, Func* f)
{
    x = px;
    y = py;
    func = f;
}


double Gex::InputRel::CurvePoint::GetX() const
{
    return x;
}


double Gex::InputRel::CurvePoint::GetY() const
{
    return y;
}


void Gex::InputRel::CurvePoint::SetX(double nx)
{
    x = nx;
    RefreshFunc();
}


void Gex::InputRel::CurvePoint::SetY(double ny)
{
    y = ny;
}


Gex::InputRel::Func* Gex::InputRel::CurvePoint::GetFunc() const
{
    return func;
}


void Gex::InputRel::CurvePoint::SetFunc(Func* f)
{
    func = f;
}


Gex::InputRel::CurvePoint Gex::InputRel::CurvePoint::Move(double mx, double my) const
{
    return {x + mx, y + my, func};
}


bool Gex::InputRel::CurvePoint::operator <(const CurvePoint& other) const
{
    return x < other.GetX();
}


bool Gex::InputRel::CurvePoint::operator >(const CurvePoint& other) const
{
    return x > other.GetX();
}


Gex::InputRel::CurvePoint Gex::InputRel::CurvePoint::operator +(const CurvePoint& other) const
{
    return Move(other.x, other.y);
}


#define X_KEY "x"
#define Y_KEY "y"


void Gex::InputRel::CurvePoint::Serialize(
        rapidjson::Value& value,
        rapidjson::Document& json) const
{
    auto& dict = value.SetObject();

    dict.AddMember(rapidjson::StringRef(X_KEY), x,
                   json.GetAllocator());

    dict.AddMember(rapidjson::StringRef(Y_KEY), y,
                   json.GetAllocator());
}


void Gex::InputRel::CurvePoint::Deserialize(
        rapidjson::Value& value)
{
    x = value[X_KEY].GetDouble();
    y = value[Y_KEY].GetDouble();
}


void Gex::InputRel::CurvePoint::Edit(
        const PointDelta* delta)
{

    delta->EditPoint(this);

    RefreshFunc();
}


void Gex::InputRel::CurvePoint::RefreshFunc()
{
    if (!func)
        return;

    func->Refresh(shared_from_this());
}


bool Gex::InputRel::PointCmp::operator()(
        const CurvePointPtr &l,
        const CurvePointPtr &r)
        const
{
    bool res = l->GetX() < r->GetX();
    return res;
}


double Gex::InputRel::PointDelta::DeltaX() const
{
    return x;
}


void Gex::InputRel::PointDelta::SetDeltaX(double dx)
{
    x = dx;
}


double Gex::InputRel::PointDelta::DeltaY() const
{
    return y;
}


void Gex::InputRel::PointDelta::SetDeltaY(double dy)
{
    y = dy;
}


void Gex::InputRel::PointDelta::Apply(
        Func* f, const CurvePointPtr& point)
        const
{
    EditPoint(point.get());

    f->Refresh(point);
}


void Gex::InputRel::PointDelta::EditPoint(
        CurvePoint* point) const
{
    point->SetX(point->GetX() + x);
    point->SetY(point->GetY() + y);
}


bool Gex::InputRel::Func::registered = false;


struct PointSearch
{
private:
    double p;
    bool n;

public:
    PointSearch(double x, bool next=false)
    {
        p = x;
        n = next;
    }

    bool operator()(const Gex::InputRel::CurvePointPtr& cp)
    {
        if (cp->GetX() == p)
            return true;

        if (n)
        {
            return (p <= cp->GetX());
        }

        return (cp->GetX() <= p);
    }
};


Gex::InputRel::CurvePointPtr Gex::InputRel::Func::PreviousPoint(double x) const
{
    PointSearch search(x);

    auto iter = std::find_if(points.begin(), points.end(), search);

    if (iter == points.end())
    {
        return *(points.begin());
    }

    return *iter;
}

Gex::InputRel::CurvePointPtr Gex::InputRel::Func::NextPoint(double x) const
{
    PointSearch search(x, true);

    auto iter = std::find_if(points.rbegin(), points.rend(), search);

    if (iter == points.rend())
    {
        return *(--iter);
    }

    return *iter;
}


double Gex::InputRel::Func::GetValue(double x) const
{
    if (points.empty())
        return 0;

    auto prev = PreviousPoint(x);
    auto next = NextPoint(x);

    if (prev == next)
    {
        return prev->GetY();
    }

    return Interpolate(x, prev, next);
}


Gex::InputRel::CurvePointPtr Gex::InputRel::Func::CreatePoint(double x, double y)
{
    return std::make_shared<CurvePoint>(x, y, this);
}


void Gex::InputRel::Func::AddPoint(double x, double y)
{
    points.insert(CreatePoint(x, y));
}


void Gex::InputRel::Func::AddPoint(double x)
{
    points.insert(CreatePoint(x, GetValue(x)));
}


void Gex::InputRel::Func::AddPoint(const CurvePointPtr& pnt)
{
    points.insert(pnt);
}


void Gex::InputRel::Func::AddPoint(const CurvePointWk& pnt)
{
    if (!pnt)
        return;

    points.insert(pnt.ToShared());
}


Gex::InputRel::CurvePointPtr Gex::InputRel::Func::GetPoint(double x) const
{
    auto search = [x](const CurvePointPtr& p)
    {
        return p->GetX() == x;
    };

    auto p = std::find_if(
            points.begin(), points.end(),
            search);

    if (p == points.end())
        return {};

    return (*p);
}


bool Gex::InputRel::Func::EditPoint(
        double x, double nx, double ny
)
{
    auto point = TakePoint(x);
    if (!point)
        return false;

    point->SetX(nx);
    point->SetY(ny);

    AddPoint(point);
    return true;
}


bool Gex::InputRel::Func::EditPoint(
        double x, const PointDelta* delta
)
{
    auto point = GetPoint(x);
    if (!point)
        return false;

    return EditPoint(point, delta);
}


bool Gex::InputRel::Func::EditPoint(
        const CurvePointWk& point,
        const PointDelta* delta
)
{
    auto locked = point.ToShared();

    auto fcmp = [locked](const CurvePointPtr& p)
    {
        return locked == p;
    };

    auto iter = std::find_if(points.begin(), points.end(),fcmp);
    if (iter == points.end())
        return false;

    points.erase(iter);

    delta->Apply(this, locked);
    AddPoint(point);
    return true;
}


bool Gex::InputRel::Func::RemovePoint(double x)
{
    auto eq = [x](const CurvePointPtr& p) {return p->GetX() == x;};
    auto iter = std::find_if(points.begin(), points.end(), eq);

    if (iter == points.end())
        return false;

    points.erase(iter);
    return true;
}


Gex::InputRel::CurvePointPtr Gex::InputRel::Func::TakePoint(double x)
{
    auto eq = [x](const CurvePointPtr& p) {return p->GetX() == x;};
    auto iter = std::find_if(points.begin(), points.end(), eq);

    if (iter == points.end())
        return {};

    CurvePointPtr r = (*iter);
    points.erase(iter);
    return r;
}


Gex::InputRel::PointsSet Gex::InputRel::Func::Points() const
{
    return points;
}


#define POINTS_KEY "points"


void Gex::InputRel::Func::Serialize(
        rapidjson::Value& dict,
        rapidjson::Document& json)
        const
{

    rapidjson::Value& pointList = rapidjson::Value().SetArray();
    for (const CurvePointPtr& point : points)
    {
        rapidjson::Value p;

        point->Serialize(p, json);

        pointList.PushBack(p.Move(), json.GetAllocator());
    }

    dict.AddMember(rapidjson::StringRef(POINTS_KEY),
                   pointList, json.GetAllocator());
}


void Gex::InputRel::Func::Deserialize(rapidjson::Value& dict)
{
    auto& pntList = dict[POINTS_KEY];

    for (unsigned int i = 0; i < pntList.Size(); i++)
    {
        auto& p = pntList[i];

        auto point = CreatePoint(0, 0);

        point->Deserialize(p);

        points.insert(point);
    }
}


bool Gex::InputRel::Func::Refresh(const CurvePointPtr& pnt)
{
    auto iter = std::find(points.begin(), points.end(), pnt);
    if (iter == points.end())
        return false;

    auto insrt = *iter;

    points.erase(iter);
    AddPoint(insrt);
    return true;
}


bool Gex::InputRel::Func::RegisterPythonWrapper()
{
    if (registered)
        return false;

//    boost::python::class_<Gex::InputRel::CurvePoint>("CurvePoint", boost::python::no_init)
//            .def("y", &Gex::InputRel::Func::y)
//            ;
//
//    boost::python::class_<Gex::InputRel::Func>("Func", boost::python::no_init)
//            .def("y", &Gex::InputRel::Func::y)
//            ;

    registered = true;
    return true;
}


double Gex::InputRel::LinearFunc::Interpolate(
        double x, const CurvePointPtr& previous,
        const CurvePointPtr& next
        ) const
{
    double delta = next->GetY() - previous->GetY();
    double distance = next->GetX() - previous->GetX();

    if (distance == 0)
        return previous->GetY();

    double coeff = (x - previous->GetX()) / distance;

    return (previous->GetY() + coeff * delta);
}

