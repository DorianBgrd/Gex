#ifndef GEX_NODES_H
#define GEX_NODES_H

#include "Gex/include/Gex.h"


namespace Geometry::Nodes
{
    class Index: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override;
    };

    GENERATE_DEFAULT_BUILDER(IndexBuilder, Index)

    class Face: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override;
    };

    GENERATE_DEFAULT_BUILDER(TriangleBuilder, Face)

    class CreateMesh: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override;
    };

    GENERATE_DEFAULT_BUILDER(CreateMeshBuilder, CreateMesh)

    class MeshPoint: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override;
    };

    GENERATE_DEFAULT_BUILDER(MeshPointBuilder, MeshPoint)

    class EditMeshPoint: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override;
    };

    GENERATE_DEFAULT_BUILDER(EditMeshPointBuilder, EditMeshPoint)

    class MeshPointNormal: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override;
    };

    GENERATE_DEFAULT_BUILDER(MeshPointNormalBuilder, MeshPointNormal)
}


#endif //GEX_NODES_H
