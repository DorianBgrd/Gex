#include "pybind11/pybind11.h"

#include "plugins/InputRelPlugin/Func.h"
#include "plugins/InputRelPlugin/BezierFunc.h"


#define DECL_GET(name, func) \
pybind11::object name (pybind11::tuple args,\
                       pybind11::dict kwargs) \
{ \
    Gex::InputRel::CurvePointPtr point = args[0].cast< \
            Gex::InputRel::CurvePointPtr>(); \
    \
    return pybind11::cast(point->func()); \
}


DECL_GET(CurvePointPtr_GetX, GetX)

DECL_GET(CurvePointPtr_GetY, GetY)


pybind11::object CurvePointPtr_SetX(
        pybind11::tuple args,
        pybind11::dict kwargs
)
{
    Gex::InputRel::CurvePointPtr point = args[0].cast<
            Gex::InputRel::CurvePointPtr>();

    double x = args[1].cast<double>();

    point->SetX(x);
    return {};
}


pybind11::object CurvePointPtr_SetY(
        pybind11::args args,
        pybind11::kwargs kwargs
)
{
    Gex::InputRel::CurvePointPtr point = args[0].cast<
            Gex::InputRel::CurvePointPtr>();

    double y = args[1].cast<double>();

    point->SetY(y);
    return {};
}


PYBIND11_MODULE(InputRel_Python, mod)
{
    pybind11::class_<
            Gex::InputRel::CurvePoint,
            Gex::InputRel::CurvePointPtr
            >(mod, "CurvePoint")
//            .def("GetX", boost::python::raw_function(CurvePointPtr_GetX, 1))
//            .def("GetY", boost::python::raw_function(CurvePointPtr_GetY, 1))
//            .def("SetX", boost::python::raw_function(CurvePointPtr_SetX, 2))
//            .def("SetY", boost::python::raw_function(CurvePointPtr_SetY, 2))
            .def("GetX", &Gex::InputRel::CurvePoint::GetX)
            .def("GetY", &Gex::InputRel::CurvePoint::GetY)
            ;

    pybind11::class_<Gex::InputRel::CurvePointWk>
            (mod, "BezierPointWk")
            .def("ToPoint", &Gex::InputRel::CurvePointWk::ToShared)
            ;

    pybind11::class_<
            Gex::InputRel::BezierPoint::BezierHandle,
            Gex::InputRel::BezierPoint::BezierHandlePtr
            >(mod, "BezierHandle")
            .def_readwrite("hx", &Gex::InputRel::BezierPoint::BezierHandle::hx)
            .def_readwrite("hy", &Gex::InputRel::BezierPoint::BezierHandle::hy)
            ;

    pybind11::class_<Gex::InputRel::BezierPoint::BezierHandleWk>
            (mod, "BezierHandleWk")
            .def("ToHandle", &Gex::InputRel::BezierPoint::BezierHandleWk::ToShared)
            ;

    pybind11::class_<
            Gex::InputRel::BezierPoint,
            Gex::InputRel::BezierPointPtr,
            Gex::InputRel::CurvePoint
            >(mod, "BezierPoint")
            .def("LeftHandle", &Gex::InputRel::BezierPoint::LeftHandle)
            .def("RightHandle", &Gex::InputRel::BezierPoint::RightHandle)
            ;

    void(Gex::InputRel::Func::*AddPointX)(double)  = &Gex::InputRel::Func::AddPoint;
    void(Gex::InputRel::Func::*AddPointXY)(double, double)  = &Gex::InputRel::Func::AddPoint;
    bool(Gex::InputRel::Func::*EditPointXYNY)(double, double, double)  = &Gex::InputRel::Func::EditPoint;

    pybind11::class_<Gex::InputRel::Func
            >(mod, "Func")
            .def("GetValue", &Gex::InputRel::Func::GetValue)
            .def("GetPoint", &Gex::InputRel::Func::GetPoint)
            .def("AddPoint", AddPointX)
            .def("AddPoint", AddPointXY)
            .def("EditPoint", EditPointXYNY)
            .def("Interpolate", &Gex::InputRel::Func::Interpolate)
            ;

    pybind11::class_<Gex::InputRel::LinearFunc,
            Gex::InputRel::LinearFuncPtr,
            Gex::InputRel::Func>
            (mod, "LinearFunc")
            ;

    pybind11::class_<Gex::InputRel::BezierFunc,
            Gex::InputRel::BezierFuncPtr,
            Gex::InputRel::Func>
            (mod, "BezierFuncPtr")
            ;
}
