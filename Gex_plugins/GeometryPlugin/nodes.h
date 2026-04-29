#ifndef GEX_NODES_H
#define GEX_NODES_H

#include "Gex/include/Gex.h"

#include "Gex_plugins/GeometryPlugin/types.h"


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

    class IterMeshPoint: public Gex::CompoundNode
    {
        size_t length = 0;
        size_t index = 0;
        Types::Mesh mesh;

        void InitAttributes() override;

        void EditCompoundInputs(const Gex::NodePtr &inputs) override;

        void EditCompoundOutputs(const Gex::NodePtr &outputs) override;

        void Schedule() override;

        bool Compute(Gex::GraphContext &context,
                     Gex::NodeProfiler& profiler)
                     override;

        bool PreEvaluate(Gex::NodeAttributeData &ctx,
                         Gex::NodeAttributeData &inputCtx,
                         Gex::GraphContext &graphContext,
                         Gex::NodeProfiler &profiler) override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override;

        bool PostEvaluate(Gex::NodeAttributeData &ctx,
                          Gex::NodeAttributeData &outputCtx,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override;

        Gex::ScheduleNodePtrList ToScheduledNodes() override;
    };

    GENERATE_DEFAULT_BUILDER(IterMeshPointBuilder, IterMeshPoint)

}


#endif //GEX_NODES_H
