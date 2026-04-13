#ifndef GEX_PYTHON_UTILS_H
#define GEX_PYTHON_UTILS_H

#include <vector>
#include "pybind11/pybind11.h"

#ifndef WEAK_PTR_CONVERTER
#define WEAK_PTR_CONVERTER(classname) \
namespace pybind11::detail \
{ \
    template<> \
    struct type_caster<classname> { \
    public: \
        PYBIND11_TYPE_CASTER(classname, _(#classname)); \
 \
        static handle cast(const classname &src, \
                           return_value_policy /* policy */, \
                           handle /* parent */) { \
            return pybind11::cast<>(src.ToShared()); \
        } \
 \
        bool load(handle src, bool) { \
            auto loaded = src.cast<classname>(); \
            if (!loaded) \
                return false; \
 \
            value = classname(loaded); \
            return true; \
        } \
    }; \
}
#endif // WEAK_PTR_CONVERTER


namespace Gex::Python
{
    class PyClassRegistry
    {
        std::vector<uint64_t> indices;

    public:
        bool IsRegistered(PyThreadState* state=nullptr) const
        {
            if (!state)
            {
                state = PyThreadState_GetUnchecked();
            }

            if (!state)
                return false;

            auto index = PyInterpreterState_GetID(state->interp);
            return (std::find(indices.begin(), indices.end(), index) != indices.end());
        }

        bool Register(PyThreadState* state=nullptr)
        {
            if (!state)
                state = PyThreadState_GetUnchecked();

            if (!state)
                return false;

            indices.push_back(
                    PyInterpreterState_GetID(
                            state->interp
                    )
            );
            return true;
        }
    };
}


#endif // GEX_PYTHON_UTILS_H

