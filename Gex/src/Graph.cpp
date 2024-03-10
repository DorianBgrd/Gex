#include "Gex/include/Graph.h"
#include <filesystem>

#include "Gex/include/Attribute.h"
#include "Gex/include/Node.h"
#include "Gex/include/NodeFactory.h"
#include "Gex/include/Evaluation.h"
#include "Gex/include/Scheduling.h"
#include "Gex/include/PluginLoader.h"
#include "Gex/include/Config.h"
#include "Gex/include/utils.h"
#include "Gex/include/io.h"

#include "Gex/include/io.h"

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
    for (auto* sn : scheduledNodes)
        delete sn;

    scheduledNodes.clear();

    for (auto n : nodes)
        delete n;

    nodes.clear();
}


Gex::Node* Gex::Graph::CreateNode(std::string type, std::string name_)
{
    if (name_.empty())
    {
        name_ = type;
    }

    std::string uniqueName = UniqueName(name_);

    auto* factory = NodeFactory::GetFactory();
    Node* node = factory->CreateNode(type, uniqueName);

    AddNode(node);

    return node;
}


bool Gex::Graph::AddNode(Node* node)
{
    if (std::find(nodes.begin(), nodes.end(), node) != nodes.end())
    {
        return false;
    }

    // Updates node name to make it unique.
    node->SetName(UniqueName(node->Name()));

    nodes.push_back(node);
    node->SetGraph(this);
    return true;
}


Gex::Attribute* CopyAttribute(Gex::Attribute* source, Gex::Node* dest)
{
    std::string atpath = source->Longname();

    Gex::Attribute* destAttribute = nullptr;
    if (source->IsUserDefined())
    {
        Gex::Attribute* newParent_ = dest->GetAttribute(
                source->ParentAttribute()->Longname());

        if (destAttribute->IsHolder())
        {
            bool multi = destAttribute->IsMulti();

            destAttribute = dest->CreateAttribute(source->Name(), source->GetAttrType(),
                                                  multi, newParent_);
        }
        else
        {
            destAttribute = dest->CreateAttributeFromValue(source->Name(), source->GetAnyValue(),
                                                           source->GetAttrValueType(),
                                                           source->GetAttrType(),
                                                           newParent_);
        }

    }
    else
    {
        destAttribute = dest->GetAttribute(atpath);
    }

    if (source->IsMulti())
    {
        for (unsigned int index : source->ValidIndices())
        {
            destAttribute->CreateIndex(index);
        }
    }

    return destAttribute;
}


Gex::Node* Duplicate(std::map<Gex::Node*, Gex::Node*>& res,
                     Gex::Graph* graph, Gex::Node* source,
                     const std::string& name="",
                     Gex::Node* parent=nullptr)
{
    std::string type = source->Type();
    std::string sourceName = source->Name();

    if (!parent)
        parent = source->Parent();

    std::string newName = name;
    if (newName.empty())
        newName = sourceName + "1";

    Gex::Node* new_ = Gex::NodeFactory::GetFactory()->CreateNode(type, newName);

    graph->AddNode(new_);

    for (auto attribute : source->GetAllAttributes())
    {
        std::string atpath = attribute->Longname();

        Gex::Attribute* destAttribute = nullptr;
        if (attribute->IsUserDefined())
        {
            Gex::Attribute* newParent_ = new_->GetAttribute(
                    attribute->ParentAttribute()->Longname());

            if (destAttribute->IsHolder())
            {
                bool multi = destAttribute->IsMulti();

                destAttribute = new_->CreateAttribute(attribute->Name(), attribute->GetAttrType(),
                                                      multi, newParent_);
            }
            else
            {
                destAttribute = new_->CreateAttributeFromValue(attribute->Name(), attribute->GetAnyValue(),
                                                               attribute->GetAttrValueType(),
                                                               attribute->GetAttrType(),
                                                               newParent_);
            }

        }
        else
        {
            destAttribute = new_->GetAttribute(atpath);
        }

        if (attribute->IsMulti())
        {
            for (unsigned int index : attribute->ValidIndices())
            {
                destAttribute->CreateIndex(index);
            }
        }

        destAttribute->SetAnyValue(attribute->GetAnyValue());
    }

    if (parent)
    {
        Gex::CompoundNode::FromNode(parent)->AddInternalNode(new_);
    }
    else
    {
        graph->AddNode(new_);
    }

    if (source->IsCompound())
    {
        for (auto* cmpn : Gex::CompoundNode::FromNode(source)->InternalNodes())
        {
            Duplicate(res, graph, cmpn, "", new_);
        }
    }

    res[source] = new_;

    return new_;
}


bool Gex::Graph::IsValid() const
{
    return valid;
}


void Gex::Graph::Invalidate()
{
    valid = false;

    for (auto* node : scheduledNodes)
    {
        delete node;
    }

    scheduledNodes.clear();

    if (!invalidateCallbacks.empty())
    {
        for (auto cb : invalidateCallbacks)
        {
            cb.second();
        }
    }
}


void Gex::Graph::Schedule()
{
    scheduledNodes = ScheduleNodes(nodes);

    valid = true;
}


Gex::CallbackId Gex::Graph::AddInvalidateCallback(InvalidateCallback callback)
{
    invalidCbId++;
    invalidateCallbacks[invalidCbId] = callback;

    return invalidCbId;
}


void Gex::Graph::RemoveInvalidateCallback(CallbackId callback)
{
    if (invalidateCallbacks.find(callback) == invalidateCallbacks.end())
        return;

    invalidateCallbacks.erase(callback);
}


void Gex::Graph::ClearInvalidateCallbacks()
{
    invalidateCallbacks.clear();
}


Gex::CallbackId Gex::Graph::AddScheduleCallback(ScheduleCallback callback)
{
    scheduleCbId++;
    scheduleCallbacks[scheduleCbId] = callback;

    return invalidCbId;
}

void Gex::Graph::RemoveScheduleCallback(CallbackId callback)
{
    if (scheduleCallbacks.find(callback) == scheduleCallbacks.end())
        return;

    scheduleCallbacks.erase(callback);
}


void Gex::Graph::ClearScheduleCallbacks()
{
    scheduleCallbacks.clear();
}


Gex::NodeList Gex::Graph::DuplicateNodes(NodeList sources, bool copyLinks,
                                         Gex::Node* parent)
{
    std::map<Gex::Node*, Gex::Node*> nmap;

    Gex::NodeList result;

    for (auto* source : sources)
    {
        Gex::Node* dup = Duplicate(nmap, this, source, "", parent);
        result.push_back(dup);
    }

    Gex::NodeList news_;
    if (copyLinks)
    {
        for (auto m : nmap)
        {
            // Get the new node.
            auto* source = m.first;
            auto* new_ = m.second;
            for (auto* attribute : source->GetAllAttributes())
            {
                // Get New node attribute.
                auto* destAttribute = new_->GetAttribute(attribute->Longname());
                if (Attribute* atsource = attribute->Source())
                {
                    // Find original attribute source node.
                    Node* nsource = atsource->Node();

                    // If source node has been duplicated, find the duplicated
                    // node attribute and connect it to preserve internal graph.
                    if (nmap.count(nsource))
                    {
                        Node*  duplicatedSource = nmap.at(nsource);
                        Attribute* duplicatedAttr = duplicatedSource->GetAttribute(atsource->Longname());

                        destAttribute->ConnectSource(duplicatedAttr);
                    }
                        // Else connect to original source.
                    else
                    {
                        destAttribute->ConnectSource(atsource);
                    }
                }
            }
        }
    }

    return result;
}


std::string Gex::Graph::UniqueName(const std::string& name) const
{
    return Gex::Utils::UniqueName(name, nodes);
}


Gex::NodeList Gex::Graph::DuplicateNodes(NodeList sources, bool copyLinks)
{
    return DuplicateNodes(sources, copyLinks, nullptr);
}


Gex::Node* Gex::Graph::ToCompound(NodeList sources, bool duplicate,
                                  bool keepExternalConnections)
{
    auto* compound = new CompoundNode();

    compound->SetName("Compound");

    NodeList extract = sources;
    if (duplicate)
    {
        extract = DuplicateNodes(sources, true);
    }

    for (auto* node : sources) {
        compound->AddInternalNode(node);
        std::remove(nodes.begin(), nodes.end(), node);

        for (auto *attribute: node->GetAllAttributes())
        {
            if (auto *src = attribute->Source())
            {
                if (std::find(sources.begin(), sources.end(), src->Node()) != sources.end())
                {
                    continue;
                }

                std::string pname = src->Node()->Name() + "_" + src->Name();

                Attribute* proxy = compound->GetAttribute(pname);
                if (!proxy)
                {
                    AttrType type = AttrType::Static;
                    if (src->GetAttrType() == AttrType::Output)
                    {
                        type = AttrType::Input;
                    }

                    if (src->IsHolder())
                    {
                        proxy = compound->CreateInternalAttribute(pname, type,
                                                                  proxy->IsMulti());
                    }
                    else
                    {
                        proxy = compound->CreateInternalAttribute(pname, src->GetAnyValue(),
                                                                  src->GetAttrValueType(), type);
                    }
                }

                attribute->DisconnectSource(src);
                attribute->ConnectSource(proxy);

                if (keepExternalConnections)
                    proxy->ConnectSource(src);

            }

            for (Attribute *dst: attribute->Dests())
            {
                if (std::find(sources.begin(), sources.end(), dst->Node()) != sources.end())
                {
                    continue;
                }

                std::string pname = dst->Node()->Name() + "_" + dst->Name();
                Attribute* proxy = compound->GetAttribute(pname);

                if (!proxy)
                {
                    AttrType type = AttrType::Static;
                    if (dst->GetAttrType() == AttrType::Input)
                    {
                        type = AttrType::Output;
                    }

                    if (dst->IsHolder())
                    {
                        bool multi = proxy->IsMulti();

                        proxy = compound->CreateAttribute(pname, dst->GetAttrType(), multi);
                    }
                    else
                    {
                        proxy = compound->CreateAttributeFromValue(pname, dst->GetAnyValue(),
                                                                   dst->GetAttrValueType(),
                                                                   dst->GetAttrType());
                    }
                }

                dst->DisconnectSource(attribute);
                proxy->ConnectSource(attribute);
                if (keepExternalConnections)
                    dst->ConnectSource(proxy);
            }
        }
    }

    AddNode(compound);
    return compound;
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
        AttributeList attributes = node->GetAllAttributes();
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

    Config conf = Config::GetConfig();
    dict.AddMember(rapidjson::StringRef(conf.graphNodesKey.c_str()),
                   nodesdict, json.GetAllocator());

    dict.AddMember(rapidjson::StringRef(conf.graphConnectionsKey.c_str()),
                   connections, json.GetAllocator());

    dict.AddMember(rapidjson::StringRef(conf.graphPluginsKey.c_str()),
                   pluginsValue, json.GetAllocator());

    dict.AddMember(rapidjson::StringRef(conf.graphNodeTypes.c_str()),
                   typesValue, json.GetAllocator());

    return true;
}


bool Gex::Graph::Deserialize(rapidjson::Value& dict)
{
    Config conf = Config::GetConfig();

    if (dict.HasMember(conf.graphNodesKey.c_str()))
    {
        rapidjson::Value& serializedNodes = dict[conf.graphNodesKey.c_str()];
        for (rapidjson::Value::MemberIterator itr = serializedNodes.MemberBegin();
             itr != serializedNodes.MemberEnd(); ++itr)
        {
            std::string name = itr->name.GetString();

            Gex::Node* node = NodeFactory::GetFactory()->LoadNode(
                    name, itr->value);

            nodes.push_back(node);
        }
    }

    if (dict.HasMember(conf.graphConnectionsKey.c_str()))
    {
        rapidjson::Value& serializedConnections = dict[conf.graphConnectionsKey.c_str()];
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

    Config conf = Config::GetConfig();

    std::set<std::string> missingTypes;
    if (dict.HasMember(conf.graphNodeTypes.c_str()))
    {
        rapidjson::Value& types = dict[conf.graphNodeTypes.c_str()];
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
    if (dict.HasMember(conf.graphPluginsKey.c_str()))
    {
        rapidjson::Value& plugins = dict[conf.graphPluginsKey.c_str()];
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
                         std::function<void(const GraphContext& context)> evalDone)
{

    profiler->Start();

    auto lmbda = ProfilerScope(profiler, "Global", "Make callback");

    std::function<void(const GraphContext&)> finalize =
            [this, evalDone](const GraphContext& context) -> void {
        this->FinalizeEvaluation(context); evalDone(context); };

    lmbda.Stop();

    if (!IsValid())
    {
        auto schelScope = ProfilerScope(profiler, "Global", "Scheduling");

        Schedule();
    }

    auto evaluator = std::make_shared<NodeEvaluator>(
            ScheduleNodes(nodes, true), context, profiler,
            false, threads, nodeStarted,
            nodeDone, finalize);

    evaluator->Run();

    return (evaluator->Status() == NodeEvaluator::EvaluationStatus::Done);
}


void Gex::Graph::FinalizeEvaluation(const GraphContext& context) const
{
    for (std::string fp : context.Resources())
    {
        std::filesystem::remove(std::filesystem::path(fp));
    }
}


Gex::NodeList Gex::Graph::Nodes() const
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
