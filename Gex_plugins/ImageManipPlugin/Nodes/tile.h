#ifndef GEX_NODE_TILE_H
#define GEX_NODE_TILE_H

#include "Gex_plugins/export.h"
#include "Gex/include/Gex.h"


namespace ImageManip::Nodes
{
    class Plugin_API TileNode: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
                      override;
    };

    GENERATE_DEFAULT_BUILDER(TileNodeBuilder, TileNode)
}

#endif //GEX_NODE_TILE_H
