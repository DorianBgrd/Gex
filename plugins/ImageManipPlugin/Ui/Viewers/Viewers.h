#ifndef GEX_IMGMANIP_VIEWERS_H
#define GEX_IMGMANIP_VIEWERS_H

#include "ui/include/ui.h"
#include "Ui/Types.h"

namespace ImageManip::Viewers
{
    struct ImageNodeViewer: public Gex::Ui::NodeViewer
    {
        ImageManip::Ui::Types::ImageViewer* viewer;

    public:
        void Setup() override;

        void OnNodeUpdated(Gex::NodeWkPtr node) override;

        void OnAttributeUpdated(
                const Gex::AttributePtr& attribute,
                const Gex::AttributeChange& change)
                override;
    };


    GENERATE_DEFAULT_VIEWER_CREATOR(
            ImageNodeViewerBuilder,
            ImageNodeViewer)
}

#endif //GEX_IMGMANIP_VIEWERS_H
