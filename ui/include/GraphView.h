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
        Gex::CompoundNodePtr graph;
        GraphWidget* graphView;
        ProfileView* profileView;

    public:
        GraphView(Gex::CompoundNodePtr graph,
                  QWidget* parent=nullptr);

        GraphWidget* GetGraphWidget() const;

        ProfileView* GetProfileWidget() const;

        void SwitchGraph(Gex::CompoundNodePtr graph);
    };
}

#endif //GEX_GRAPHVIEW_H
