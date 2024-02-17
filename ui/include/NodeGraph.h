#ifndef NODELIBRARY2COMPLETE_AUTOMATIONEDITORS_H
#define NODELIBRARY2COMPLETE_AUTOMATIONEDITORS_H

#include "api.h"
#include "BaseGraph.h"
#include "include/Gex.h"

#include <QObject>
#include <QVector>
#include <QWidget>
#include <QCheckBox>
#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QRectF>
#include <QMenu>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QPushButton>
#include <QLineEdit>
#include <QStackedWidget>


#define NODE_HEADER_HEIGHT 50
#define NODE_DEFAULT_WIDTH 100
#define ATTRIBUTE_HEIGHT 25
#define PLUG_RECT QRect(0, 0, 10, 10)



namespace Gex
{
    namespace Ui
    {
        class LinkItem;
        class NodeItem;
        class AttributeItem;

        class GEX_UI_API PlugItem: public QGraphicsEllipseItem
        {
            AttributeItem* attr;
            bool isInput;

        public:
            PlugItem(AttributeItem* attribute,
                     bool input=true);

            AttributeItem* Attribute() const;

            bool IsInputAnchor() const;

            bool IsOutputAnchor() const;
        };


        class GEX_UI_API MultiAttributeItem: public QGraphicsItem
        {
            qreal size = 10;
            qreal radius = 2;
            qreal margins = 2;
            bool hover = false;
            bool pressed = false;
            AttributeItem* attributeItem=nullptr;

            QColor backgroundColor = "#484848";
            QColor hoverBackgroundColor = "#686868";
            QColor pressedBackgroundColor = "#282828";

            QColor iconColor = "#E58C02";
            QColor hoverIconColor = "#F3A52B";
            QColor pressedIconColor = "#AB6E0F";

        public:
            MultiAttributeItem(AttributeItem* attribute);

            bool Collapsed() const;

            QRectF boundingRect() const override;

            void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

            void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

            void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

            void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

            void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
        };


        class NodeGraphScene;


        class GEX_UI_API AttributeItem: public QGraphicsRectItem
        {
            friend LinkItem;
            friend NodeItem;
            friend NodeGraphScene;

            Gex::Attribute* attribute;
            PlugItem* input;
            PlugItem* output;
            QGraphicsTextItem* text;
            MultiAttributeItem* multiButton = nullptr;
            QVector<AttributeItem*> indexedAttributes;
            QVector<AttributeItem*> childAttributes;
            qreal textIndent = 0;
            bool asInternal = false;

            AttributeItem* parentAttribute;
            NodeItem* parentNode;

            QVector<QPointer<LinkItem>> links;

            bool collapsed;
        public:
            AttributeItem(Gex::Attribute* attr,
                          AttributeItem* parent,
                          NodeItem* node=nullptr);

            AttributeItem(Gex::Attribute* attr,
                          NodeItem* node=nullptr);

            void Initialize(Gex::Attribute* attr,
                            AttributeItem* parent,
                            NodeItem* node);

            ~AttributeItem();

        protected:
            void RemoveLink(LinkItem* link);

            void SetTextIndent(qreal indent);

            void ShowAsInternal(bool internal);
        public:
            bool AsInternal() const;

            PlugItem* InputAnchor() const;

            PlugItem* OutputAnchor() const;

            NodeItem* ParentNode() const;

            void SetTextVisibility(bool visibility);

            bool TextVisibility() const;

            Gex::Attribute* Attribute() const;

            AttributeItem* GetSubAttribute(Gex::Attribute* attr) const;

//        void Adjust();
            void CreateAttributes();

            void RebuildAttributes();

            void InitializeLinks();

            QPointF SceneInputPosition() const;

            QPointF SceneOutputPosition() const;

            bool IsMulti() const;

            void Collapse();

            void Expand();

            bool Collapsed() const;

            qreal TotalHeight() const;

            QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

            void UpdateLinks();

            QList<QString> SubNames() const;

            QList<LinkItem*> SourceLinks() const;
        };


        class GEX_UI_API NodePlugItem: public QGraphicsEllipseItem
        {
            NodeItem* node;
            bool isInput;

        public:
            NodePlugItem(NodeItem* node,
                         bool input=true);

            NodeItem* Node() const;

            bool IsInputAnchor() const;

            bool IsOutputAnchor() const;
        };


        class GEX_UI_API NodeItem: public QGraphicsItem, public QObject
        {
            QVector<LinkItem*> links;
            QVector<AttributeItem*> attributes;
            Gex::Node* node;
            NodePlugItem* sourcePlug;
            NodePlugItem* destPlug;
            QGraphicsTextItem* title;
            bool showInternal = false;
            QColor customBorderColor;
            bool userCustomBorderColor = false;

        public:
            enum class AttributeVisibilityMode
            {
                All,
                Settable,
                Connected,
                OnlyInputs,
                OnlyOutputs,
                OnlyInternalInputs,
                OnlyInternalOutputs
            };

        protected:
            AttributeVisibilityMode attrVisMode;

        public:
            NodeItem(Gex::Node* node,
                     QGraphicsItem* parent=nullptr);

            ~NodeItem();

            void TitleChanged();

            Gex::Node* Node() const;

            void SetAttributeVisibility(AttributeVisibilityMode mode);

            AttributeVisibilityMode AttributeVisibility() const;

            void CreateAttributes();

            void RebuildAttributes();

            void PlaceAttributes();

            AttributeItem* FindAttribute(std::string longname) const;

            AttributeItem* GetAttribute(Gex::Attribute* attr) const;

            void InitializeLinks();

            QSize AdjustedSize() const;

            void AdjustSize();

            QRectF boundingRect() const override;

            QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

            QColor DefaultBorderColor() const;

            QColor DefaultSelectedBorderColor() const;

            void SetCustomBorderColor(QColor color);

            void ClearCustomBorderColor();

            QColor CustomBorderColor() const;

            bool HasCustomBorderColor() const;

            void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

            QList<QString> HiddenAttributes() const;

            void UpdateLinks();

        protected:
            void ShowAsInternal(bool internal);

        public:
            bool IsShowingInternal() const;

        public:
            void ShowAsInternalInput();

            void ShowAsInternalOutput();
        };


        class GEX_UI_API LinkItem: public QGraphicsPathItem, public QObject
        {
            AttributeItem* src;
            AttributeItem* dst;
            bool sourceInternal = false;
            bool destInternal = false;
        public:
            LinkItem(AttributeItem* source,
                     AttributeItem* dest);

            ~LinkItem();

            AttributeItem* Source() const;

            AttributeItem* Dest() const;

            void Update();

            void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

            QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

            bool operator==(LinkItem* other)
            {
                return (other->src == src && other->dst == dst);
            }
        };


        class GEX_UI_API PreviewLinkItem: public QGraphicsPathItem
        {
            PlugItem* anchorPlug;

        public:
            enum class State
            {
                Default,
                Valid,
                Invalid
            };

            PreviewLinkItem(PlugItem* anchorPlug);

            PlugItem* SourcePlug() const;

            Gex::Attribute* Attribute() const;

            void SetState(State);

            void Draw(QPointF dest);
        };


        struct NodeGraphContext
        {
            QString name;

            Gex::CompoundNode* compound = nullptr;

            std::function<std::vector<Gex::Node*>()> GetNodes;

            std::function<Gex::Node*(std::string, std::string)> CreateNode;

            std::function<void(Gex::Node*)> DeleteNode;

            QMap<Gex::Node*, QPointF> nodePositions;
        };


        class GEX_UI_API NodeGraphScene: public QGraphicsScene
        {
            Q_OBJECT

            PreviewLinkItem* previewLink;
            bool mouseZooming = false;
            bool mouseZoomingClicked = false;
            QPointF mouseZoomingPos;

            NodeGraphContext* graphContext;
            QMap<Gex::Node*, NodeItem*> nodeItems;
            NodeItem* input = nullptr;
            NodeItem* output = nullptr;
            LinkItem* pressedLink = nullptr;

        public:
            NodeGraphScene(QObject *parent=nullptr);

            ~NodeGraphScene();

            void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

            void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

            void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

            void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

            Q_SIGNAL void NodePlugClicked(NodePlugItem* clickedPlug);

            Q_SIGNAL void CompoundNodeDoubleClicked(Gex::CompoundNode*);

            void StartPlugLinking(PlugItem* source);

        public:
            void OnItemDoubleClicked(QGraphicsItem* item);

            void UpdateNode(Gex::Node* node);

            void UpdateNodeAttribute(Gex::Node* node,
                                     QString attribute);

            void StartMouseZooming();

            void EndMouseZooming();

            void Clear();

            void SwitchGraphContext(NodeGraphContext* context);

            void DeleteNode(NodeItem* item);

            void DeleteLink(LinkItem* item);

            void DeleteSelection();

            void CreateNode(QString type, QString name);

            void NodeEvaluationStarted(Gex::Node* node);

            void NodeEvaluationDone(Gex::Node* node, bool success);

            void ClearNodeEvaluation();

            void DisableInteraction();

            void EnableInteraction();

            void AutoLayoutNodes(const QPointF& destination,
                                 qreal horizontalSpacing=100,
                                 qreal verticalSpacing=100);
        };


        class GEX_UI_API NodeGraphView: public BaseGraphView
        {
        public:
            explicit
            NodeGraphView(QGraphicsScene* scene, QWidget* parent=nullptr);

        protected:
            struct ConnectCallback: public QObject
            {
                QMenu* mainMenu = nullptr;
                PlugItem* plug = nullptr;
                Gex::Ui::NodeGraphScene* scene = nullptr;

                void DoIt();
            };

        protected:
            void SetSelectedNodesVis(NodeItem::AttributeVisibilityMode mode);

            void SetSelectedNodesVisAll();

            void SetSelectedNodesVisSettable();

            void SetSelectedNodesVisConnected();

            QList<QGraphicsItem*> FilterSelectedItems(
                    const QList<QGraphicsItem*> items) const
            override;

            QMenu* GetMenu() override;

            void keyPressEvent(QKeyEvent* event) override;

        public:
            void OnNodePlugClicked(NodePlugItem* plug);
        };


        class GEX_UI_API ClickableMenu: public QMenu
        {
        Q_OBJECT

        public:
            using QMenu::QMenu;

            void mouseReleaseEvent(QMouseEvent* event) override;

            Q_SIGNAL void clicked();
        };


        class GEX_UI_API ContextButton: public QPushButton
        {
        Q_OBJECT

        protected:
            unsigned int index;
            std::string name;

        public:
            ContextButton(std::string name, unsigned int index,
                          QWidget* parent=nullptr);

            Q_SIGNAL void ContextClicked(unsigned int index);

            void SignalClicked(bool);
        };


        class GEX_UI_API ContextsWidget: public QWidget
        {
        Q_OBJECT

            QList<ContextButton*> contexts;
            QHBoxLayout* buttonsLayout;

        public:
            ContextsWidget(QWidget* parent = nullptr);

            void AddContext(std::string name);

            void ContextClicked(unsigned int index);

            Q_SIGNAL void ContextRequested(int index);

            void RemoveContexts(unsigned int number);
        };


        class AttributeEditor;


        class GEX_UI_API GraphWidget: public QDialog
        {
            Q_OBJECT

            Gex::AutomationGraph* automation;
            Gex::Graph* graph;
            NodeGraphScene* scene = nullptr;
            NodeGraphView* view = nullptr;
            AttributeEditor* editor = nullptr;
            QList<NodeGraphContext*> contexts;
            ContextsWidget* contextsWidget;

        public:
            GraphWidget(Gex::Graph* graph,
                        QWidget* parent=nullptr);

            ~GraphWidget();

            void Initialize();

            void Save() const;

            void OnNodeSelected();

            void RegisterContext(Gex::CompoundNode* compound);

            void SwitchContext(unsigned int index);

        public:

//        void mouseReleaseEvent(QMouseEvent* event) override;

            void UpdateNode(Gex::Node* node);

            void UpdateNodeAttribute(Gex::Node* node,
                                     QString attribute);

            void DisableInteraction();

            void EnableInteraction();

            void RunGraph();

            void InteractiveRun();

            void AutoLayoutNodes();

            void ShowMessage(Gex::Ui::UiFeedback feedback);
        };
    }
}

#endif //NODELIBRARY2COMPLETE_AUTOMATIONEDITORS_H
