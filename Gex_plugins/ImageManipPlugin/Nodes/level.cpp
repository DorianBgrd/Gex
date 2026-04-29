#include "level.h"

#include "Types/level.h"
#include "Manip/level.h"

#include <QImage>


void ImageManip::Nodes::LevelNode::InitAttributes()
{
    CreateAttribute<ImageManip::Types::LevelMap>(
            "Level", Gex::AttrValueType::Single,
            Gex::AttrType::Input);
    CreateAttribute<QImage>("Source", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);
    CreateAttribute<QImage>("Image", Gex::AttrValueType::Single,
                            Gex::AttrType::Output);
}


bool ImageManip::Nodes::LevelNode::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto img = context.GetAttribute("Source").GetValue<QImage>();
    auto lvl = context.GetAttribute("Level").GetValue<
            ImageManip::Types::LevelMap>();

    return context.GetAttribute("Image").SetValue(
            ImageManip::Manip::AdjustLevel(img, lvl)
            );
}
