#ifndef GEX_CLI_EXEC_H
#define GEX_CLI_EXEC_H

#include "Gex/include/Gex.h"
#include "ArgParse/ArgParse.h"
#include "rapidjson/document.h"

namespace Exec
{
    bool ResolveJsonValue(Gex::AttributePtr attr, rapidjson::Value& value);


    void VerboseNodeStart(Gex::NodePtr node);


    void VerboseNodeEnd(Gex::NodePtr node, bool success);


    int ExecuteGraph(int argc, char** argv, int start);
}


#endif //GEX_CLI_EXEC_H
