#ifndef GEX_NODE_BLEND_H
#define GEX_NODE_BLEND_H

#include "Gex/include/Gex.h"
#include "../export.h"

namespace ImageManip::Nodes
{
    class Plugin_API BlendImages: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(BlendImagesBuilder, BlendImages)


    class Plugin_API LayerBlend: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(LayerBlendBuilder, LayerBlend)


    class Plugin_API RGBMask: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(RGBMaskBuilder, RGBMask)


    class Plugin_API AlphaMask: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(AlphaMaskBuilder, AlphaMask)
}

#endif //GEX_NODE_BLEND_H
