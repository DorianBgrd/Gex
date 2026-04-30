#ifndef GEX_USD_VIEWERS_H
#define GEX_USD_VIEWERS_H

#include "Gex_plugins/export.h"
#include <QtOpenGLWidgets/QOpenGLWidget>

#include "Gex_ui/include/Gex_ui/NodeViewer.h"

namespace UsdPlugin::Ui
{
    class Plugin_API UsdViewport: public QOpenGLWidget
    {
    public:
        UsdViewport(QWidget* parent=nullptr);

        void initializeGL() override;

        void paintGL() override;

        void resizeGL(int w, int h) override;
    };


    class UsdViewportViewer: public Gex::Ui::NodeViewer
    {
        void Setup() override;

        void OnNodeUpdated(Gex::NodeWkPtr node) override;

        void OnAttributeUpdated(const Gex::AttributePtr& attribute,
                                const Gex::AttributeChange& change)
                                override;
    };
}

#endif //GEX_USD_VIEWERS_H
