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
     auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").Get<int>();
    bool bw = context.GetAttribute("BlackAndWhite").Get<bool>();
    bool alpha = context.GetAttribute("Alpha").Get<bool>();

    QImage noise = ImageManip::Manip::RandomNoise(
            res.at(0), res.at(1),
            seed, bw, alpha);

    return context.GetAttribute("Image").Set<QImage>(noise);
}


void ImageManip::Nodes::PerlinNoise::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("Frequency", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(10);

    CreateAttribute<int>("Octave", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(5);

    CreateAttribute<double>("Lacunarity", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(2);

    CreateAttribute<double>("Persistence", Gex::AttrValueType::Single,
                            Gex::AttrType::Input)->SetDefaultValue(0.5);

    CreateAttribute<int>("Seed", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::PerlinNoise::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").Get<int>();
    int frequency = context.GetAttribute("Frequency").Get<int>();
    int octave = context.GetAttribute("Octave").Get<int>();
    double lacunarity = context.GetAttribute("Lacunarity").Get<int>();
    double octavePersistence = context.GetAttribute("Persistence").Get<double>();

    QImage noise = ImageManip::Manip::FractalPerlinNoise(
            res.at(0), res.at(1), octave,
            frequency, lacunarity,
            octavePersistence,seed
    );

    return context.GetAttribute("Image").Set<QImage>(noise);
}


void ImageManip::Nodes::CloudNoise::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};
    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<int>("Frequency", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(10);

    CreateAttribute<int>("Octave", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(5);

    CreateAttribute<double>("Lacunarity", Gex::AttrValueType::Single,
                            Gex::AttrType::Input)->SetDefaultValue(2);

    CreateAttribute<double>("SmoothLevel", Gex::AttrValueType::Single,
                            Gex::AttrType::Input)->SetDefaultValue(2);

    CreateAttribute<double>("Persistence", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(0.5);

    CreateAttribute<int>("Seed", Gex::AttrValueType::Single,
                         Gex::AttrType::Input)->SetDefaultValue(1);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::CloudNoise::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    double smoothLevel = context.GetAttribute("SmoothLevel").Get<double>();
    int seed = context.GetAttribute("Seed").Get<int>();
    int frequency = context.GetAttribute("Frequency").Get<int>();
    int octave = context.GetAttribute("Octave").Get<int>();
    double octaveFrequencyFactor = context.GetAttribute("Lacunarity").Get<double>();
    double octavePersistence = context.GetAttribute("Persistence").Get<double>();

    QImage noise = ImageManip::Manip::FractalCloudNoise(
            res.at(0), res.at(1), octave,
            frequency, octaveFrequencyFactor,
            smoothLevel, octavePersistence, seed
    );

    return context.GetAttribute("Image").Set<QImage>(noise);
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
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").Get<int>();
//    bool bw = context.GetAttribute("BlackAndWhite").Get<bool>();
//    bool alpha = context.GetAttribute("Alpha").Get<bool>();
    int frequency = context.GetAttribute("Frequency").Get<int>();
    bool colored = context.GetAttribute("Colored").Get<bool>();

    QImage noise = ImageManip::Manip::DelaunayNoise(
            res.at(0), res.at(1),
            frequency, seed, colored, true);

    return context.GetAttribute("Image").Set<QImage>(noise);
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
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").Get<int>();
    int size = context.GetAttribute("Size").Get<int>();
    int randomSize = context.GetAttribute("RandomSize").Get<int>();
    int density = context.GetAttribute("Density").Get<int>();
    int randomDensity = context.GetAttribute("RandomDensity").Get<int>();

    QImage noise = ImageManip::Manip::DirtNoise(
        res.at(0), res.at(1), size,
        randomSize, density,
        randomDensity, seed);

    return context.GetAttribute("Image").Set<QImage>(noise);
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
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").Get<int>();
    int frequency = context.GetAttribute("Frequency").Get<int>();
    bool colored = context.GetAttribute("Colored").Get<bool>();
    bool filled = context.GetAttribute("Filled").Get<bool>();
    int lineWidth = context.GetAttribute("LineWidth").Get<int>();

    QImage noise = ImageManip::Manip::VoronoiNoise(
            res.at(0), res.at(1), frequency,
            seed, lineWidth, filled, colored, true);

    return context.GetAttribute("Image").Set<QImage>(noise);
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
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").Get<int>();

    QImage noise = ImageManip::Manip::TriangleCirconscrit(
            res.at(0), res.at(1), seed);

    return context.GetAttribute("Image").Set<QImage>(noise);
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
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").Get<int>();
    int width = context.GetAttribute("GridWidth").Get<int>();
    int height = context.GetAttribute("GridHeight").Get<int>();
    bool tileable = context.GetAttribute("Tileable").Get<bool>();

    QImage grid = ImageManip::Manip::GridViz(res.at(0), res.at(1),
                                             width, height, seed, tileable);

    return context.GetAttribute("Image").Set<QImage>(grid);
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
    auto res = context.GetAttribute("Resolution").Get<ImageManip::Types::Resolution>();
    int seed = context.GetAttribute("Seed").Get<int>();
    int freq = context.GetAttribute("Frequency").Get<int>();

    QImage image = ImageManip::Manip::CircallyOrdered(
            res.at(0), res.at(1),
            freq, seed);

    return context.GetAttribute("Image").Set<QImage>(image);
}