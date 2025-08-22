#ifndef GEX_NOISES_NODES_H
#define GEX_NOISES_NODES_H

#include "Gex/include/Gex.h"

#include "../export.h"

namespace ImageManip::Nodes
{
    class Plugin_API RandomNoise: public Gex::Node
    {
        std::string Description() const override
        {
            return "Generate a random noise.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(RandomNoiseBuilder, RandomNoise)


    class Plugin_API PerlinNoise: public Gex::Node
    {
        std::string Description() const override
        {
            return "Generate a perlin noise.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(PerlinNoiseBuilder, PerlinNoise)


    class Plugin_API DelaunayNoise: public Gex::Node
    {
        std::string Description() const override
        {
            return "Generate a random noise.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(DelaunayNoiseBuilder, DelaunayNoise)


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


    class Plugin_API TriangleCircle: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(TriangleCircleBuilder, TriangleCircle)


    class Plugin_API GridVis: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(GridVisBuilder, GridVis)


    class Plugin_API CircleOrdered: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(CircleOrderedBuilder, CircleOrdered)
}

#endif //GEX_NOISES_NODES_H
