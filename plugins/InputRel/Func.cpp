#include "Func.h"
#include <cmath>


bool Gex::InputRel::Func::registered = false;


double Gex::InputRel::Func::operator()(double x) const
{
    return y(x);
}


bool Gex::InputRel::Func::RegisterPythonWrapper()
{
    if (registered)
        return false;

//    boost::python::class_<Gex::InputRel::Func>("AbstractFunc", boost::python::no_init)
//            .def("y", &Gex::InputRel::Func::y)
//            ;
    registered = true;
    return true;
}



Gex::InputRel::LinearFunc::LinearFunc(
        double x0_, double y0_,
        double x1_, double y1_,
        double xmin_, double xmax_,
        double ymin_, double ymax_)
{
    x0 = x0_;
    y0 = y0_;
    x1 = x1_;
    y1 = y1_;
    xmin = xmin_;
    xmax = xmax_;
    ymin = ymin_;
    ymax = ymax_;

    ResolveEquation();
}


void Gex::InputRel::LinearFunc::Get(double& x0_, double& y0_,
                                    double& x1_, double& y1_)
                                    const

{
    x0_ = x0;
    y0_ = y0;
    x1_ = x1;
    y1_ = y1;
}

void Gex::InputRel::LinearFunc::Set(double x0_, double y0_,
                                    double x1_, double y1_)
{
    x0 = x0_;
    x1 = x1_;
    y0 = y0_;
    y1 = y1_;
}


double Gex::InputRel::LinearFunc::MinX() const
{
    return xmin;
}


double Gex::InputRel::LinearFunc::MaxX() const
{
    return xmax;
}


double Gex::InputRel::LinearFunc::MinY() const
{
    return ymin;
}


double Gex::InputRel::LinearFunc::MaxY() const
{
    return ymax;
}


void Gex::InputRel::LinearFunc::ResolveEquation()
{
    a = (y1 - y0) / (x1 - x0);
    b = y0 - a * x0;
}


double Gex::InputRel::LinearFunc::y(double x) const
{
    double v = (a * x) + b;

    if (v < ymin)
        return ymin;
    else if (v > ymax)
        return ymax;

    return v;
}


#define X0 "x0"
#define Y0 "y0"
#define X1 "x1"
#define Y1 "y1"
#define X_MIN "x_min"
#define X_MAX "x_max"
#define Y_MIN "y_min"
#define Y_MAX "y_max"


bool Gex::InputRel::LinearFunc::Serialize(
        rapidjson::Value& dict,
        rapidjson::Document& json)
        const
{
    rapidjson::Value& x0k = rapidjson::Value().SetString(X0, json.GetAllocator());
    rapidjson::Value& x0v = rapidjson::Value().SetDouble(x0);
    dict.AddMember(x0k.Move(), x0v.Move(), json.GetAllocator());

    rapidjson::Value& x1k = rapidjson::Value().SetString(X1, json.GetAllocator());
    rapidjson::Value& x1v = rapidjson::Value().SetDouble(x1);
    dict.AddMember(x1k.Move(), x1v.Move(), json.GetAllocator());

    rapidjson::Value& y0k = rapidjson::Value().SetString(Y0, json.GetAllocator());
    rapidjson::Value& y0v = rapidjson::Value().SetDouble(y0);
    dict.AddMember(y0k.Move(), y0v.Move(), json.GetAllocator());

    rapidjson::Value& y1k = rapidjson::Value().SetString(X1, json.GetAllocator());
    rapidjson::Value& y1v = rapidjson::Value().SetDouble(x1);
    dict.AddMember(y1k.Move(), y1v.Move(), json.GetAllocator());

    rapidjson::Value& mink = rapidjson::Value().SetString(X_MIN, json.GetAllocator());
    rapidjson::Value& minv = rapidjson::Value().SetDouble(xmin);
    dict.AddMember(mink.Move(), minv.Move(), json.GetAllocator());

    rapidjson::Value& maxk = rapidjson::Value().SetString(X_MAX, json.GetAllocator());
    rapidjson::Value& maxv = rapidjson::Value().SetDouble(xmax);
    dict.AddMember(maxk.Move(), maxv.Move(), json.GetAllocator());

    rapidjson::Value& ymink = rapidjson::Value().SetString(Y_MIN, json.GetAllocator());
    rapidjson::Value& yminv = rapidjson::Value().SetDouble(ymin);
    dict.AddMember(ymink.Move(), yminv.Move(), json.GetAllocator());

    rapidjson::Value& ymaxk = rapidjson::Value().SetString(Y_MAX, json.GetAllocator());
    rapidjson::Value& ymaxv = rapidjson::Value().SetDouble(ymax);
    dict.AddMember(ymaxk.Move(), ymaxv.Move(), json.GetAllocator());

    return true;
}


bool Gex::InputRel::LinearFunc::Deserialize(rapidjson::Value& dict)
{
    if (!dict.IsObject())
        return false;

    if (dict.HasMember(X0))
        x0 = dict[X0].GetDouble();
    if (dict.HasMember(Y0))
        y0 = dict[Y0].GetDouble();
    if (dict.HasMember(X1))
        x1 = dict[X1].GetDouble();
    if (dict.HasMember(Y1))
        y1 = dict[Y1].GetDouble();
    if (dict.HasMember(X_MIN))
        xmin = dict[X_MIN].GetDouble();
    if (dict.HasMember(X_MAX))
        xmax = dict[X_MAX].GetDouble();
    if (dict.HasMember(Y_MIN))
        ymin = dict[Y_MIN].GetDouble();
    if (dict.HasMember(Y_MAX))
        ymax = dict[Y_MAX].GetDouble();

    return true;
}


bool Gex::InputRel::LinearFunc::registered = false;


boost::python::object Lf_Get(boost::python::tuple args,
                             boost::python::dict kwargs)
{
    boost::python::list l;

    Gex::InputRel::LinearFunc* lfunc = boost::python::extract<
            Gex::InputRel::LinearFunc*>(args[0]);

    double x0, x1, y0, y1;

    lfunc->Get(x0, y0, x1, y1);

    l.append(x0);
    l.append(x1);
    l.append(y0);
    l.append(y1);

    return l;
}


bool Gex::InputRel::LinearFunc::RegisterPythonWrapper()
{
    if (registered)
        return false;

    boost::python::class_<Gex::InputRel::LinearFunc>(
            "LinearFunc", boost::python::init())
            .def("y", &Gex::InputRel::Func::y)
            .def("Get", boost::python::raw_function(&Lf_Get, 0))
            ;

    registered = true;
    return true;
}
