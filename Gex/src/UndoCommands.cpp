#include "Gex/include/UndoCommands.h"


Gex::Undo::AddNode::AddNode(
        const NodePtr& parent_,
        const NodePtr& node_,
        const NodeCB& createCB,
        const NodeCB& deleteCB)
{
    parent = parent_;
    node = node_;
    createCallback = createCB;
    deleteCallback = deleteCB;
}


std::string Gex::Undo::AddNode::Name() const
{
    return ("Creating node with type " +
            node->Type() + " and name " +
            node->Name());
}


void Gex::Undo::AddNode::Undo()
{
    deleteCallback(node);
}


void Gex::Undo::AddNode::Redo()
{
    createCallback(node);
}


Gex::Undo::RemoveNode::RemoveNode(
        const NodePtr& n,
        const NodeCB& rmCallback,
        const NodeCB& addCallback)
{
    node = n;
    removeNodeCb = rmCallback;
    addNodeCb = addCallback;

    for (auto& attribute : node->GetAllAttributes())
    {
        if (attribute->HasSource())
        {
            sourceConnections.emplace_back(
                    attribute->Source().ToShared(),
                    attribute
            );
        }

        for (auto& destination : attribute->Dests())
        {
            if (!destination)
                continue;

            destConnections.emplace_back(
                    attribute,
                    destination.ToShared()
            );
        }
    }
}


Gex::Undo::RemoveNode::RemoveNode(const RemoveNode& other):
    node(other.node),
    removeNodeCb(other.removeNodeCb),
    addNodeCb(other.addNodeCb),
    sourceConnections(other.sourceConnections),
    destConnections(other.destConnections)
{

}


std::string Gex::Undo::RemoveNode::Name() const
{
    return "Deleting node " + node->Name();
}


void Gex::Undo::RemoveNode::Undo()
{
    addNodeCb(node);

    for (const auto& sourceConnection : sourceConnections)
    {
        sourceConnection.second->ConnectSource(
                sourceConnection.first
        );
    }

    for (const auto& destConnection: destConnections)
    {
        destConnection.second->ConnectSource(
                destConnection.first
        );
    }
}


void Gex::Undo::RemoveNode::Redo()
{
    for (const auto& sourceConnection : sourceConnections)
    {
        sourceConnection.second->DisconnectSource(
                sourceConnection.first
        );
    }

    for (const auto& destConnection: destConnections)
    {
        destConnection.second->DisconnectSource(
                destConnection.first
        );
    }

    removeNodeCb(node);
}


Gex::Undo::ConnectAttr::ConnectAttr(
        const AttributePtr& src,
        const AttributePtr& dst,
        const AttributePtr& pdst)
{
    source = src->Node().ToShared();
    sourceAttr = src;
    dest = dst->Node().ToShared();
    destAttr = dst;
    previous = pdst->Node().ToShared();
    previousAttr = pdst;
}


std::string Gex::Undo::ConnectAttr::Name() const
{
    return ("Connecting " + source->Name() + "." + sourceAttr->Name() +
            " to " + dest->Name() + "." + destAttr->Name());
}


void Gex::Undo::ConnectAttr::Undo()
{
    destAttr->DisconnectSource(sourceAttr);
}


void Gex::Undo::ConnectAttr::Redo()
{
    destAttr->ConnectSource(sourceAttr);
}


Gex::Undo::DisconnectAttr::DisconnectAttr(
        const AttributePtr& src,
        const AttributePtr& dst)
{
    source = src->Node().ToShared();
    sourceAttr = src;
    dest = dst->Node().ToShared();
    destAttr = dst;
}


std::string Gex::Undo::DisconnectAttr::Name() const
{
    return ("Connecting " + source->Name() + "." + sourceAttr->Name() +
            " from " + dest->Name() + "." + destAttr->Name());
}


void Gex::Undo::DisconnectAttr::Undo()
{
    destAttr->ConnectSource(sourceAttr);
}

void Gex::Undo::DisconnectAttr::Redo()
{
    destAttr->DisconnectSource(sourceAttr);
}


Gex::Undo::SetAttr::SetAttr(
        const AttributePtr& at,
        const std::any& v,
        const std::any& p)
{
    node = at->Node().ToShared();
    attr = at;
    value = v;
    previous = p;
}


std::string Gex::Undo::SetAttr::Name() const
{
    return ("Setting attribute " + node->Name() +
            "." + attr->Name() + " value.");
}


void Gex::Undo::SetAttr::Undo()
{
    attr->SetAnyValue(previous);
}

void Gex::Undo::SetAttr::Redo()
{
    attr->SetAnyValue(value);
}

