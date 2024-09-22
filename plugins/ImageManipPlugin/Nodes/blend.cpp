#include "blend.h"

#include "Manip/blend.h"

#include <QPixmap>

#include "Tsys/include/defaultTypes.h"


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
            .GetValue<QImage>();
    auto dst = context.GetAttribute("Input2")
            .GetValue<QImage>();
    auto mask = context.GetAttribute("Mask")
            .GetValue<QImage>();
    auto blend = context.GetAttribute("Blender")
            .GetValue<float>();

    return context.GetAttribute("Output").SetValue(
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

    TSys::Enum compositionMode;
    unsigned int i = 0;
    for (auto name : ImageManip::Manip::CompositionModeNames)
    {
        compositionMode.AddValue(i, name);
        i++;
    }

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
            .GetValue<QImage>();
    auto bld = context.GetAttribute("Input2")
            .GetValue<QImage>();
    auto mode = context.GetAttribute("CompositionMode")
            .GetValue<TSys::Enum>();

    return context.GetAttribute("Output").SetValue(
            ImageManip::Manip::LayerBlend(src, bld, mode.CurrentValue()));
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
            .GetValue<QImage>();
    auto msk = context.GetAttribute("Mask")
            .GetValue<QImage>();

    return context.GetAttribute("Output").SetValue(
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
            .GetValue<QImage>();
    auto msk = context.GetAttribute("Mask")
            .GetValue<QImage>();

    return context.GetAttribute("Output").SetValue(
            ImageManip::Manip::ApplyRGBMask(src, msk));
}


