#include "noises.h"

#include <QPixmap>

#include "Manip/noises.h"
#include "Types/resolution.h"


void ImageManip::Nodes::RandomNoise::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("Seed", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
    CreateAttribute<bool>("BlackAndWhite", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(true);
    CreateAttribute<bool>("Alpha", Gex::AttrValueType::Single,
                          Gex::AttrType::Input)->SetDefaultValue(false);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::RandomNoise::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
     auto res = context.GetAttribute("Resolution").GetValue<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").GetValue<int>();
    bool bw = context.GetAttribute("BlackAndWhite").GetValue<bool>();
    bool alpha = context.GetAttribute("Alpha").GetValue<bool>();

    QImage noise = ImageManip::Manip::RandomNoise(
            res.at(0), res.at(1),
            seed, bw, alpha);

    return context.GetAttribute("Image").SetValue<QImage>(noise);
}


void ImageManip::Nodes::DirtNoise::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("Seed", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<int>("Size", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(10);

    CreateAttribute<int>("RandomSize", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(5);

    CreateAttribute<int>("Density", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(50);

    CreateAttribute<int>("RandomDensity", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(10);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::DirtNoise::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").GetValue<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").GetValue<int>();
    int size = context.GetAttribute("Size").GetValue<int>();
    int randomSize = context.GetAttribute("RandomSize").GetValue<int>();
    int density = context.GetAttribute("Density").GetValue<int>();
    int randomDensity = context.GetAttribute("RandomDensity").GetValue<int>();

    QImage noise = ImageManip::Manip::DirtNoise(
        res.at(0), res.at(1), size,
        randomSize, density,
        randomDensity, seed);

    return context.GetAttribute("Image").SetValue<QImage>(noise);
}