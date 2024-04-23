#include "Gex/include/Node.h"
#include "Gex/include/Attribute.h"
#include "Gex/include/NodeFactory.h"
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include "Gex/include/Evaluation.h"
#include "Gex/include/Config.h"
#include "Gex/include/utils.h"
#include "Gex/include/PluginLoader.h"

#include "Tsys/include/tsys.h"
#include "Tsys/include/defaultTypes.h"

#include <filesystem>
#include <map>
#include <any>


Gex::GraphContext::GraphContext()
{

}


Gex::GraphContext::GraphContext(const GraphContext& other)
{
    resources = other.resources;
}


void Gex::GraphContext::RegisterResource(std::string resource)
{
    resources.push_back(resource);
}


std::vector<std::string> Gex::GraphContext::Resources() const
{
    return resources;
}


Gex::Node::Node(Node* parent_)
{
    previous = nullptr;
    next = nullptr;
    parent = parent_;
    nextId = 0;
}


void Gex::Node::SetParent(Node* parent_)
{
    parent = parent_;
}


Gex::Node::~Node()
{
	for (std::pair<std::string, Attribute*> p : attributes)
	{
		delete p.second;
	}

	attributes.clear();
}


Gex::Node* Gex::Node::Parent()
{
    return parent;
}


bool Gex::Node::operator ==(const Node& other)
{
    return (other.nodeName == nodeName);
}

bool Gex::Node::operator ==(const Node* other)
{
    return (other->nodeName == nodeName);
}


bool Gex::Node::IsInitializing() const
{
    return initializing;
}


std::string Gex::Node::Name() const
{
	return nodeName;
}


std::string Gex::Node::Path() const
{
    std::string path;
    if (parent)
    {
        path = parent->Path();
    }

    return (path + Config::GetConfig().nodePathSeparator + nodeName);
}


bool Gex::Node::IsEditable() const
{
    return true;
}


void Gex::Node::SetType(std::string type)
{
    typeName = type;
}


void Gex::Node::SetPlugin(std::string plugin)
{
    pluginName = plugin;
}


bool Gex::Node::SetEditable(bool editable)
{
    return true;
}


bool Gex::Node::IsCompound() const
{
    return false;
}


std::any Gex::Node::InitValueFromHash(size_t hash, Feedback* success)
{
    auto* registry = TSys::TypeRegistry::GetRegistry();
    auto* handler = registry->GetTypeHandle(hash);
    if (!handler)
    {
        if (success)
            success->status = Status::Failed;

        return {};
    }
    if (success)
        success->status = Status::Success;

    return handler->InitValue();
}


Gex::Attribute* Gex::Node::CreateAttributeFromValue(std::string name, std::any v, AttrValueType valueType,
                                                    AttrType type, Attribute* parent)
{
    auto* attribute = new Attribute(name, v, valueType,
                                    type, !IsInitializing(),
                                    this, parent);

    bool success = false;
    if (!parent)
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


Gex::Attribute* Gex::Node::CreateAttributeFromTypeName(const std::string& name, const std::string& apiName,
                                                       AttrValueType valueType, AttrType type,
                                                       Attribute* parent)
{
    auto* registry = TSys::TypeRegistry::GetRegistry();
    auto* handler = registry->GetTypeHandleFromApiName(apiName);
    if (!handler)
    {
        return nullptr;
    }

    return CreateAttributeFromValue(name, handler->InitValue(),
                                    valueType, type, parent);
}


Gex::Attribute* Gex::Node::CreateAttribute(const std::string& name, AttrType type,
                                           bool multi, Attribute* parent)
{
    Attribute* attribute = new Attribute(name, type, multi, !IsInitializing(), this, parent);
    bool success = false;
    if (!parent)
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
    }

    return attribute;
}



void Gex::Node::StartInitialization()
{
    initializing = true;
}


void Gex::Node::EndInitialization()
{
    initializing = false;
}


std::string Gex::Node::Type() const
{
    return typeName;
}


std::string Gex::Node::Plugin() const
{
    return pluginName;
}


std::string Gex::Node::Description() const
{
    return "Base node";
}


std::string Gex::Node::SetName(const std::string& p)
{
    std::string name = Gex::Utils::ValidateName(p);
    if (parent)
    {
        name = parent->UniqueName(p);
    }

    nodeName = Gex::Utils::ValidateName(name);
    return nodeName;
}


unsigned int Gex::Node::RegisterAttributeCallback(
        AttributeChangeCallback cb)
{
    unsigned int id = nextId;
    attrCallbacks[id] = std::move(cb);

    nextId++;
    return id;
}


bool Gex::Node::DeregisterAttributeCallback(
        unsigned int index)
{
    auto idx = attrCallbacks.find(index);
    if (idx == attrCallbacks.end())
        return false;

    attrCallbacks.erase(idx);
    return true;
}


bool Gex::Node::AddAttribute(Attribute* attr)
{
	std::string _name = attr->Longname();

	if (attributes.find(_name) != attributes.end())
	{
		return false;
	}

	attr->SetParentNode(this);
	attributes[_name] = attr;

    SignalAttributeChange(attr, AttributeChange::AttributeAdded);

	return true;
}


bool Gex::Node::RemoveAttribute(Attribute* attr)
{
    std::string _name = attr->Longname();

    auto idx = attributes.find(_name);
    if (idx == attributes.end())
    {
        return false;
    }

    SignalAttributeChange(idx->second, AttributeChange::AttributeRemoved);

    delete idx->second;
    attributes.erase(idx);
    return true;
}


bool Gex::Node::HasAttribute(std::string name) const
{
	return (attributes.find(name) != attributes.end());
}


Gex::Attribute* Gex::Node::GetAttribute(std::string name) const
{
	size_t delimiter = name.find(Config::GetConfig().attributeSeparator);
	std::string shortname = name.substr(0, delimiter);

	if (!HasAttribute(shortname))
	{
		return nullptr;
	}

	if (delimiter == std::string::npos)
	{
		return attributes.at(name);
	}

	delimiter++;
	std::string subname = name.erase(0, delimiter);
	return attributes.at(shortname)->GetAttribute(subname);
}


Gex::AttributeList Gex::Node::ExtraAttributes()
{
	AttributeList extraAttrs;
	for (std::pair<std::string, Attribute*> a : attributes)
	{
		if (!a.second->IsUserDefined())
		{
			continue;
		}

		extraAttrs.push_back(a.second);
	}

	return extraAttrs;
}


Gex::AttributeList Gex::Node::GetAttributes() const
{
	AttributeList attrs;
	for (std::pair<std::string, Attribute*> n : attributes)
	{
		attrs.push_back(n.second);
	}

	return attrs;
}


Gex::AttributeList Gex::Node::GetAllAttributes() const
{
    AttributeList attrs;
    for (std::pair<std::string, Attribute*> n : attributes)
    {
        attrs.push_back(n.second);

        for (Attribute* at : n.second->GetAllAttributes())
        {
            attrs.push_back(at);
        }
    }

    return attrs;
}


Gex::NodeList Gex::Node::UpstreamNodes()
{
	NodeList sources;
	for (Attribute* a : GetAttributes())
	{
		Gex::AttributeList srcs = a->Sources();
        if (srcs.empty())
        {
            continue;
        }

        for (auto src : srcs)
        {
            Node* srcNode = src->Node();
            if (srcNode == Parent())
            {
                continue;
            }

            if (sources.empty())
            {
                sources.push_back(srcNode);
            }

            if (std::find(sources.begin(), sources.end(), srcNode) == sources.end())
            {
                sources.push_back(srcNode);
            }
        }
	}

	return sources;
}


// Note :
// As recursion is considered a bad practice in C++, this function uses
// a while loop. Still, filling a vector with nodes still feels like
// increasing the stack.
Gex::NodeList IterUpstreamNodes(Gex::Node* node)
{
    Gex::NodeList list;

    Gex::NodeList analyse = node->UpstreamNodes();

    size_t i = 0;
    while (i < analyse.size())
    {
//        auto* node_ = analyse.at(i);
//        if (node_->IsDynamic())
//        {
//            analyse.push_back(node_);
//        }
//        else
//        {
//            list.push_back(node_);
//        }
//
//        i += 1;
    }

    return list;
}


Gex::NodeList Gex::Node::DownstreamNodes()
{
	NodeList destinations;
	for (Attribute* a : GetAttributes())
	{
		if (!a->HasDests())
		{
			continue;
		}

		AttributeList attrDests = a->Dests();
		for (Attribute* attrDest : attrDests)
		{
			destinations.push_back(attrDest->Node());
		}
	}

	return destinations;
}


void Gex::Node::Serialize(rapidjson::Value& dict, rapidjson::Document& json) const
{
    Config conf = Config::GetConfig();

    rapidjson::Value& attrValues = rapidjson::Value(rapidjson::kObjectType).SetObject();
    rapidjson::Value& customAttrs = rapidjson::Value(rapidjson::kObjectType).SetObject();
    for (Attribute* attr : GetAttributes())
    {
        rapidjson::Value& attrValue = rapidjson::Value(rapidjson::kObjectType).SetObject();
        if (attr->IsUserDefined())
        {
            attr->SerializeAttribute(customAttrs, json);
        }

        attr->Serialize(attrValue, json);

        attrValues.AddMember(rapidjson::Value().SetString(attr->Name().c_str(), json.GetAllocator()).Move(),
                             attrValue, json.GetAllocator());
    }

    rapidjson::Value& attrKey = rapidjson::Value().SetString(conf.nodeAttributesKey.c_str(),
                                                             json.GetAllocator());
    dict.AddMember(attrKey, attrValues, json.GetAllocator());

    if (customAttrs.MemberCount())
    {
        rapidjson::Value& cstmAttrKey = rapidjson::Value().SetString(
                conf.nodeCustomAttributesKey.c_str(),
                json.GetAllocator());
        dict.AddMember(cstmAttrKey.Move(), customAttrs, json.GetAllocator());
    }
}


bool Gex::Node::Deserialize(rapidjson::Value& dict)
{
    Config conf = Config::GetConfig();
    if (dict.HasMember(conf.nodeCustomAttributesKey.c_str()))
    {
        rapidjson::Value& custAttrs = dict[conf.nodeCustomAttributesKey.c_str()];
        for (auto itr = custAttrs.MemberBegin(); itr != custAttrs.MemberEnd(); ++itr)
        {
            Attribute* cust = Attribute::DeserializeAttribute(itr->name.GetString(), itr->value.GetObject(),
                                                              this, nullptr, true);
            AddAttribute(cust);
        }
    }

    if (dict.HasMember(conf.nodeAttributesKey.c_str()))
    {
        rapidjson::Value& attrValues = dict[conf.nodeAttributesKey.c_str()];
        for (auto itr = attrValues.MemberBegin(); itr != attrValues.MemberEnd(); ++itr)
        {
            std::string name = itr->name.GetString();
            Attribute* attr = GetAttribute(name);
            if (!attr)
            {
                continue;
            }

            attr->Deserialize(itr->value);
        }
    }

    if (dict.HasMember(conf.nodeNameKey.c_str()))
    {
        rapidjson::Value& nameValue = dict[conf.nodeNameKey.c_str()];
        SetName(nameValue.GetString());
    }

    return true;
}


void Gex::Node::FactoryReset()
{
    for (auto attribute : GetAttributes())
    {
        attribute->FactoryReset();
    }
}


bool Gex::Node::PullAttributes()
{
    bool result = true;
    for (Attribute* attr : GetAttributes())
    {
        if (!attr->Pull())
            result = false;
    }

    return result;
}


std::string Gex::Node::UniqueName(const std::string& name) const
{
    return name;
}


bool Gex::Node::Pull()
{
    return PullAttributes();
}


void Gex::Node::CreateBuiltinsAttributes()
{
    previous = CreateAttribute<TSys::None>("Previous", AttrValueType::Multi, AttrType::Input);
    next = CreateAttribute<TSys::None>("Next", AttrValueType::Single, AttrType::Output);
}


void Gex::Node::Initialize()
{
    StartInitialization();

    CreateBuiltinsAttributes();

    InitAttributes();

    EndInitialization();
}


void Gex::Node::InitAttributes()
{

}


void Gex::Node::SignalAttributeChange(Attribute* attribute,
                                      AttributeChange change)
{
    AttributeChanged(attribute, change);

    for (const auto& callb : attrCallbacks)
    {
        callb.second(attribute, change);
    }

    if (parent)
    {
        switch (change)
        {
            case AttributeChange::Connected:
                parent->SignalAttributeChange(attribute, AttributeChange::ChildAttributeConnected);
                break;
            case AttributeChange::Disconnected:
                parent->SignalAttributeChange(attribute, AttributeChange::ChildAttributeDisconnected);
                break;
            case AttributeChange::ValueChanged:
                parent->SignalAttributeChange(attribute, AttributeChange::ChildAttributeValueChanged);
                break;
            case AttributeChange::IndexAdded:
                parent->SignalAttributeChange(attribute, AttributeChange::ChildAttributeIndexAdded);
                break;
            case AttributeChange::IndexRemoved:
                parent->SignalAttributeChange(attribute, AttributeChange::ChildAttributeIndexRemoved);
                break;
            default:
                parent->SignalAttributeChange(attribute, change);
                break;
        }
    }

}


void Gex::Node::AttributeChanged(Attribute* attribute, AttributeChange change)
{

}


Gex::NodeAttributeData Gex::Node::createEvalContext()
{
    return NodeAttributeData(this);
}


bool Gex::Node::Compute(GraphContext &context,
                        NodeProfiler& profiler)
{
    auto pullp = profiler.StartEvent("Compute::Pull");
    Pull();
    profiler.StopEvent(pullp);

    NodeAttributeData evalContext = createEvalContext();

    auto evalp = profiler.StartEvent("Compute::Evaluate");
    bool success = Evaluate(evalContext, context, profiler);
    profiler.StopEvent(evalp);

    return success;
}


bool Gex::Node::Run(Profiler& profiler,
                    unsigned int threads,
                    NodeCallback nodeStarted,
                    NodeResCallback nodeDone,
                    GraphCtxCallback evalDone)
{
    GraphContext context;

    profiler->Start();

    auto lmbda = ProfilerScope(profiler, "Global", "Make callback");

    std::function<void(const GraphContext&)> finalize =
            [this, evalDone](const GraphContext& context) -> void {
                this->FinalizeEvaluation(context); if (evalDone){evalDone(context);}; };

    lmbda.Stop();

    if (!IsScheduled())
    {
        auto schelScope = ProfilerScope(profiler, "Global", "Scheduling");

        Schedule();
    }

    auto evaluator = std::make_shared<NodeEvaluator>(
            scheduledNodes, context, profiler,
            false, threads, nodeStarted,
            nodeDone, finalize);

    evaluator->Run();

    return (evaluator->Status() == NodeEvaluator::EvaluationStatus::Done);
}


void Gex::Node::FinalizeEvaluation(const GraphContext& context) const
{
    for (std::string fp : context.Resources())
    {
        std::filesystem::remove(std::filesystem::path(fp));
    }
}


bool Gex::Node::Evaluate(NodeAttributeData &context,
                         GraphContext &graphContext,
                         NodeProfiler& profiler)
{
    return true;
}


bool Gex::Node::IsScheduled() const
{
    return isScheduled;
}


void Gex::Node::InvalidateScheduling()
{
    isScheduled = false;
}


void Gex::Node::ValidateScheduling()
{
    isScheduled = true;
}


void Gex::Node::Schedule()
{
    scheduledNodes = ScheduleNodes({this}, true);

    ValidateScheduling();
}


Gex::ScheduledNode* Gex::Node::ToScheduledNode()
{
    return new ScheduledNode(this);
}


Gex::ScheduleNodeList Gex::Node::ToScheduledNodes()
{
    return {ToScheduledNode()};
}



Gex::CallbackId Gex::Node::AddInvalidateCallback(InvalidateCallback callback)
{
    invalidCbId++;
    invalidateCallbacks[invalidCbId] = callback;

    return invalidCbId;
}


void Gex::Node::RemoveInvalidateCallback(CallbackId callback)
{
    if (invalidateCallbacks.find(callback) == invalidateCallbacks.end())
        return;

    invalidateCallbacks.erase(callback);
}


void Gex::Node::ClearInvalidateCallbacks()
{
    invalidateCallbacks.clear();
}


Gex::CallbackId Gex::Node::AddScheduleCallback(ScheduleCallback callback)
{
    scheduleCbId++;
    scheduleCallbacks[scheduleCbId] = callback;

    return invalidCbId;
}

void Gex::Node::RemoveScheduleCallback(CallbackId callback)
{
    if (scheduleCallbacks.find(callback) == scheduleCallbacks.end())
        return;

    scheduleCallbacks.erase(callback);
}


void Gex::Node::ClearScheduleCallbacks()
{
    scheduleCallbacks.clear();
}



bool Gex::CompoundPreScheduledNode::Evaluate(Gex::GraphContext &context,
                                             Gex::NodeProfiler &profiler)
{
    auto* cmp = Gex::CompoundNode::FromNode(node);
    cmp->Pull();

    return cmp->PreCompute(context, profiler);
}


bool Gex::CompoundPostScheduledNode::Evaluate(Gex::GraphContext &context,
                                             Gex::NodeProfiler &profiler)
{
    auto* cmp = Gex::CompoundNode::FromNode(node);
    cmp->PullInternalOutputs();

    return cmp->PostCompute(context, profiler);
}


Gex::CompoundNode::~CompoundNode()
{
    for (auto* node : nodes)
    {
        delete node;
    }

    nodes.clear();
}


bool Gex::CompoundNode::IsCompound() const
{
    return true;
}


bool Gex::CompoundNode::AddNode(Node *node)
{
    if (HasNode(node))
    {
        return false;
    }

    node->SetParent(this);
    node->SetName(node->Name());

    nodes.push_back(node);
    return true;
}


bool Gex::CompoundNode::RemoveNode(Node *node)
{
    if (!HasNode(node))
    {
        return false;
    }

    nodes.erase(std::find(nodes.begin(), nodes.end(), node));

    delete node;
    return true;
}


struct MatchingNode
{
    std::string match;

    bool operator()(Gex::Node* node) const
    {
        return(node->Name() == match);
    }
};


Gex::Node* Gex::CompoundNode::GetNode(const std::string& node) const
{

    size_t pos = node.find(Config::GetConfig().nodePathSeparator);
    std::string n = node;
    std::string sn;
    if (pos != std::string::npos)
    {
        n = node.substr(0, pos);
        sn = node.substr(pos + 1, std::string::npos);
    }

    MatchingNode search;
    search.match = n;

    auto result = std::find_if(nodes.begin(), nodes.end(), search);

    if (result == nodes.end())
        return nullptr;

    if (sn.empty())
        return *result;

    if (!(*result)->IsCompound())
        return nullptr;

    if (pos != std::string::npos)
        return CompoundNode::FromNode(*result)->GetNode(sn);

    return *result;
}


Gex::NodeList Gex::CompoundNode::GetNodes() const
{
    return nodes;
}


std::vector<std::string> Gex::CompoundNode::GetNodeNames() const
{
    std::vector<std::string> names;
    for (auto* node : nodes)
    {
        names.push_back(node->Name());
    }

    return names;
}


Gex::Attribute* Gex::CompoundNode::FindAttribute(std::string attr) const
{
    size_t pos  = attr.find(Config::GetConfig().attributeSeparator.c_str());
    if (pos == std::string::npos)
    {
        return GetAttribute(attr);
    }

    std::string nodePath = attr.substr(0, pos);
    std::string attrPath = attr.substr(pos + 1, std::string::npos);

    auto* node = GetNode(nodePath);
    if (!node)
    {
        return nullptr;
    }

    return node->GetAttribute(attrPath);
}


std::string Gex::CompoundNode::UniqueName(const std::string& name) const
{
    return Gex::Utils::UniqueName(name, nodes);
}


bool Gex::CompoundNode::RemoveNode(const std::string& node)
{
    Node* nodePtr = GetNode(node);
    if (!nodePtr)
    {
        return false;
    }

    nodes.erase(std::find(nodes.begin(), nodes.end(), nodePtr));

    delete nodePtr;
    return true;
}


bool Gex::CompoundNode::HasNode(Node* node) const
{
    return (std::find(nodes.begin(), nodes.end(), node) != nodes.end());
}


bool Gex::CompoundNode::HasNode(const std::string& node) const
{
    Node* nodePtr = GetNode(node);
    if (!nodePtr)
        return false;

    return HasNode(nodePtr);
}


Gex::NodeList Gex::CompoundNode::InternalNodes() const
{
    return nodes;
}


std::vector<std::string> Gex::CompoundNode::InternalNodeNames() const
{
    std::vector<std::string> names;
    for (auto* node : nodes)
    {
        names.push_back(node->Name());
    }

    return names;
}


Gex::Node* Gex::CompoundNode::CreateNode(std::string type, std::string name)
{
    if (name.empty())
    {
        name = type;
    }

    int counter = 0;
    std::string uniqueName = name;
    std::vector<std::string> names = InternalNodeNames();
    while(std::find(names.begin(), names.end(), uniqueName) != names.end())
    {
        uniqueName = name + std::to_string(counter);
        counter++;
    }

    Node* newNode = NodeFactory::GetFactory()->CreateNode(type, uniqueName);
    if (!newNode)
    {
        return nullptr;
    }

    if (!AddNode(newNode))
    {
        delete newNode;
        return nullptr;
    }

    return newNode;
}


Gex::AttributeList Gex::CompoundNode::InternalAttributes() const
{
    AttributeList internals;
    for (auto attribute : attributes)
    {
        if (attribute.second->IsInternal())
            internals.push_back(attribute.second);
    }

    return internals;
}


Gex::AttributeList Gex::CompoundNode::AllInternalAttributes() const
{
    AttributeList internals;
    for (auto* attribute : GetAllAttributes())
    {
        if (attribute->IsInternal())
            internals.push_back(attribute);
    }

    return internals;
}


#define COMPOUND_NODES_K "CompoundNodes"
#define COMPOUND_CONNECTIONS_K "CompoundConnections"
#define THIS "This"


std::string _JsonNodeName(const Gex::Node* node,
                          const Gex::Node* root)
{
    if (node == root)
    {
        return THIS;
    }

    return node->Name();
}


void _JsonPushAttributeConnection(const Gex::Node* root,
                                  rapidjson::Value& connections,
                                  Gex::Attribute* attr,
                                  rapidjson::Document& json,
                                  std::function<bool(Gex::Node*)> validate=nullptr)
{
    Gex::Attribute* src = attr->Source();
    if (!src)
    {
        return;
    }

    Gex::Node* srcNode = src->Node();
    if (validate && !validate(srcNode))
        return;

    rapidjson::Value& connection = rapidjson::Value(rapidjson::kArrayType).GetArray();

    std::string cnx1Name = _JsonNodeName(srcNode, root);
    rapidjson::Value& cnx1 = rapidjson::Value().SetString(
            rapidjson::StringRef(cnx1Name.c_str()),
            json.GetAllocator());
    connection.PushBack(cnx1.Move(), json.GetAllocator());

    rapidjson::Value& cnx2 = rapidjson::Value().SetString(
            rapidjson::StringRef(src->Name().c_str()),
            json.GetAllocator());
    connection.PushBack(cnx2.Move(), json.GetAllocator());

    std::string cnx3Name = _JsonNodeName(attr->Node(), root);
    rapidjson::Value& cnx3 = rapidjson::Value().SetString(
            rapidjson::StringRef(cnx3Name.c_str()),
            json.GetAllocator());
    connection.PushBack(cnx3.Move(), json.GetAllocator());

    rapidjson::Value& cnx4 = rapidjson::Value().SetString(
            rapidjson::StringRef(attr->Name().c_str()),
            json.GetAllocator());
    connection.PushBack(cnx4.Move(), json.GetAllocator());

    connections.PushBack(connection.Move(), json.GetAllocator());
}


void Gex::CompoundNode::Serialize(rapidjson::Value& dict,
                                  rapidjson::Document& json)
                                  const
{
	Gex::Node::Serialize(dict, json);

    Config conf = Config::GetConfig();

    std::set<std::string> plugins;
    std::set<std::string> nodeTypes;

    rapidjson::Value& internalNodesDict = rapidjson::Value().SetObject();
    for (Node* node : nodes)
    {
        std::string plugin = node->Plugin();
        if (!plugin.empty())
            plugins.insert(plugin);

        nodeTypes.insert(node->Type());

        rapidjson::Value& internalNodeDict = rapidjson::Value().SetObject();

        NodeFactory::GetFactory()->SaveNode(node, internalNodeDict, json);

        rapidjson::Value& key = rapidjson::Value().SetString(rapidjson::StringRef(node->Name().c_str()),
                                                             json.GetAllocator());

        internalNodesDict.AddMember(key.Move(), internalNodeDict, json.GetAllocator());
    }

    rapidjson::Value& connections = rapidjson::Value(rapidjson::kArrayType).GetArray();
    for (auto node : nodes)
    {
        AttributeList attributes = node->GetAllAttributes();
        for (auto* attr : attributes)
        {
            Attribute* src = attr->Source();
            if (!src)
            {
                continue;
            }

            Node* srcNode = src->Node();

            std::string srcNodeValue = srcNode->Name();
            if (srcNode == this)
            {
                srcNodeValue = conf.thisKey;
            }

            rapidjson::Value& connection = rapidjson::Value(rapidjson::kArrayType).GetArray();
            connection.PushBack(rapidjson::Value().SetString(srcNodeValue.c_str(), json.GetAllocator()).Move(),
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

    for (auto* attr : AllInternalAttributes())
    {
        if (!attr->HasSource())
        {
            continue;
        }

        auto* source = attr->Source();
        if (!HasNode(source->Node()))
        {
            continue;
        }

        rapidjson::Value& connection = rapidjson::Value(rapidjson::kArrayType).GetArray();

        rapidjson::Value& srcNode = rapidjson::Value().SetString(
                source->Node()->Name().c_str(), json.GetAllocator());
        connection.PushBack(srcNode.Move(), json.GetAllocator());

        rapidjson::Value& srcAttr = rapidjson::Value().SetString(
                source->Longname().c_str(), json.GetAllocator());
        connection.PushBack(srcAttr.Move(), json.GetAllocator());

        rapidjson::Value& dstNode = rapidjson::Value().SetString(
                conf.thisKey.c_str(), json.GetAllocator());
        connection.PushBack(dstNode.Move(), json.GetAllocator());

        rapidjson::Value& dstAttr = rapidjson::Value().SetString(
                attr->Longname().c_str(), json.GetAllocator());
        connection.PushBack(dstAttr.Move(), json.GetAllocator());

        connections.PushBack(connection.Move(), json.GetAllocator());
    }

    rapidjson::Value& pluginsValue = rapidjson::Value().SetArray();
    for (const auto& plugin : plugins)
    {
        pluginsValue.PushBack(rapidjson::Value().SetString(plugin.c_str(), json.GetAllocator()).Move(),
                              json.GetAllocator());
    }

    rapidjson::Value& typesValue = rapidjson::Value().SetArray();
    for (const auto& type : nodeTypes)
    {
        typesValue.PushBack(rapidjson::Value().SetString(type.c_str(), json.GetAllocator()).Move(),
                            json.GetAllocator());
    }

    rapidjson::Value& nodesKey = rapidjson::Value().SetString(
            conf.compoundNodesKey.c_str(), json.GetAllocator());
    dict.AddMember(nodesKey.Move(), internalNodesDict,
                   json.GetAllocator());

    rapidjson::Value& connectionsKey = rapidjson::Value().SetString(
            conf.compoundConnectionsKey.c_str(), json.GetAllocator());
    dict.AddMember(connectionsKey.Move(), connections,
                   json.GetAllocator());

    rapidjson::Value& graphPluginsKey = rapidjson::Value().SetString(
            conf.compoundPluginsKey.c_str(), json.GetAllocator());
    dict.AddMember(graphPluginsKey.Move(), pluginsValue,
                   json.GetAllocator());

    rapidjson::Value& graphNodeTypes = rapidjson::Value().SetString(
            conf.compoundNodeTypes.c_str(), json.GetAllocator());
    dict.AddMember(graphNodeTypes.Move(), typesValue,
                   json.GetAllocator());
}


bool Gex::CompoundNode::Deserialize(rapidjson::Value& dict)
{
    bool success = Gex::Node::Deserialize(dict);

    if (!success)
        return success;

    Config conf = Config::GetConfig();

    std::map<std::string, Node*> createdNodes;
    if (dict.HasMember(conf.compoundNodesKey.c_str()))
    {
        rapidjson::Value& nodes_ = dict[conf.compoundNodesKey.c_str()];
        for (auto iter = nodes_.MemberBegin(); iter != nodes_.MemberEnd(); iter++)
        {
            Node* node = NodeFactory::GetFactory()->LoadNode(iter->value);
            if (!node)
            {
                success = false;
                continue;
            }

            AddNode(node);

            createdNodes[iter->name.GetString()] = node;
        }
    }

    if (dict.HasMember(conf.compoundConnectionsKey.c_str()))
    {
        rapidjson::Value& connections = dict[conf.compoundConnectionsKey.c_str()];
        for (auto iter = connections.Begin(); iter != connections.End(); iter++)
        {
            rapidjson::Value& list = iter->GetArray();
            std::string srcname = list[0].GetString();
            std::string srcnattr = list[1].GetString();
            std::string dstname = list[2].GetString();
            std::string dstattr = list[3].GetString();

            Node* src = nullptr;
            if (srcname == conf.thisKey)
            {
                src = this;
            }
            else
            {
                if (createdNodes.find(srcname) == createdNodes.end())
                {
                    continue;
                }

                src = createdNodes.at(srcname);
            }

            Node* dst = nullptr;
            if (dstname == conf.thisKey)
            {
                dst = this;
            }
            else
            {
                if (createdNodes.find(dstname) == createdNodes.end())
                {
                    continue;
                }

                dst = createdNodes.at(dstname);
            }

            Attribute* srcat = src->GetAttribute(srcnattr);
            if (!srcat)
            {
                continue;
            }

            Attribute* dstat = dst->GetAttribute(dstattr);
            if (!dstat)
            {
                continue;
            }

            if (!dstat->ConnectSource(srcat))
            {
                continue;
            }
        }
    }

    return success;
}


Gex::Feedback Gex::CompoundNode::CheckLoadStatus(rapidjson::Value& dict)
{
    Feedback res;

    Config conf = Config::GetConfig();

    std::set<std::string> missingTypes;
    if (dict.HasMember(conf.compoundNodeTypes.c_str()))
    {
        rapidjson::Value& types = dict[conf.compoundNodeTypes.c_str()];
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
    if (dict.HasMember(conf.compoundNodeTypes.c_str()))
    {
        rapidjson::Value& plugins = dict[conf.compoundNodeTypes.c_str()];
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
    for (const std::string& missingType : missingTypes)
    {
        message += "    - " + missingType + "\n";
    }

    if (!missingPlugins.empty())
    {
        message += "This may be related to missing plugins : \n";
        for (const std::string& missingPlugin : missingPlugins)
        {
            message += "    - " + missingPlugin + "\n";
        }
    }

    res.status = Status::Failed;
    res.message = message;
    return res;
}


bool Gex::CompoundNode::Evaluate(NodeAttributeData &ctx,
                                 GraphContext &graphCtx,
                                 NodeProfiler& profiler)
{
    return true;
}


void Gex::CompoundNode::AttributeChanged(Attribute* attr, AttributeChange change)
{
    Node::AttributeChanged(attr, change);

    if (change == AttributeChange::Connected
        || change == AttributeChange::Disconnected
        || change == AttributeChange::ChildAttributeConnected
        || change == AttributeChange::ChildAttributeDisconnected)
    {
        InvalidateScheduling();
    }
}


Gex::ScheduleNodeList Gex::CompoundNode::ToScheduledNodes()
{
    auto schelNodes = Gex::ScheduleNodes(nodes);

    auto* preScheduleNode = new CompoundPreScheduledNode(this);
    auto* postScheduleNode = new CompoundPostScheduledNode(this);

    // For scheduled nodes, if any has the current compound as input,
    // replace it with preScheduled. Use postScheduled if they have
    // output.
    for (auto* schel : schelNodes)
    {
        auto srcs = schel->node->UpstreamNodes();
        if (std::find(srcs.begin(), srcs.end(), this) != srcs.end())
        {
            schel->previousNodes.push_back(preScheduleNode);
        }

        auto dsts = schel->node->DownstreamNodes();
        if (std::find(dsts.begin(), dsts.end(), this) != dsts.end())
        {
            schel->futureNodes.push_back(postScheduleNode);
        }
    }

    schelNodes.insert(schelNodes.begin(), preScheduleNode);
    schelNodes.push_back(postScheduleNode);

    return schelNodes;
}


bool Gex::CompoundNode::PreEvaluate(NodeAttributeData &ctx,
                                    GraphContext &graphContext,
                                    NodeProfiler& profiler)
{
    return true;
}


bool Gex::CompoundNode::PreCompute(GraphContext &context,
                                   NodeProfiler& profiler)
{
    auto data = createEvalContext();
    return PreEvaluate(data, context, profiler);
}


bool Gex::CompoundNode::PostEvaluate(NodeAttributeData &ctx,
                                     GraphContext &graphContext,
                                     NodeProfiler& profiler)
{
    return true;
}


bool Gex::CompoundNode::PostCompute(GraphContext &context,
                                   NodeProfiler& profiler)
{
    auto data = createEvalContext();
    return PostEvaluate(data, context, profiler);
}


void Gex::CompoundNode::PullInternalOutputs()
{
    for (auto* attribute : InternalAttributes())
    {
        if (attribute->IsOutput() && !attribute->IsStatic())
        {
            attribute->Pull();
        }
    }
}


bool Gex::CompoundNode::Compute(GraphContext &context,
                                NodeProfiler& profiler)
{
    Pull();
    NodeAttributeData evalContext = createEvalContext();
    if (!PreEvaluate(evalContext, context, profiler))
    {
        return false;
    }

    if (!Evaluate(evalContext, context, profiler))
    {
        return false;
    }

    return PostEvaluate(evalContext, context, profiler);
}



Gex::Node* Duplicate(std::map<Gex::Node*, Gex::Node*>& res,
                     Gex::Node* source, const std::string& name,
                     Gex::CompoundNode* parent)
{
    std::string type = source->Type();
    std::string sourceName = source->Name();

    std::string newName = name;
    if (newName.empty())
        newName = sourceName + "1";

    Gex::Node* new_ = Gex::NodeFactory::GetFactory()->CreateNode(type, newName);

    parent->AddNode(new_);

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

    if (source->IsCompound())
    {
        auto* compound = Gex::CompoundNode::FromNode(source);
        auto* newCompound = Gex::CompoundNode::FromNode(new_);
        for (auto* cmpn : compound->InternalNodes())
        {
            Duplicate(res, cmpn, "", newCompound);
        }
    }

    res[source] = new_;

    return new_;
}


Gex::NodeList Gex::CompoundNode::DuplicateNodes(NodeList sources, bool copyLinks)
{
    std::map<Gex::Node*, Gex::Node*> nmap;

    Gex::NodeList result;

    for (auto* source : sources)
    {
        Gex::Node* dup = Duplicate(nmap, source, "", this);
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
//                    else if (copyExternal)
//                    {
                    destAttribute->ConnectSource(atsource);
//                    }
                }
            }
        }
    }

    return result;
}


Gex::Node* Gex::CompoundNode::ToCompound(NodeList sources, bool duplicate,
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
        compound->AddNode(node);
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
                        proxy = compound->CreateAttribute(
                                pname, type,
                                proxy->IsMulti());
                    }
                    else
                    {
                        proxy = compound->CreateAttributeFromValue(
                                pname, src->GetAnyValue(),
                                src->GetAttrValueType(),
                                type);
                    }

                    proxy->SetInternal(true);
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




Gex::CompoundNode* Gex::CompoundNode::FromNode(Node* node)
{
    if (!node->IsCompound())
        return nullptr;

    // As node->IsCompound() already declares whether
    // this cast is possible, static_cast should be
    // faster here. Still, no further check is applied
    // meaning that declaring a standard node as a
    // compound will cause a crash.
    return static_cast<CompoundNode*>(node);
}
