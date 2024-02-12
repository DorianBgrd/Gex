//
// Created by Dorian on 8/12/2022.
//
#include "include/Node.h"
#include "include/NodeAttributeData.h"


Gex::NodeAttributeData::NodeAttributeData(Node* n)
{
    node = n;
    attribute = nullptr;
}


Gex::NodeAttributeData::NodeAttributeData(Attribute* attr)
{
    node = attr->Node();
    attribute = attr;
}


Gex::Node* Gex::NodeAttributeData::GetNode() const
{
    return node;
}


std::any Gex::NodeAttributeData::GetAnyValue(Feedback* success)
{
    if (!attribute)
    {
        if (success)
            success->status = Status::Failed;
        return std::make_any<bool>();
    }

    if (success)
        success->status = Status::Success;
    return attribute->GetAnyValue();
}


bool Gex::NodeAttributeData::SetAnyValue(std::any value)
{
    if(!attribute)
    {
        return false;
    }

    return attribute->SetAnyValue(value);
}


std::vector<unsigned int> Gex::NodeAttributeData::GetIndices(Feedback* status)
{
    if (!attribute)
    {
        if (status)
        {
            status->status = Status::Failed;
            status->message = "No attribute to query.";
        }

        return {};
    }

    if (!attribute->IsMulti())
    {
        if (status)
        {
            status->status = Status::Failed;
            status->message = ("Attribute " + attribute->Name() + " is not a multi attribute.");
        }

        return {};
    }

    return attribute->ValidIndices();
}


Gex::NodeAttributeData Gex::NodeAttributeData::GetAttribute(std::string name, Feedback* success) const
{
    if(!attribute)
    {
        if (!node->HasAttribute(name))
        {
            if (success)
                success->status = Status::Failed;
            return NodeAttributeData(node);
        }

        return NodeAttributeData(node->GetAttribute(name));
    }

    if (!attribute->HasChildAttribute(name))
    {
        if (success)
            success->status = Status::Failed;
        return NodeAttributeData(attribute);
    }

    if (success)
        success->status = Status::Success;
    return NodeAttributeData(attribute->GetAttribute(name));
}


Gex::NodeAttributeData Gex::NodeAttributeData::GetIndex(unsigned int index, Feedback* success) const
{
    if (!attribute)
    {
        if (success)
            success->status = Status::Failed;
        return NodeAttributeData(node);
    }

    if (!attribute->IsMulti() || !attribute->HasIndex(index))
    {
        if(success)
            success->status = Status::Failed;
        return NodeAttributeData(attribute);
    }

    if (success)
        success->status = Status::Success;
    return NodeAttributeData(attribute->GetIndexAttribute(index));
}


bool Gex::NodeAttributeData::ClearMultiIndices() const
{
    if (!attribute)
        return false;

    return attribute->ClearIndices();
}


bool Gex::NodeAttributeData::HasAttribute(std::string name) const
{
    if (!attribute)
    {
        return node->HasAttribute(name);
    }

    return attribute->HasChildAttribute(name);
}


bool Gex::NodeAttributeData::HasIndex(unsigned int index) const
{
    if (!attribute)
    {
        return false;
    }

    return attribute->HasIndex(index);
}


std::vector<unsigned int> Gex::NodeAttributeData::Indices() const
{
    if (!attribute)
    {
        return {};
    }

    return attribute->ValidIndices();
}
