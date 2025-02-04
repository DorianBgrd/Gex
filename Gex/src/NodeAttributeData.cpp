#include "Gex/include/Node.h"
#include "Gex/include/NodeAttributeData.h"


Gex::NodeAttributeData::NodeAttributeData(const NodePtr& n)
{
    node = n;
}


Gex::NodeAttributeData::NodeAttributeData(const AttributePtr& attr)
{
    node = attr->Node().lock();
    attribute = attr;
}


Gex::NodePtr Gex::NodeAttributeData::GetNode() const
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


bool Gex::NodeAttributeData::SetAnyValue(const std::any& value)
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


Gex::NodeAttributeData Gex::NodeAttributeData::GetAttribute(
        const std::string& name,
        Feedback* success
) const
{
    if(!attribute)
    {
        auto attr = node->GetAttribute(name);
        if (!attr)
        {
            if (success)
                success->status = Status::Failed;

            return NodeAttributeData(node);
        }

        return NodeAttributeData(attr.ToShared());
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


Gex::NodeAttributeData Gex::NodeAttributeData::GetIndex(
        unsigned int index,
        Feedback* success
) const
{
    if (!attribute)
    {
        if (success)
            success->status = Status::Failed;
        return NodeAttributeData(node);
    }

    if (!attribute->IsMulti())
    {
        if(success)
            success->status = Status::Failed;
        return NodeAttributeData(attribute);
    }

    if (success)
        success->status = Status::Success;

    auto indexAttr = attribute->GetIndexAttribute(index);
    if (indexAttr)
        return NodeAttributeData(indexAttr);

    return NodeAttributeData(attribute);
}


bool Gex::NodeAttributeData::ClearMultiIndices() const
{
    if (!attribute)
        return false;

    return attribute->ClearIndices();
}


bool Gex::NodeAttributeData::HasAttribute(const std::string& name) const
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
