#ifndef GEX_NODE_SCATTER_H
#define GEX_NODE_SCATTER_H

#include "Gex_plugins/export.h"

#include "Gex/include/Gex/Gex.h"


namespace ImageManip::Nodes
{
    class Plugin_API ScatterNode: public Gex::Node
    {
        std::string Description() const override
    {
        return "Scatters images through existing image.";
    }

    void InitAttributes() override;

    bool Evaluate(Gex::NodeAttributeData &context,
                  Gex::GraphContext &graphContext,
                  Gex::NodeProfiler &profiler)
    override;
    };

    GENERATE_DEFAULT_BUILDER(ScatterNodeBuilder, ScatterNode)
}

#endif //GEX_NODE_SCATTER_H
