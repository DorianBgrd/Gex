#ifndef NODELIBRARY2COMPLETE_AUTOMATIONEDITORS_H
#define NODELIBRARY2COMPLETE_AUTOMATIONEDITORS_H

#include "api.h"
#include "BaseGraph.h"
#include "Gex/include/Gex.h"

#include <QObject>
#include <QVector>
#include <QWidget>
#include <QCheckBox>
#include <QSpinBox>
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
            enum {
                Type = UserType + 1
            };

            int type() const override
            {
                return Type;
            }

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
            enum {
                Type = UserType + 2
            };

            int type() const override
            {
                return Type;
            }

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


        class GEX_UI_API TitleDoc: public QTextDocument
        {
            Q_OBJECT
        public:
            TitleDoc(QObject* parent=nullptr);

            void Validate();

            Q_SIGNAL
            void Validated(const QString& text);
        };


        class GEX_UI_API NodeNameItem: public QGraphicsTextItem
        {
            TitleDoc* doc;
            QString fallbackText;
        public:
            NodeNameItem(QGraphicsItem* parent=nullptr);

            void focusInEvent(QFocusEvent *event) override;

            void focusOutEvent(QFocusEvent *event) override;

            void keyReleaseEvent(QKeyEvent *event) override;

            TitleDoc* TitleDocument() const;
        };


        class GEX_UI_API AttributeItem: public QGraphicsRectItem, public QObject
        {
            friend LinkItem;
            friend NodeItem;
            friend NodeGraphScene;

            Gex::AttributeWkPtr attribute;
            PlugItem* input;
            PlugItem* output;
            QGraphicsTextItem* text;
            MultiAttributeItem* multiButton = nullptr;
            QVector<AttributeItem*> indexedAttributes;
            QVector<AttributeItem*> childAttributes;
            qreal textIndent = 0;
            qreal defaultHeight;
            QRect plugRect;
            bool asInternal = false;

            AttributeItem* parentAttribute;
            NodeItem* parentNode;

            QVector<QPointer<LinkItem>> links;

            bool collapsed;
        public:
            enum {
                Type = UserType + 3
            };

            int type() const override
            {
                return Type;
            }

            AttributeItem(Gex::AttributeWkPtr attr,
                          AttributeItem* parent,
                          NodeItem* node=nullptr);

            AttributeItem(Gex::AttributeWkPtr attr,
                          NodeItem* node=nullptr);

            void Initialize(Gex::AttributeWkPtr attr,
                            AttributeItem* parent,
                            NodeItem* node);

            ~AttributeItem();

        protected:
            void RemoveLink(LinkItem* link);

            void SetTextIndent(qreal indent);

            void ShowAsInternal(bool internal);
        public:
            NodeGraphScene* GraphScene() const;

            bool AsInternal() const;

            PlugItem* InputAnchor() const;

            PlugItem* OutputAnchor() const;

            NodeItem* ParentNode() const;

            void SetTextVisibility(bool visibility);

            bool TextVisibility() const;

            Gex::AttributeWkPtr Attribute() const;

            AttributeItem* GetSubAttribute(Gex::AttributeWkPtr attr) const;

            void CreateAttributes();

            void RebuildAttributes();

            void InitializeLinks();

            void ClearLinks(bool recursive=false);

            QPointF SceneInputPosition() const;

            QPointF SceneOutputPosition() const;

            bool IsMulti() const;

            void Collapse();

            void Expand();

            void AdjustPlacement();

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
            enum {
                Type = UserType + 4
            };

            int type() const override
            {
                return Type;
            }

            NodePlugItem(NodeItem* node,
                         bool input=true);

            NodeItem* Node() const;

            bool IsInputAnchor() const;

            bool IsOutputAnchor() const;
        };


        class GEX_UI_API NodeItem: public QGraphicsObject
        {
            friend AttributeItem;

            QVector<LinkItem*> links;
            QVector<AttributeItem*> attributes;
            QVector<AttributeItem*> allAttributes;
            Gex::NodePtr node;
            NodePlugItem* sourcePlug;
            NodePlugItem* destPlug;
            NodeNameItem* title;
            QGraphicsTextItem* internalTitle;
            NodeGraphScene* graphScene;
            bool showInternal = false;
            QColor customBorderColor;
            bool userCustomBorderColor = false;
            qreal attributesY;

            Gex::CallbackId cbIndex;
            Gex::CallbackId delIndex;
            bool cbConnected = false;

            bool savePosition = true;
            bool positionChanged = false;

        protected:
            void RegisterAttribute(AttributeItem* item);

            void DeregisterAttribute(AttributeItem* item);

        public:
            static qreal defaultCompTitleOffset;
            static qreal defaultWidth;
            static qreal defaultSpacing;
            static qreal defaultFooter;

        public:
            static qreal DefaultWidth();

            static void SetDefaultWidth(qreal width);

            static qreal DefaultTitleOffset();

            static void SetDefaultTitleOffset(qreal offset);

            static qreal DefaultSpacing();

            static void SetDefaultSpacing(qreal spacing);

            static qreal DefaultFooter();

            static void SetDefaultFooter(qreal footer);

        public:
            enum {
                Type = UserType + 5
            };

            int type() const override
            {
                return Type;
            }

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
            explicit
            NodeItem(Gex::NodePtr node, NodeGraphScene* scene,
                     QGraphicsItem* parent=nullptr);

            void ConnectToNode();

            void DisconnectFromNode();

            ~NodeItem();

            void TitleChanged(const QString& text);

            Gex::NodePtr Node() const;

            NodeGraphScene* GraphScene() const;

            void SetAttributeVisibility(AttributeVisibilityMode mode);

            AttributeVisibilityMode AttributeVisibility() const;

            void CreateAttributes();

            void RebuildAttributes();

            void PlaceAttributes();

            AttributeItem* FindAttribute(std::string longname) const;

            AttributeItem* GetAttribute(Gex::AttributeWkPtr attr) const;

            void InitializeLinks();

            QSize AdjustedSize() const;

            void AdjustSize();

            QRectF boundingRect() const override;

            void SavePosition(QPointF pos);

            void RestorePosition();

            void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

            void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

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
            QPointer<AttributeItem> src;
            QPointer<AttributeItem> dst;
            bool sourceInternal = false;
            bool destInternal = false;
        public:
            enum {
                Type = UserType + 6
            };

            int type() const override
            {
                return Type;
            }

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
            enum {
                Type = UserType + 7
            };

            int type() const override
            {
                return Type;
            }

            enum class State
            {
                Default,
                Valid,
                Invalid
            };

            PreviewLinkItem(PlugItem* anchorPlug);

            PlugItem* SourcePlug() const;

            Gex::AttributeWkPtr Attribute() const;

            void SetState(State);

            void Draw(QPointF dest);
        };


        class GEX_UI_API FrameItem: public QGraphicsObject
        {
        public:
            enum {
                Type = UserType + 125
            };

            int type() const override
            {
                return Type;
            }

        private:
            QGraphicsTextItem* textItem;
            QGraphicsRectItem* resizeHandle;

            QPointF originPos;
            QList<QPointer<QGraphicsObject>> items;
            QMap<QPointer<QGraphicsObject>, QPointF> movedItemsPositions;

            QRect rect;

            QRectF sceneRect;
            QColor background;
            QColor textColor;

            enum class Grip
            {
                None,
                TopLeft,
                TopRight,
                BottomLeft,
                BottomRight
            };

            int gripWidth = 10;
            int gripHeight = 10;

            bool moveItems = true;
            Grip heldGrip = Grip::None;

            NodeGraphScene* graphScene;

        public:
            FrameItem(const QPoint& topLeft, qreal width,
                      qreal height, NodeGraphScene* graphScene,
                      QGraphicsItem* parent=nullptr);

            QRect Rect() const;

            void SetRect(const QRect& rect);

            bool Active() const;

            void SetActive(bool active);

            QList<QPointer<QGraphicsObject>> ContainedItems() const;

            void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

            QRectF boundingRect() const;

            QColor BackgroundColor() const;

            void SetBackgroundColor(const QColor& color);

            void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

            void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

            void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

        protected:
            Grip CurrentGrip(QPointF pos) const;

            void Resize(QRectF resize);
        public:

            void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

            void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

            void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

            QVariant itemChange(QGraphicsItem::GraphicsItemChange change,
                                const QVariant &value) override;

            std::string Save() const;

            void Restore(const std::string& d);
        };


        class GEX_UI_API FrameEditDialog: public QDialog
        {
            FrameItem* item;
        public:
            FrameEditDialog(FrameItem* item, QWidget* widget=nullptr);
        };


        class NodeGraphContext
        {
            QString name;
            Gex::CompoundNodePtr node;

        public:
            NodeGraphContext(const QString& name, Gex::CompoundNodePtr node);

            QString Name() const;

            Gex::CompoundNodePtr Compound() const;

            Gex::NodeList GetNodes() const;

            Gex::NodePtr CreateNode(std::string, std::string);

            Gex::NodePtr ReferenceNode(std::string, std::string);

            bool DeleteNode(Gex::NodePtr);

            Gex::NodeList DuplicateNodes(Gex::NodeList nodes, bool copyLinks);
        };


        class GEX_UI_API NodeGraphScene: public QGraphicsScene
        {
            Q_OBJECT

            bool pressed = false;

            PreviewLinkItem* previewLink;
            bool mouseZooming = false;
            bool mouseZoomingClicked = false;
            QPointF mouseZoomingPos;

            bool creatingFrame = false;
            QPointF frameTopLeft;

            NodeGraphContext* graphContext = nullptr;
            QMap<Gex::NodePtr, NodeItem*> nodeItems;
            QList<FrameItem*> frames;
            NodeItem* input = nullptr;
            NodeItem* output = nullptr;
            LinkItem* pressedLink = nullptr;

            QPen previewFramePen;
            QBrush previewFrameBrush;
            QPointF previewFrameStart;
            QGraphicsRectItem* previewFrame = nullptr;

        public:
            NodeGraphScene(QObject *parent=nullptr);

            void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

            void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

            void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

            void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

            Q_SIGNAL void NodePlugClicked(NodePlugItem* clickedPlug);

            Q_SIGNAL void CompoundNodeDoubleClicked(Gex::CompoundNodePtr);

            Q_SIGNAL void NodeModified(const Gex::NodeWkPtr& node,
                                       const Gex::AttributeWkPtr& attr,
                                       const Gex::AttributeChange& change);

            void StartPlugLinking(PlugItem* source);

        public:
            void OnItemDoubleClicked(QGraphicsItem* item);

            void StartFrameMode();

            void StopFrameMode();

            void SaveFrames();

            void RestoreFrames();

            FrameItem* CreateFrame(QPointF topLeft, QPointF bottomRight);

            QList<FrameItem*> FrameItems() const;

            QList<FrameItem*> SortedFrameItems(const QRectF& rect) const;

            void UpdateNode(Gex::NodePtr node);

            void OnNodeModified(const Gex::NodeWkPtr& node,
                                const Gex::AttributeWkPtr& attr,
                                const Gex::AttributeChange& change);

            void UpdateNodeAttribute(const Gex::NodePtr& node,
                                     const QString& attribute);

            NodeItem* GetInput() const;

            NodeItem* GetOutput() const;

            void Clear();

            void SwitchGraphContext(NodeGraphContext* context);

            void DeleteNode(NodeItem* item);

            void DuplicateNodes(std::vector<Gex::NodePtr> nodes, bool copyLinks);

            QList<NodeItem*> NodeItems() const;

            QList<NodeItem*> SelectedNodeItems() const;

            Gex::NodeList SelectedNodes() const;

            void SelectNodes(const Gex::NodeWkList& sel,
                             bool notify=true);

            NodeGraphContext* CurrentContext() const;

            NodeItem* ItemFromNode(const Gex::NodeWkPtr& node) const;

            NodeItem* ItemFromNode(const Gex::NodePtr& node) const;

            void DuplicateSelectedNodes(bool copyLinks);

            void DuplicateSelectedNodesNoLinks();

            void DuplicateSelectedNodesAndLinks();

            void ConvertSelectedNodesToCompound();

            void DeleteLink(LinkItem* item);

            void DeleteFrame(FrameItem* item);

            void DeleteSelection();

            void CreateNode(QString type, QString name);

            void ReferenceNode(QString type, QString name);

            void NodeEvaluationStarted(const Gex::NodePtr& node);

            void NodeEvaluationDone(const Gex::NodePtr& node, bool success);

            void ClearNodeEvaluation();

            void DisableInteraction();

            void EnableInteraction();

            void AutoLayoutNodes(const QPointF& destination,
                                 qreal horizontalSpacing=100,
                                 qreal verticalSpacing=100);
        };


        class ConnectionContext: public ViewContext
        {
            PreviewLinkItem* previewLink = nullptr;
            NodeItem* input = nullptr;
            NodeItem* output = nullptr;

            QPointF ScenePos(QPoint viewPos);

            QGraphicsItem* SceneItem(QPoint viewPos);

            bool AcceptBaseEvents() const override;

            void Setup(BaseGraphView* scene) override;

            void OnPressEvent(QMouseEvent* event) override;

            void OnMoveEvent(QMouseEvent* event) override;

            void OnReleaseEvent(QMouseEvent* event) override;
        };


        class GEX_UI_API NodeGraphView: public BaseGraphView
        {
            Q_OBJECT

            NodeGraphScene* graphScene;
        public:
//            NodeGraphView(const NodeGraphView& other);

            explicit
            NodeGraphView(NodeGraphScene* scene, QWidget* parent=nullptr);

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
                    const QList<QGraphicsItem*> items)
                    const override;

            void ExportSelectedNodes();

            void ExportSelectedNodesAsCompound();

            QMenu* GetMenu() override;

            void keyPressEvent(QKeyEvent* event) override;

            void mousePressEvent(QMouseEvent *event) override;

            void mouseReleaseEvent(QMouseEvent *event) override;

        public:
            NodeItem* GetInput() const;

            NodeItem* GetOutput() const;

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
            ContextButton(const std::string& name, unsigned int index,
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

            void AddContext(const std::string& name);

            void ContextClicked(unsigned int index);

            Q_SIGNAL void ContextRequested(int index);

            void RemoveContexts(unsigned int number);

            void ClearContexts();
        };


        class AttributeEditor;


        class GEX_UI_API GraphWidget: public QWidget
        {
            Q_OBJECT

            Gex::CompoundNodePtr graph;
            Gex::Profiler profiler;
            NodeGraphScene* scene = nullptr;
            NodeGraphView* view = nullptr;
            AttributeEditor* editor = nullptr;
            QList<NodeGraphContext*> contexts;
            ContextsWidget* contextsWidget;
            QSpinBox* threadsSpinBox;
            Gex::NodeEvaluator* interactiveEval = nullptr;
            bool interactiveEvalEnabled = true;

            static QEvent::Type scheduleEventType;

        public:
            GraphWidget(const Gex::CompoundNodePtr& graph,
                        QWidget* parent=nullptr);

            ~GraphWidget() override;

            void Initialize();

            void OnNodeSelected();

            void RegisterContext(const Gex::CompoundNodePtr& compound);

            void SwitchContext(unsigned int index);

            void SwitchGraph(const Gex::CompoundNodePtr& graph);

        protected:
            void ClearContexts();

        public:
            bool InteractiveEvaluation() const;

            void ToggleInteractiveEvaluation(bool state);

            void InitIdlePrepare();

            bool event(QEvent *event) override;

//        void mouseReleaseEvent(QMouseEvent* event) override;

            void UpdateNode(const Gex::NodePtr& node);

            void UpdateNodeAttribute(const Gex::NodePtr& node,
                                     const QString& attribute);

            void DisableInteraction();

            void EnableInteraction();

            Gex::Profiler GetProfiler() const;

            void RunGraph();

            void OnNodeChanged(const Gex::NodeWkPtr& node,
                               const Gex::AttributeWkPtr& attr,
                               const Gex::AttributeChange& change);

            void RunFromNode(const Gex::NodePtr& node);

            void InteractiveRun();

            void AutoLayoutNodes();

            void ShowMessage(const Gex::Ui::UiFeedback& feedback);

            Q_SIGNAL void GraphEvaluated(const Gex::Profiler profiler);

            Q_SIGNAL void SelectedNodeChanged(const Gex::NodeWkList& nodes);

            void Select(const Gex::NodeWkList& nodes, bool notify=true);

            Gex::NodeWkList CurrentSelection() const;
        };
    }
}

#endif //NODELIBRARY2COMPLETE_AUTOMATIONEDITORS_H
