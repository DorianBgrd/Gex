#include "boost/python.hpp"

#include "plugins/InputRelPlugin/Func.h"
#include "plugins/InputRelPlugin/BezierFunc.h"


#define DECL_GET(name, func) \
boost::python::object name (boost::python::tuple args,\
                           boost::python::dict kwargs) \
{ \
    Gex::InputRel::CurvePointPtr point = boost::python::extract< \
            Gex::InputRel::CurvePointPtr>(args[0]); \
    \
    return boost::python::object(point->func()); \
}


DECL_GET(CurvePointPtr_GetX, GetX)

DECL_GET(CurvePointPtr_GetY, GetY)


boost::python::object CurvePointPtr_SetX(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    Gex::InputRel::CurvePointPtr point = boost::python::extract<
            Gex::InputRel::CurvePointPtr>(args[0]);

    double x = boost::python::extract<double>(args[1]);

    point->SetX(x);
    return {};
}


boost::python::object CurvePointPtr_SetY(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    Gex::InputRel::CurvePointPtr point = boost::python::extract<
            Gex::InputRel::CurvePointPtr>(args[0]);

    double y = boost::python::extract<double>(args[1]);

    point->SetY(y);
    return {};
}


BOOST_PYTHON_MODULE(InputRel_Python)
{
    boost::python::class_<
            Gex::InputRel::CurvePoint,
            Gex::InputRel::CurvePointPtr
            >("CurvePoint", boost::python::no_init)
//            .def("GetX", boost::python::raw_function(CurvePointPtr_GetX, 1))
//            .def("GetY", boost::python::raw_function(CurvePointPtr_GetY, 1))
//            .def("SetX", boost::python::raw_function(CurvePointPtr_SetX, 2))
//            .def("SetY", boost::python::raw_function(CurvePointPtr_SetY, 2))
            .def("GetX", &Gex::InputRel::CurvePoint::GetX)
            .def("GetY", &Gex::InputRel::CurvePoint::GetY)
            ;

    boost::python::class_<Gex::InputRel::CurvePointWk>
            ("BezierPointWk", boost::python::no_init)
            .def("ToPoint", &Gex::InputRel::CurvePointWk::ToShared)
            ;

    boost::python::class_<
            Gex::InputRel::BezierPoint::BezierHandle,
            Gex::InputRel::BezierPoint::BezierHandlePtr
            >("BezierHandle", boost::python::no_init)
            .add_property("hx", &Gex::InputRel::BezierPoint::BezierHandle::hx)
            .add_property("hy", &Gex::InputRel::BezierPoint::BezierHandle::hy)
            ;

    boost::python::class_<Gex::InputRel::BezierPoint::BezierHandleWk>
            ("BezierHandleWk", boost::python::no_init)
            .def("ToHandle", &Gex::InputRel::BezierPoint::BezierHandleWk::ToShared)
            ;

    boost::python::class_<
            Gex::InputRel::BezierPoint,
            Gex::InputRel::BezierPointPtr,
            boost::python::bases<Gex::InputRel::CurvePoint>
            >("BezierPoint", boost::python::no_init)
            .def("LeftHandle", &Gex::InputRel::BezierPoint::LeftHandle)
            .def("RightHandle", &Gex::InputRel::BezierPoint::RightHandle)
            ;

    void(Gex::InputRel::Func::*AddPointX)(double)  = &Gex::InputRel::Func::AddPoint;
    void(Gex::InputRel::Func::*AddPointXY)(double, double)  = &Gex::InputRel::Func::AddPoint;
    bool(Gex::InputRel::Func::*EditPointXYNY)(double, double, double)  = &Gex::InputRel::Func::EditPoint;

    boost::python::class_<Gex::InputRel::Func
            >("Func", boost::python::no_init)
            .def("GetValue", &Gex::InputRel::Func::GetValue)
            .def("GetPoint", &Gex::InputRel::Func::GetPoint)
            .def("AddPoint", AddPointX)
            .def("AddPoint", AddPointXY)
            .def("EditPoint", EditPointXYNY)
            .def("Interpolate", boost::python::pure_virtual(&Gex::InputRel::Func::Interpolate))
            ;

    boost::python::class_<Gex::InputRel::LinearFunc,
            Gex::InputRel::LinearFuncPtr,
            boost::python::bases<Gex::InputRel::Func>>
            ("LinearFunc", boost::python::no_init)
            ;

    boost::python::class_<Gex::InputRel::BezierFunc,
            Gex::InputRel::BezierFuncPtr,
            boost::python::bases<Gex::InputRel::Func>>
            ("BezierFuncPtr", boost::python::no_init)
            ;
}
