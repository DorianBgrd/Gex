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
    auto source = context.GetAttribute("Source").GetValue<QImage>();

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

    return (context.GetAttribute("Red").SetValue(red) &&
            context.GetAttribute("Green").SetValue(green) &&
            context.GetAttribute("Blue").SetValue(blue) &&
            context.GetAttribute("Alpha").SetValue(alpha));
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
    auto red = context.GetAttribute("Red").GetValue<QImage>();
    auto green = context.GetAttribute("Green").GetValue<QImage>();
    auto blue = context.GetAttribute("Blue").GetValue<QImage>();
    auto alpha = context.GetAttribute("Alpha").GetValue<QImage>();

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
        return context.GetAttribute("Image").SetValue(QImage());
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

    return context.GetAttribute("Image").SetValue(img);
}

