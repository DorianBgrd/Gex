#include "Gex/include/UndoStack_Wrap.h"

bool  Gex::Python::UndoStack_Wrap::registered = false;
Gex::Python::PyClassRegistry Gex::Python::UndoStack_Wrap::registry;


bool Gex::Python::UndoStack_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                        PyThreadState* state)
{
    if (registry.IsRegistered(state))
        return false;

    pybind11::class_<Gex::Undo::UndoStack>(mod, "Undo", pybind11::module_local(false))
            .def_static("Undo", &Gex::Undo::UndoStack::Undo)
            .def_static("Redo", &Gex::Undo::UndoStack::Redo)
            .def_static("IsUndoing", &Gex::Undo::UndoStack::IsUndoing)
            .def_static("IsRedoing", &Gex::Undo::UndoStack::IsRedoing)
            .def_static("Clear", &Gex::Undo::UndoStack::Clear)
            .def_static("IsActive", &Gex::Undo::UndoStack::IsActive)
            .def_static("SetActive", &Gex::Undo::UndoStack::SetActive)
            .def_static("Enable", &Gex::Undo::UndoStack::Enable)
            .def_static("Disable", &Gex::Undo::UndoStack::Disable)
            .def_static("OpenGroup", &Gex::Undo::UndoStack::OpenGroup)
            .def_static("CloseGroup", &Gex::Undo::UndoStack::CloseGroup)
            ;

    return registry.Register(state);
}


bool Gex::Python::UndoStack_Wrap::IsRegistered()
{
    return registered;
}