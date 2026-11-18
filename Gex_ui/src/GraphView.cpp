#include "Gex_ui/include/Gex_ui/GraphView.h"
#include <QTabBar>
#include <QButtonGroup>

#include <QStylePainter>


Gex::Ui::ColoredTabButton::ColoredTabButton(
        const QString& text, const QColor& color,
        QWidget* parent
): QPushButton(parent)
{
    setText(text);
    setCheckable(true);

    setObjectName("ColoredTabButton");

    QString stylesheet = (
      "QPushButton {\n"
      "    background-color: transparent;\n"
      "    border: none;\n"
      "    border-top-left-radius: 0px;\n"
      "    border-top-right-radius: 0px;\n"
      "}\n"
      "QPushButton:checked {\n"
      "    background-color: #000000;\n"
      "    border-top: none;\n"
      "    border-left: 1px solid %COLOR%;\n"
      "    border-right: 1px solid %COLOR%;\n"
      "    border-bottom: 1px solid %COLOR%;\n"
      "}\n"
    );

    setStyleSheet(
            stylesheet.replace(
                    "%COLOR%", color.name()
            )
    );


}


Gex::Ui::ColoredFrame::ColoredFrame(QWidget* widget):
    QWidget(widget)
{

}

QColor Gex::Ui::ColoredFrame::Color() const
{
    return borderColor;
}

void Gex::Ui::ColoredFrame::SetColor(QColor color)
{
    borderColor = color;

    update();
}


QWidget* Gex::Ui::ColoredFrame::Target() const
{
    return targetWidget;
}


void Gex::Ui::ColoredFrame::SetTarget(QWidget* target)
{
    targetWidget = target;
}


void Gex::Ui::ColoredFrame::paintEvent(QPaintEvent *event)
{
    QStylePainter painter(this);

    painter.setPen(QPen(borderColor, 1));
    painter.setBrush(Qt::NoBrush);

    auto left = rect().bottomLeft();
    left.setY(left.y() + 1);

    auto right = rect().bottomRight();
    right.setY(right.y() + 1);

    qreal bottom = rect().bottom();

    if (targetWidget)
    {
        QPointF targetGlobalLeft = targetWidget->mapToGlobal(
                targetWidget->rect().topLeft()
        );
        QPointF targetGlobalRight = targetWidget->mapToGlobal(
                targetWidget->rect().topRight()
        );

        QPointF targetLocalLeft = mapFromGlobal(targetGlobalLeft);
        targetLocalLeft.setY(bottom);

        QPointF targetLocalRight = mapFromGlobal(targetGlobalRight);
        targetLocalRight.setY(bottom);

        painter.drawLine(QPointF(0, bottom), targetLocalLeft);
        painter.drawLine(targetLocalRight, QPointF(rect().right(), bottom));

        painter.drawLine(rect().topLeft(), rect().topRight());

        painter.setPen(QPen(QColor(0, 0, 0), 2));
        painter.drawLine(targetLocalLeft, targetLocalRight);
    }
    else
    {
        painter.drawLine(rect().bottomLeft(), rect().bottomRight());
        painter.drawLine(rect().topLeft(), rect().topRight());
    }


    painter.end();
}


Gex::Ui::TabWidget::TabWidget(QWidget* parent): QWidget(parent)
{
    group = new QButtonGroup(this);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    coloredFrame = new ColoredFrame(this);
    layout->addWidget(coloredFrame);

    auto* mainLayout = new QVBoxLayout(coloredFrame);
    mainLayout->setContentsMargins(0, 1, 0, 1);

    stackedLayout = new QStackedLayout();
    mainLayout->addLayout(stackedLayout);

    tabBar = new QWidget(this);
    layout->addWidget(tabBar);

    tabBarLayout = new QHBoxLayout(tabBar);
    tabBarLayout->setAlignment(Qt::AlignLeft);
    tabBarLayout->setSpacing(0);
    tabBarLayout->setContentsMargins(0, 0, 0, 0);

    QObject::connect(
            group, &QButtonGroup::idToggled,
            stackedLayout, &QStackedLayout::setCurrentIndex
     );
}


void Gex::Ui::TabWidget::AddTab(
        const QString& title,
        const QColor& color,
        QWidget* widget
)
{
    auto* button = new ColoredTabButton(
            title, color, tabBar
    );

    button->setCheckable(true);

    tabBarLayout->addWidget(button);

    int index = stackedLayout->addWidget(widget);

    QObject::connect(
            button, &QPushButton::toggled,
            [this, button, color](bool checked)
            {
                if (checked)
                {
                    coloredFrame->SetColor(color);
                    coloredFrame->SetTarget(button);
                }
            }
    );

    if (stackedLayout->count() == 1)
    {
        button->setChecked(true);
    }

    group->addButton(button, index);
}


Gex::Ui::GraphView::GraphView(Gex::CompoundNodePtr graph_, QWidget* parent): QWidget(parent)
{
    graph = graph_;

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    auto* tab = new TabWidget(this);
    tab->setObjectName("GraphViewTab");
//    tab->tabBar()->setObjectName("GraphViewTabBar");
//    tab->setTabPosition(QTabWidget::South);
    layout->addWidget(tab);

    graphView = new GraphWidget(graph, this);
    tab->AddTab("Graph", QColor(247, 216, 106), graphView);

    profileView = new ProfileView(this);
    tab->AddTab("Profiler", QColor(135, 245, 105), profileView);

    scheduleViewer = new ScheduleViewer(this);
    tab->AddTab("Scheduling", QColor(105, 156, 245), scheduleViewer);

    QObject::connect(graphView, &GraphWidget::GraphEvaluated,
                     profileView, &ProfileView::FromContext);

    QObject::connect(graphView, &GraphWidget::ViewedGraphChanged,
                     scheduleViewer, &ScheduleViewer::LoadCompound);
}


Gex::Ui::GraphWidget* Gex::Ui::GraphView::GetGraphWidget() const
{
    return graphView;
}


Gex::Ui::ProfileView* Gex::Ui::GraphView::GetProfileWidget() const
{
    return profileView;
}


void Gex::Ui::GraphView::SwitchGraph(Gex::CompoundNodePtr graph_)
{
    graphView->SwitchGraph(graph_);
    graph = graph_;
}