#ifndef GEX_LOAD_H
#define GEX_LOAD_H

#include "Gex/include/Gex.h"

#include "../export.h"

namespace ImageManip::Nodes
{
    class Plugin_API LoadImage: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
                      override;
    };

    GENERATE_DEFAULT_BUILDER(LoadImageBuilder, LoadImage)


    class Plugin_API CreateImage: public Gex::Node
    {
        std::string Description() const override
        {
            return "Creates a new image with "
                   "specified resolution "
                   "and color.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
                      override;
    };

    GENERATE_DEFAULT_BUILDER(CreateImageBuilder, CreateImage)


    class Plugin_API SaveImage: public Gex::Node
    {
        std::string Description() const override
        {
            return "Saves image to specified "
                   "file path.";
        }

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(SaveImageBuilder, SaveImage)
}


#endif //GEX_LOAD_H
