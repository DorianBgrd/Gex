#ifndef GEX_EXPRESSION_H
#define GEX_EXPRESSION_H

#include <string>
#include <map>
#include <any>

#include "boost/python.hpp"

#include "Tsys/include/defaultTypes.h"

#include "Gex/include/Gex.h"


namespace Python
{
    typedef std::vector<std::any> Arguments;

    class Expression
    {
        std::string code;

    public:
        Expression(const std::string& code);

        bool Execute(Gex::NodeAttributeData& context);
    };
}

#endif //GEX_EXPRESSION_H
