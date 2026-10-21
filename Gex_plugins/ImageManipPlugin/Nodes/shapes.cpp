#include "shapes.h"
#include "Manip/shapes.h"

#include "Types/resolution.h"
#include "Types/geometry.h"
#include <QImage>


void ImageManip::Nodes::RectangleNode::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("x", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<int>("y", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<int>("width", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<int>("height", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<QColor>("Color", Gex::AttrValueType::Single,
                          Gex::AttrType::Input)->SetDefaultValue(true);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::RectangleNode::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int x = context.GetAttribute("x").Get<int>();
    int y = context.GetAttribute("y").Get<int>();
    int width = context.GetAttribute("width").Get<int>();
    int height = context.GetAttribute("height").Get<int>();
    QColor color = context.GetAttribute("Color").Get<QColor>();

    return context.GetAttribute("Image").Set<QImage>(
                ImageManip::Manip::GenerateRectangle(
                        res.at(0), res.at(1),
                        x, y, width, height, color)
            );
}



void ImageManip::Nodes::EllipseNode::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("x", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<int>("y", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<int>("width", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<int>("height", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<QColor>("Color", Gex::AttrValueType::Single,
                            Gex::AttrType::Input)->SetDefaultValue(true);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::EllipseNode::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int x = context.GetAttribute("x").Get<int>();
    int y = context.GetAttribute("y").Get<int>();
    int width = context.GetAttribute("width").Get<int>();
    int height = context.GetAttribute("height").Get<int>();
    QColor color = context.GetAttribute("Color").Get<QColor>();

    return context.GetAttribute("Image").Set<QImage>(
            ImageManip::Manip::GenerateEllipse(
                    res.at(0), res.at(1),
                    x, y, width, height, color)
    );
}


void ImageManip::Nodes::PolygonNode::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("width", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<int>("height", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<QImage>("Source", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);

    CreateAttribute<QColor>("Color", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);

    CreateAttribute<Types::Point>("Points", Gex::AttrValueType::Array,
                                  Gex::AttrType::Input);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::PolygonNode::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int width = context.GetAttribute("width").Get<int>();
    int height = context.GetAttribute("height").Get<int>();
    QColor color = context.GetAttribute("Color").Get<QColor>();
    QImage src = context.GetAttribute("Source").Get<QImage>();

    auto points = context.GetAttribute("Points").GetArrayValues<Types::Point>();

    return context.GetAttribute("Image").Set<QImage>(
            ImageManip::Manip::GeneratePolygon(
                    res.at(0), res.at(1),
                    points, color, src)
    );
}