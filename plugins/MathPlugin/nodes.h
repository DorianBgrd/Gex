#ifndef GEX_NODES_H
#define GEX_NODES_H

#include "../export.h"
#include "Gex/include/Gex.h"



namespace Math::Nodes
{
    class Plugin_API ConstantInt: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;
    };


    GENERATE_DEFAULT_BUILDER(ConstantIntBuilder, ConstantInt)


    class Plugin_API ConstantFloat: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;
    };


    GENERATE_DEFAULT_BUILDER(ConstantFloatBuilder, ConstantFloat)



    class Plugin_API ConstantDouble: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;
    };


    GENERATE_DEFAULT_BUILDER(ConstantDoubleBuilder, ConstantDouble)


    class Plugin_API Add: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(AddBuilder, Add)


    class Plugin_API Subtract: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(SubtractBuilder, Subtract)


    class Plugin_API Multiply: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(MultiplyBuilder, Multiply)


    class Plugin_API Divide: public Gex::Node
    {
    public:
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(DivideBuilder, Divide)


    class Plugin_API Cos: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(CosBluider, Cos)


    class Plugin_API Acos: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(AcosBuilder, Acos)


    class Plugin_API RadianToDegree: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(RadianToDegreeBuilder, RadianToDegree)


    class Plugin_API DegreeToRadian: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(DegreeToRadianBuilder, DegreeToRadian)


    // Points ------------------------------------------------
    class Plugin_API ComposePoint2: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(ComposePoint2Builder, ComposePoint2)


    class Plugin_API DecomposePoint2: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(DecomposePoint2Builder, DecomposePoint2)


    class Plugin_API Point2Distance: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Point2DistanceBuilder, Point2Distance)


    class Plugin_API ComposePoint3: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(ComposePoint3Builder, ComposePoint3)


    class Plugin_API DecomposePoint3: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(DecomposePoint3Builder, DecomposePoint3)


    class Plugin_API Point3Distance: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Point3DistanceBuilder, Point3Distance)



    // Vectors ------------------------------------------------
    class Plugin_API ConstantVector2: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;
    };


    GENERATE_DEFAULT_BUILDER(ConstantVector2Builder, ConstantVector2)


    class Plugin_API ComposeVector2: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(ComposeVector2Builder, ComposeVector2)


    class Plugin_API Vector2Magnitude: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector2MagnitudeBuilder, Vector2Magnitude)


    class Plugin_API NormalizeVector2: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(NormalizeVector2Builder, NormalizeVector2)


    class Plugin_API AngleVector2: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(AngleVector2Builder, AngleVector2)


    class Plugin_API DecomposeVector2: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(DecomposeVector2Builder, DecomposeVector2)


    class Plugin_API Vector2DotProduct: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector2DotProductBuilder, Vector2DotProduct)


    class Plugin_API Vector2Add: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector2AddBuilder, Vector2Add)


    class Plugin_API Vector2Subtract: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector2SubtractBuilder, Vector2Subtract)


    class Plugin_API Vector2Divide: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector2DivideBuilder, Vector2Divide)


    class Plugin_API Vector2Multiply: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector2MultiplyBuilder, Vector2Multiply)


    class Plugin_API ConstantVector3: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;
    };


    GENERATE_DEFAULT_BUILDER(ConstantVector3Builder, ConstantVector3)


    class Plugin_API ComposeVector3: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(ComposeVector3Builder, ComposeVector3)


    class Plugin_API DecomposeVector3: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(DecomposeVector3Builder, DecomposeVector3)


    class Plugin_API Vector3DotProduct: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector3DotProductBuilder, Vector3DotProduct)


    class Plugin_API Vector3Add: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector3AddBuilder, Vector3Add)


    class Plugin_API Vector3Subtract: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector3SubtractBuilder, Vector3Subtract)


    class Plugin_API Vector3Divide: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector3DivideBuilder, Vector3Divide)


    class Plugin_API Vector3Multiply: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector3MultiplyBuilder, Vector3Multiply)


    class Plugin_API Vector3CrossProduct: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector3CrossProductBuilder, Vector3CrossProduct)


    class Plugin_API Vector3Magnitude: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(Vector3MagnitudeBuilder, Vector3Magnitude)


    class Plugin_API NormalizeVector3: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(NormalizeVector3Builder, NormalizeVector3)


    class Plugin_API AngleVector3: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override;
    };


    GENERATE_DEFAULT_BUILDER(AngleVector3Builder, AngleVector3)

}

#endif //GEX_NODES_H
