#include "Gex/include/UndoCommands.h"


Gex::Undo::CreateNode::CreateNode(
        const NodePtr& p,
        const NodePtr& n)
{
    parent = p;
    node = n;
}


std::string Gex::Undo::CreateNode::Name() const
{
    return ("Creating node with type " +
            node->Type() + " and name " +
            node->Name());
}


void Gex::Undo::CreateNode::Undo() const
{
    if (parent)
    {
        CompoundNode::FromNode(parent)->RemoveNode(node);
    }
}


void Gex::Undo::CreateNode::Redo() const
{
    if (parent)
    {
        CompoundNode::FromNode(parent)->AddNode(node);
    }
}


Gex::Undo::DeleteNode::DeleteNode(
        const NodePtr& p,
        const NodePtr& n)
{
    node = n;
    parent = p;

    for (auto attr : node->GetAllAttributes())
    {
        if (attr->HasSource())
        {
            connections.emplace_back(attr->Source().ToShared(), attr);
        }
        for (const auto& d : attr->Dests())
        {
            if (!d)
                continue;

            connections.emplace_back(attr, d.ToShared());
        }
    }
}


std::string Gex::Undo::DeleteNode::Name() const
{
    return "Deleting node " + node->Name();
}


void Gex::Undo::DeleteNode::Undo() const
{
    if (parent)
    {
        CompoundNode::FromNode(parent)->AddNode(node);

        for (auto cnx : connections)
        {
            cnx.second->ConnectSource(cnx.first);
        }
    }
}


void Gex::Undo::DeleteNode::Redo() const
{
    if (parent)
    {
        CompoundNode::FromNode(parent)->RemoveNode(node);
    }
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


void Gex::Undo::ConnectAttr::Undo() const
{
    destAttr->DisconnectSource(sourceAttr);
}


void Gex::Undo::ConnectAttr::Redo() const
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


void Gex::Undo::DisconnectAttr::Undo() const
{
    destAttr->ConnectSource(sourceAttr);
}

void Gex::Undo::DisconnectAttr::Redo() const
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


void Gex::Undo::SetAttr::Undo() const
{
    attr->SetAnyValue(previous);
}

void Gex::Undo::SetAttr::Redo() const
{
    attr->SetAnyValue(value);
}

