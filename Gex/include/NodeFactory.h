#pragma once
#include "api.h"
#include "Node.h"

#include <string>
#include <map>


/*
Node Factory :
The factory has for goal to Serialize / Deserialize nodes while they already exists.

*/
namespace Gex
{
    class PluginLoader;


	class GEX_API NodeBuilder
	{
    protected:
        friend Node;
        friend PluginLoader;

        std::string pluginName;

	public:
        NodeBuilder() = default;

        NodeBuilder(const NodeBuilder &) = default;

        /**
         * Returns current builder plugin.
         * @return std::string plugin name.
         */
        std::string Plugin() const;

    protected:
        /**
         * Registers plugin name.
         * @param std::string plugin.
         */
        void SetPlugin(std::string plugin);

    public:

        /**
         * Creates new node of type.
         * @return Node ptr.
         */
        virtual Node* CreateNode() const = 0;

        /**
         * Initializes node with name.
         * @param name: node name.
         * @return Node ptr.
         */
        virtual Node* InitNode(std::string name) const;

        /**
         * Deserializes node data.
         * @param dict node data.
         * @param node Node ptr.
         */
        virtual void DeserializeNode(rapidjson::Value& dict, Node* node) const;

        /**
         * Load node from serialized values.
         * @param name: node name.
         * @param dict: data.
         * @return Node ptr.
         */
        virtual Node* LoadNode(std::string name, rapidjson::Value& dict) const;

        /**
         * Serializes node.
         * @param node: node ptr.
         * @param dict: node data.
         * @param doc: json file.
         */
        virtual void SaveNode(Node* node, rapidjson::Value& dict, rapidjson::Document& json) const;

        /**
         * Serializes node data.
         * @param node: node ptr.
         * @param dict: data.
         * @param json: document.
         */
        virtual void SerializeNode(Node* node, rapidjson::Value& dict, rapidjson::Document& json) const;
	};


	class GEX_API DefaultNodeBuilder : public NodeBuilder
	{
	public:
		virtual Node* CreateNode() const override;
    };


	class GEX_API CompoundNodeBuilder : public DefaultNodeBuilder
	{
	public:
		virtual Node* CreateNode() const override;
	};


    class GEX_API NodeFactory
	{
    private:
        static NodeFactory* factory;
	protected:
		NodeBuilder* defaultBuilder;
		std::map<std::string, NodeBuilder*> builders;

		bool HasBuilder(const std::string& type) const;

	public:
		bool RegisterNodeBuilder(const std::string type, NodeBuilder* builder,
                                 bool default_ = false,  bool force = false);

		NodeBuilder* DefaultBuilder() const;

		NodeBuilder* GetBuilder(const std::string& type) const;

	public:

		Node* CreateNode(const std::string& type, const std::string& name) const;

        Node* LoadNode(std::string name, rapidjson::Value& dict) const;

        bool SaveNode(Node* node, rapidjson::Value& dict, rapidjson::Document& json) const;

        bool TypeExists(std::string typeName) const;

        std::vector<std::string> NodeTypes() const;

        static NodeFactory* GetFactory();
	};

}


#ifndef GENERATE_DEFAULT_BUILDER
#define GENERATE_DEFAULT_BUILDER(buildername, nodeclass)                        \
class __declspec(dllexport) buildername: public Gex::DefaultNodeBuilder \
{                                                                               \
public:                                                                         \
    Gex::Node* CreateNode() const override                              \
    {                                                                           \
        return new nodeclass();                                                 \
    }                                                                           \
};

#define GENERATE_BUILDER(nodeclass) GENERATE_DEFAULT_BUILDER((nodeclass)Builder, nodeclass)
#endif


#ifndef REGISTER_PLUGIN
#define REGISTER_PLUGIN extern "C" void __declspec(dllexport) __cdecl RegisterPlugin
#endif