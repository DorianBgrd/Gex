#include "scatter.h"

#include <QImage>




void ImageManip::Nodes::ScatterNode::InitAttributes()
{
    CreateAttribute<int>("x", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<int>("y", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<int>("width", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<int>("height", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<QImage>("Color", Gex::AttrValueType::Single,
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
