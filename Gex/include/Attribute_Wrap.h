#ifndef GEX_ATTRIBUTE_WRAP_H
#define GEX_ATTRIBUTE_WRAP_H

#include "api.h"
#include "Attribute.h"
#include "pybind11/pybind11.h"

#include "Gex/python/utils.h"


WEAK_PTR_CONVERTER(Gex::AttributeWkPtr)


namespace Gex::Python
{


    struct GEX_API Attribute_Wrap
    {
        protected:
        static bool registered;
        static PyClassRegistry registry;

        public:
        static bool RegisterPythonWrapper(pybind11::module_& mod,
                                          PyThreadState* state=nullptr);

        static bool IsRegistered();
    };
}



#endif //GEX_ATTRIBUTE_WRAP_H
