#ifndef GEX_NODE
#define GEX_NODE

#include "api.h"
#include "defs.h"
#include "Metadatas.h"
#include "boost/python.hpp"
#include "NodeAttributeData.h"
#include "Attribute.h"
#include "Profiler.h"
#include "Scheduling.h"

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "rapidjson/document.h"


namespace Gex
{
    class Attribute;
	class DefaultNodeBuilder;
	class CompoundNodeBuilder;
	class CompoundNode;
	class NodeFactory;
	class NodeBuilder;
	class NodeAttributeData;
	class EvaluationContext;
    class ScheduledNode;
    class NodeEvaluator;


    class GEX_API GraphContext
    {
        friend NodeEvaluator;
        std::vector<std::string> resources;

    public:
        GraphContext();

        GraphContext(const GraphContext& other);

        void RegisterResource(std::string resource);

        std::vector<std::string> Resources() const;
    };


    enum class NodeChange
    {
        ChildNodeAdded,
        ChildNodeRemoved,
        AttributeAdded,
        AttributeRemoved,
        Deleted
    };


	class GEX_API Node: public std::enable_shared_from_this<Node>
	{
    protected:
        friend CompoundNode;
        friend Attribute;
		friend NodeFactory;
        friend NodeBuilder;
		friend DefaultNodeBuilder;
		friend CompoundNodeBuilder;
		friend NodeAttributeData;
        friend EvaluationContext;
        friend NodeEvaluator;

        bool initializing;
        bool isEditable = true;
        CallbackId nextId = 0;

        /**
         * Starts node initialization.
         */
        void StartInitialization();

        /**
         * Ends node initialization.
         */
        void EndInitialization();

		std::string nodeName;
        std::string typeName;
        std::string pluginName;
//		std::map<std::string, AttributePtr> attributes;
        std::vector<AttributePtr> attributes;

        std::vector<std::string> resources;

        NodeWkPtr parent;
        AttributeWkPtr previous;
        AttributeWkPtr next;

        ScheduleNodePtrList scheduledNodes;
        bool isScheduled = false;

        CallbackId invalidCbId = 0;
        CallbackId scheduleCbId = 0;
        CallbackId deleteCbId = 0;
        CallbackId changeCbId = 0;

        InvalidateCallbacks invalidateCallbacks;
        NodeChangedCallbacks changeCallbacks;
        ScheduleCallbacks scheduleCallbacks;
        AttributeChangedCallbacks attrCallbacks;
        AboutToBeDeletedCallbacks deleteCallbacks;

        NodeMetadata metadata;

        std::string referencePath;

	public:
        /**
         * Returns whether this node
         * is referenced.
         * @return is referenced.
         */
        bool IsReferenced() const;

        /**
         * Returns reference path.
         * @return path.
         */
        std::string ReferencePath() const;

    protected:
        /**
         * Sets current node reference path.
         * @param path: ref path.
         */
        void SetReferencePath(const std::string& path);

    public:

        /**
         * Returns node's type name.
         * @return std::string type.
         */
		virtual std::string Type() const;

        /**
         * Returns node plugin name, if it
         * comes from a custom plugin.
         * @return std::string: plugin name.
         */
        virtual std::string Plugin() const;

        /**
         * Returns node's description.
         * @return std::string description.
         */
        virtual std::string Description() const;

        /**
         * Returns node's identifier.
         * @return std::string identifier.
         */
        std::string Name() const;

        /**
         * Returns node path.
         * @return path.
         */
        std::string Path() const;

        /**
         * Returns whether node is in its initialization
         * phase.
         * @return bool: is initializing.
         */
        bool IsInitializing() const;

        /**
         * Get Metadata value for specified name.
         * @tparam M: Metadata value type.
         * @param name: Metadata name.
         * @param res: Result of the operation.
         * @return resulting value.
         */
        template<class M>
        M GetMetadata(std::string name, Feedback* res=nullptr) const
        {
            return metadata.Get<M>(name, res);
        }

        /**
         * Sets metadata value for specified name.
         * @tparam M: Metadata value type.
         * @param name: Metadata name.
         * @param value: Value to set.
         * @return Operation success.
         */
        template<class M>
        bool SetMetadata(std::string name, M value)
        {
            return metadata.Set<M>(name, value);
        }

    protected:
        /**
         * Sets node type name.
         * @param typeName: name.
         */
        void SetType(std::string typeName);

        /**
         * Sets node plugin name.
         * @param pluginName: plugin name.
         */
        void SetPlugin(std::string pluginName);

    public:
        /**
         * Sets node identifier.
         * @param std::string p: node identifier.
         */
		std::string SetName(const std::string& p);

        CallbackId RegisterAttributeCallback(
                AttributeChangeCallback cb);

        bool DeregisterAttributeCallback(CallbackId index);

        CallbackId RegisterAboutToBeDeletedCallback(
                AboutToBeDeletedCallback cb);

        bool DeregisterAboutToBeDeletedCallback(
                CallbackId id);

        CallbackId RegisterNodeChangedCallback(
                NodeChangeCallback cb);

        bool DeregisterNodeChangedCallback(
                CallbackId index);

        void SignalChange(const NodeChange& change, const NodeWkPtr& node) const;

    protected:
		// Adds attribute to node.
        /**
         * Registers attribute.
         * @param Attribute attr: attribute.
         * @return bool success.
         */
		bool AddAttribute(const AttributePtr& attr);

        bool RemoveAttribute(const AttributePtr& attr);

        /**
         * Default constructor.
         */
    public:
        explicit
		Node(const NodePtr& parent=nullptr);

        // Non copyable.
        Node(const Node& other) = delete;

    protected:
        void SetParent(const NodePtr& parent);

        /**
         * Creates builtins attributes :
         * Self, Previous, Next.
         */
        void CreateBuiltinsAttributes();

        /**
         * Initializes node.
         */
         void Initialize();

        /**
         * Creates node default attributes.
         * Should be overriden in node types.
         */
        virtual void InitAttributes();

        /**
         * Signals changes.
         * @param attribute: attribure ptr.
         * @param change: change that occured.
         */
        void SignalAttributeChange(const AttributePtr& attribute,
                                   const AttributeChange& change);

        /**
         * Member function called when an attribute changes.
         * @param Attribute attribute: attribute that changed.
         */
        virtual void AttributeChanged(const AttributePtr& attribute,
                                      const AttributeChange& change);

	public:
		// Destructor.
		virtual ~Node();

        NodeWkPtr Parent() const;

		bool operator ==(const Node& other);

		bool operator ==(const Node* other);

        bool operator ==(const NodePtr& other);

        bool operator ==(const NodeWkPtr& other);

	public:

         /**
          * Returns whether node is editable.
          * @return bool can be edited.
          */
         virtual bool IsEditable() const;

    protected:
         /**
          * Sets node editable.
          * @param bool editable: set editable state.
          * @return bool editable.
          */
         virtual void SetEditable(bool editable);

    public:
         /**
          * Returns whether current node is a
          * compound node.
          * @return is compound.
          */
         virtual bool IsCompound() const;

    private:
        std::any InitValue(const std::type_info& t);

    public:

        /**
         * Creates new attribute.
         * @tparam T type: attribute value type.
         * @param std::string name: attribute name.
         * @param AttrValueType valueType: value type.
         * @param AttrType type: attribute type.
         * @param Attribute* parent: parent attribute.
         * @return Attribute* created attribute.
         */
		template<class T>
		AttributeWkPtr CreateAttribute(const std::string& name,
                                       const AttrValueType& valueType = AttrValueType::Single,
                                       const AttrType& type = AttrType::Static,
                                       const AttributeWkPtr& parent = AttributeWkPtr())
		{
            return CreateAttributeFromValue(name, InitValue(typeid(T)),
                                            valueType, type, parent);
		}


		AttributeWkPtr CreateAttributeFromValue(const std::string& name, const std::any& v,
                                                const AttrValueType& valueType = AttrValueType::Single,
                                                const AttrType& type = AttrType::Static,
                                                const AttributeWkPtr& parent=AttributeWkPtr());


        AttributeWkPtr CreateAttributeFromTypeName(const std::string& name, const std::string& apiName,
                                                   const AttrValueType& valueType = AttrValueType::Single,
                                                   const AttrType& type = AttrType::Static,
                                                   const AttributeWkPtr& parent = AttributeWkPtr());


        AttributeWkPtr CreateAttribute(const std::string& name,
                                       const AttrType& type = AttrType::Static,
                                       bool multi=false,
                                       const AttributeWkPtr& parent = AttributeWkPtr());

		/**
		 * Returns whether attribute with name exists.
		 * @returns bool exists.
		 */
		bool HasAttribute(const std::string& name) const;

        /**
         * Gets Attribute pointer from name,
		 * Returns nullptr if not found.
         * @param std::string name: attribute name.
         * @return Attribute* attribute.
         */
		AttributeWkPtr GetAttribute(const std::string& name) const;

        /**
         * Returns all attributes pointers.
         * @return std::vector<Attribute*> attributes.
         */
        AttributeWkList GetAttributes() const;

        /**
         * Returns all attributes pointers, including
         * sub and child attributes.
         * Warning : sub attributes may be removed or
         * changed, be careful while using those
         * pointers
         * @return std::vector<Attribute*> attributes.
         */
        AttributeWkList GetAllAttributes() const;

        /**
         * Returns all user defined attributes pointers.
         * @return std::vector<Attribute*> attributes.
         */
        AttributeWkList ExtraAttributes();

        /**
         * Returns input node pointers, if materialized.
         * @return std::vector<Node**> nodes.
         */
        NodeWkList UpstreamNodes();

        /**
         * Returns Output node pointers, if materialized.
         * @return std::vector<Node**> nodes.
         */
        NodeWkList DownstreamNodes();

		/**
		 * Serializes node to json values.
		 */
		virtual void Serialize(rapidjson::Value& dict,
                               rapidjson::Document& json) const;

        /**
		 * Deserializes node from json values.
		 */
        virtual bool Deserialize(rapidjson::Value& dict);

    protected:
        /**
         * Performs factory reset on all attributes.
         */
        void FactoryReset();

        /**
         * Pulls node upstream attributes.
         * @return bool success.
         */
        bool PullAttributes();

        /**
         * Retrieves an unique name from provided node name.
         * @param name: input name.
         * @return unique name.
         */
        virtual std::string UniqueName(const std::string& name) const;

    public:

		/**
		 * Pulls sources.
		 * When pulling, node "chain" will be recursed
		 * to find all dynamic nodes, and will compute
		 * them.
		 * It will pull all static node values.
		 * @return bool success.
		 */
		bool Pull();

    protected:
        /**
         * Creates an evaluation context.
         * @return NodeAttributeData evaluation context.
         */
        NodeAttributeData CreateEvalContext();

    public:
        /**
         * Computes node.
         * @return bool success.
         */
        virtual bool Compute(GraphContext &context,
                             NodeProfiler& profiler);

        /**
         * Runs the node as a graph.
         * This function schedules subsequent nodes and
         * launches an evaluator. Launching this function
         * on a default node would provide a scheduling
         * and calculation overhead, prefer using the
         * Compute() member function for such use case.
         *
         * @param profile: input profiler.
         * @param threads: thread number.
         * @param nodeStarted: callback triggered on each
         *  node start.
         * @param nodeDone: callback triggered on each
         *  node end.
         * @param evalDone: callback triggered when overall
         *  evaluation is done.
         * @return
         */
        bool Run(Profiler& profile, unsigned int threads=1,
                 NodeCallback nodeStarted=nullptr,
                 NodeResCallback nodeDone=nullptr,
                 GraphCtxCallback evalDone=nullptr);

    private:
        void FinalizeEvaluation(const GraphContext& context) const;

    protected:
        /**
         * Computation function.
         * This function should be overrided to provide new
         * node types calculations.
         * @param context Node context.
         * @return bool success.
         */
        virtual bool Evaluate(NodeAttributeData &context,
                              GraphContext& graphContext,
                              NodeProfiler& profiler);

    public:
        /**
         * Returns whether node is currently scheduled. Base implementation
         * of this function returns true, as a 'single' node does not need to
         * perform complex scheduling, as opposed to compound nodes.
         * @return bool: is scheduled.
         */
        bool IsScheduled() const;

        /**
         * Declares scheduling as invalid, which will cause re-scheduling at
         * next evaluation. Default node implementation triggers invalidation
         * when a new connection is made.
         */
        void InvalidateScheduling();

    protected:
        /**
         * Declares node as re-scheduled.
         */
        void ValidateScheduling();

        virtual void Schedule();

    public:
        ScheduledNodePtr ToScheduledNode();

        virtual ScheduleNodePtrList ToScheduledNodes();

    public:
        CallbackId AddInvalidateCallback(InvalidateCallback callback);

        void RemoveInvalidateCallback(CallbackId callback);

        void ClearInvalidateCallbacks();

        CallbackId AddScheduleCallback(ScheduleCallback callback);

        void RemoveScheduleCallback(CallbackId callback);

        void ClearScheduleCallbacks();

    public:

		template<class T>
        static std::shared_ptr<T> ConvertTo(const NodePtr& node)
        {
            return std::dynamic_pointer_cast<T>(node);
        }
	};


    class CompoundPreScheduledNode: public ScheduledNode
    {
        using ScheduledNode::ScheduledNode;

        bool Evaluate(Gex::GraphContext &context,
                      Gex::NodeProfiler &profiler)
                      override;
    };


    class CompoundPostScheduledNode: public Gex::ScheduledNode
    {
        using ScheduledNode::ScheduledNode;

        bool Evaluate(Gex::GraphContext &context,
                      Gex::NodeProfiler &profiler)
                      override;
    };


    class GEX_API CompoundNode : public Node
	{
	private:
		friend CompoundNodeBuilder;
        friend NodeAttributeData;
        friend CompoundPreScheduledNode;
        friend CompoundPostScheduledNode;
        friend NodeEvaluator;

        NodeList nodes;

	public:
        explicit
        CompoundNode(const NodePtr& parent=nullptr);

        CompoundNode(const CompoundNode& other) = delete;

        ~CompoundNode() override;

		std::string Description() const override
        {
            return "Base compound node";
        }

        bool IsCompound() const override;

        std::string UniqueName(const std::string& name) const;

        NodeList InternalNodes() const;

        std::vector<std::string> InternalNodeNames() const;

        /**
         * Creates a new internal node.
         * @param type: type name.
         * @param name: node name.
         * @return node pointer.
         */
        virtual NodePtr CreateNode(const std::string& type, std::string name= std::string());

        /**
         * References a new internal node.
         * @param path: reference path.
         * @return referenced node.
         */
        virtual NodePtr ReferenceNode(const std::string& path, std::string name=std::string());

    private:
        virtual bool _AddNode(const NodePtr& node);

    public:
        /**
         * Adds node pointer as internal node.
         * @param node: node pointer.
         * @return success.
         */
        virtual bool AddNode(const NodePtr& node);

    private:
        bool _RemoveNode(const NodePtr& node);

        void RemoveNodeWithUndo(const NodePtr& node);

    public:
        bool CanRemoveNode() const;

        /**
         * Removes internal node.
         * @param node: node to remove.
         * @return success.
         */
        virtual bool RemoveNode(const NodePtr& node);

        /**
         * Removes internal node using its name.
         * @param node: nod name.
         * @return success.
         */
        virtual bool RemoveNode(const std::string& node);

        /**
         * Returns node pointer with name if it exists.
         * @param node: node name.
         * @return node pointer.
         */
        virtual NodeWkPtr GetNode(const std::string& node) const;

        NodeList GetNodes() const;

        std::vector<std::string> GetNodeNames() const;

        AttributeWkPtr FindAttribute(const std::string& attr) const;

        /**
         * Returns whether specified node pointer is an internal
         * node.
         * @param node: node pointer.
         * @return is internal node.
         */
        virtual bool HasNode(const NodePtr& node) const;

        virtual bool HasNode(const NodeWkPtr& node) const;

        /**
         * Returns whether specified node name is an internal node.
         * @param node: node name.
         * @return is internal node.
         */
        virtual bool HasNode(const std::string& node) const;

        /**
         * Returns top level internal attributes.
         * @return std::vector<Attribute*> internal attributes.
         */
        AttributeList InternalAttributes() const;

        /**
         * Returns all internal attributes, including child / indices
         * attributes.
         * @return std::vector<Attribute*> internal attributes.
         */
        AttributeList AllInternalAttributes() const;

    private:

        /**
         * Function launched by the Compute() function, just
         * before the Evaluate() function is called.
         *
         * @param ctx: Node evaluation context.
         * @return bool: success.
         */
        virtual bool PreEvaluate(NodeAttributeData& ctx,
                                 GraphContext &graphContext,
                                 NodeProfiler& profiler);

        /**
         * Function launched by the Compute() function, just
         * after the Evaluate() function is called.
         *
         * @param ctx: Node evaluation context.
         * @return bool: success.
         */
        bool PreCompute(GraphContext &context,
                        NodeProfiler& profiler);

    protected:
        /**
         * Evaluates compound node inner nodes, starting from
         * outputs to recurse upstream nodes.
         * Note : A node added to a compound but not conntected
         * to any other nodes, and not connected to inner outputs
         * will not be evaluated.
         *
         * @param ctx: node evaluation context.
         * @return bool: success.
         */
		bool Evaluate(NodeAttributeData &ctx,
                      GraphContext &graphContext,
                      NodeProfiler& profiler)
                      override;

    private:
        /**
         * Function launched by the Compute() function, just
         * after the Evaluate() function is called.
         *
         * @param ctx node evaluation context.
         * @return bool: success.
         */
        virtual bool PostEvaluate(NodeAttributeData& ctx,
                                  GraphContext &graphContext,
                                  NodeProfiler& profiler);

    public:
        bool PostCompute(GraphContext &context,
                         NodeProfiler& profiler);

    protected:

        void PullInternalOutputs();

    public:
        void AttributeChanged(const AttributePtr& attr, const AttributeChange& change) override;

        ScheduleNodePtrList ToScheduledNodes() override;

        /**
         * Launches compound computing.
         * @return bool: success.
         */
        bool Compute(GraphContext &context,
                     NodeProfiler& profiler) override;

        /**
         * Serializes node values.
         * @param dict
         * @param json
         */
		void Serialize(rapidjson::Value& dict,
                       rapidjson::Document& json)
                       const override;

        /**
         * Deserializes node values.
         * @param dict
         */
        bool Deserialize(rapidjson::Value& dict) override;

        static Feedback CheckLoadStatus(rapidjson::Value& dict);

    public:
        /**
         * Duplicates specified internal nodes.
         * @param sources: source nodes.
         * @param copyLinks: copy sources links.
         * @return resulting nodes.
         */
        NodeList DuplicateNodes(NodeList sources, bool copyLinks=false);

        /**
         * Compacts sources nodes to a new compound node.
         * @param sources: source nodes.
         * @param duplicate: duplicates to new compound.
         * @param keepExternalConnections: Keep external connections.
         * @return resulting compound node.
         */
        NodePtr ToCompound(NodeList sources, bool duplicate=false,
                           bool keepExternalConnections=true);


        /**
         * Creates a compound node from specified node
         * if conversion is possible.
         * This is the equivalent of doing a dynamic_cast.
         * The function returns nullptr if conversion could
         * not be made.
         * @param node: node pointer.
         * @return CompoundNode pointer.
         */
        static CompoundNodePtr FromNode(const NodePtr& node);

        static CompoundNodePtr FromNode(const NodeWkPtr& node);
	};

    typedef std::function<bool(Gex::NodePtr node)> TraversalCallback;

    void TraverseNodes(const Gex::NodePtr& root, TraversalCallback callback);

    Gex::NodePtr TraverseParents(const Gex::NodePtr& root, TraversalCallback callback);
}

#endif // GEX_NODE