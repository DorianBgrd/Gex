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
        Gex::CompoundNode::FromNode(parent)->AddNode(new_);
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
                                         Gex::Node* parent, Graph* graph)
{
    std::map<Gex::Node*, Gex::Node*> nmap;

    if (!graph)
        graph = this;

    Gex::NodeList result;

    for (auto* source : sources)
    {
        Gex::Node* dup = Duplicate(nmap, graph, source, "", parent);
        result.push_back(dup);
    }

    bool copyExternal = (graph == this);

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
                    else if (copyExternal)
                    {
                        destAttribute->ConnectSource(atsource);
                    }
                }
            }
        }
    }

    return result;
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


Gex::Attribute* Gex::Graph::FindAttribute(std::string attr) const
{
    size_t pos  = attr.find(Config::GetConfig().attributeSeparator.c_str());
    if (pos == std::string::npos)
    {
        return nullptr;
    }

    std::string nodePath = attr.substr(0, pos - 1);
    std::string attrPath = attr.substr(pos + 1, std::string::npos);

    auto* node = GetNode(nodePath);
    if (!node)
    {
        return nullptr;
    }

    return node->GetAttribute(attrPath);
}


bool Gex::Graph::Run(Profiler& profiler,
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
        this->FinalizeEvaluation(context); evalDone(context); };

    lmbda.Stop();

    if (!IsValid())
    {
        auto schelScope = ProfilerScope(profiler, "Global", "Scheduling");

        Schedule();
    }

    auto evaluator = std::make_shared<NodeEvaluator>(
            ScheduleNodes({this}, true), context, profiler,
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


void Gex::Graph::AttributeChanged(Attribute* attr, AttributeChange change)
{
    CompoundNode::AttributeChanged(attr, change);

    if (change == AttributeChange::Connected
        || change == AttributeChange::Disconnected
        || change == AttributeChange::ChildAttributeConnected
        || change == AttributeChange::ChildAttributeDisconnected)
    {
        Invalidate();
    }
}
