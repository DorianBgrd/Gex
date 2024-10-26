#include "blur.h"

#include <QPixmap>

#include "Manip/blur.h"

void ImageManip::Nodes::BoxBlurNode::InitAttributes()
{
    CreateAttribute<QImage>("Input", Gex::AttrValueType::Single,
                             Gex::AttrType::Input);
    CreateAttribute<int>("Steps", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::BoxBlurNode::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto src = context.GetAttribute("Input").GetValue<QImage>();
    auto steps = context.GetAttribute("Steps").GetValue<int>();

    return context.GetAttribute("Output").SetValue(
            ImageManip::Manip::BoxBlurPixmap(src, steps));
}
