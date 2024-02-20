#ifndef GEX_GRAPHVIEW_H
#define GEX_GRAPHVIEW_H

#include <QWidget>
#include <QTabWidget>
#include <QDockWidget>

#include "ui/include/NodeGraph.h"
#include "ui/include/ProfileViewer.h"

namespace Gex::Ui
{
    class GEX_UI_API GraphView: public QWidget
    {
        Q_OBJECT

    private:
        Gex::Graph* graph;
        GraphWidget* graphView;
        ProfileWidget* profileView;

    public:
        GraphView(Gex::Graph* graph,
                  QWidget* parent=nullptr);

        GraphWidget* GetGraphWidget() const;

        ProfileWidget* GetProfileWidget() const;
    };
}

#endif //GEX_GRAPHVIEW_H
