#include "load.h"
#include <QPixmap>
#include <QColor>

#include "Types/resolution.h"


void ImageManip::Nodes::LoadImage::InitAttributes()
{
    CreateAttribute<std::string>("Filepath", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                             Gex::AttrType::Output);
}


bool ImageManip::Nodes::LoadImage::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto path = context.GetAttribute("Filepath")
            .Get<std::string>();

    return context.GetAttribute("Image")
            .Set(QImage(path.c_str()));
}


void ImageManip::Nodes::CreateImage::InitAttributes()
{
    ImageManip::Types::Resolution defaultRes = {1024, 1024};

    CreateAttribute<ImageManip::Types::Resolution>(
            "Resolution", Gex::AttrValueType::Single,
            Gex::AttrType::Input)->SetDefaultValue(defaultRes);

    CreateAttribute<QColor>("Color", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                             Gex::AttrType::Output);
}


bool ImageManip::Nodes::CreateImage::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto resolution = context.GetAttribute("Resolution")
            .Get<ImageManip::Types::Resolution>();

    auto color = context.GetAttribute("Color").Get<QColor>();

    QImage image(resolution.at(0),
                 resolution.at(1),
                 QImage::Format_RGBA64);
    image.fill(color);

    return context.GetAttribute("Image").Set(image);
}


void ImageManip::Nodes::SaveImage::InitAttributes()
{
    CreateAttribute<std::string>("Filepath", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
}


bool ImageManip::Nodes::SaveImage::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto filepath = context.GetAttribute("Filepath")
            .Get<std::string>();
    auto image = context.GetAttribute("Image")
            .Get<QImage>();

    return image.save(filepath.c_str());
}