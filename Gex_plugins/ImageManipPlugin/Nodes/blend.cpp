#include "blend.h"

#include "Manip/blend.h"

#include <QPixmap>

#include "Tsys/defaultTypes.h"


void ImageManip::Nodes::BlendImages::InitAttributes()
{
    CreateAttribute<QImage>("Input1", Gex::AttrValueType::Single,
                             Gex::AttrType::Input);
    CreateAttribute<QImage>("Input2", Gex::AttrValueType::Single,
                             Gex::AttrType::Input);
    CreateAttribute<QImage>("Mask", Gex::AttrValueType::Single,
                             Gex::AttrType::Input);
    CreateAttribute<float>("Blender", Gex::AttrValueType::Single,
                         Gex::AttrType::Input);
    CreateAttribute<QImage>("Output", Gex::AttrValueType::Single,
                             Gex::AttrType::Output);
}


bool ImageManip::Nodes::BlendImages::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto src = context.GetAttribute("Input1")
            .Get<QImage>();
    auto dst = context.GetAttribute("Input2")
            .Get<QImage>();
    auto mask = context.GetAttribute("Mask")
            .Get<QImage>();
    auto blend = context.GetAttribute("Blender")
            .Get<float>();

    return context.GetAttribute("Output").Set(
            ImageManip::Manip::Blend(src, dst, blend, mask));
}


void ImageManip::Nodes::LayerBlend::InitAttributes()
{
    CreateAttribute<QImage>("Input1", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Input2", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Output", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);

    std::vector<std::string> compModes(
            ImageManip::Manip::CompositionModeNames.begin(),
            ImageManip::Manip::CompositionModeNames.end()
    );

    TSys::Enum compositionMode(compModes);

    CreateAttribute<TSys::Enum>(
            "CompositionMode", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(compositionMode);

}


bool ImageManip::Nodes::LayerBlend::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto src = context.GetAttribute("Input1")
            .Get<QImage>();
    auto bld = context.GetAttribute("Input2")
            .Get<QImage>();
    auto mode = context.GetAttribute("CompositionMode")
            .Get<TSys::Enum>();

    return context.GetAttribute("Output").Set(
        ImageManip::Manip::LayerBlend(
                src, bld, mode.GetCurrentString()
            )
    );
}


void ImageManip::Nodes::RGBMask::InitAttributes()
{
    CreateAttribute<QImage>("Input", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Mask", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Output", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::RGBMask::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto src = context.GetAttribute("Input")
            .Get<QImage>();
    auto msk = context.GetAttribute("Mask")
            .Get<QImage>();

    return context.GetAttribute("Output").Set(
            ImageManip::Manip::ApplyRGBMask(src, msk));
}


void ImageManip::Nodes::AlphaMask::InitAttributes()
{
    CreateAttribute<QImage>("Input", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Mask", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Output", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::AlphaMask::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto src = context.GetAttribute("Input")
            .Get<QImage>();
    auto msk = context.GetAttribute("Mask")
            .Get<QImage>();

    return context.GetAttribute("Output").Set(
            ImageManip::Manip::ApplyRGBMask(src, msk));
}


