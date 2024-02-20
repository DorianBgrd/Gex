#include "ui/include/GraphView.h"
#include <QTabBar>


Gex::Ui::GraphView::GraphView(Gex::Graph* graph_, QWidget* parent): QWidget(parent)
{
    graph = graph_;

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    auto* tab = new QTabWidget(this);
    tab->setObjectName("GraphViewTab");
    tab->tabBar()->setObjectName("GraphViewTab");
    tab->setTabPosition(QTabWidget::South);
    layout->addWidget(tab);

    graphView = new GraphWidget(graph, this);
    tab->addTab(graphView, "Graph");

    profileView = new ProfileWidget(this);
    tab->addTab(profileView, "Profiler");

    QObject::connect(graphView, &GraphWidget::GraphEvaluated,
                     profileView, &ProfileWidget::FromContext);
}


Gex::Ui::GraphWidget* Gex::Ui::GraphView::GetGraphWidget() const
{
    return graphView;
}


Gex::Ui::ProfileWidget* Gex::Ui::GraphView::GetProfileWidget() const
{
    return profileView;
}