#ifndef GEX_IMM_PLUGIN_NODES_BLUR_H
#define GEX_IMM_PLUGIN_NODES_BLUR_H

#include "Gex/include/Gex.h"

#include "../export.h"

namespace ImageManip::Nodes
{
    class Plugin_API BoxBlurNode: public Gex::Node
    {
        std::string Description() const override
        {
            return "Blurs the image using box algorithm.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(BoxBlurNodeBuilder, BoxBlurNode)
}


#endif //GEX_IMM_PLUGIN_NODES_BLUR_H
