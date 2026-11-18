#include "Types.h"
#include "../export.h"
#include "Types/resolution.h"
#include "Types/geometry.h"


#include "Gex/include/Gex/Gex.h"
#include "Gex_ui/include/Gex_ui/PluginLoader.h"
#include "Ui/Viewers/Viewers.h"


extern "C" Plugin_API
void RegisterUiPlugin(Gex::Ui::UiPluginLoader* loader)
{
    loader->RegisterType<QImage>(
            "ImageManip::Image", QColor(222, 145, 67), nullptr,
            MakeUiPtr(ImageManip::Ui::Types::ImageWidgetBuilder)
    );

    loader->RegisterType<QColor>(
            "ImageManip::Color", QColor(222, 145, 67), nullptr,
            MakeUiPtr(ImageManip::Ui::Types::ColorWidgetBuilder)
    );

    loader->RegisterType<ImageManip::Types::Resolution>(
            "ImageManip::Resolution", QColor(106, 212, 174), nullptr,
            MakeUiPtr(ImageManip::Ui::Types::ResolutionWidgetBuilder)
    );

    loader->RegisterType<ImageManip::Types::LinearGradient>(
            "ImageManip::LinearGradient", QColor(222, 145, 67), nullptr,
            MakeUiPtr(ImageManip::Ui::Types::LinearGradientWidgetBuilder)
    );

    loader->RegisterType<ImageManip::Types::LevelMap>(
            "ImageManip::LevelMap", QColor(222, 145, 67), nullptr,
            MakeUiPtr(ImageManip::Ui::Types::LevelWidgetBuilder)
    );

    loader->RegisterType<ImageManip::Types::Point>(
            "ImageManip::Point", QColor(106, 212, 174), nullptr,
            MakeUiPtr(ImageManip::Ui::Types::PointWidgetBuilder)
    );


    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/IO/Create");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/IO/Load");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/IO/Save");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Modifiers/Blend");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Modifiers/LayerBlend");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Modifiers/RGBMask");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Modifiers/AlphaMask");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Modifiers/Blur/BoxBlur");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Noises/PerlinNoise");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Noises/CloudNoise");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Noises/RandomNoise");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Noises/VoronoiCells");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Noises/DelaunayNoise");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Noises/TriangleCircle");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Noises/CircleOrdered");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Noises/Grid");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Modifiers/ColorMap");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Modifiers/ColorMap");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Modifiers/Level");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Modifiers/Tile");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Shapes/Rectangle");
    loader->RegisterNodeViewer<ImageManip::Viewers::ImageNodeViewerBuilder>("Image/Shapes/Ellipse");

    loader->RegisterNodeViewer<ImageManip::Viewers::PolygonNodeEditorBuilder>("Image/Shapes/Polygon");

    std::vector<std::string> nodeTypes = {
            "Image/IO/Create",
            "Image/IO/Load",
            "Image/IO/Save",
            "Image/Modifiers/Blend",
            "Image/Modifiers/LayerBlend",
            "Image/Modifiers/RGBMask",
            "Image/Modifiers/AlphaMask",
            "Image/Modifiers/Blur/BoxBlur",
            "Image/Noises/PerlinNoise",
            "Image/Noises/CloudNoise",
            "Image/Noises/RandomNoise",
            "Image/Noises/VoronoiCells",
            "Image/Noises/DelaunayNoise",
            "Image/Noises/TriangleCircle",
            "Image/Noises/CircleOrdered",
            "Image/Noises/Grid",
            "Image/Modifiers/ColorMap",
            "Image/Modifiers/ColorMap",
            "Image/Modifiers/Level",
            "Image/Modifiers/Tile",
            "Image/Shapes/Rectangle",
            "Image/Shapes/Ellipse",
            "Image/Shapes/Polygon"
    };

    for (const auto& nodeType : nodeTypes)
    {
        loader->RegisterNodeResource(
                nodeType, Res::UiRes::GetRes()->GetQtAwesome()->icon(
                    fa::fa_solid, fa::fa_image
                ), QColor(222, 145, 67)
        );
    }
}