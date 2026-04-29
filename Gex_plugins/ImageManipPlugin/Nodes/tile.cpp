#include "tile.h"

#include "Manip/tile.h"

#include <QImage>


void ImageManip::Nodes::TileNode::InitAttributes()
{
    CreateAttribute<QImage>("Source", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);

    CreateAttribute<int>("TileNumber", Gex::AttrValueType::Single,
                         Gex::AttrType::Input);

    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::TileNode::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto src = context.GetAttribute("Source").GetValue<QImage>();
    auto tile = context.GetAttribute("TileNumber").GetValue<int>();

    return context.GetAttribute("Image").SetValue(
            ImageManip::Manip::TileImage(src, tile));
}
