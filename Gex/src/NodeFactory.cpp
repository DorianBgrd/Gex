#include "Gex/include/NodeFactory.h"
#include "Gex/include/Config.h"
#include "rapidjson/document.h"

#include <boost/python.hpp>


std::string Gex::NodeBuilder::Plugin() const
{
    return pluginName;
}


void Gex::NodeBuilder::SetPlugin(std::string plugin)
{
    pluginName = plugin;
}


Gex::Node* Gex::NodeBuilder::InitNode() const
{
    Node* node = CreateNode();

    node->SetPlugin(Plugin());

    node->Initialize();
    return node;
}


void Gex::NodeBuilder::SerializeNode(Node *node, rapidjson::Value &dict, rapidjson::Document &json) const
{
    node->Serialize(dict, json);
}


void Gex::NodeBuilder::DeserializeNode(rapidjson::Value& dict, Node* node) const
{
    node->Deserialize(dict);
}


Gex::Node* Gex::NodeBuilder::LoadNode(rapidjson::Value& dict) const
{
    Node* node = InitNode();

    DeserializeNode(dict, node);

    return node;
}


void Gex::NodeBuilder::SaveNode(Node *node, rapidjson::Value &dict, rapidjson::Document &json) const
{
    SerializeNode(node, dict, json);
}


Gex::Node* Gex::DefaultNodeBuilder::CreateNode() const
{
    return new Node();
}


Gex::Node* Gex::CompoundNodeBuilder::CreateNode() const
{
	return new CompoundNode();
}


Gex::NodeFactory* Gex::NodeFactory::factory = nullptr;


Gex::NodeFactory* Gex::NodeFactory::GetFactory()
{
    if (!factory)
    {
        factory = new NodeFactory();
    }

    return factory;
}


class CompoundNodeBuilder: public Gex::NodeBuilder
{
    Gex::Node* CreateNode() const
    {
        return new Gex::CompoundNode();
    }
};


Gex::NodeFactory::NodeFactory()
{
    RegisterNodeBuilder("CompoundNode", new CompoundNodeBuilder());
}


bool Gex::NodeFactory::HasBuilder(const std::string& type) const
{
	return (builders.find(type) != builders.end());
}


bool Gex::NodeFactory::RegisterNodeBuilder(const std::string type, NodeBuilder* builder, bool default_, bool force)
{
	if (!force && HasBuilder(type))
	{
		return false;
	}

	builders[type] = builder;
	if (default_)
	{
		defaultBuilder = builder;
	}

	return true;
}

Gex::NodeBuilder* Gex::NodeFactory::DefaultBuilder() const
{
	return defaultBuilder;
}

Gex::NodeBuilder* Gex::NodeFactory::GetBuilder(const std::string& type) const
{
	if (!HasBuilder(type))
	{
		return DefaultBuilder();
	}

	return builders.at(type);
}


Gex::Node* Gex::NodeFactory::LoadNode(rapidjson::Value& dict) const
{
    if (!dict.HasMember(Config::GetConfig().nodeTypeKey.c_str()))
    {
        return nullptr;
    }

    std::string nodeType = dict[Config::GetConfig().nodeTypeKey.c_str()].GetString();

    NodeBuilder* builder = GetBuilder(nodeType);
	if (builder == nullptr)
	{
		return nullptr;
	}

    Node* node = builder->LoadNode(dict);

    node->SetType(nodeType);

    return node;
}


bool Gex::NodeFactory::SaveNode(Node* node, rapidjson::Value& dict, rapidjson::Document& json) const
{
    NodeBuilder* builder = GetBuilder(node->Type());
    if (!builder)
    {
        return false;
    }

    std::string typename_ = node->Type();

    rapidjson::Value& typeValue = rapidjson::Value(rapidjson::kStringType)
            .SetString(rapidjson::StringRef(typename_.c_str()), json.GetAllocator());
    rapidjson::Value& typeKey = rapidjson::Value().SetString(Config::GetConfig().nodeTypeKey.c_str(),
                                                             json.GetAllocator());

    dict.AddMember(typeKey, typeValue, json.GetAllocator());

    rapidjson::Value& nameValue = rapidjson::Value(rapidjson::kStringType)
            .SetString(rapidjson::StringRef(node->Name().c_str()), json.GetAllocator());
    rapidjson::Value& nameKey = rapidjson::Value().SetString(Config::GetConfig().nodeNameKey.c_str(),
                                                             json.GetAllocator());

    dict.AddMember(nameKey, nameValue, json.GetAllocator());

    builder->SerializeNode(node, dict, json);
    return true;
}


bool Gex::NodeFactory::TypeExists(std::string typeName) const
{
    return HasBuilder(typeName);
}


Gex::Node* Gex::NodeFactory::CreateNode(const std::string& type, const std::string& name) const
{
    NodeBuilder* builder = GetBuilder(type);
    if (!builder)
    {
        return nullptr;
    }

    Node* node = builder->InitNode();
    node->SetName(name);
    node->SetType(type);

    return node;
}



std::vector<std::string> Gex::NodeFactory::NodeTypes() const
{
    std::vector<std::string> types;
    for (const auto& type: builders)
    {
        types.push_back(type.first);
    }

    return types;
}
