#ifndef GEX_CHANNELS_H
#define GEX_CHANNELS_H

#include "Gex/include/Gex.h"
#include "plugins/export.h"

#include <QImage>


namespace ImageManip::Nodes
{
    class Plugin_API DecomposeChannelsNode: public Gex::Node
    {
        std::string Description() const override
        {
            return "Decompose image channels.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
                      override;
    };

    GENERATE_DEFAULT_BUILDER(DecomposeChannelsNodeBuilder, DecomposeChannelsNode)


    class Plugin_API ComposeChannelsNode: public Gex::Node
    {
        std::string Description() const override
        {
            return "Compose image channels.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(ComposeChannelsNodeBuilder, ComposeChannelsNode)
}

#endif //GEX_CHANNELS_H
