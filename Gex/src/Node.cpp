#include "Gex/include/Node.h"
#include "Gex/include/Attribute.h"
#include "Gex/include/NodeFactory.h"
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include "Gex/include/Evaluation.h"
#include "Gex/include/Config.h"
#include "Gex/include/utils.h"
#include "Gex/include/PluginLoader.h"
#include "Gex/include/UndoCommands.h"

#include "Tsys/include/tsys.h"
#include "Tsys/include/defaultTypes.h"

#include <iterator>
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


Gex::Node::Node(const NodePtr& parent_)
{
    parent = parent_;
    nextId = 0;
}


void Gex::Node::SetParent(const NodePtr& parent_)
{
    parent = parent_;
}


Gex::Node::~Node()
{
    if (!deleteCallbacks.empty())
    {
        for (auto iter = deleteCallbacks.rbegin();
             iter != deleteCallbacks.rend();
             iter++)
        {
            iter->second();
        }
    }

	attributes.clear();
}


Gex::NodeWkPtr Gex::Node::Parent()
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

bool Gex::Node::operator ==(const NodePtr& other)
{
    return (other.get() == this);
}

bool Gex::Node::operator ==(const NodeWkPtr& other)
{
    if (other.expired())
        return false;

    return (other.ToShared().get() == this);
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
    return isEditable;
}


void Gex::Node::SetType(std::string type)
{
    typeName = type;
}


void Gex::Node::SetPlugin(std::string plugin)
{
    pluginName = plugin;
}


void Gex::Node::SetEditable(bool editable)
{
    isEditable = editable;
}


bool Gex::Node::IsCompound() const
{
    return false;
}


std::any Gex::Node::InitValue(const std::type_info& t)
{
    auto* registry = TSys::TypeRegistry::GetRegistry();

    auto handler = registry->GetTypeHandle(t);
    if (!handler)
    {
        return {};
    }

    return handler->InitValue();
}


Gex::AttributeWkPtr Gex::Node::CreateAttributeFromValue(
        const std::string& name,
        const std::any& v,
        const AttrValueType& valueType,
        const AttrType& type,
        const AttributeWkPtr& parent
)
{
    auto attribute = std::make_shared<Attribute>(
            name, v, valueType, type, !IsInitializing(),
            shared_from_this(), parent);

    bool success = false;
    if (!parent)
    {
        success = AddAttribute(attribute);
    }
    else
    {
        success = parent->AddChildAttribute(attribute);
    }

    return attribute;
}


Gex::AttributeWkPtr Gex::Node::CreateAttributeFromTypeName(
        const std::string& name, const std::string& apiName,
        const AttrValueType& valueType, const AttrType& type,
        const AttributeWkPtr& parent)
{
    auto* registry = TSys::TypeRegistry::GetRegistry();
    auto handler = registry->GetTypeHandle(apiName);
    if (!handler)
    {
        std::cerr << "Could not find handler for type " << apiName << std::endl;
        return {};
    }

    return CreateAttributeFromValue(name, handler->InitValue(),
                                    valueType, type, parent);
}


Gex::AttributeWkPtr Gex::Node::CreateAttribute(
        const std::string& name, const AttrType& type,
        bool multi, const AttributeWkPtr& parent)
{
    AttributePtr attribute = std::make_shared<Attribute>(
            name, type, multi, !IsInitializing(),
            shared_from_this(), parent);
    bool success = false;
    if (!parent)
    {
        success = AddAttribute(attribute);
    }
    else
    {
        success = parent->AddChildAttribute(attribute);
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


bool Gex::Node::IsReferenced() const
{
    return (!referencePath.empty());
}


std::string Gex::Node::ReferencePath() const
{
    return referencePath;
}


void Gex::Node::SetReferencePath(const std::string& path)
{
    referencePath = path;
}


void Gex::Node::SignalChange(const NodeChange& change, const NodeWkPtr& node) const
{
    if (initializing)
        return;

    for (const auto& callback : changeCallbacks)
    {
        callback.second(change, node);
    }
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
    if (!parent.expired())
    {
        name = parent.ToShared()->UniqueName(p);
    }

    nodeName = Gex::Utils::ValidateName(name);
    return nodeName;
}


Gex::CallbackId Gex::Node::RegisterAttributeCallback(
        AttributeChangeCallback cb)
{
    CallbackId id = nextId;
    attrCallbacks[id] = std::move(cb);

    nextId++;
    return id;
}


bool Gex::Node::DeregisterAttributeCallback(
        unsigned int index)
{
    if (attrCallbacks.empty())
        return false;

    auto idx = attrCallbacks.find(index);
    if (idx == attrCallbacks.end())
        return false;

    attrCallbacks.erase(idx);
    return true;
}


Gex::CallbackId Gex::Node::RegisterAboutToBeDeletedCallback(
        AboutToBeDeletedCallback cb)
{
    CallbackId id = deleteCbId;
    deleteCallbacks[deleteCbId] = std::move(cb);

    deleteCbId++;
    return id;
}


bool Gex::Node::DeregisterAboutToBeDeletedCallback(
        CallbackId index)
{
    if (deleteCallbacks.empty())
        return false;

    auto idx = deleteCallbacks.find(index);
    if (idx == deleteCallbacks.end())
        return false;

    deleteCallbacks.erase(idx);
    return true;
}


Gex::CallbackId Gex::Node::RegisterNodeChangedCallback(
        NodeChangeCallback cb)
{
    CallbackId id = changeCbId;
    changeCallbacks[changeCbId] = std::move(cb);

    changeCbId++;
    return id;
}


bool Gex::Node::DeregisterNodeChangedCallback(
        CallbackId index)
{
    if (changeCallbacks.empty())
        return false;

    auto idx = changeCallbacks.find(index);
    if (idx == changeCallbacks.end())
        return false;

    changeCallbacks.erase(idx);
    return true;
}


bool Gex::Node::AddAttribute(const AttributePtr& attr)
{
	std::string _name = attr->Longname();

	if (ContainsAttributeByName(attributes, _name))
	{
		return false;
	}

	attr->SetParentNode(shared_from_this());
	attributes.push_back(attr);

    SignalAttributeChange(attr, AttributeChange::AttributeAdded);
    SignalChange(NodeChange::AttributeAdded, shared_from_this());
	return true;
}


bool Gex::Node::RemoveAttribute(const AttributePtr& attr)
{
    std::string _name = attr->Longname();

    auto idx = FindAttributeItByName(attributes, _name);
    if (idx == attributes.end())
    {
        return false;
    }

    SignalAttributeChange(*idx, AttributeChange::AttributeRemoved);
    SignalChange(NodeChange::AttributeAdded, shared_from_this());
    attributes.erase(idx);
    return true;
}


bool Gex::Node::HasAttribute(const std::string& name) const
{
	return ContainsAttributeByName(attributes, name);
}


Gex::AttributeWkPtr Gex::Node::GetAttribute(const std::string& name) const
{
	size_t delimiter = name.find(Config::GetConfig().attributeSeparator);
	std::string shortname = name.substr(0, delimiter);

	if (!HasAttribute(shortname))
	{
		return {};
	}

	if (delimiter == std::string::npos)
	{
		return FindAttributeByName(attributes, name);
	}

	delimiter++;
    std::string cname = name;
	std::string subname = cname.erase(0, delimiter);
	return FindAttributeByName(attributes, shortname)->GetAttribute(subname);
}


Gex::AttributeWkList Gex::Node::ExtraAttributes()
{
	AttributeWkList extraAttrs;
	for (const auto& a : attributes)
	{
		if (!a->IsUserDefined())
		{
			continue;
		}

		extraAttrs.push_back(a);
	}

	return extraAttrs;
}


Gex::AttributeWkList Gex::Node::GetAttributes() const
{
	AttributeWkList attrs;
	for (const auto& n : attributes)
	{
		attrs.emplace_back(n);
	}

	return attrs;
}


Gex::AttributeWkList Gex::Node::GetAllAttributes() const
{
    AttributeWkList attrs;
    for (const auto& n : attributes)
    {
        attrs.emplace_back(n);

        for (const AttributeWkPtr& at : n->GetAllAttributes())
        {
            attrs.push_back(at);
        }
    }

    return attrs;
}


Gex::NodeWkList Gex::Node::UpstreamNodes()
{
	NodeWkList sources;
	for (const AttributeWkPtr& a : GetAttributes())
	{
        if (!a)
            continue;

		Gex::AttributeWkList srcs = a->Sources();
        if (srcs.empty())
        {
            continue;
        }

        for (const auto& src : srcs)
        {
            if (src.expired())
                continue;

            NodeWkPtr srcNode = src->Node();
            if (!srcNode)
                continue;

            if (AreWkEqual(srcNode, Parent()))
            {
                continue;
            }

            if (sources.empty())
            {
                sources.push_back(srcNode);
            }

            if (!WkInVector(sources, srcNode))
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

    Gex::NodeWkList analyse = node->UpstreamNodes();

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


Gex::NodeWkList Gex::Node::DownstreamNodes()
{
	NodeWkList destinations;
	for (const AttributeWkPtr& a : GetAttributes())
	{
        if (!a)
            continue;

        auto sharedAttr = a.ToShared();
		if (!sharedAttr->HasDests())
		{
			continue;
		}

        AttributeWkList attrDests = sharedAttr->Dests();
		for (const AttributeWkPtr& attrDest : attrDests)
		{
            if (attrDest.expired())
                continue;

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
    for (const AttributeWkPtr& wkattr : GetAttributes())
    {
        if (!wkattr)
            continue;

        rapidjson::Value& attrValue = rapidjson::Value(rapidjson::kObjectType).SetObject();
        if (wkattr->IsUserDefined())
        {
            wkattr->SerializeAttribute(customAttrs, json);
        }

        wkattr->Serialize(attrValue, json);

        // Only add attribute if some data was serialized.
        if (attrValue.MemberCount())
        {
            attrValues.AddMember(rapidjson::Value().SetString(wkattr->Name().c_str(), json.GetAllocator()).Move(),
                                 attrValue, json.GetAllocator());
        }
    }

    if (attrValues.MemberCount())
    {
        rapidjson::Value& attrKey = rapidjson::Value().SetString(conf.nodeAttributesKey.c_str(),
                                                                 json.GetAllocator());
        dict.AddMember(attrKey, attrValues, json.GetAllocator());
    }

    // TODO : For now the custom attributes are never stored
    //  on referenced node, when we'd like to store attributes
    //  that may have been created on it after referencing.
    if (customAttrs.MemberCount() && !IsReferenced())
    {
        rapidjson::Value& cstmAttrKey = rapidjson::Value().SetString(
                conf.nodeCustomAttributesKey.c_str(),
                json.GetAllocator());
        dict.AddMember(cstmAttrKey.Move(), customAttrs, json.GetAllocator());
    }

    rapidjson::Value mtdt = metadata.Serialize(json);
    if (mtdt.Size())
    {
        rapidjson::Value& mKey = rapidjson::Value().SetString(
                conf.nodeMetadataKey.c_str(), json.GetAllocator());
        dict.AddMember(mKey.Move(), mtdt, json.GetAllocator());
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
            AttributePtr cust = Attribute::DeserializeAttribute(
                    itr->name.GetString(), itr->value.GetObject(),
                    shared_from_this(), nullptr, true);
            AddAttribute(cust);
        }
    }

    if (dict.HasMember(conf.nodeAttributesKey.c_str()))
    {
        rapidjson::Value& attrValues = dict[conf.nodeAttributesKey.c_str()];
        for (auto itr = attrValues.MemberBegin(); itr != attrValues.MemberEnd(); ++itr)
        {
            std::string name = itr->name.GetString();
            AttributeWkPtr attr = GetAttribute(name);
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

    if (dict.HasMember(conf.nodeMetadataKey.c_str()))
    {
        rapidjson::Value& m = dict[conf.nodeMetadataKey.c_str()];
        metadata.Deserialize(m);
    }

    return true;
}


void Gex::Node::FactoryReset()
{
    for (const auto& attribute : attributes)
    {
        attribute->FactoryReset();
    }
}


bool Gex::Node::PullAttributes()
{
    bool result = true;
    for (const auto& attr : attributes)
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

    InitAttributes();

    CreateBuiltinsAttributes();

    EndInitialization();
}


void Gex::Node::InitAttributes()
{

}


void Gex::Node::SignalAttributeChange(const AttributePtr& attribute,
                                      const AttributeChange& change)
{
    if (initializing)
        return;

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


void Gex::Node::AttributeChanged(const AttributePtr& attribute,
                                 const AttributeChange& change)
{

}


Gex::NodeAttributeData Gex::Node::createEvalContext()
{
    return NodeAttributeData(shared_from_this());
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
    scheduledNodes = ScheduleNodes({shared_from_this()}, true);

    ValidateScheduling();
}


Gex::ScheduledNodePtr Gex::Node::ToScheduledNode()
{
    return std::make_shared<ScheduledNode>(shared_from_this());
}


Gex::ScheduleNodePtrList Gex::Node::ToScheduledNodes()
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
    if (node.expired())
        return false;

    auto cmp = Gex::CompoundNode::FromNode(node);
    cmp->Pull();

    return cmp->PreCompute(context, profiler);
}


bool Gex::CompoundPostScheduledNode::Evaluate(Gex::GraphContext &context,
                                             Gex::NodeProfiler &profiler)
{
    if (node.expired())
        return false;

    auto cmp = Gex::CompoundNode::FromNode(node);
    cmp->PullInternalOutputs();

    return cmp->PostCompute(context, profiler);
}


Gex::CompoundNode::CompoundNode(const NodePtr& parent): Node(parent)
{

}


Gex::CompoundNode::~CompoundNode()
{
    nodes.clear();
}


bool Gex::CompoundNode::IsCompound() const
{
    return true;
}


bool Gex::CompoundNode::_AddNode(const NodePtr& node)
{
    node->SetParent(shared_from_this());
    node->SetName(node->Name());

    nodes.push_back(node);

    SignalChange(NodeChange::ChildNodeAdded, node);
    return true;
}


bool Gex::CompoundNode::AddNode(const NodePtr& node)
{
    if (!IsEditable() || HasNode(node))
    {
        return false;
    }

    auto thisSharedCompound = Gex::Node::ConvertTo<Gex::CompoundNode>(shared_from_this());

    auto addCb = [thisSharedCompound](const Gex::NodePtr& node)
    {
        return thisSharedCompound->_AddNode(node);
    };

    auto rmCB = [thisSharedCompound](const Gex::NodePtr& node)
    {
        return thisSharedCompound->_RemoveNode(node);
    };

    addCb(node);

    Gex::Undo::UndoStack::AddUndo(
        Gex::Undo::CreateUndo<Gex::Undo::CreateNode>(
                thisSharedCompound,
                node, addCb, rmCB
        )
    );

    return true;
}


bool Gex::CompoundNode::_RemoveNode(const NodePtr& node)
{
    auto index = std::find_if(
        nodes.begin(), nodes.end(),
        [node](const NodePtr& other)
        {
            return node == other;
        }
    );

    nodes.erase(index);
    return true;
}


void Gex::CompoundNode::RemoveNodeWithUndo(const NodePtr& node)
{
    // To be efficiently able to use Undo commands, the next
    // step of this command consists in making an undo command
    // that will get all the necessary intels and callbacks
    // to perform the action.

    auto thisShared = Node::ConvertTo<Gex::CompoundNode>(shared_from_this());

    auto rmCallback = [thisShared](const Gex::NodePtr& node)
    {
        return thisShared->_RemoveNode(node);
    };

    auto addCallback = [thisShared](const Gex::NodePtr& node)
    {
        return thisShared->AddNode(node);
    };

    auto command = Undo::CreateUndo<Undo::DeleteNode>(
            node, rmCallback, addCallback
    );

    rmCallback(node);

    Undo::UndoStack::AddUndo(command);

    SignalChange(NodeChange::ChildNodeRemoved, node);
}


bool Gex::CompoundNode::CanRemoveNode() const
{
    return (!IsReferenced() || IsEditable());
}


bool Gex::CompoundNode::RemoveNode(const NodePtr& node)
{
    if (!CanRemoveNode())
    {
        return false;
    }

    RemoveNodeWithUndo(node);
    return true;
}


struct MatchingNode
{
    std::string match;

    bool operator()(const Gex::NodePtr& node) const
    {
        return(node->Name() == match);
    }
};


Gex::NodeWkPtr Gex::CompoundNode::GetNode(const std::string& node) const
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

    auto result = std::find_if(
            nodes.begin(), nodes.end(), search);

    if (result == nodes.end())
        return {};

    if (sn.empty())
        return *result;

    if (!(*result)->IsCompound())
        return {};

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
    for (const auto& node : nodes)
    {
        names.push_back(node->Name());
    }

    return names;
}


Gex::AttributeWkPtr Gex::CompoundNode::FindAttribute(const std::string& attr) const
{
    size_t pos  = attr.find(Config::GetConfig().attributeSeparator.c_str());
    if (pos == std::string::npos)
    {
        return GetAttribute(attr);
    }

    std::string nodePath = attr.substr(0, pos);
    std::string attrPath = attr.substr(pos + 1, std::string::npos);

    auto node = GetNode(nodePath);
    if (!node)
    {
        return {};
    }

    return node->GetAttribute(attrPath);
}


std::string Gex::CompoundNode::UniqueName(const std::string& name) const
{
    return Gex::Utils::UniqueName(name, nodes);
}


bool Gex::CompoundNode::RemoveNode(const std::string& node)
{
    if (!CanRemoveNode())
        return false;

    auto pred = [node](const NodePtr& other) {
        bool res = (other->Name() == node);
        return res;
    };

    auto index = std::find_if(nodes.begin(), nodes.end(), pred);
    if (index == nodes.end())
        return false;

    RemoveNodeWithUndo(*index);
    return true;
}


bool Gex::CompoundNode::HasNode(const NodePtr& node) const
{
    return (std::find(nodes.begin(), nodes.end(), node) != nodes.end());
}


bool Gex::CompoundNode::HasNode(const NodeWkPtr& node) const
{
    if (!node)
        return false;

    return HasNode(node.ToShared());
}


bool Gex::CompoundNode::HasNode(const std::string& node) const
{
    return (!GetNode(node).expired());
}


Gex::NodeList Gex::CompoundNode::InternalNodes() const
{
    return nodes;
}


std::vector<std::string> Gex::CompoundNode::InternalNodeNames() const
{
    std::vector<std::string> names;
    for (const auto& node : nodes)
    {
        names.push_back(node->Name());
    }

    return names;
}


Gex::NodePtr Gex::CompoundNode::CreateNode(const std::string& type, std::string name)
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

    NodePtr newNode = NodeFactory::GetFactory()->CreateNode(type, uniqueName);
    if (!newNode)
    {
        return nullptr;
    }

    if (!AddNode(newNode))
    {
        return nullptr;
    }

    return newNode;
}


Gex::NodePtr Gex::CompoundNode::ReferenceNode(const std::string& path, std::string name)
{
    if (name.empty())
        name = std::filesystem::path(name).filename().stem().string();

    NodePtr referencedNode = NodeFactory::GetFactory()->ReferenceNode(path, name);
    if (!referencedNode)
    {
        return nullptr;
    }

    auto thisSharedCompound = Node::ConvertTo<Gex::CompoundNode>(shared_from_this());

    auto addCb = [thisSharedCompound](const Gex::NodePtr& node)
    {
        return thisSharedCompound->_AddNode(node);
    };

    auto rmCB = [thisSharedCompound](const Gex::NodePtr& node)
    {
        return thisSharedCompound->_RemoveNode(node);
    };

    addCb(referencedNode);

    Gex::Undo::UndoStack::AddUndo(
            Gex::Undo::CreateUndo<Gex::Undo::CreateNode>(
                    thisSharedCompound,
                    referencedNode,
                    addCb, rmCB
            )
    );

    return referencedNode;
}


Gex::AttributeList Gex::CompoundNode::InternalAttributes() const
{
    AttributeList internals;
    for (const auto& attribute : attributes)
    {
        if (attribute->IsInternal())
            internals.push_back(attribute);
    }

    return internals;
}


Gex::AttributeList Gex::CompoundNode::AllInternalAttributes() const
{
    AttributeList internals;
    for (const auto& attribute : GetAllAttributes())
    {
        if (!attribute)
            continue;

        if (attribute->IsInternal())
            internals.push_back(attribute.ToShared());
    }

    return internals;
}


#define COMPOUND_NODES_K "CompoundNodes"
#define COMPOUND_CONNECTIONS_K "CompoundConnections"
#define THIS "This"


std::string _JsonNodeName(const Gex::NodePtr& node,
                          const Gex::NodePtr& root)
{
    if (node == root)
    {
        return THIS;
    }

    return node->Name();
}


void _JsonPushAttributeConnection(const Gex::NodePtr root,
                                  rapidjson::Value& connections,
                                  Gex::AttributePtr attr,
                                  rapidjson::Document& json,
                                  std::function<bool(Gex::NodePtr)> validate=nullptr)
{
    Gex::AttributeWkPtr src = attr->Source();
    if (src.expired())
    {
        return;
    }

    Gex::NodeWkPtr srcNode = src->Node();
    if (validate && !validate(srcNode.ToShared()))
        return;

    rapidjson::Value& connection = rapidjson::Value(rapidjson::kArrayType).GetArray();

    std::string cnx1Name = _JsonNodeName(srcNode.ToShared(), root);
    rapidjson::Value& cnx1 = rapidjson::Value().SetString(
            rapidjson::StringRef(cnx1Name.c_str()),
            json.GetAllocator());
    connection.PushBack(cnx1.Move(), json.GetAllocator());

    rapidjson::Value& cnx2 = rapidjson::Value().SetString(
            rapidjson::StringRef(src.ToShared()->Name().c_str()),
            json.GetAllocator());
    connection.PushBack(cnx2.Move(), json.GetAllocator());

    std::string cnx3Name = _JsonNodeName(attr->Node().ToShared(), root);
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

    // Referenced nodes does not need to store all
    // internal data as it is already stored in
    // graph file. Instead, serialize all parent
    // node attributes and connections, and leave
    // all the internal data as it is described in
    // the graph file.
    if (IsReferenced())
        return;

    Config conf = Config::GetConfig();

    std::set<std::string> plugins;
    std::set<std::string> nodeTypes;

    rapidjson::Value& internalNodesDict = rapidjson::Value().SetObject();
    for (const NodePtr& node : nodes)
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
    for (const NodePtr& node : nodes)
    {
        AttributeWkList attributes = node->GetAllAttributes();
        for (const AttributeWkPtr& attr : attributes)
        {
            if (!attr)
            {
                continue;
            }

            AttributeWkPtr src = attr->Source();
            if (!src)
            {
                continue;
            }

            NodeWkPtr srcNode = src->Node();
            if (!srcNode)
                continue;

            std::string srcNodeValue = srcNode.ToShared()->Name();
            if (srcNode.ToShared().get() == this)
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

    for (const auto& attr : AllInternalAttributes())
    {
        if (!attr->HasSource())
        {
            continue;
        }

        AttributeWkPtr source = attr->Source();
        if (!source)
            continue;

        NodeWkPtr sourceNode = source->Node();
        if (!sourceNode)
            continue;

        if (!HasNode(sourceNode))
        {
            continue;
        }

        rapidjson::Value& connection = rapidjson::Value(rapidjson::kArrayType).GetArray();

        rapidjson::Value& srcNode = rapidjson::Value().SetString(
                sourceNode->Name().c_str(), json.GetAllocator());
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

    if (internalNodesDict.MemberCount())
    {
        rapidjson::Value& nodesKey = rapidjson::Value().SetString(
                conf.compoundNodesKey.c_str(), json.GetAllocator());
        dict.AddMember(nodesKey.Move(), internalNodesDict,
                       json.GetAllocator());
    }

    if (connections.Size())
    {
        rapidjson::Value& connectionsKey = rapidjson::Value().SetString(
                conf.compoundConnectionsKey.c_str(), json.GetAllocator());
        dict.AddMember(connectionsKey.Move(), connections,
                       json.GetAllocator());
    }

    if (pluginsValue.Size())
    {
        rapidjson::Value& graphPluginsKey = rapidjson::Value().SetString(
                conf.compoundPluginsKey.c_str(), json.GetAllocator());
        dict.AddMember(graphPluginsKey.Move(), pluginsValue,
                       json.GetAllocator());
    }

    if (typesValue.Size())
    {
        rapidjson::Value& graphNodeTypes = rapidjson::Value().SetString(
                conf.compoundNodeTypes.c_str(), json.GetAllocator());
        dict.AddMember(graphNodeTypes.Move(), typesValue,
                       json.GetAllocator());
    }
}


bool Gex::CompoundNode::Deserialize(rapidjson::Value& dict)
{
    bool success = Gex::Node::Deserialize(dict);

    if (!success)
        return success;

    Config conf = Config::GetConfig();

    std::map<std::string, NodePtr> createdNodes;
    if (dict.HasMember(conf.compoundNodesKey.c_str()))
    {
        rapidjson::Value& nodes_ = dict[conf.compoundNodesKey.c_str()];
        for (auto iter = nodes_.MemberBegin(); iter != nodes_.MemberEnd(); iter++)
        {
            NodePtr node = NodeFactory::GetFactory()->LoadNode(iter->value);
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

            NodePtr src = nullptr;
            if (srcname == conf.thisKey)
            {
                src = shared_from_this();
            }
            else
            {
                if (createdNodes.find(srcname) == createdNodes.end())
                {
                    continue;
                }

                src = createdNodes.at(srcname);
            }

            NodePtr dst = nullptr;
            if (dstname == conf.thisKey)
            {
                dst = shared_from_this();
            }
            else
            {
                if (createdNodes.find(dstname) == createdNodes.end())
                {
                    continue;
                }

                dst = createdNodes.at(dstname);
            }

            AttributeWkPtr srcat = src->GetAttribute(srcnattr);
            if (!srcat)
            {
                continue;
            }

            auto sharedSrc = ToSharedPtr(srcat);

            AttributeWkPtr dstat = dst->GetAttribute(dstattr);
            if (!dstat)
            {
                continue;
            }

            auto sharedDst = ToSharedPtr(dstat);

            if (!sharedDst->ConnectSource(sharedSrc))
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


void Gex::CompoundNode::AttributeChanged(const AttributePtr& attr, const AttributeChange& change)
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


Gex::ScheduleNodePtrList Gex::CompoundNode::ToScheduledNodes()
{
    auto schelNodes = Gex::ScheduleNodes(nodes);

    auto* preScheduleNode = new CompoundPreScheduledNode(shared_from_this());
    auto* postScheduleNode = new CompoundPostScheduledNode(shared_from_this());

    // For scheduled nodes, if any has the current compound as input,
    // replace it with preScheduled. Use postScheduled if they have
    // output.
    NodeWkPtr weakThis =weak_from_this();
    for (const auto& schel : schelNodes)
    {
        auto srcs = schel->node->UpstreamNodes();
        if (WkInVector(srcs, weakThis))
        {
            schel->previousNodes.emplace_back(preScheduleNode);
        }

        auto dsts = schel->node->DownstreamNodes();
        if (WkInVector(dsts, weakThis))
        {
            schel->futureNodes.emplace_back(postScheduleNode);
        }
    }

    schelNodes.emplace(schelNodes.begin(), preScheduleNode);
    schelNodes.emplace_back(postScheduleNode);

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
    for (const auto& attribute : InternalAttributes())
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



Gex::NodePtr Duplicate(std::map<Gex::NodePtr, Gex::NodePtr>& res,
                       const Gex::NodePtr& source, const std::string& name,
                       Gex::CompoundNodePtr parent)
{
    std::string type = source->Type();
    std::string sourceName = source->Name();

    std::string newName = name;
    if (newName.empty())
        newName = sourceName + "1";

    Gex::NodePtr new_ = Gex::NodeFactory::GetFactory()->CreateNode(type, newName);

    parent->AddNode(new_);

    for (const auto& wkAttribute : source->GetAllAttributes())
    {
        if (!wkAttribute)
        {
            continue;
        }

        auto attribute = wkAttribute.ToShared();
        std::string atpath = attribute->Longname();

        Gex::AttributeWkPtr destAttribute;
        if (attribute->IsUserDefined())
        {
            Gex::AttributeWkPtr newWkParent = new_->GetAttribute(
                    attribute->ParentAttribute()->Longname());
            if (!newWkParent)
                continue;

            auto newParent_ = newWkParent.ToShared();

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
            destAttribute = new_->GetAttribute(atpath).ToShared();
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
        auto compound = Gex::CompoundNode::FromNode(source);
        auto newCompound = Gex::CompoundNode::FromNode(new_);
        for (const auto& cmpn : compound->InternalNodes())
        {
            Duplicate(res, cmpn, "", newCompound);
        }
    }

    res[source] = new_;

    return new_;
}


Gex::NodeList Gex::CompoundNode::DuplicateNodes(NodeList sources, bool copyLinks)
{
    std::map<Gex::NodePtr, Gex::NodePtr> nmap;

    Gex::NodeList result;

    auto thisSharedCompound = CompoundNode::FromNode(shared_from_this());

    for (const auto& source : sources)
    {
        Gex::NodePtr dup = Duplicate(
                nmap, source, "",
                thisSharedCompound);

        result.push_back(dup);
    }

    Gex::NodeList news_;
    if (copyLinks)
    {
        for (const auto& m : nmap)
        {
            // Get the new node.
            auto source = m.first;
            auto new_ = m.second;
            for (const auto& wkAttribute : source->GetAllAttributes())
            {
                if (!wkAttribute)
                    continue;

                auto attribute = ToSharedPtr(wkAttribute);

                // Get New node attribute.
                auto wkDestAttribute = new_->GetAttribute(attribute->Longname());
                if (!wkDestAttribute)
                    continue;

                auto destAttribute = ToSharedPtr(wkDestAttribute);

                AttributeWkPtr atsource = attribute->Source();
                if (atsource)
                {
                    // Find original attribute source node.
                    NodeWkPtr nsource = atsource->Node();
                    if (!nsource)
                        continue;

                    NodePtr lockedNSource = nsource.ToShared();

                    // If source node has been duplicated, find the duplicated
                    // node attribute and connect it to preserve internal graph.
                    if (nmap.count(lockedNSource))
                    {
                        NodePtr  duplicatedSource = nmap.at(lockedNSource);
                        AttributeWkPtr duplicatedAttr = duplicatedSource->GetAttribute(
                                atsource->Longname());

                        if (duplicatedAttr)
                            destAttribute->ConnectSource(ToSharedPtr(duplicatedAttr));
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


Gex::NodePtr Gex::CompoundNode::ToCompound(NodeList sources, bool duplicate,
                                         bool keepExternalConnections)
{
    auto compound = std::make_shared<CompoundNode>();

    compound->SetName("Compound");

    NodeList extract = sources;
    if (duplicate)
    {
        extract = DuplicateNodes(sources, true);
    }

    for (const auto& node : sources) {
        compound->AddNode(node);

        auto iter = std::find(
                nodes.begin(),
                nodes.end(),
                node
        );
        if (iter != nodes.end())
            nodes.erase(iter);

        for (const auto& wkAttribute: node->GetAllAttributes())
        {
            auto attribute = ToSharedPtr(wkAttribute);

            auto src = attribute->Source();
            if (!src.expired())
            {
                auto lockedSource = ToSharedPtr(src);
                if (lockedSource->Node().expired())
                    continue;

                auto lockedSourceNode = ToSharedPtr(lockedSource->Node());

                if (std::find(sources.begin(), sources.end(), lockedSourceNode) != sources.end())
                {
                    continue;
                }

                std::string pname = lockedSourceNode->Name() + "_" + lockedSource->Name();

                AttributeWkPtr proxy = compound->GetAttribute(pname);
                AttributeWkPtr sharedProxy;
                if (!proxy)
                {
                    sharedProxy = ToSharedPtr(proxy);

                    AttrType type = AttrType::Static;
                    if (lockedSource->GetAttrType() == AttrType::Output)
                    {
                        type = AttrType::Input;
                    }

                    if (lockedSource->IsHolder())
                    {
                        sharedProxy = compound->CreateAttribute(
                                pname, type,
                                sharedProxy->IsMulti());
                    }
                    else
                    {
                        sharedProxy = compound->CreateAttributeFromValue(
                                pname, lockedSource->GetAnyValue(),
                                lockedSource->GetAttrValueType(),
                                type);
                    }

                    sharedProxy->SetInternal(true);
                }

                attribute->DisconnectSource(lockedSource);
                attribute->ConnectSource(proxy);

                if (keepExternalConnections)
                    sharedProxy->ConnectSource(lockedSource);

            }

            for (const AttributeWkPtr& dst: attribute->Dests())
            {
                if (dst.expired())
                    continue;

                auto lockedDest = ToSharedPtr(dst);
                if (lockedDest->Node().expired())
                    continue;

                auto lockedDestNode = lockedDest->Node().ToShared();
                if (std::find(sources.begin(), sources.end(), lockedDestNode) != sources.end())
                {
                    continue;
                }

                std::string pname = lockedDestNode->Name() + "_" + lockedDest->Name();
                AttributeWkPtr proxy = compound->GetAttribute(pname);

                AttributeWkPtr sharedProxy;
                if (!proxy)
                {
                    sharedProxy = ToSharedPtr(proxy);

                    AttrType type = AttrType::Static;
                    if (lockedDest->GetAttrType() == AttrType::Input)
                    {
                        type = AttrType::Output;
                    }

                    if (lockedDest->IsHolder())
                    {
                        bool multi = sharedProxy->IsMulti();

                        sharedProxy = compound->CreateAttribute(pname, lockedDest->GetAttrType(), multi);
                    }
                    else
                    {
                        sharedProxy = compound->CreateAttributeFromValue(pname, lockedDest->GetAnyValue(),
                                                                   lockedDest->GetAttrValueType(),
                                                                   lockedDest->GetAttrType());
                    }
                }

                lockedDest->DisconnectSource(attribute);
                sharedProxy->ConnectSource(attribute);
                if (keepExternalConnections)
                    lockedDest->ConnectSource(proxy);
            }
        }
    }

    AddNode(compound);
    return compound;
}




Gex::CompoundNodePtr Gex::CompoundNode::FromNode(const NodePtr& node)
{
    if (!node->IsCompound())
        return nullptr;

    // As node->IsCompound() already declares whether
    // this cast is possible, static_cast should be
    // faster here. Still, no further check is applied
    // meaning that declaring a standard node as a
    // compound will cause a crash.
    return std::static_pointer_cast<CompoundNode>(node);
}


Gex::CompoundNodePtr Gex::CompoundNode::FromNode(const NodeWkPtr& node)
{
    if (!node->IsCompound() || !node)
        return nullptr;

    // As node->IsCompound() already declares whether
    // this cast is possible, static_cast should be
    // faster here. Still, no further check is applied
    // meaning that declaring a standard node as a
    // compound will cause a crash.
    return std::static_pointer_cast<CompoundNode>(node.ToShared());
}



void Gex::TraverseNodes(const Gex::NodePtr& root, Gex::TraversalCallback callback)
{
    if (!callback(root))
    {
        return;
    }

    if (root->IsCompound())
    {
        auto cmp = CompoundNode::FromNode(root);
        for (const auto& internal : cmp->InternalNodes())
        {
            TraverseNodes(internal, callback);
        }
    }
}


Gex::NodePtr Gex::TraverseParents(const Gex::NodePtr& root, TraversalCallback callback)
{
    if (callback(root))
    {
        return root;
    }

    auto parent = root->Parent();
    if (parent.expired())
    {
        return nullptr;
    }

    return TraverseParents(parent.ToShared(), callback);
}




