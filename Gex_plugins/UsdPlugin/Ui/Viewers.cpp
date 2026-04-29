#include "Viewers.h"

#include <QVBoxLayout>


UsdPlugin::Ui::UsdViewport::UsdViewport(QWidget* parent):
    QOpenGLWidget(parent)
{

}


void UsdPlugin::Ui::UsdViewport::initializeGL()
{

}


void UsdPlugin::Ui::UsdViewport::paintGL()
{

}


void UsdPlugin::Ui::UsdViewport::resizeGL(int w, int h)
{

}


void UsdPlugin::Ui::UsdViewportViewer::Setup()
{
    auto* layout = new QVBoxLayout();
    setLayout(layout);

    auto* viewport = new UsdViewport(this);
    layout->addWidget(viewport);
}