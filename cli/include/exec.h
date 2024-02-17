#ifndef GEX_EXEC_H
#define GEX_EXEC_H

#include "ArgParse/ArgParse.h"

namespace Exec
{
    void ExecuteGraph(int argc, char** argv, int start)
    {
        Parser parser;

        parser.AddFlag("-onv", "--override-node-value", -1,
                       "Overrides node attribute value."
                       " This will not work if attribute is connected.");

        parser.Parse(argc, argv, start);

        for (auto onv : parser.FlagResults("-onv"))
        {
            size_t v = onv.find("=");
            if (v == std::string::npos)
            {
                continue;
            }


        }
    }
}


#endif //GEX_EXEC_H
