#ifndef GEX_IMAGE_REMAP_H
#define GEX_IMAGE_REMAP_H

#include "Gex/include/Gex.h"

#include "../export.h"


namespace ImageManip::Nodes
{

    class Plugin_API ColorRemapNode: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
                      override;
    };

    GENERATE_DEFAULT_BUILDER(ColorMapNodeBuilder, ColorRemapNode)

}


#endif //GEX_IMAGE_REMAP_H
