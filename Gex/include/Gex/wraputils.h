#ifndef GEX_WRAPUTILS_H
#define GEX_WRAPUTILS_H

#include <memory>

namespace Gex::Python
{
    template<class T>
    pybind11::object WeakRefToStrong(pybind11::args args,
                                     pybind11::kwargs kwargs)
    {
        T wkObj = args[0].cast<T>();
        if (wkObj.expired())
            return {};

        return pybind11::object(wkObj.lock());
    }

    template<class T, class WT>
    pybind11::object StrongRefToWeak(pybind11::args args,
                                          pybind11::kwargs kwargs)
    {
        T sharedObj =  args[0].cast<T>();

        WT weakObj = sharedObj;
        return pybind11::object(weakObj);
    }
}

#endif //GEX_WRAPUTILS_H
