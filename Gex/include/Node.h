#ifndef GEX_NODE
#define GEX_NODE

#include "api.h"
#include "defs.h"
#include "boost/python.hpp"
#include "NodeAttributeData.h"
#include "Attribute.h"
#include "Profiler.h"
#include "Evaluation.h"
#include "Scheduling.h"
#include <map>
#include <vector>
#include <string>

#include "rapidjson/document.h"


namespace Gex
{
    class Graph;
    class Attribute;
	class DefaultNodeBuilder;
	class CompoundNodeBuilder;
	class CompoundNode;
	class NodeFactory;
	class NodeBuilder;
    class GraphContext;
	class NodeAttributeData;
	class EvaluationContext;
    class ScheduledNode;

	class GEX_API Node
	{
	protected:
        friend CompoundNode;
        friend Attribute;
        friend Graph;
		friend NodeFactory;
        friend NodeBuilder;
		friend DefaultNodeBuilder;
		friend CompoundNodeBuilder;
		friend NodeAttributeData;
        friend EvaluationContext;
        friend NodeEvaluator;

        bool initializing;

        /**
         * Starts node initialization.
         */
        void StartInitialization();

        /**
         * Ends node initialization.
         */
        void EndInitialization();

	protected:
		std::string nodeName;
        std::string typeName;
        std::string pluginName;
		std::map<std::string, Attribute*> attributes;

        std::vector<std::string> resources;

        Node* parent = nullptr;
        Graph* graph = nullptr;
        Attribute* previous = nullptr;
        Attribute* next = nullptr;

        size_t computeHash;
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

    protected:
		// Adds attribute to node.
        /**
         * Registers attribute.
         * @param Attribute attr: attribute.
         * @return bool success.
         */
		bool AddAttribute(Attribute* attr);

        /**
         * Default constructor.
         */
		Node(Node* parent=nullptr);

        void SetParent(Node* parent);

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
         * Member function called when an attribute changes.
         * @param Attribute attribute: attribute that changed.
         */
        virtual void AttributeChanged(Attribute* attribute, AttributeChange change);

	public:
		// Destructor.
		virtual ~Node();

        Node* Parent();

		bool operator ==(const Node& other);

		bool operator ==(const Node* other);

	public:

         /**
          * Returns whether node is editable.
          * @return bool can be edited.
          */
         virtual bool IsEditable() const;

         /**
          * Sets node editable.
          * @param bool editable: set editable state.
          * @return bool editable.
          */
         virtual bool SetEditable(bool editable);

         /**
          * Returns whether current node is a
          * compound node.
          * @return is compound.
          */
         virtual bool IsCompound() const;

         Graph* Graph() const;

    protected:

        /**
         * Builds hash from node's attribute.
         * @return size_t hash.
         */
        virtual size_t BuildHash(bool followCnx=false) const;

        void SetComputeHash(size_t hash);

        virtual void SetGraph(Gex::Graph* graph);

    public:
        /**
         * Returns last compute hash.
         * @return size_t hash.
         */
        size_t ComputeHash() const;

        /**
         * Returns whether node is dirty and need re-computation.
         * @return bool: is dirty.
         */
        virtual bool IsDirty() const;

    protected:
        std::any InitValueFromHash(size_t hash, Feedback* success=nullptr);

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
		Attribute* CreateAttribute(std::string name, AttrValueType valueType = AttrValueType::Single,
			AttrType type = AttrType::Static, Attribute* parent=nullptr)
		{
            Feedback status;
            std::any initValue = InitValueFromHash(typeid(T).hash_code(),
                                                   &status);
            if (!status)
            {
                return nullptr;
            }

			auto* attribute = new Attribute(std::move(name), initValue,
                                            valueType, type, !IsInitializing(),
                                            this, parent);
			
			bool success = false;
			if (parent == nullptr)
			{
				success = AddAttribute(attribute);
			}
			else
			{
				success = parent->AddChildAttribute(attribute);
			}

            if (!success)
			{
				delete attribute;
                return nullptr;
			}

			return attribute;
		}


		Attribute* CreateAttributeFromValue(std::string name, std::any v, AttrValueType valueType = AttrValueType::Single,
                                            AttrType type = AttrType::Static, Attribute* parent = nullptr);


        Attribute* CreateAttribute(const std::string& name, AttrType type = AttrType::Static,
                                   bool multi=false, Attribute* parent = nullptr);

		/**
		 * Returns whether attribute with name exists.
		 * @returns bool exists.
		 */
		bool HasAttribute(std::string name);

        /**
         * Gets Attribute pointer from name,
		 * Returns nullptr if not found.
         * @param std::string name: attribute name.
         * @return Attribute* attribute.
         */
		Attribute* GetAttribute(std::string name);

        /**
         * Returns python object for attribute.
         * @param std::string name: attribute name.
         * @return boost::python::object object.
         */
		boost::python::object PythonGetAttribute(std::string name);
		
        /**
         * Returns all attributes pointers.
         * @return std::vector<Attribute*> attributes.
         */
        AttributeList GetAttributes() const;

        /**
         * Returns all attributes pointers, including
         * sub and child attributes.
         * Warning : sub attributes may be removed or
         * changed, be careful while using those
         * pointers
         * @return std::vector<Attribute*> attributes.
         */
        AttributeList GetAllAttributes() const;

        /**
         * Returns all user defined attributes pointers.
         * @return std::vector<Attribute*> attributes.
         */
        AttributeList ExtraAttributes();

        /**
         * Returns input node pointers, if materialized.
         * @return std::vector<Node**> nodes.
         */
		NodeList UpstreamNodes();

        /**
         * Returns Output node pointers, if materialized.
         * @return std::vector<Node**> nodes.
         */
		NodeList DownstreamNodes();

		/**
		 * Serializes node to file.
		 */
		virtual void Serialize(rapidjson::Value& dict,
                               rapidjson::Document& json) const;

        virtual void Deserialize(rapidjson::Value& dict);

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
        NodeAttributeData createEvalContext();

    public:
        /**
         * Computes node.
         * @return bool success.
         */
        virtual bool Compute(GraphContext &context,
                             NodeProfiler& profiler);

    protected:
        /**
         * Computation function.
         * This function should be overrided to provide new
         * node types calculations.
         * @param context Node context.
         * @return bool success.
         */
        virtual bool Evaluate(NodeAttributeData &context,
                              GraphContext &graphContext,
                              NodeProfiler& profiler);

    public:
        ScheduledNode* ToScheduledNode();

    public:

		template<class T>
		static T* ConvertTo(Node* node)
		{
			return dynamic_cast<T*>(node);
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

	protected:
		NodeList innerNodes;

        using Node::Node;

	public:
        ~CompoundNode() override;

		std::string Description() const override
        {
            return "Base compound node";
        }

        bool IsCompound() const override;

        void SetGraph(Gex::Graph* graph) override;

        template <typename T>
        Attribute* CreateInternalAttribute(std::string name, AttrValueType valueType = AttrValueType::Single,
                                           AttrType type = AttrType::Static, Attribute* parent=nullptr)
        {
            Attribute* attribute = CreateAttribute<T>(name, valueType, type, parent);
            if (!attribute)
            {
                return attribute;
            }

            attribute->SetInternal(true);
            return attribute;
        }


        Attribute* CreateInternalAttribute(std::string name, std::any v, AttrValueType valueType = AttrValueType::Single,
                                           AttrType type = AttrType::Static, Attribute* parent = nullptr);


        Attribute* CreateInternalAttribute(const std::string& name, AttrType type = AttrType::Static,
                                           bool multi=false, Attribute* parent = nullptr);


        Attribute* CreateInternalAttribute(std::string name, boost::python::object type, AttrType attributeType);


        Attribute* CreateInternalAttribute(std::string name, boost::python::object type, AttrType attributeType,
                                           AttrValueType valueType);


        Attribute* CreateInternalAttribute(std::string name, boost::python::object type, AttrType attributeType,
                                           AttrValueType valueType, Attribute* parent);

        NodeList InternalNodes() const;

        std::vector<std::string> InternalNodeNames() const;

        /**
         * Creates a new internal node.
         * @param type: type name.
         * @param name: node name.
         * @return node pointer.
         */
        virtual Node* CreateInternalNode(std::string type, std::string name=std::string());

        /**
         * Adds node pointer as internal node.
         * @param node: node pointer.
         * @return success.
         */
        virtual bool AddInternalNode(Node* node);

        /**
         * Removes internal node.
         * @param node: node to remove.
         * @return success.
         */
        virtual bool RemoveInternalNode(Node* node);

        /**
         * Removes internal node using its name.
         * @param node: nod name.
         * @return success.
         */
        virtual bool RemoveInternalNode(const std::string& node);

        /**
         * Returns node pointer with name if it exists.
         * @param node: node name.
         * @return node pointer.
         */
        virtual Node* GetInternalNode(const std::string& node) const;

        NodeList GetInternalNodes() const;

        std::string UniqueName(const std::string& name) const override;

        /**
         * Returns whether specified node pointer is an internal
         * node.
         * @param node: node pointer.
         * @return is internal node.
         */
        virtual bool IsInternalNode(Node* node) const;

        /**
         * Returns whether specified node name is an internal node.
         * @param node: node name.
         * @return is internal node.
         */
        virtual bool IsInternalNode(const std::string& node) const;

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

    protected:

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

    public:
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
        virtual ScheduleNodeList ToScheduledNodes();

    public:
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
        void Deserialize(rapidjson::Value& dict) override;

    public:
        /**
         * Creates a compound node from specified node
         * if conversion is possible.
         * This is the equivalent of doing a dynamic_cast.
         * The function returns nullptr if conversion could
         * not be made.
         * @param node: node pointer.
         * @return CompoundNode pointer.
         */
        static CompoundNode* FromNode(Node* node);

        /**
         * Checks whether node is a compound node. This function
         * uses the FromNode function, checking the result.
         * @param node: node pointer.
         * @return bool is compound node.
         */
        static bool IsCompound(Node* node);
	};

}

#endif // GEX_NODE