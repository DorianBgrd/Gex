#ifndef GEX_NODEVIEWER_H
#define GEX_NODEVIEWER_H

#include "Gex/include/Gex.h"

#include "ui/include/api.h"


#include <QWidget>
#include <QDockWidget>
#include <QDialog>
#include <QStackedWidget>

namespace Gex::Ui
{
    struct ViewerCreator;

    class GEX_UI_API NodeViewer: public QWidget
    {
        friend ViewerCreator;

        Gex::Node* node = nullptr;
        int callbackIndex;

    protected:
        NodeViewer();

    protected:
        void ConnectToNode(Gex::Node* node);

        void DisconnectFromNode(Gex::Node* node);

    public:
        Gex::Node* CurrentNode() const;

        void SetCurrentNode(Gex::Node* node);

        virtual void Setup() = 0;

        virtual void OnNodeUpdated(Gex::Node* node) = 0;

        virtual void OnAttributeUpdated(
                Attribute* attribute,
                const AttributeChange& change) = 0;

        void closeEvent(QCloseEvent *event) override;
    };


    struct ViewerCreator
    {
        virtual NodeViewer* CreateViewer() const = 0;

        NodeViewer* MakeViewer(QWidget* parent=nullptr) const;
    };


    class GEX_UI_API ViewerRegistry
    {
        static ViewerRegistry* instance;
        std::map<std::string, ViewerCreator*> registry;

    public:
        void RegisterViewer(const std::string& type,
                            ViewerCreator* creator);

        template <class C>
        void RegisterViewer(const std::string& type)
        {
            RegisterViewer(type, new C());
        }

        NodeViewer* CreateViewer(const std::string& nodeType,
                                 QWidget* parent=nullptr);

        bool HasViewer(const std::string& type);

        static ViewerRegistry* GetRegistry();
    };


    class GEX_UI_API ViewerWindow: public QDialog
    {
        NodeViewer* v;
    public:
        ViewerWindow(NodeViewer* viewer, QWidget* parent=nullptr);
    };


    class GEX_UI_API ViewerDock: public QDockWidget
    {
        NodeViewer* viewer = nullptr;
        QWidget* emptyWidget;
        QStackedWidget* stacked;

    public:
        ViewerDock(QWidget* parent=nullptr);

        void OnNodeSelected(Gex::Node* node);

        void NodeSelectionChanged(const std::vector<Gex::Node*> nodes);

        void Extract() const;
    };
}


#define GENERATE_DEFAULT_VIEWER_CREATOR(builderClass, viewerClass)        \
    class builderClass: public Gex::Ui::ViewerCreator                     \
    {                                                                     \
        using Gex::Ui::ViewerCreator::ViewerCreator;                      \
                                                                          \
        Gex::Ui::NodeViewer* CreateViewer() const override                \
        {                                                                 \
            return new viewerClass();                                     \
        }                                                                 \
    };

#endif //GEX_NODEVIEWER_H
