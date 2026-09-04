#ifndef GEX_PLUGIN_IMAGE_PLUGIN_PLUGIN_CPP
#define GEX_PLUGIN_IMAGE_PLUGIN_PLUGIN_CPP

#include "Gex/include/Gex/Gex.h"

#include "../export.h"

#include <string>
#include <QPixmap>

#include "Types/image.h"
#include "Types/color.h"
#include "Types/resolution.h"
#include "Types/gradient.h"
#include "Types/level.h"
#include "Types/geometry.h"

#include "Nodes/blur.h"
#include "Nodes/load.h"
#include "Nodes/blend.h"
#include "Nodes/noises.h"
#include "Nodes/remap.h"
#include "Nodes/shapes.h"
#include "Nodes/level.h"
#include "Nodes/tile.h"
#include "Nodes/channels.h"


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->GetRegistry()->RegisterType<QImage>(
            "ImageManip::Image", TSys::DefaultCreator,
            ImageManip::Types::SerializeImage,
            ImageManip::Types::DeserializeImage
    );

    loader->GetRegistry()->RegisterType<QColor>(
            "ImageManip::Color", TSys::DefaultCreator,
            ImageManip::Types::SerializeColor,
            ImageManip::Types::DeserializeColor
    );


    loader->GetRegistry()->RegisterType<ImageManip::Types::Resolution>(
            "ImageManip::Resolution", TSys::DefaultCreator,
            ImageManip::Types::SerializeResolution,
            ImageManip::Types::DeserializeResolution
    );


    loader->GetRegistry()->RegisterType<ImageManip::Types::LinearGradient>(
            "ImageManip::LinearGradient", TSys::DefaultCreator,
            ImageManip::Types::SerializeGradient,
            ImageManip::Types::DeserializeGradient
    );

    loader->GetRegistry()->RegisterType<ImageManip::Types::LevelMap>(
            "ImageManip::LevelMap", TSys::DefaultCreator,
            ImageManip::Types::SerializeLevelMap,
            ImageManip::Types::DeserializeLevelMap
    );


    loader->GetRegistry()->RegisterType<ImageManip::Types::LevelMap>(
            "ImageManip::Point", TSys::DefaultCreator,
            ImageManip::Types::SerializePoint,
            ImageManip::Types::DeserializePoint
    );


    loader->RegisterNode<ImageManip::Nodes::CreateImageBuilder>("Image/IO/Create");

    loader->RegisterNode<ImageManip::Nodes::LoadImageBuilder>("Image/IO/Load");

    loader->RegisterNode<ImageManip::Nodes::SaveImageBuilder>("Image/IO/Save");

    loader->RegisterNode<ImageManip::Nodes::BlendImagesBuilder>("Image/Modifiers/Blend");

    loader->RegisterNode<ImageManip::Nodes::LayerBlendBuilder>("Image/Modifiers/LayerBlend");

    loader->RegisterNode<ImageManip::Nodes::RGBMaskBuilder>("Image/Modifiers/RGBMask");

    loader->RegisterNode<ImageManip::Nodes::AlphaMaskBuilder>("Image/Modifiers/AlphaMask");

    loader->RegisterNode<ImageManip::Nodes::BoxBlurNodeBuilder>("Image/Modifiers/Blur/BoxBlur");

    loader->RegisterNode<ImageManip::Nodes::RandomNoiseBuilder>("Image/Noises/RandomNoise");

    loader->RegisterNode<ImageManip::Nodes::PerlinNoiseBuilder>("Image/Noises/PerlinNoise");

    loader->RegisterNode<ImageManip::Nodes::DelaunayNoiseBuilder>("Image/Noises/DelaunayNoise");

    loader->RegisterNode<ImageManip::Nodes::CloudNoiseBuilder>("Image/Noises/CloudNoise");

    loader->RegisterNode<ImageManip::Nodes::TriangleCircleBuilder>("Image/Noises/TriangleCircle");

    loader->RegisterNode<ImageManip::Nodes::GridVisBuilder>("Image/Noises/Grid");

    loader->RegisterNode<ImageManip::Nodes::CircleOrderedBuilder>("Image/Noises/CircleOrdered");

    loader->RegisterNode<ImageManip::Nodes::VoronoiCellNoiseBuilder>("Image/Noises/VoronoiCells");

    loader->RegisterNode<ImageManip::Nodes::ColorMapNodeBuilder>("Image/Modifiers/ColorMap");

    loader->RegisterNode<ImageManip::Nodes::LevelNodeBuilder>("Image/Modifiers/Level");

    loader->RegisterNode<ImageManip::Nodes::TileNodeBuilder>("Image/Modifiers/Tile");

    loader->RegisterNode<ImageManip::Nodes::DecomposeChannelsNodeBuilder>("Image/Modifiers/DecomposeChannels");

    loader->RegisterNode<ImageManip::Nodes::ComposeChannelsNodeBuilder>("Image/Modifiers/ComposeChannels");

    loader->RegisterNode<ImageManip::Nodes::RectangleNodeBuilder>("Image/Shapes/Rectangle");

    loader->RegisterNode<ImageManip::Nodes::EllipseNodeBuilder>("Image/Shapes/Ellipse");

    loader->RegisterNode<ImageManip::Nodes::PolygonNodeBuilder>("Image/Shapes/Polygon");

}

#endif  // GEX_PLUGIN_IMAGE_PLUGIN_PLUGIN_CPP