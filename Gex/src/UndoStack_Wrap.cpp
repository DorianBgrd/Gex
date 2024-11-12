#include "Gex/include/UndoStack_Wrap.h"

bool  Gex::Python::UndoStack_Wrap::registered = false;


bool Gex::Python::UndoStack_Wrap::RegisterPythonWrapper()
{
    if (registered)
        return false;

    boost::python::class_<Gex::Undo::UndoStack>("Undo", boost::python::no_init)
            .def("Undo", &Gex::Undo::UndoStack::Undo)
            .staticmethod("Undo")
            .def("Redo", &Gex::Undo::UndoStack::Redo)
            .staticmethod("Redo")
            .def("IsUndoing", &Gex::Undo::UndoStack::IsUndoing)
            .staticmethod("IsUndoing")
            .def("IsRedoing", &Gex::Undo::UndoStack::IsRedoing)
            .staticmethod("IsRedoing")
            .def("Clear", &Gex::Undo::UndoStack::Clear)
            .staticmethod("Clear")
            .def("IsActive", &Gex::Undo::UndoStack::IsActive)
            .staticmethod("IsActive")
            .def("SetActive", &Gex::Undo::UndoStack::SetActive)
            .staticmethod("SetActive")
            .def("Enable", &Gex::Undo::UndoStack::Enable)
            .staticmethod("Enable")
            .def("Disable", &Gex::Undo::UndoStack::Disable)
            .staticmethod("Disable")
            .def("OpenGroup", &Gex::Undo::UndoStack::OpenGroup)
            .staticmethod("OpenGroup")
            .def("CloseGroup", &Gex::Undo::UndoStack::CloseGroup)
            .staticmethod("CloseGroup")
            ;

    registered = true;
    return true;
}