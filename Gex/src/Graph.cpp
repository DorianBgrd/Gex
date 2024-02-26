#include "Gex/include/Graph.h"
#include <filesystem>

#include "Gex/include/Attribute.h"
#include "Gex/include/Node.h"
#include "Gex/include/NodeFactory.h"
#include "Gex/include/Evaluation.h"
#include "Gex/include/PluginLoader.h"
#include "Gex/include/Config.h"

#include <future>
#include <memory>
#include <set>



Gex::GraphContext::GraphContext()
{

}


Gex::GraphContext::GraphContext(const GraphContext& other)
{

}


void Gex::GraphContext::RegisterResource(std::string resource)
{
    resources.push_back(resource);
}


std::vector<std::string> Gex::GraphContext::Resources() const
{
    return resources;
}



Gex::Graph::~Graph()
{
    for (auto n : nodes)
        delete n;

    nodes.clear();
}


void Validate(std::string& name)
{
    size_t p = std::string::npos;
    std::string sep = Gex::Config::GetConfig().nodePathSeparator;
    while ((p = name.find(sep)) != std::string::npos)
    {
        name.replace(p, 1, "_");
    }
}


Gex::Node* Gex::Graph::CreateNode(std::string type, std::string name_)
{
    if (name_.empty())
    {
        name_ = type;
    }

    Validate(name_);

    std::string uniqueName = name_;

    int counter = 0;
    std::vector<std::string> nodeNames = NodeNames();
    while (std::find(nodeNames.begin(), nodeNames.end(), uniqueName) != nodeNames.end())
    {
        uniqueName = name_ + std::to_string(counter);
        counter++;
    }

    auto* factory = NodeFactory::GetFactory();
    Node* node = factory->CreateNode(type, uniqueName);

    if (node)
        nodes.push_back(node);

    return node;
}


bool Gex::Graph::RemoveNode(Gex::Node* node)
{
    auto index = std::find(nodes.begin(), nodes.end(), node);
    if (index == nodes.end())
    {
        return false;
    }

    nodes.erase(index);
    delete node;
    return true;
}



struct FindMatchingNode
{
    std::string match;

    bool operator()(Gex::Node* node)
    {
        return (node->Name() == match);
    }
};


FindMatchingNode CreateMatch(std::string name)
{
    FindMatchingNode matcher;
    matcher.match = name;
    return matcher;
}


bool Gex::Graph::RemoveNode(std::string nodename)
{
    auto idx = std::find_if(nodes.begin(), nodes.end(), CreateMatch(nodename));
    if (idx == nodes.end())
    {
        return false;
    }

    delete *idx;
    nodes.erase(idx);
    return true;
}


Gex::Node* Gex::Graph::FindNode(std::string path) const
{
    std::string n;
    std::string sp;
    size_t pos = path.find(Config::GetConfig().nodePathSeparator);
    if (pos == std::string::npos)
    {
        n = path;
    }
    else
    {
        n = path.substr(0, pos - 1);
        sp = path.substr(pos + 1, std::string::npos);
    }

    FindMatchingNode match;
    match.match = n;

    auto index = std::find_if(nodes.begin(), nodes.end(), match);
    if (index == nodes.end())
    {
        return nullptr;
    }

    Node* result = *index;
    if (sp.empty())
        return result;

    if (!result->IsCompound())
        return nullptr;

    return Node::ConvertTo<CompoundNode>(result)->GetInternalNode(sp);
}


Gex::Attribute* Gex::Graph::FindAttribute(std::string attr) const
{
    size_t pos  = attr.find(Config::GetConfig().attributeSeparator.c_str());
    if (pos == std::string::npos)
    {
        return nullptr;
    }

    std::string nodePath = attr.substr(0, pos - 1);
    std::string attrPath = attr.substr(pos + 1, std::string::npos);

    auto* node = FindNode(nodePath);
    if (!node)
    {
        return nullptr;
    }

    return node->GetAttribute(attrPath);
}


#define NODES_K "NODES"
#define CONNECTIONS_K "CONNECTIONS"
#define PLUGINS_K "PLUGINS"
#define TYPES_K "REQUIRED_NODE_TYPES"


bool Gex::Graph::Serialize(rapidjson::Value& dict, rapidjson::Document& json) const
{
    rapidjson::Value& nodesdict = rapidjson::Value(rapidjson::kObjectType).GetObject();
    rapidjson::Value& connections = rapidjson::Value(rapidjson::kArrayType).GetArray();

    std::set<std::string> plugins;
    std::set<std::string> nodeTypes;
    for (auto node : nodes)
    {
        // Plugins.
        std::string plugin = node->Plugin();
        if (!plugin.empty())
            plugins.insert(plugin);

        nodeTypes.insert(node->Type());

        // General data.
        rapidjson::Value& nodedict = rapidjson::Value(rapidjson::kObjectType).GetObject();

        NodeFactory::GetFactory()->SaveNode(node, nodedict, json);

        rapidjson::Value& key = rapidjson::Value().SetString(node->Name().c_str(),
                                                             json.GetAllocator());

        nodesdict.AddMember(key, nodedict, json.GetAllocator());

        // Connections.
        std::vector<Attribute*> attributes = node->GetAllAttributes();
        for (auto* attr : attributes)
        {
            Attribute* src = attr->Source();
            if (!src)
            {
                continue;
            }

            Node* srcNode = src->Node();

            // Do not serialize compound input that could also exist
            // in this level of the graph and would cause a mixing in
            // connections.
            if (node->IsCompound())
            {
                if (CompoundNode::FromNode(node)->IsInternalNode(srcNode))
                {
                    continue;
                }
            }

            rapidjson::Value& connection = rapidjson::Value(rapidjson::kArrayType).GetArray();
            connection.PushBack(rapidjson::Value().SetString(srcNode->Name().c_str(), json.GetAllocator()).Move(),
                                json.GetAllocator());
            connection.PushBack(rapidjson::Value().SetString(src->Longname().c_str(), json.GetAllocator()).Move(),
                                json.GetAllocator());
            connection.PushBack(rapidjson::Value().SetString(node->Name().c_str(), json.GetAllocator()).Move(),
                                json.GetAllocator());
            connection.PushBack(rapidjson::Value().SetString(attr->Longname().c_str(), json.GetAllocator()).Move(),
                                json.GetAllocator());

            connections.PushBack(connection.Move(), json.GetAllocator());
        }
    }

    rapidjson::Value& pluginsValue = rapidjson::Value().SetArray();
    for (auto plugin : plugins)
    {
        pluginsValue.PushBack(rapidjson::Value().SetString(plugin.c_str(), json.GetAllocator()).Move(),
                              json.GetAllocator());
    }

    rapidjson::Value& typesValue = rapidjson::Value().SetArray();
    for (auto type : nodeTypes)
    {
        typesValue.PushBack(rapidjson::Value().SetString(type.c_str(), json.GetAllocator()).Move(),
                            json.GetAllocator());
    }

    dict.AddMember(rapidjson::StringRef(NODES_K),
                   nodesdict, json.GetAllocator());

    dict.AddMember(rapidjson::StringRef(CONNECTIONS_K),
                   connections, json.GetAllocator());

    dict.AddMember(rapidjson::StringRef(PLUGINS_K),
                   pluginsValue, json.GetAllocator());

    dict.AddMember(rapidjson::StringRef(TYPES_K),
                   typesValue, json.GetAllocator());

    return true;
}


bool Gex::Graph::Deserialize(rapidjson::Value& dict)
{
    if (dict.HasMember(NODES_K))
    {
        rapidjson::Value& serializedNodes = dict[NODES_K];
        for (rapidjson::Value::MemberIterator itr = serializedNodes.MemberBegin();
             itr != serializedNodes.MemberEnd(); ++itr)
        {
            std::string name = itr->name.GetString();

            Gex::Node* node = NodeFactory::GetFactory()->LoadNode(
                    name, itr->value);

            nodes.push_back(node);
        }
    }

    if (dict.HasMember(CONNECTIONS_K))
    {
        rapidjson::Value& serializedConnections = dict[CONNECTIONS_K];
        for(rapidjson::Value::ValueIterator cnsitr = serializedConnections.Begin();
            cnsitr != serializedConnections.End(); cnsitr++)
        {
            auto array = cnsitr->GetArray();
            std::string srcn = array[0].GetString();
            std::string srca = array[1].GetString();
            std::string dstn = array[2].GetString();
            std::string dsta = array[3].GetString();

            auto* srcnode = FindNode(srcn);
            if (!srcnode)
                continue;

            auto* dstnode = FindNode(dstn);
            if (!dstnode)
                continue;

            Attribute* srcattr = srcnode->GetAttribute(srca);
            if (!srcattr)
                continue;

            Attribute* dstattr = dstnode->GetAttribute(dsta);
            if (!dstattr)
                continue;

            dstattr->ConnectSource(srcattr);
        }
    }

    return true;
}



Gex::Feedback Gex::Graph::CheckLoadStatus(rapidjson::Value& dict)
{
    Feedback res;

    std::set<std::string> missingTypes;
    if (dict.HasMember(TYPES_K))
    {
        rapidjson::Value& types = dict[TYPES_K];
        for (int i = 0; i < types.Size(); i++)
        {
            std::string typeName = types[i].GetString();
            if (!NodeFactory::GetFactory()->TypeExists(typeName))
            {
                missingTypes.insert(typeName);
            }
        }
    }

    if (missingTypes.empty())
    {
        return res;
    }

    std::set<std::string> missingPlugins;
    if (dict.HasMember(PLUGINS_K))
    {
        rapidjson::Value& plugins = dict[PLUGINS_K];
        for (int i = 0; i < plugins.Size(); i++)
        {
            std::string pluginName = plugins[i].GetString();
            if (!PluginLoader::PluginLoaded(pluginName))
            {
                missingPlugins.insert(pluginName);
            }
        }
    }

    std::string message = "Some node types are missing or not loaded : \n";
    for (std::string missingType : missingTypes)
    {
        message += "    - " + missingType + "\n";
    }

    if (!missingPlugins.empty())
    {
        message += "This may be related to missing plugins : \n";
        for (std::string missingPlugin : missingPlugins)
        {
            message += "    - " + missingPlugin + "\n";
        }
    }

    res.status = Status::Failed;
    res.message = message;
    return res;
}



bool Gex::Graph::Compute(GraphContext& context, Profiler& profiler,
                         unsigned int threads,
                         std::function<void(Node*)> nodeStarted,
                         std::function<void(Node*, bool)> nodeDone,
                         std::function<void(const GraphContext& context)> evalDone) const
{
    profiler->Start();

    auto lmbda = profiler->StartEvent("Global", "Make callback");

    std::function<void(const GraphContext&)> finalize =
            [this, evalDone](const GraphContext& context) -> void {
        this->FinalizeEvaluation(context); evalDone(context); };

    profiler->StopEvent(lmbda);


    auto evaluator = std::make_shared<NodeEvaluator>(
            nodes, context, profiler,
            false, threads, nodeStarted,
            nodeDone, finalize);

    return (evaluator->Status() == NodeEvaluator::EvaluationStatus::Done);
}


void Gex::Graph::FinalizeEvaluation(const GraphContext& context) const
{
    for (std::string fp : context.Resources())
    {
        std::filesystem::remove(std::filesystem::path(fp));
    }
}


std::vector<Gex::Node*> Gex::Graph::Nodes() const
{
   return nodes;
}


std::vector<std::string> Gex::Graph::NodeNames() const
{
    std::vector<std::string> names;
    for (auto* node : nodes)
    {
        names.push_back(node->Name());
    }

    return names;
}
