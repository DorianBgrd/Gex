#ifndef GEX_CLI_EXEC_H
#define GEX_CLI_EXEC_H

#include "Gex/include/Gex/Gex.h"
#include "ArgParse/ArgParse.h"
#include "rapidjson/document.h"

namespace Exec
{
    bool ResolveJsonValue(const Gex::AttributePtr& attr, rapidjson::Value& value);


    void VerboseNodeStart(const Gex::ScheduledItemPtr& node);


    void VerboseNodeEnd(const Gex::ScheduledItemPtr& node, bool success);


    int ExecuteGraph(int argc, char** argv, int start);
}


#endif //GEX_CLI_EXEC_H
