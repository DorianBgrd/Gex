#include "Types.h"
#include "../export.h"
#include "Types/resolution.h"
#include "Types/geometry.h"


#include "Gex/include/Gex.h"
#include "ui/include/PluginLoader.h"
#include "Ui/Viewers/Viewers.h"


extern "C" Plugin_API
void RegisterUiPlugin(Gex::Ui::UiPluginLoader* loader)
{
    loader->RegisterTypeWidget<QImage>(
            "Image", nullptr,
            MakeUiPtr(ImageManip::Ui::Types::ImageWidgetBuilder)
    );

    loader->RegisterTypeWidget<QColor>(
            "Color", nullptr,
            MakeUiPtr(ImageManip::Ui::Types::ColorWidgetBuilder)
    );

    loader->RegisterTypeWidget<ImageManip::Types::Resolution>(
            "Resolution", nullptr,
            MakeUiPtr(ImageManip::Ui::Types::ResolutionWidgetBuilder)
    );

    loader->RegisterTypeWidget<ImageManip::Types::LinearGradient>(
            "LinearGradient", nullptr,
            MakeUiPtr(ImageManip::Ui::Types::LinearGradientWidgetBuilder)
    );

    loader->RegisterTypeWidget<ImageManip::Types::LevelMap>(
            "LevelMap", nullptr,
            MakeUiPtr(ImageManip::Ui::Types::LevelWidgetBuilder)
    );

    loader->RegisterTypeWidget<ImageManip::Types::Point>(
            "ImageManip::Point", nullptr,
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

}