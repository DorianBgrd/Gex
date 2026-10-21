#include "channels.h"

#include "Manip/channels.h"


void ImageManip::Nodes::DecomposeChannelsNode::InitAttributes()
{
    CreateAttribute<QImage>("Source", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Red", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
    CreateAttribute<QImage>("Green", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
    CreateAttribute<QImage>("Blue", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
    CreateAttribute<QImage>("Alpha", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}

bool ImageManip::Nodes::DecomposeChannelsNode::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto source = context.GetAttribute("Source").Get<QImage>();

    int width = source.width();
    int height = source.height();
    QImage red(width, height, QImage::Format_RGBA64);
    QImage green(width, height, QImage::Format_RGBA64);
    QImage blue(width, height, QImage::Format_RGBA64);
    QImage alpha(width, height, QImage::Format_RGBA64);

    QColor empty(0, 0, 0, 0);
    red.fill(empty);
    green.fill(empty);
    blue.fill(empty);
    alpha.fill(empty);

    ImageManip::Manip::DecomposeImage(source, red, green, blue, alpha);

    return (context.GetAttribute("Red").Set(red) &&
            context.GetAttribute("Green").Set(green) &&
            context.GetAttribute("Blue").Set(blue) &&
            context.GetAttribute("Alpha").Set(alpha));
}


void ImageManip::Nodes::ComposeChannelsNode::InitAttributes()
{
    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
    CreateAttribute<QImage>("Red", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Green", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Blue", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Alpha", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
}

bool ImageManip::Nodes::ComposeChannelsNode::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto red = context.GetAttribute("Red").Get<QImage>();
    auto green = context.GetAttribute("Green").Get<QImage>();
    auto blue = context.GetAttribute("Blue").Get<QImage>();
    auto alpha = context.GetAttribute("Alpha").Get<QImage>();

    int width = INT_MAX;
    int height = INT_MAX;
    bool sizeFound = false;

    std::vector<QImage> images = {red, green, blue, alpha};
    for (auto img : images)
    {
        if (img.isNull())
            continue;

        sizeFound = true;

        int imgWidth = img.width();
        if (imgWidth < width)
            width = imgWidth;

        int imgHeight = img.height();
        if (imgHeight < height)
            height = imgHeight;
    }

    if (!sizeFound)
    {
        return context.GetAttribute("Image").Set(QImage());
    }

    if (red.isNull())
        red = QImage(width, height, QImage::Format_RGBA64);
    if (green.isNull())
        green = QImage(width, height, QImage::Format_RGBA64);
    if (blue.isNull())
        blue = QImage(width, height, QImage::Format_RGBA64);
    if (alpha.isNull())
        alpha = QImage(width, height, QImage::Format_RGBA64);

    auto img = ImageManip::Manip::ComposeImage(red, green, blue, alpha);

    return context.GetAttribute("Image").Set(img);
}

