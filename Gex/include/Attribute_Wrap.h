#ifndef GEX_ATTRIBUTE_WRAP_H
#define GEX_ATTRIBUTE_WRAP_H

#include "api.h"
#include "Attribute.h"
#include "boost/python.hpp"

namespace Gex::Python
{
    struct GEX_API Attribute_Wrap: public Gex::Attribute,
        public boost::python::wrapper<Gex::Attribute>
    {
        protected:
        static bool registered;

        public:
        static bool RegisterPythonWrapper();
    };
}



#endif //GEX_ATTRIBUTE_WRAP_H
