#ifndef GEX_SHAPES_H
#define GEX_SHAPES_H

#include "plugins/export.h"

#include "Gex/include/Gex.h"

namespace ImageManip::Nodes
{
    class Plugin_API RectangleNode: public Gex::Node
    {
        std::string Description() const override
        {
            return "Generate a rectangle.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(RectangleNodeBuilder, RectangleNode)


    class Plugin_API EllipseNode: public Gex::Node
    {
        std::string Description() const override
        {
            return "Generate an ellipse.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(EllipseNodeBuilder, EllipseNode)


    class Plugin_API PolygonNode: public Gex::Node
    {
        std::string Description() const override
        {
            return "Generates a polygon.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(PolygonNodeBuilder, PolygonNode)
}

#endif //GEX_SHAPES_H
