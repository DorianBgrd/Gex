#include "nodes.h"

#include "types.h"

#include "Gex/include/Gex/Scheduling.h"


std::string Geometry::Nodes::Index::Description() const
{
    return "Creates a new index from an int.";
}

void Geometry::Nodes::Index::InitAttributes()
{
    CreateAttribute<int>(
            "Int", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Geometry::Types::Index>(
            "Index", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}


bool Geometry::Nodes::Index::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    auto i = context.GetAttribute("Int")
            .Get<int>();

    return context.GetAttribute("Index")
        .Set((Geometry::Types::Index)i);
}


std::string Geometry::Nodes::Face::Description() const
{
    return "Creates a new Mesh Face, "
           "composed of Mesh point indices.";
}

void Geometry::Nodes::Face::InitAttributes()
{
    CreateAttribute<Geometry::Types::Index>(
            "Indices", Gex::AttrValueType::Array,
            Gex::AttrType::Input
    );
    CreateAttribute<Geometry::Types::Face>(
            "Face", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}


bool Geometry::Nodes::Face::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    auto indices = context.GetAttribute("Indices")
            .GetArrayValues<Geometry::Types::Index>();

    Geometry::Types::Face face(indices.begin(), indices.end());

    return context.GetAttribute("Face").Set(face);
}


std::string Geometry::Nodes::CreateMesh::Description() const
{
    return "Creates a new mesh based on points and faces.";
}

void Geometry::Nodes::CreateMesh::InitAttributes()
{
    CreateAttribute<Geometry::Types::Point3>(
            "Points", Gex::AttrValueType::Array,
            Gex::AttrType::Input
    );

    CreateAttribute<Geometry::Types::Face>(
            "Faces", Gex::AttrValueType::Array,
            Gex::AttrType::Input
    );

    CreateAttribute<Geometry::Types::Mesh>(
            "Mesh", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Geometry::Nodes::CreateMesh::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    auto points = context.GetAttribute("Points")
            .GetArrayValues<Geometry::Types::Point3>();

    auto faces = context.GetAttribute("Faces")
            .GetArrayValues<Geometry::Types::Face>();

    Geometry::Types::Mesh mesh(points, faces);

    return context.GetAttribute("Mesh").Set(mesh);
}


std::string Geometry::Nodes::MeshPoint::Description() const
{
    return "Gets a mesh point data from its index.";
}

void Geometry::Nodes::MeshPoint::InitAttributes()
{
    CreateAttribute<Geometry::Types::Index>(
            "Index", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Geometry::Types::Mesh>(
            "Mesh", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Geometry::Types::Point3>(
            "Point", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Geometry::Nodes::MeshPoint::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    auto mesh = context.GetAttribute("Mesh")
            .Get<Geometry::Types::Mesh>();

    auto index = context.GetAttribute("Index")
            .Get<Geometry::Types::Index>();

    auto point = Geometry::Types::Point3(mesh.GetPoint(index));

    return context.GetAttribute("Point").Set(point);
}


std::string Geometry::Nodes::EditMeshPoint::Description() const
{
    return "Gets a mesh point data from its index.";
}

void Geometry::Nodes::EditMeshPoint::InitAttributes()
{
    CreateAttribute<Geometry::Types::Mesh>(
            "Mesh", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );
    CreateAttribute<Geometry::Types::Index>(
            "Index", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );


    CreateAttribute<Geometry::Types::Point3>(
            "Point", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Geometry::Types::Mesh>(
            "OutMesh", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Geometry::Nodes::EditMeshPoint::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    auto mesh = context.GetAttribute("Mesh")
            .Get<Geometry::Types::Mesh>();

    auto index = context.GetAttribute("Index")
            .Get<Geometry::Types::Index>();

    auto point = context.GetAttribute("Point")
            .Get<Geometry::Types::Point3>();

    mesh.GetPoint(index).x = point.x;
    mesh.GetPoint(index).y = point.y;
    mesh.GetPoint(index).z = point.z;

    return context.GetAttribute("OutMesh").Set(mesh);
}


std::string Geometry::Nodes::MeshPointNormal::Description() const
{
    return "Gets a mesh point normal data from its index.";
}

void Geometry::Nodes::MeshPointNormal::InitAttributes()
{
    CreateAttribute<Geometry::Types::Mesh>(
            "Mesh", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Geometry::Types::Index>(
            "Index", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Geometry::Types::Vector3>(
            "Normal", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Geometry::Nodes::MeshPointNormal::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    auto mesh = context.GetAttribute("Mesh")
            .Get<Geometry::Types::Mesh>();

    auto index = context.GetAttribute("Index")
            .Get<Geometry::Types::Index>();

    return context.GetAttribute("Normal").Set(
            mesh.GetPointNormal(index)
    );
}


void Geometry::Nodes::IterMeshPoint::InitAttributes()
{
    CreateAttribute<Geometry::Types::Mesh>("InMesh",
                                           Gex::AttrValueType::Single,
                                           Gex::AttrType::Input);
    CreateAttribute<Geometry::Types::Mesh>("OutMesh",
                                           Gex::AttrValueType::Single,
                                           Gex::AttrType::Output);
}


void Geometry::Nodes::IterMeshPoint::EditCompoundInputs(
        const Gex::NodePtr &inputs
)
{
    inputs->CreateAttribute<Geometry::Types::Point3>(
            "InputPoint", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}


void Geometry::Nodes::IterMeshPoint::EditCompoundOutputs(
        const Gex::NodePtr &outputs
)
{
    outputs->CreateAttribute<Geometry::Types::Point3>(
            "OutputPoint", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );
}


//void Geometry::Nodes::IterMeshPoint::Schedule()
//{
//    scheduledNodes = Gex::ScheduleNodes(GetNodes());
//
//    ValidateScheduling();
//}


bool Geometry::Nodes::IterMeshPoint::Compute(
        Gex::GraphContext &context,
        Gex::NodeProfiler& profiler
)
{
    Pull();

    length = GetAttribute("InMesh")
            ->Get<Types::Mesh>()
            .GetPointCount();

    if (!length)
    {
        return true;
    }

    mesh = GetAttribute("InMesh")->Get<Types::Mesh>();

    for (index = 0; index < length; index++)
    {
        if (!Gex::CompoundNode::Compute(context, profiler))
        {
            return false;
        }
    }

    return true;
}


bool Geometry::Nodes::IterMeshPoint::PreEvaluate(
        Gex::NodeAttributeData &ctx,
        Gex::NodeAttributeData &inputCtx,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    Schedule();

    return inputCtx.GetAttribute("InputPoint").Set(mesh.GetPoint(index));
}

bool Geometry::Nodes::IterMeshPoint::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    Gex::NodeEvaluator evaluator(
            Gex::CompoundNode::Schedule(), graphContext,
            profiler.GetProfiler(),
            false, 1
    );

    evaluator.Run();

    return evaluator.Done();
}

bool Geometry::Nodes::IterMeshPoint::PostEvaluate(
        Gex::NodeAttributeData &ctx,
        Gex::NodeAttributeData &outputCtx,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    auto point = outputCtx.GetAttribute("OutputPoint").Get<Types::Point3>();

    mesh.GetPoint(index).Swap(point);

    return ctx.GetAttribute("OutMesh").Set<Types::Mesh>(mesh);
}


//Gex::ScheduleNodePtrList Geometry::Nodes::IterMeshPoint::ToScheduledNodes()
//{
//    return {};
//}


