#ifndef GEX_NOISES_NODES_H
#define GEX_NOISES_NODES_H

#include "Gex/include/Gex.h"

#include "../export.h"

namespace ImageManip::Nodes
{
    class Plugin_API RandomNoise: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(RandomNoiseBuilder, RandomNoise)


    class Plugin_API DirtNoise: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };


    GENERATE_DEFAULT_BUILDER(DirtNoiseBuilder, DirtNoise)

    class Plugin_API VoronoiCellNoise: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(VoronoiCellNoiseBuilder, VoronoiCellNoise)

}

#endif //GEX_NOISES_NODES_H
