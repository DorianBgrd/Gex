#include "shapes.h"
#include "Manip/shapes.h"

#include "Types/resolution.h"
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
    auto res = context.GetAttribute("Resolution").GetValue<ImageManip::Types::Resolution>();
    int x = context.GetAttribute("x").GetValue<int>();
    int y = context.GetAttribute("y").GetValue<int>();
    int width = context.GetAttribute("width").GetValue<int>();
    int height = context.GetAttribute("height").GetValue<int>();
    QColor color = context.GetAttribute("Color").GetValue<QColor>();

    return context.GetAttribute("Image").SetValue<QImage>(
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
    auto res = context.GetAttribute("Resolution").GetValue<ImageManip::Types::Resolution>();
    int x = context.GetAttribute("x").GetValue<int>();
    int y = context.GetAttribute("y").GetValue<int>();
    int width = context.GetAttribute("width").GetValue<int>();
    int height = context.GetAttribute("height").GetValue<int>();
    QColor color = context.GetAttribute("Color").GetValue<QColor>();

    return context.GetAttribute("Image").SetValue<QImage>(
            ImageManip::Manip::GenerateEllipse(
                    res.at(0), res.at(1),
                    x, y, width, height, color)
    );
}