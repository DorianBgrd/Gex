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


void ImageManip::Nodes::DelaunayNoise::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("Seed", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);
//    CreateAttribute<bool>("BlackAndWhite", Gex::AttrValueType::Single,
//                          Gex::AttrType::Input)->SetDefaultValue(true);
//    CreateAttribute<bool>("Alpha", Gex::AttrValueType::Single,
//                          Gex::AttrType::Input)->SetDefaultValue(false);

    CreateAttribute<int>("Frequency", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(5);

    CreateAttribute<bool>("Colored", Gex::AttrValueType::Single,
                          Gex::AttrType::Input)->SetDefaultValue(false);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::DelaunayNoise::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").GetValue<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").GetValue<int>();
//    bool bw = context.GetAttribute("BlackAndWhite").GetValue<bool>();
//    bool alpha = context.GetAttribute("Alpha").GetValue<bool>();
    int frequency = context.GetAttribute("Frequency").GetValue<int>();
    bool colored = context.GetAttribute("Colored").GetValue<bool>();

    QImage noise = ImageManip::Manip::DelaunayNoise(
            res.at(0), res.at(1),
            frequency, seed, colored, true);

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


void ImageManip::Nodes::VoronoiCellNoise::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("Seed", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<int>("Frequency", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(10);

    CreateAttribute<int>("LineWidth", Gex::AttrValueType::Single,
                          Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<bool>("Filled", Gex::AttrValueType::Single,
                          Gex::AttrType::Input)->SetDefaultValue(false);

    CreateAttribute<bool>("Colored", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(false);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::VoronoiCellNoise::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").GetValue<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").GetValue<int>();
    int frequency = context.GetAttribute("Frequency").GetValue<int>();
    bool colored = context.GetAttribute("Colored").GetValue<bool>();
    bool filled = context.GetAttribute("Filled").GetValue<bool>();
    int lineWidth = context.GetAttribute("LineWidth").GetValue<int>();

    QImage noise = ImageManip::Manip::VoronoiNoise(
            res.at(0), res.at(1), frequency,
            seed, lineWidth, filled, colored, true);

    return context.GetAttribute("Image").SetValue<QImage>(noise);
}


void ImageManip::Nodes::TriangleCircle::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("Seed", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::TriangleCircle::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").GetValue<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").GetValue<int>();

    QImage noise = ImageManip::Manip::TriangleCirconscrit(
            res.at(0), res.at(1), seed);

    return context.GetAttribute("Image").SetValue<QImage>(noise);
}


void ImageManip::Nodes::GridVis::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("GridWidth", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(5);

    CreateAttribute<int>("GridHeight", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(5);

    CreateAttribute<bool>("Tileable", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(true);

    CreateAttribute<int>("Seed", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::GridVis::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").GetValue<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").GetValue<int>();
    int width = context.GetAttribute("GridWidth").GetValue<int>();
    int height = context.GetAttribute("GridHeight").GetValue<int>();
    bool tileable = context.GetAttribute("Tileable").GetValue<bool>();

    QImage grid = ImageManip::Manip::GridViz(res.at(0), res.at(1),
                                             width, height, seed, tileable);

    return context.GetAttribute("Image").SetValue<QImage>(grid);
}


void ImageManip::Nodes::CircleOrdered::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("ImageWidth", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(5);

    CreateAttribute<int>("ImageHeight", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(5);

    CreateAttribute<int>("Seed", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<int>("Frequency", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(5);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::CircleOrdered::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").GetValue<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").GetValue<int>();
    int freq = context.GetAttribute("Frequency").GetValue<int>();

    QImage image = ImageManip::Manip::CircallyOrdered(
            res.at(0), res.at(1),
            freq, seed);

    return context.GetAttribute("Image").SetValue<QImage>(image);
}