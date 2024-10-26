#ifndef GEX_WRAPUTILS_H
#define GEX_WRAPUTILS_H

#include <boost/python.hpp>
#include <memory>

namespace Gex::Python
{
    template<class T>
    boost::python::object WeakRefToStrong(boost::python::tuple args,
                                          boost::python::dict kwargs)
    {
        T wkObj = boost::python::extract<T>(args[0]);
        if (wkObj.expired())
            return {};

        return boost::python::object(wkObj.lock());
    }

    template<class T, class WT>
    boost::python::object StrongRefToWeak(boost::python::tuple args,
                                          boost::python::dict kwargs)
    {
        T sharedObj =  boost::python::extract<T>(args[0]);

        WT weakObj = sharedObj;
        return boost::python::object(weakObj);
    }
}

#endif //GEX_WRAPUTILS_H
