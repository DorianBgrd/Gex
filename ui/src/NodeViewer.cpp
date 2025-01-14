#include "ui/include/NodeViewer.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "UiRes/include/uires.h"

Gex::Ui::ViewerRegistry* Gex::Ui::ViewerRegistry::instance;


Gex::Ui::NodeViewer::NodeViewer():
    QWidget()
{
    callbackIndex = -1;
}


Gex::NodeWkPtr Gex::Ui::NodeViewer::CurrentNode() const
{
    return node;
}


void Gex::Ui::NodeViewer::SetCurrentNode(Gex::NodeWkPtr n)
{
    if (node && connected)
    {
        DisconnectFromNode(node);

        if (deleteCallbackIndex)
            node->DeregisterAboutToBeDeletedCallback(
                    deleteCallbackIndex);
    }

    node = n;

    OnNodeUpdated(node);

    ConnectToNode(node);
}


void Gex::Ui::NodeViewer::closeEvent(QCloseEvent *event)
{
    if (connected && node)
        DisconnectFromNode(node);

    QWidget::closeEvent(event);
}


void Gex::Ui::NodeViewer::ConnectToNode(Gex::NodeWkPtr n)
{
    if (!node || connected)
    {
        return;
    }

    auto cb = [this](
            const Gex::AttributePtr& attribute,
            const Gex::AttributeChange& change
            )
    {
        this->OnAttributeUpdated(attribute, change);
    };

    callbackIndex = n->RegisterAttributeCallback(cb);
    connected = true;
//
//    auto dcb = [this]()
//    {
//        if (this->node && this->connected)
//        {
//            this->DisconnectFromNode(this->node);
//            this->connected = false;
//        }
//    };
//
//    deleteCallbackIndex = node->RegisterAboutToBeDeletedCallback(dcb);
}


void Gex::Ui::NodeViewer::DisconnectFromNode(Gex::NodeWkPtr n)
{
    if (!connected)
        return;

    if (callbackIndex == -1)
        return;

    if (n)
        n->DeregisterAttributeCallback(callbackIndex);

    callbackIndex = -1;
    connected = false;
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


Gex::Ui::ViewerWindow::ViewerWindow(NodeViewer* viewer, QWidget* parent): QDockWidget(parent)
{
    setWindowTitle(("Viewer [" + viewer->CurrentNode()->Path() + "]").c_str());

    viewer->setParent(this);
    viewer->setVisible(true);
    setWidget(viewer);
}


Gex::Ui::ViewerDock::ViewerDock(QWidget* parent):
        QDockWidget(parent)
{
    setWindowTitle("Node viewer");
//    emptyWidget = new QWidget(this);
//    auto* emptyWidgetLayout = new QVBoxLayout();
//    emptyWidgetLayout->setAlignment(Qt::AlignCenter);
    auto* mainWidget = new QWidget(this);
    setWidget(mainWidget);

    auto* mainLayout  = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    mainWidget->setLayout(mainLayout);

    auto* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setAlignment(Qt::AlignTop | Qt::AlignRight);
    mainLayout->addLayout(buttonsLayout);

    syncBtn = new QPushButton(this);
    syncBtn->setCheckable(true);
    syncBtn->setChecked(true);
    syncBtn->setFlat(true);
    syncBtn->setObjectName("ViewerSyncBtn");
    syncBtn->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(
            fa::fa_solid, fa::fa_sync));
    buttonsLayout->addWidget(syncBtn);

    selBtn = new QPushButton(this);
    selBtn->setFlat(true);
    selBtn->setObjectName("ViewerSelBtn");
    selBtn->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(
            fa::fa_solid, fa::fa_mouse_pointer));
    buttonsLayout->addWidget(selBtn);

    extractBtn = new QPushButton(this);
    extractBtn->setFlat(true);
    extractBtn->setObjectName("ViewerSelBtn");
    extractBtn->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(
            fa::fa_solid, fa::fa_up_right_from_square));
    buttonsLayout->addWidget(extractBtn);

    auto* emptyLabel = new QLabel(this);
    emptyLabel->setAlignment(Qt::AlignCenter);
//    emptyWidgetLayout->addWidget(emptyLabel);

    emptyLabel->setText("No viewer attached to\ncurrent selected node.");

    stacked = new QStackedWidget(this);
    mainLayout->addWidget(stacked);

    stacked->addWidget(emptyLabel);

    QObject::connect(syncBtn, &QPushButton::toggled,
                     this, &ViewerDock::SetAutoUpdates);
    QObject::connect(selBtn, &QPushButton::clicked,
                     this, &ViewerDock::UpdateSelection);
    QObject::connect(extractBtn, &QPushButton::clicked,
                     this, &ViewerDock::Extract);
}


void Gex::Ui::ViewerDock::SetAutoUpdates(bool updates)
{
    autoUpdate = updates;
}


bool Gex::Ui::ViewerDock::AutoUpdates() const
{
    return autoUpdate;
}


void Gex::Ui::ViewerDock::SetCurrentNode(const Gex::NodeWkPtr& node)
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
        const Gex::NodeWkList& nodes)
{
    if (!autoUpdate)
        return;

    UpdateCurrentNode(nodes);
}


void Gex::Ui::ViewerDock::UpdateCurrentNode(const Gex::NodeWkList& nodes)
{
    Gex::NodeWkPtr node;
    for (const NodeWkPtr& n : nodes)
    {
        if (!n)
            continue;

        node = n;
        break;
    }

    SetCurrentNode(node);
}


Gex::Ui::ViewerWindow* Gex::Ui::ViewerDock::Extract()
{
    if (!viewer)
        return nullptr;

    stacked->removeWidget(viewer);

    auto* window = new ViewerWindow(viewer, parentWidget());
    window->show();

    auto curNode = viewer->CurrentNode();
    viewer = nullptr;

    SetCurrentNode(curNode);

    return window;
}


void Gex::Ui::ViewerDock::SetSelectionCallback(const SelectionGetter& callback)
{
    selCallback = callback;
}


Gex::NodeWkList Gex::Ui::ViewerDock::GetSelection()
{
    if (!selCallback)
        return {};

    return selCallback();
}


void Gex::Ui::ViewerDock::UpdateSelection()
{
    UpdateCurrentNode(GetSelection());
}

