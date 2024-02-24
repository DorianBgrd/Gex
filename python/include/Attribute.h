#ifndef GEX_ATTRIBUTE_H
#define GEX_ATTRIBUTE_H

#include "api.h"
#include "Gex/include/Attribute.h"
#include "boost/python.hpp"

namespace Gex::Python
{
    struct GEX_PYTHON_API Attribute_Wrap: public Gex::Attribute,
        public boost::python::wrapper<Gex::Attribute>
    {
        protected:
        static bool registered;

        public:
        static bool RegisterPythonWrapper();
    };
}



#endif //GEX_ATTRIBUTE_H
