#include "nodes.h"

#include "types.h"


std::string Math::Nodes::ConstantInt::Description() const
{
return "Node containing constant int value.";
}


void Math::Nodes::ConstantInt::InitAttributes()
{
    CreateAttribute<int>("Output", Gex::AttrValueType::Single,
                         Gex::AttrType::Static);
}


std::string Math::Nodes::ConstantFloat::Description() const
{
    return "Node containing constant float value.";
}


void Math::Nodes::ConstantFloat::InitAttributes()
{
    CreateAttribute<float>("Output", Gex::AttrValueType::Single,
                           Gex::AttrType::Static);
}


std::string Math::Nodes::ConstantDouble::Description() const
{
    return "Node containing constant double value.";
}

void Math::Nodes::ConstantDouble::InitAttributes()
{
    CreateAttribute<float>("Output", Gex::AttrValueType::Single,
                           Gex::AttrType::Static);
}


std::string Math::Nodes::Add::Description() const
{
    return "Dynamic node adding values.";
}

void Math::Nodes::Add::InitAttributes()
{
    CreateAttribute<double>("Inputs", Gex::AttrValueType::Multi,
                            Gex::AttrType::Input);
    CreateAttribute<double>("Output", Gex::AttrValueType::Single,
                        Gex::AttrType::Output);
}

bool Math::Nodes::Add::Evaluate(Gex::NodeAttributeData &ctx,
                         Gex::GraphContext &graph,
                         Gex::NodeProfiler& profiler)
{
    auto inputsAttr = ctx.GetAttribute("Inputs");

    double value = 0.0;
    for (unsigned int index : inputsAttr.Indices())
    {
        value += inputsAttr.GetIndex(index).GetValue<double>();
    }

    return ctx.GetAttribute("Output").SetValue(value);
}


std::string Math::Nodes::Subtract::Description() const
{
    return "Subtract values.";
}

void Math::Nodes::Subtract::InitAttributes()
{
    CreateAttribute<double>("Inputs", Gex::AttrValueType::Multi,
                            Gex::AttrType::Input);
    CreateAttribute<double>("Output", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::Subtract::Evaluate(Gex::NodeAttributeData &ctx,
              Gex::GraphContext &graph,
              Gex::NodeProfiler& profiler)
{
    auto inputsAttr = ctx.GetAttribute("Inputs");

    double value = 0.0;

    for (unsigned int index : inputsAttr.Indices())
    {
        value -= inputsAttr.GetIndex(index).GetValue<double>();
    }

    return ctx.GetAttribute("Output").SetValue(value);
}


std::string Math::Nodes::Multiply::Description() const
{
    return "Multiply values.";
}

void Math::Nodes::Multiply::InitAttributes()
{
    CreateAttribute<double>("Inputs", Gex::AttrValueType::Multi,
                            Gex::AttrType::Input);
    CreateAttribute<double>("Output", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::Multiply::Evaluate(Gex::NodeAttributeData &ctx,
              Gex::GraphContext &graph,
              Gex::NodeProfiler& profiler)
{
    auto inputsAttr = ctx.GetAttribute("Inputs");

    bool set = false;

    double value = 0.0;
    for (unsigned int index : inputsAttr.Indices())
    {
        if (!set)
        {
            value = inputsAttr.GetIndex(index).GetValue<double>();
            set = true;
            continue;
        }

        value *= inputsAttr.GetIndex(index).GetValue<double>();
    }

    return ctx.GetAttribute("Output").SetValue(value);
}


std::string Math::Nodes::Divide::Description() const
{
    return "Divide values.";
}

void Math::Nodes::Divide::InitAttributes()
{
    CreateAttribute<double>("Inputs", Gex::AttrValueType::Multi,
                            Gex::AttrType::Input);
    CreateAttribute<double>("Output", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::Divide::Evaluate(Gex::NodeAttributeData &ctx,
                            Gex::GraphContext &graph,
                            Gex::NodeProfiler& profiler)
{
    auto inputsAttr = ctx.GetAttribute("Inputs");

    bool set = false;
    double value = 0.0;

    for (unsigned int index : inputsAttr.Indices())
    {
        if (!set)
        {
            value = inputsAttr.GetIndex(index).GetValue<double>();
            set = true;
            continue;
        }

        value /= inputsAttr.GetIndex(index).GetValue<double>();
    }

    return ctx.GetAttribute("Output").SetValue(value);
}


std::string Math::Nodes::Cos::Description() const
{
    return "Cosine function.";
}

void Math::Nodes::Cos::InitAttributes()
{
    CreateAttribute<double>("Value", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<double>("Cos", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::Cos::Evaluate(Gex::NodeAttributeData &ctx,
                                   Gex::GraphContext &graph,
                                   Gex::NodeProfiler& profiler)
{
    auto value = ctx.GetAttribute("Value").GetValue<double>();

    return ctx.GetAttribute("Cos").SetValue(std::cos(value));
}


std::string Math::Nodes::Acos::Description() const
{
    return "Arc-cosine function.";
}

void Math::Nodes::Acos::InitAttributes()
{
    CreateAttribute<double>("Value", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<double>("Acos", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::Acos::Evaluate(Gex::NodeAttributeData &ctx,
                                Gex::GraphContext &graph,
                                Gex::NodeProfiler& profiler)
{
    auto value = ctx.GetAttribute("Value").GetValue<double>();

    return ctx.GetAttribute("Acos").SetValue(std::acos(value));
}


std::string Math::Nodes::RadianToDegree::Description() const
{
    return "Converts radian value to degree.";
}

void Math::Nodes::RadianToDegree::InitAttributes()
{
    CreateAttribute<double>("Radian", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<double>("Degree", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::RadianToDegree::Evaluate(Gex::NodeAttributeData &ctx,
                                 Gex::GraphContext &graph,
                                 Gex::NodeProfiler& profiler)
{
    auto value = ctx.GetAttribute("Radian").GetValue<double>();

    return ctx.GetAttribute("Degree").SetValue(value * 57.2958);
}


std::string Math::Nodes::DegreeToRadian::Description() const
{
    return "Converts degree value to radian.";
}

void Math::Nodes::DegreeToRadian::InitAttributes()
{
    CreateAttribute<double>("Degree", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<double>("Radian", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::DegreeToRadian::Evaluate(Gex::NodeAttributeData &ctx,
                                           Gex::GraphContext &graph,
                                           Gex::NodeProfiler& profiler)
{
    auto value = ctx.GetAttribute("Degree").GetValue<double>();

    return ctx.GetAttribute("Radian").SetValue(value / 57.2958);
}



std::string Math::Nodes::ComposePoint2::Description() const
{
    return "Composes a new Point2.";
}

void Math::Nodes::ComposePoint2::InitAttributes()
{
    CreateAttribute<double>(
            "X", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Y", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Point2>(
            "Point", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::ComposePoint2::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    Math::Types::Point2 p = {
            ctx.GetAttribute("X").GetValue<double>(),
            ctx.GetAttribute("Y").GetValue<double>()
    };

    return ctx.GetAttribute("Point").SetValue(p);
}




std::string Math::Nodes::DecomposePoint2::Description() const
{
    return "Decomposes a Point2 fields.";
}

void Math::Nodes::DecomposePoint2::InitAttributes()
{
    CreateAttribute<Math::Types::Point2>(
            "Point", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "X", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );

    CreateAttribute<double>(
            "Y", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::DecomposePoint2::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto p = ctx.GetAttribute("Point").GetValue<Math::Types::Point2>();

    ctx.GetAttribute("X").SetValue(p.x);
    ctx.GetAttribute("Y").SetValue(p.y);

    return true;
}




std::string Math::Nodes::Point2Distance::Description() const
{
    return "Computes the distance between 2 Point2.";
}

void Math::Nodes::Point2Distance::InitAttributes()
{
    CreateAttribute<Math::Types::Point2>(
            "Point1", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Point2>(
            "Point2", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Distance", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Point2Distance::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto p1 = ctx.GetAttribute("Point1").GetValue<Math::Types::Point2>();
    auto p2 = ctx.GetAttribute("Point2").GetValue<Math::Types::Point2>();

    return ctx.GetAttribute("Distance").SetValue(p1.Distance(p2));
}




std::string Math::Nodes::ComposePoint3::Description() const
{
    return "Composes a new Point3.";
}

void Math::Nodes::ComposePoint3::InitAttributes()
{
    CreateAttribute<double>(
            "X", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Y", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Z", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Point3>(
            "Point", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::ComposePoint3::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    Math::Types::Point3 p = {
            ctx.GetAttribute("X").GetValue<double>(),
            ctx.GetAttribute("Y").GetValue<double>(),
            ctx.GetAttribute("Z").GetValue<double>()
    };

    return ctx.GetAttribute("Point").SetValue(p);
}




std::string Math::Nodes::DecomposePoint3::Description() const
{
    return "Decomposes a Point3 fields.";
}

void Math::Nodes::DecomposePoint3::InitAttributes()
{
    CreateAttribute<Math::Types::Point3>(
            "Point", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "X", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );

    CreateAttribute<double>(
            "Y", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );

    CreateAttribute<double>(
            "Z", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::DecomposePoint3::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto p = ctx.GetAttribute("Point").GetValue<Math::Types::Point3>();

    ctx.GetAttribute("X").SetValue(p.x);
    ctx.GetAttribute("Y").SetValue(p.y);
    ctx.GetAttribute("Z").SetValue(p.z);

    return true;
}




std::string Math::Nodes::Point3Distance::Description() const
{
    return "Computes the distance between 2 Point3.";
}

void Math::Nodes::Point3Distance::InitAttributes()
{
    CreateAttribute<Math::Types::Point3>(
            "Point1", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Point3>(
            "Point2", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Distance", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Point3Distance::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto p1 = ctx.GetAttribute("Point1").GetValue<Math::Types::Point3>();
    auto p2 = ctx.GetAttribute("Point2").GetValue<Math::Types::Point3>();

    return ctx.GetAttribute("Distance").SetValue(p1.Distance(p2));
}




std::string Math::Nodes::ConstantVector2::Description() const
{
    return "Constant Vector2 value.";
}


void Math::Nodes::ConstantVector2::InitAttributes()
{
    CreateAttribute<Math::Types::Vector2>(
            "Vector2", Gex::AttrValueType::Single,
            Gex::AttrType::Static
    );
}


std::string Math::Nodes::ComposeVector2::Description() const
{
    return "Composes a new vector2.";
}

void Math::Nodes::ComposeVector2::InitAttributes()
{
    CreateAttribute<double>("X");
    CreateAttribute<double>("Y");
    CreateAttribute<Math::Types::Vector2>(
            "OutVector", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::ComposeVector2::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto x = ctx.GetAttribute("X").GetValue<double>();
    auto y = ctx.GetAttribute("Y").GetValue<double>();

    Math::Types::Vector2 vec2 = {x, y};

    return ctx.GetAttribute("OutVector").SetValue(vec2);
}


std::string Math::Nodes::Vector2Magnitude::Description() const
{
    return "Computes the magnitude of a vector2.";
}

void Math::Nodes::Vector2Magnitude::InitAttributes()
{
    CreateAttribute<Math::Types::Vector2>(
            "Vector", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Magnitude", Gex::AttrValueType::Single,
        Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector2Magnitude::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    return ctx.GetAttribute("Magnitude").SetValue(
            ctx.GetAttribute("Vector")
            .GetValue<Math::Types::Vector2>().Magnitude()
    );
}


std::string Math::Nodes::NormalizeVector2::Description() const
{
    return "Normalizes a vector2.";
}

void Math::Nodes::NormalizeVector2::InitAttributes()
{
    CreateAttribute<Math::Types::Vector2>(
            "Vector", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector2>(
            "Normalized", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::NormalizeVector2::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec = ctx.GetAttribute("Vector").GetValue<Math::Types::Vector2>();

    vec.Normalize();

    return ctx.GetAttribute("Normalized").SetValue(vec);
}


std::string Math::Nodes::AngleVector2::Description() const
{
    return "Normalizes a vector2.";
}

void Math::Nodes::AngleVector2::InitAttributes()
{
    CreateAttribute<Math::Types::Vector2>(
            "Vector1", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector2>(
            "Vector2", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Angle", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::AngleVector2::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    return ctx.GetAttribute("Angle").SetValue(
        Math::Types::Vector2::Angle(
            ctx.GetAttribute("Vector1").GetValue<Math::Types::Vector2>(),
            ctx.GetAttribute("Vector2").GetValue<Math::Types::Vector2>()
        )
    );
}


std::string Math::Nodes::DecomposeVector2::Description() const
{
    return "Decomposes a vector2.";
}

void Math::Nodes::DecomposeVector2::InitAttributes()
{
    CreateAttribute<Math::Types::Vector2>(
            "Vector"
    );
    CreateAttribute<double>("X", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
    CreateAttribute<double>("Y", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::DecomposeVector2::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec = ctx.GetAttribute("Vector").GetValue<Math::Types::Vector2>();

    ctx.GetAttribute("X").SetValue(vec.x);
    ctx.GetAttribute("Y").SetValue(vec.y);
    return true;
}


std::string Math::Nodes::Vector2DotProduct::Description() const
{
    return "Computes the Dot product between 2 vector2.";
}

void Math::Nodes::Vector2DotProduct::InitAttributes()
{
    CreateAttribute<Math::Types::Vector2>("Vector1");

    CreateAttribute<Math::Types::Vector2>("Vector2");

    CreateAttribute<double>("DotProduct", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::Vector2DotProduct::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    return ctx.GetAttribute("DotProduct").SetValue(
        Math::Types::Vector2::DotProduct(
            ctx.GetAttribute("Vector1").GetValue<Math::Types::Vector2>(),
            ctx.GetAttribute("Vector2").GetValue<Math::Types::Vector2>()
        )
    );
}


std::string Math::Nodes::Vector2Add::Description() const
{
    return "Adds 2 vector2.";
}

void Math::Nodes::Vector2Add::InitAttributes()
{
    CreateAttribute<Math::Types::Vector2>("Vector1");

    CreateAttribute<Math::Types::Vector2>("Vector2");

    CreateAttribute<Math::Types::Vector2>(
            "OutVector", Gex::AttrValueType::Single,
        Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector2Add::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec1 = ctx.GetAttribute("Vector1").GetValue<Math::Types::Vector2>();
    auto vec2 = ctx.GetAttribute("Vector2").GetValue<Math::Types::Vector2>();

    vec1.Add(vec2);

    return ctx.GetAttribute("OutVector").SetValue(vec1);
}


std::string Math::Nodes::Vector2Subtract::Description() const
{
    return "Subtract 2 vector2.";
}

void Math::Nodes::Vector2Subtract::InitAttributes()
{
    CreateAttribute<Math::Types::Vector2>("Vector1");

    CreateAttribute<Math::Types::Vector2>("Vector2");

    CreateAttribute<Math::Types::Vector2>(
            "OutVector", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector2Subtract::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec1 = ctx.GetAttribute("Vector1").GetValue<Math::Types::Vector2>();
    auto vec2 = ctx.GetAttribute("Vector2").GetValue<Math::Types::Vector2>();

    vec1.Subtract(vec2);

    return ctx.GetAttribute("OutVector").SetValue(vec1);
}


std::string Math::Nodes::Vector2Divide::Description() const
{
    return "Divides a vector2.";
}

void Math::Nodes::Vector2Divide::InitAttributes()
{
    CreateAttribute<Math::Types::Vector2>(
            "Vector", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );

    CreateAttribute<double>(
            "Div", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );

    CreateAttribute<Math::Types::Vector2>(
            "OutVector", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector2Divide::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec = ctx.GetAttribute("Vector").GetValue<Math::Types::Vector2>();

    vec.Divide(ctx.GetAttribute("Div").GetValue<double>());

    return ctx.GetAttribute("OutVector").SetValue(vec);
}


std::string Math::Nodes::Vector2Multiply::Description() const
{
    return "Multiplies a vector2.";
}

void Math::Nodes::Vector2Multiply::InitAttributes()
{
    CreateAttribute<Math::Types::Vector2>(
            "Vector", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Mult", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector2>(
            "OutVector", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector2Multiply::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec = ctx.GetAttribute("Vector").GetValue<Math::Types::Vector2>();

    vec.Multiply(ctx.GetAttribute("Mult").GetValue<double>());

    return ctx.GetAttribute("OutVector").SetValue(vec);
}


std::string Math::Nodes::ConstantVector3::Description() const
{
    return "Constant Vector3 value.";
}


void Math::Nodes::ConstantVector3::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>(
            "Vector3", Gex::AttrValueType::Single,
            Gex::AttrType::Static
    );
}


std::string Math::Nodes::ComposeVector3::Description() const
{
    return "Composes a new vector3.";
}

void Math::Nodes::ComposeVector3::InitAttributes()
{
    CreateAttribute<double>("X", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<double>("Y", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<double>("Z", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<Math::Types::Vector3>(
            "OutVector", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::ComposeVector3::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto x = ctx.GetAttribute("X").GetValue<double>();
    auto y = ctx.GetAttribute("Y").GetValue<double>();
    auto z = ctx.GetAttribute("Z").GetValue<double>();

    Math::Types::Vector3 vec3 = {x, y, z};

    return ctx.GetAttribute("OutVector").SetValue(vec3);
}


std::string Math::Nodes::DecomposeVector3::Description() const
{
    return "Decomposes a vector3.";
}

void Math::Nodes::DecomposeVector3::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>(
            "Vector", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );
    CreateAttribute<double>("X", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
    CreateAttribute<double>("Y", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
    CreateAttribute<double>("Z", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::DecomposeVector3::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec = ctx.GetAttribute("Vector").GetValue<Math::Types::Vector3>();

    ctx.GetAttribute("X").SetValue(vec.x);
    ctx.GetAttribute("Y").SetValue(vec.y);
    ctx.GetAttribute("Z").SetValue(vec.z);
    return true;
}


std::string Math::Nodes::Vector3DotProduct::Description() const
{
    return "Computes the Dot product between 2 vector3.";
}

void Math::Nodes::Vector3DotProduct::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>("Vector1");

    CreateAttribute<Math::Types::Vector3>("Vector2");

    CreateAttribute<double>("DotProduct", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool Math::Nodes::Vector3DotProduct::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec1 = ctx.GetAttribute("Vector1").GetValue<Math::Types::Vector3>();
    auto vec2 = ctx.GetAttribute("Vector2").GetValue<Math::Types::Vector3>();

    return ctx.GetAttribute("DotProduct").SetValue(vec1.DotProduct(vec2));
}


std::string Math::Nodes::Vector3Add::Description() const
{
    return "Adds 2 vector2.";
}

void Math::Nodes::Vector3Add::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>(
            "Vector1", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector3>(
            "Vector2", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector2>(
            "OutVector", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector3Add::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec1 = ctx.GetAttribute("Vector1").GetValue<Math::Types::Vector3>();
    auto vec2 = ctx.GetAttribute("Vector2").GetValue<Math::Types::Vector3>();

    vec1.Add(vec2);

    return ctx.GetAttribute("OutVector").SetValue(vec1);
}


std::string Math::Nodes::Vector3Subtract::Description() const
{
    return "Subtract 2 vector3.";
}

void Math::Nodes::Vector3Subtract::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>(
            "Vector1", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector3>(
            "Vector2", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector3>(
            "OutVector", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector3Subtract::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec1 = ctx.GetAttribute("Vector1").GetValue<Math::Types::Vector3>();
    auto vec2 = ctx.GetAttribute("Vector2").GetValue<Math::Types::Vector3>();

    vec1.Subtract(vec2);

    return ctx.GetAttribute("OutVector").SetValue(vec1);
}


std::string Math::Nodes::Vector3Divide::Description() const
{
    return "Divides 2 vector3.";
}

void Math::Nodes::Vector3Divide::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>(
            "Vector1", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Div", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector3>(
            "OutVector", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector3Divide::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec = ctx.GetAttribute("Vector").GetValue<Math::Types::Vector3>();
    auto div = ctx.GetAttribute("Div").GetValue<double>();

    vec.Divide(div);

    return ctx.GetAttribute("OutVector").SetValue(vec);
}


std::string Math::Nodes::Vector3Multiply::Description() const
{
    return "Multiplies 2 vector3.";
}

void Math::Nodes::Vector3Multiply::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>(
            "Vector", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Mult", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector3>(
            "OutVector", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector3Multiply::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec = ctx.GetAttribute("Vector").GetValue<Math::Types::Vector3>();
    auto mult = ctx.GetAttribute("Mult").GetValue<double>();

    vec.Multiply(mult);

    return ctx.GetAttribute("OutVector").SetValue(vec);
}


std::string Math::Nodes::Vector3CrossProduct::Description() const
{
    return "Computes the cross product of 2 vector3.";
}

void Math::Nodes::Vector3CrossProduct::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>(
            "Vector1", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector3>(
            "Vector2", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector3>(
            "CrossProduct", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector3CrossProduct::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec1 = ctx.GetAttribute("Vector1").GetValue<Math::Types::Vector3>();
    auto vec2 = ctx.GetAttribute("Vector2").GetValue<Math::Types::Vector3>();

    Math::Types::Vector3 res = vec1.CrossProduct(vec2);

    return ctx.GetAttribute("CrossProduct").SetValue(res);
}


std::string Math::Nodes::Vector3Magnitude::Description() const
{
    return "Computes the magnitude of a vector3.";
}

void Math::Nodes::Vector3Magnitude::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>(
            "Vector", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Magnitude", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::Vector3Magnitude::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    return ctx.GetAttribute("Magnitude").SetValue(
            ctx.GetAttribute("Vector")
                    .GetValue<Math::Types::Vector3>().Magnitude()
    );
}


std::string Math::Nodes::NormalizeVector3::Description() const
{
    return "Normalizes a vector3.";
}

void Math::Nodes::NormalizeVector3::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>(
            "Vector", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector3>(
            "Normalized", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::NormalizeVector3::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    auto vec = ctx.GetAttribute("Vector").GetValue<Math::Types::Vector3>();

    vec.Normalize();

    return ctx.GetAttribute("Normalized").SetValue(vec);
}


std::string Math::Nodes::AngleVector3::Description() const
{
    return "Normalizes a vector3.";
}

void Math::Nodes::AngleVector3::InitAttributes()
{
    CreateAttribute<Math::Types::Vector3>(
            "Vector1", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<Math::Types::Vector3>(
            "Vector2", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<double>(
            "Angle", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}

bool Math::Nodes::AngleVector3::Evaluate(
        Gex::NodeAttributeData &ctx,
        Gex::GraphContext &graph,
        Gex::NodeProfiler& profiler
)
{
    return ctx.GetAttribute("Angle").SetValue(
            Math::Types::Vector3::Angle(
                    ctx.GetAttribute("Vector1").GetValue<Math::Types::Vector3>(),
                    ctx.GetAttribute("Vector2").GetValue<Math::Types::Vector3>()
            )
    );
}
