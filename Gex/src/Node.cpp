#include "Gex/include/Node.h"
#include "Gex/include/Attribute.h"
#include "Gex/include/NodeFactory.h"
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include "Gex/include/Evaluation.h"
#include "Gex/include/Config.h"

#include "Tsys/include/tsys.h"
#include "Tsys/include/defaultTypes.h"

#include <filesystem>
#include <iostream>
#include <any>


Gex::Node::Node(Node* parent_)
{
    previous = nullptr;
    next = nullptr;
    parent = parent_;
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


size_t Gex::Node::BuildHash(bool followCnx) const
{
    size_t hash = 0;
    for (auto at : attributes)
    {
        boost::hash_combine(hash, at.second->ValueHash(followCnx));
    }

    return hash;
}


void Gex::Node::SetComputeHash(size_t hash)
{
    computeHash = hash;
}


size_t Gex::Node::ComputeHash() const
{
    return computeHash;
}


bool Gex::Node::IsDirty() const
{
    return (computeHash != BuildHash(/*followCnx=*/true));
}


std::any Gex::Node::InitValueFromHash(size_t hash, Feedback* success)
{
    auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(hash);
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


void Gex::Node::SetName(std::string p)
{
    nodeName = p;
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
	return true;
}


bool Gex::Node::HasAttribute(std::string name)
{
	return (attributes.find(name) != attributes.end());
}


Gex::Attribute* Gex::Node::GetAttribute(std::string name)
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


boost::python::object Gex::Node::PythonGetAttribute(std::string name)
{
	boost::python::object result;
	Attribute* attr = GetAttribute(name);
	if (attr == nullptr)
	{
		return result;
	}

	return boost::python::object(attr);
}


std::vector<Gex::Attribute*> Gex::Node::ExtraAttributes()
{
	std::vector<Attribute*> extraAttrs;
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


std::vector<Gex::Attribute*> Gex::Node::GetAttributes() const
{
	std::vector<Attribute*> attrs;
	for (std::pair<std::string, Attribute*> n : attributes)
	{
		attrs.push_back(n.second);
	}

	return attrs;
}


std::vector<Gex::Attribute*> Gex::Node::GetAllAttributes() const
{
    std::vector<Attribute*> attrs;
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


std::vector<Gex::Node*> Gex::Node::UpstreamNodes()
{
	std::vector<Node*> sources;
	for (Attribute* a : GetAttributes())
	{
		std::vector<Gex::Attribute*> srcs = a->Sources();
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
std::vector<Gex::Node*> IterUpstreamNodes(Gex::Node* node)
{
    std::vector<Gex::Node*> list;

    std::vector<Gex::Node*> analyse = node->UpstreamNodes();

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


std::vector<Gex::Node*> Gex::Node::DownstreamNodes()
{
	std::vector<Node*> destinations;
	for (Attribute* a : GetAttributes())
	{
		if (!a->HasDests())
		{
			continue;
		}

		std::vector<Attribute*> attrDests = a->Dests();
		for (Attribute* attrDest : attrDests)
		{
			destinations.push_back(attrDest->Node());
		}
	}

	return destinations;
}


void Gex::Node::Serialize(rapidjson::Value& dict, rapidjson::Document& json) const
{
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

    rapidjson::Value& attrKey = rapidjson::Value().SetString(Config::GetConfig().nodeAttributesKey.c_str(),
                                                             json.GetAllocator());
    dict.AddMember(attrKey, attrValues, json.GetAllocator());

    if (customAttrs.MemberCount())
    {
        rapidjson::Value& cstmAttrKey = rapidjson::Value().SetString(
                Config::GetConfig().nodeCustomAttributesKey.c_str(),
                json.GetAllocator());
        dict.AddMember(cstmAttrKey.Move(), customAttrs, json.GetAllocator());
    }

}


void Gex::Node::Deserialize(rapidjson::Value& dict)
{
    if (dict.HasMember(Config::GetConfig().nodeCustomAttributesKey.c_str()))
    {
        rapidjson::Value& custAttrs = dict[Config::GetConfig().nodeCustomAttributesKey.c_str()];
        for (auto itr = custAttrs.MemberBegin(); itr != custAttrs.MemberEnd(); ++itr)
        {
            Attribute* cust = Attribute::DeserializeAttribute(itr->name.GetString(), itr->value.GetObject(),
                                                              this, nullptr, true);
            AddAttribute(cust);
        }
    }

    if (dict.HasMember(Config::GetConfig().nodeAttributesKey.c_str()))
    {
        rapidjson::Value& attrValues = dict[Config::GetConfig().nodeAttributesKey.c_str()];
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


bool Gex::Node::Evaluate(NodeAttributeData &context,
                         GraphContext &graphContext,
                         NodeProfiler& profiler)
{
    return true;
}





bool Gex::CompoundNode::IsCompound() const
{
    return true;
}


Gex::Attribute* Gex::CompoundNode::CreateInternalAttribute(std::string name, std::any v,
                                                                           AttrValueType valueType, AttrType type,
                                                                           Attribute* parent)
{
    Attribute* attribute = CreateAttributeFromValue(name, v, valueType, type, parent);
    if (!attribute)
    {
        return attribute;
    }

    attribute->SetInternal(true);
    return attribute;
}


Gex::Attribute* Gex::CompoundNode::CreateInternalAttribute(const std::string& name,
                                                                           AttrType type, bool multi,
                                                                           Attribute* parent)
{
    Attribute* attribute = CreateAttribute(name, type, multi, parent);
    if (!attribute)
    {
        return attribute;
    }

    attribute->SetInternal(true);
    return attribute;
}

bool Gex::CompoundNode::AddInternalNode(Node *node)
{
    if (IsInternalNode(node))
    {
        return false;
    }

    innerNodes.push_back(node);
    node->SetParent(this);
    return true;
}


bool Gex::CompoundNode::RemoveInternalNode(Node *node)
{
    if (!IsInternalNode(node))
    {
        return false;
    }

    innerNodes.erase(std::find(innerNodes.begin(), innerNodes.end(), node));

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


Gex::Node* Gex::CompoundNode::GetInternalNode(const std::string& node) const
{

    size_t pos = node.find(Config::GetConfig().nodePathSeparator);
    std::string n = node;
    std::string sn;
    if (pos != std::string::npos)
    {
        n = node.substr(0, pos - 1);
        sn = node.substr(pos + 1, std::string::npos);
    }

    MatchingNode search;
    search.match = n;

    auto result = std::find_if(innerNodes.begin(), innerNodes.end(), search);

    if (result == innerNodes.end())
        return nullptr;

    if (sn.empty())
        return *result;

    if (!(*result)->IsCompound())
        return nullptr;

    return ConvertTo<CompoundNode>(*result)->GetInternalNode(sn);
}


bool Gex::CompoundNode::RemoveInternalNode(const std::string& node)
{
    Node* nodePtr = GetInternalNode(node);
    if (!nodePtr)
    {
        return false;
    }

    innerNodes.erase(std::find(innerNodes.begin(), innerNodes.end(), nodePtr));

    delete nodePtr;
    return true;
}


bool Gex::CompoundNode::IsInternalNode(Node* node) const
{
    return (std::find(innerNodes.begin(), innerNodes.end(), node) != innerNodes.end());
}


bool Gex::CompoundNode::IsInternalNode(const std::string& node) const
{
    Node* nodePtr = GetInternalNode(node);
    if (!nodePtr)
        return false;

    return IsInternalNode(nodePtr);
}


std::vector<Gex::Node*> Gex::CompoundNode::InternalNodes() const
{
    return innerNodes;
}


std::vector<std::string> Gex::CompoundNode::InternalNodeNames() const
{
    std::vector<std::string> names;
    for (auto* node : innerNodes)
    {
        names.push_back(node->Name());
    }

    return names;
}


Gex::Node* Gex::CompoundNode::CreateInternalNode(std::string type, std::string name)
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

    if (!AddInternalNode(newNode))
    {
        delete newNode;
        return nullptr;
    }

    return newNode;
}


std::vector<Gex::Attribute*> Gex::CompoundNode::InternalAttributes() const
{
    std::vector<Attribute*> internals;
    for (auto attribute : attributes)
    {
        if (attribute.second->IsInternal())
            internals.push_back(attribute.second);
    }

    return internals;
}


std::vector<Gex::Attribute*> Gex::CompoundNode::AllInternalAttributes() const
{
    std::vector<Attribute*> internals;
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

    rapidjson::Value& internalNodesDict = rapidjson::Value().SetObject();
    for (Node* node : innerNodes)
    {
        rapidjson::Value& internalNodeDict = rapidjson::Value().SetObject();

        NodeFactory::GetFactory()->SaveNode(node, internalNodeDict, json);

        rapidjson::Value& key = rapidjson::Value().SetString(rapidjson::StringRef(node->Name().c_str()),
                                                             json.GetAllocator());

        internalNodesDict.AddMember(key.Move(), internalNodeDict, json.GetAllocator());
    }

    rapidjson::Value& connections = rapidjson::Value(rapidjson::kArrayType).GetArray();
    for (auto node : innerNodes)
    {
        std::vector<Attribute*> attributes = node->GetAllAttributes();
        for (auto* attr : attributes)
        {
            _JsonPushAttributeConnection(this, connections, attr, json);
        }
    }

    for (auto* attr : AllInternalAttributes())
    {
        auto val = [this](Node* src){return this->IsInternalNode(src);};
        _JsonPushAttributeConnection(this, connections, attr, json, val);
    }

    dict.AddMember(rapidjson::StringRef(COMPOUND_NODES_K), internalNodesDict,
                   json.GetAllocator());

    dict.AddMember(rapidjson::StringRef(COMPOUND_CONNECTIONS_K), connections,
                   json.GetAllocator());
}


void Gex::CompoundNode::Deserialize(rapidjson::Value& dict)
{
    Gex::Node::Deserialize(dict);

    std::map<std::string, Node*> createdNodes;
    if (dict.HasMember(COMPOUND_NODES_K))
    {
        rapidjson::Value& nodes = dict[COMPOUND_NODES_K];
        for (auto iter = nodes.MemberBegin(); iter != nodes.MemberEnd(); iter++)
        {
            Node* node = NodeFactory::GetFactory()->LoadNode(iter->name.GetString(), iter->value);
            if (!node)
            {
                continue;
            }

            AddInternalNode(node);

            createdNodes[iter->name.GetString()] = node;
        }
    }

    if (dict.HasMember(COMPOUND_CONNECTIONS_K))
    {
        rapidjson::Value& connections = dict[COMPOUND_CONNECTIONS_K];
        for (auto iter = connections.Begin(); iter != connections.End(); iter++)
        {
            rapidjson::Value& list = iter->GetArray();
            std::string srcname = list[0].GetString();
            std::string srcnattr = list[1].GetString();
            std::string dstname = list[2].GetString();
            std::string dstattr = list[3].GetString();

            Node* src = nullptr;
            if (srcname == THIS)
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
            if (dstname == THIS)
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
}


bool Gex::CompoundNode::Evaluate(NodeAttributeData &ctx,
                                 GraphContext &graphCtx,
                                 NodeProfiler& profiler)
{
    NodeEvaluator evaluator(innerNodes, graphCtx,
                            profiler.GetProfiler(),
                            false, 1);

	bool success = (evaluator.Status() == NodeEvaluator::EvaluationStatus::Done);

    PullInternalOutputs();

    return success;
}


bool Gex::CompoundNode::PreEvaluate(NodeAttributeData &ctx,
                                    GraphContext &graphContext,
                                    NodeProfiler& profiler)
{
    return true;
}


bool Gex::CompoundNode::PostEvaluate(NodeAttributeData &ctx,
                                     GraphContext &graphContext,
                                     NodeProfiler& profiler)
{
    return true;
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


Gex::CompoundNode* Gex::CompoundNode::FromNode(Node* node)
{
    return dynamic_cast<CompoundNode*>(node);
}


bool Gex::CompoundNode::IsCompound(Node* node)
{
    return (FromNode(node) != nullptr);
}