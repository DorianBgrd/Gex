#ifndef GEX_NODE_LEVEL_H
#define GEX_NODE_LEVEL_H

#include "plugins/export.h"
#include "Gex/include/Gex.h"

namespace ImageManip::Nodes
{
    class Plugin_API LevelNode: public Gex::Node
    {
        std::string Description() const override
        {
            return "Adjusts image levels.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
                      override;
    };

    GENERATE_DEFAULT_BUILDER(LevelNodeBuilder, LevelNode)
}

#endif //GEX_NODE_LEVEL_H
