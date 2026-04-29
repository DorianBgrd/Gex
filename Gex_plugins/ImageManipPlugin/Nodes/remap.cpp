#include "remap.h"

#include "Types/gradient.h"
#include "Manip/remap.h"

#include <QImage>



void ImageManip::Nodes::ColorRemapNode::InitAttributes()
{
    CreateAttribute<ImageManip::Types::LinearGradient>(
            "Gradient", Gex::AttrValueType::Single,
            Gex::AttrType::Input);
    CreateAttribute<QImage>("Source", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::ColorRemapNode::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto gradient = context.GetAttribute("Gradient").GetValue<
            ImageManip::Types::LinearGradient>();

    auto source = context.GetAttribute("Source").GetValue<QImage>();

    auto dest = ImageManip::Manip::GradientRemapImage(source, gradient);

    return context.GetAttribute("Image").SetValue(dest);

}
