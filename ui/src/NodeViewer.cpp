#include "ui/include/NodeViewer.h"

#include <QLabel>
#include <QVBoxLayout>

Gex::Ui::ViewerRegistry* Gex::Ui::ViewerRegistry::instance;


Gex::Ui::NodeViewer::NodeViewer():
    QWidget()
{
    callbackIndex = -1;
}


Gex::Node* Gex::Ui::NodeViewer::CurrentNode() const
{
    return node;
}


void Gex::Ui::NodeViewer::SetCurrentNode(Gex::Node* n)
{
    if (node)
    {
        DisconnectFromNode(node);
    }

    node = n;

    OnNodeUpdated(node);

    ConnectToNode(node);
}


void Gex::Ui::NodeViewer::closeEvent(QCloseEvent *event)
{
    DisconnectFromNode(node);

    QWidget::closeEvent(event);
}


void Gex::Ui::NodeViewer::ConnectToNode(Gex::Node* n)
{
    if (!node)
        return;

    if (callbackIndex != -1)
        DisconnectFromNode(node);

    auto cb = [this](Gex::Attribute* attribute,
           const Gex::AttributeChange& change)
    {
        this->OnAttributeUpdated(attribute, change);
    };

    callbackIndex = n->RegisterAttributeCallback(cb);
}


void Gex::Ui::NodeViewer::DisconnectFromNode(Gex::Node* n)
{
    if (callbackIndex == -1)
        return;

    n->DeregisterAttributeCallback(callbackIndex);
    callbackIndex = -1;
}


Gex::Ui::NodeViewer* Gex::Ui::ViewerCreator::MakeViewer(
        QWidget* parent) const
{
    auto* viewer = CreateViewer();
    if (parent)
        viewer->setParent(parent);

    return viewer;
}


void Gex::Ui::ViewerRegistry::RegisterViewer(
        const std::string& type,
        Gex::Ui::ViewerCreator* creator)
{
    registry[type] = creator;
}


bool Gex::Ui::ViewerRegistry::HasViewer(const std::string& type)
{
    return (registry.find(type) != registry.end());
}


Gex::Ui::ViewerRegistry* Gex::Ui::ViewerRegistry::GetRegistry()
{
    if (!instance)
        instance = new ViewerRegistry();

    return instance;
}


Gex::Ui::NodeViewer* Gex::Ui::ViewerRegistry::CreateViewer(
        const std::string& type, QWidget* parent)
{
    auto iter = registry.find(type);
    if (iter == registry.end())
    {
        return nullptr;
    }

    auto* viewer = iter->second->MakeViewer(parent);
    viewer->Setup();

    return viewer;
}


Gex::Ui::ViewerWindow::ViewerWindow(NodeViewer* viewer, QWidget* parent): QDialog(parent)
{
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    layout->addWidget(viewer);
}


Gex::Ui::ViewerDock::ViewerDock(QWidget* parent):
        QDockWidget(parent)
{
//    emptyWidget = new QWidget(this);
//    auto* emptyWidgetLayout = new QVBoxLayout();
//    emptyWidgetLayout->setAlignment(Qt::AlignCenter);

    auto* emptyLabel = new QLabel(this);
    emptyLabel->setAlignment(Qt::AlignCenter);
//    emptyWidgetLayout->addWidget(emptyLabel);

    emptyLabel->setText("No viewer attached to\ncurrent selected node.");

    stacked = new QStackedWidget(this);
    setWidget(stacked);

    stacked->addWidget(emptyLabel);
}


void Gex::Ui::ViewerDock::OnNodeSelected(Gex::Node* node)
{
    if (viewer)
    {
        viewer->close();
        viewer->deleteLater();
        viewer = nullptr;
    }

    if (!node)
    {
        stacked->setCurrentIndex(0);
        return;
    }

    auto* registry = ViewerRegistry::GetRegistry();

    std::string typeName = node->Type();
    if (!registry->HasViewer(typeName))
    {
        stacked->setCurrentIndex(0);
    }

    viewer = registry->CreateViewer(typeName, stacked);
    if (!viewer)
    {
        stacked->setCurrentIndex(0);
        return;
    }

    viewer->SetCurrentNode(node);
    stacked->addWidget(viewer);

    stacked->setCurrentIndex(1);
}


void Gex::Ui::ViewerDock::NodeSelectionChanged(
        const std::vector<Gex::Node*> nodes)
{
    Gex::Node* node;
    if (nodes.empty())
        node = nullptr;
    else
        node = *nodes.begin();

    OnNodeSelected(node);
}


void Gex::Ui::ViewerDock::Extract() const
{
    if (!viewer)
        return;

    auto* registry = ViewerRegistry::GetRegistry();

    auto* extracted = registry->CreateViewer(viewer->CurrentNode()->Type());
    extracted->SetCurrentNode(viewer->CurrentNode());

    auto* window = new ViewerWindow(viewer, parentWidget());
    window->show();
}

