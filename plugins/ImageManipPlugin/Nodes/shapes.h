#ifndef GEX_SHAPES_H
#define GEX_SHAPES_H

#include "plugins/export.h"

#include "Gex/include/Gex.h"

namespace ImageManip::Nodes
{
    class Plugin_API RectangleNode: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(RectangleNodeBuilder, RectangleNode)


    class Plugin_API EllipseNode: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(EllipseNodeBuilder, EllipseNode)
}

#endif //GEX_SHAPES_H
