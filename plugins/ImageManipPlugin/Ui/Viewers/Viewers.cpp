#include "Viewers.h"


void ImageManip::Viewers::ImageNodeViewer::Setup()
{
    auto* layout = new QVBoxLayout();
    setLayout(layout);

    viewer = new ImageManip::Ui::Types::ImageViewer(
            QImage(), this);
    layout->addWidget(viewer);
}

void ImageManip::Viewers::ImageNodeViewer::OnNodeUpdated(Gex::NodeWkPtr node)
{
    auto imageAttr = node->GetAttribute("Image");
    if (!imageAttr)
    {
        viewer->SetImage(QImage());
        return;
    }

    QImage img = imageAttr->Get<QImage>();
    viewer->SetImage(img);
}


void ImageManip::Viewers::ImageNodeViewer::OnAttributeUpdated(
        const Gex::AttributePtr& attribute,
        const Gex::AttributeChange& change)
{
    OnNodeUpdated(CurrentNode());
}
