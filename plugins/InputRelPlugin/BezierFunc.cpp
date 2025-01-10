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


#define LHX "lhx"
#define LHY "lhy"
#define RHX "rhx"
#define RHY "rhy"


void Gex::InputRel::BezierPoint::Serialize(
        rapidjson::Value& value,
        rapidjson::Document& json)
        const
{
    CurvePoint::Serialize(value, json);

    rapidjson::Value& lhx = rapidjson::Value().SetDouble(LeftHandle()->hx);
    value.AddMember(LHX, lhx, json.GetAllocator());

    rapidjson::Value& lhy = rapidjson::Value().SetDouble(LeftHandle()->hy);
    value.AddMember(LHY, lhy, json.GetAllocator());

    rapidjson::Value& rhx = rapidjson::Value().SetDouble(RightHandle()->hx);
    value.AddMember(RHX, rhx, json.GetAllocator());

    rapidjson::Value& rhy = rapidjson::Value().SetDouble(RightHandle()->hy);
    value.AddMember(RHY, rhy, json.GetAllocator());
}


void Gex::InputRel::BezierPoint::Deserialize(
        rapidjson::Value& value)
{
    CurvePoint::Deserialize(value);

    if (value.HasMember(LHX))
    {
        LeftHandle()->hx = value[LHX].GetDouble();
    }

    if (value.HasMember(LHY))
    {
        LeftHandle()->hy = value[LHY].GetDouble();
    }

    if (value.HasMember(RHX))
    {
        RightHandle()->hx = value[RHX].GetDouble();
    }

    if (value.HasMember(RHY))
    {
        RightHandle()->hy = value[RHY].GetDouble();
    }
}


Gex::InputRel::CurvePointPtr
Gex::InputRel::BezierFunc::CreatePoint(double x, double y)
{
    return std::make_shared<BezierPoint>(x, y, -10, 0, 10, 0, this);
}


Gex::InputRel::BezierFunc::InterpMethod
Gex::InputRel::BezierFunc::Method() const
{
    return method;
}


void Gex::InputRel::BezierFunc::SetMethod(InterpMethod m)
{
    method = m;
}


double Gex::InputRel::BezierFunc::Bezier(
        double a, double b,
        double c, double d,
        double t) const
{
//    return ((a * std::pow(1 - t, 3)) +
//             3 * b * std::pow(1 - t, 2) * t +
//             3 * c * (1 - t) * std::pow(t, 2) +
//             d * std::pow(t, 2));
    return (
            std::pow(1 - t, 3) * a +
            3 * std::pow(1 - t, 2) * t * b +
            3 * (1 - t) * std::pow(t, 2) * c +
            std::pow(t, 3) * d
    );
}


double Gex::InputRel::BezierFunc::BezierX(
        BezierPointPtr previous,
        BezierPointPtr next,
        double t) const
{
    double a = previous->GetX();
    double b = previous->GetX() + previous->RightHandle()->hx;
    double c = next->GetX() + next->LeftHandle()->hx;
    double d = next->GetX();

    if (b > d)
        b = d;

    if (c < a)
        c = a;

    return Bezier(a, b, c, d, t);
}


double Gex::InputRel::BezierFunc::BezierY(
        BezierPointPtr previous,
        BezierPointPtr next,
        double t) const
{
    double a = previous->GetY();
    double b = previous->GetY() + previous->RightHandle()->hy;
    double c = next->GetY() + next->LeftHandle()->hy;
    double d = next->GetY();

//    if (b > d)
//        b = d;
//
//    if (c < a)
//        c = a;

    return Bezier(a, b, c, d, t);
}


#define I_PI 3.14159265359


double Gex::InputRel::BezierFunc::TFromX(
        double a, double b,
        double c, double d,
        double x) const
{
    // Move x to zero.
    a -= x;
    b -= x;
    c -= x;
    d -= x;

    auto accept = [](double t)
    {
        return (t >= 0 && t <= 1);
    };

    auto cuberoot = [](double v){
        if(v<0) return -pow(-v,1/3);
        return pow(v,1/3);
    };

    auto filter = [accept](
            const std::vector<double>& v)
    {
        std::vector<double> l;

        for (double d : v)
        {
            if (accept(d))
            {
                l.push_back(d);
            }
        }

        return l;
    };

    // This solution was taken from https://pomax.github.io/bezierinfo/#extremities
    // Still needs some explanations...
    auto getCubicRoots = [cuberoot, accept, filter](
            double pa, double pb, double pc, double pd)
    {
        double a = (3 * pa - 6 * pb + 3 * pc);
        double b = (-3 * pa + 3 * pb);
        double c = pa;
        double d = (-pa + 3 * pb - 3 * pc + pd);

        // do a check to see whether we even need cubic solving:
        if (d == 0) {
            // this is not a cubic curve.
            if (a == 0) {
                // in fact, this is not a quadratic curve either.
                if (b == 0) {
                    // in fact in fact, there are no solutions.
                    return std::vector<double>();
                }
                // linear solution
                return filter({-c / b});
            }
            // quadratic solution
            double q = std::sqrt(std::pow(b, 2) - 4 * a * c);
            return filter({(q - b) / 2 * a, (-b -q) / 2 * a});
        }

        // at this point, we know we need a cubic solution.
        a /= d;
        b /= d;
        c /= d;

        double p = (3 * b - a * a) / 3;
        double p3 = p / 3;
        double q = (2 * std::pow(a, 3) - 9 * a * b + 27 * c)/27;
        double q2 = q/2;
        double discriminant = std::pow(q2, 2) + std::pow(p3, 3);

        // and some variables we're going to use later on:
        double u1, v1, root1, root2, root3;

        // three possible real roots:
        if (discriminant < 0)
        {
            double mp3  = -p / 3;
            double mp33 = std::pow(mp3, 3);
            double r = sqrt( mp33 );
            double t = -q / (2 * r);

            double cosphi = t < -1 ? -1 : t > 1 ? 1 : t;
            double phi  = std::acos(cosphi);
            double crtr = cuberoot(r);
            double t1   = 2 * crtr;
            root1 = t1 * std::cos(phi / 3) - a / 3;
            root2 = t1 * std::cos((phi + 2 * I_PI) / 3) - a / 3;
            root3 = t1 * std::cos((phi + 4 * I_PI) / 3) - a / 3;

            return filter({root1, root2, root3});
        }

        // three real roots, but two of them are equal:
        if(discriminant == 0) {
            u1 = q2 < 0 ? cuberoot(-q2) : -cuberoot(q2);
            root1 = 2 * u1 - a/3;
            root2 = -u1 - a/3;
            return filter({root1, root2});
        }

        // one real root, two complex roots
        double sd = std::sqrt(discriminant);
        u1 = cuberoot(sd - q2);
        v1 = cuberoot(sd + q2);
        root1 = u1 - v1 - a / 3;
        return filter({root1});
    };

    auto roots = getCubicRoots(a, b, c, d);

    if (roots.empty())
        return -1;

    return 1 - roots[0];
}


double Gex::InputRel::BezierFunc::TIterative(
        double a, double b,
        double c, double d,
        double x, int precision)
        const
{
    auto iter = [this, precision, a, b, c, d, x]
            (double mint, double maxt)
    {
        double interval = maxt - mint;
        double it = interval / static_cast<double>(precision);

        double startT = mint;
        double endT = maxt;
        for (double t = mint; t <= maxt; t += it)
        {
            double dx = Bezier(a, b, c, d, t);
            if (dx == x)
            {
                // return this 1.
                std::array<double, 2> r = {t, t};
                return r;
            }

            if (dx < x && t > startT)
            {
                startT = t;
            }
            else if (dx > x && t < endT)
            {
                endT = t;
            }
        }

        std::array<double, 2> r = {startT, endT};
        return r;
    };

    double st = 0;
    double nd = 1;
    for (int i = 0; i < precision; i++)
    {
        auto arr = iter(st, nd);
        if (arr.at(0) == arr.at(1))
            return arr.at(0);

        st = arr.at(0);
        nd = arr.at(1);
    }

    return st + ((nd - st) / 2);
}


double Gex::InputRel::BezierFunc::Interpolate(
        double x, const CurvePointPtr& previous,
        const CurvePointPtr& next) const
{
    auto prev = std::dynamic_pointer_cast<BezierPoint>(previous);
    auto nxt = std::dynamic_pointer_cast<BezierPoint>(next);

    double a = prev->GetX();
    double d = nxt->GetX();

    double b = prev->GetX() + prev->RightHandle()->hx;
//    if (b > d)
//    {
//        b = d;
//    }

    double c = nxt->GetX() + nxt->LeftHandle()->hx;
//    if (c < a)
//    {
//        c = a;
//    }

    // Find t from given x.
    double t = -1;
    if (method == InterpMethod::CubicRoots)
        t = TFromX(a, b, c, d, x);
    else if (method == InterpMethod::Iterative)
        t = TIterative(a, b, c, d, x);

    if (t == -1)
        return 0;

    return BezierY(prev, nxt, t);
}