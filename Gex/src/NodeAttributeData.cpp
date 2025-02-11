#include "Gex/include/Node.h"
#include "Gex/include/NodeAttributeData.h"


#define SET_STATUS(status_obj, stat) \
if (status_obj)                      \
{                                    \
    status_obj->status = stat;       \
}


#define SET_STATUS_MSG(status_obj, stat, message) \
if (status_obj)                                   \
{                                                 \
    status_obj->Set(stat, message);               \
}



Gex::NodeAttributeData::NodeAttributeData(const NodePtr& n)
{
    node = n;
}


Gex::NodeAttributeData::NodeAttributeData(const AttributePtr& attr)
{
    node = attr->Node().lock();
    attribute = attr;
}


Gex::NodeAttributeData::NodeAttributeData(const AttributeWkPtr& attr):
        NodeAttributeData(attr.ToShared())
{

}


Gex::NodePtr Gex::NodeAttributeData::GetNode() const
{
    return node;
}


std::any Gex::NodeAttributeData::GetAnyValue(Feedback* status)
{
    if (!attribute)
    {
        SET_STATUS(status, Status::Failed)

        return std::make_any<bool>();
    }

    SET_STATUS(status, Status::Failed)

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
        SET_STATUS_MSG(status, Status::Failed, "No attribute to query.")

        return {};
    }

    if (!attribute->IsMulti())
    {
        SET_STATUS_MSG(status, Status::Failed, ("Attribute " + attribute->Name() + " is not a multi attribute."))

        return {};
    }

    return attribute->ValidIndices();
}


Gex::NodeAttributeData Gex::NodeAttributeData::GetAttribute(
        const std::string& name,
        Feedback* status
) const
{
    if(!attribute)
    {
        auto attr = node->GetAttribute(name);
        if (!attr)
        {
            SET_STATUS(status, Status::Failed)

            return NodeAttributeData(node);
        }

        SET_STATUS(status, Status::Success)

        return NodeAttributeData(attr.ToShared());
    }

    auto attr = attribute->GetAttribute(name);
    if (!attr)
    {
        SET_STATUS(status, Status::Failed)

        return NodeAttributeData(attribute);
    }

    SET_STATUS(status, Status::Success)

    return NodeAttributeData(attr);
}


Gex::NodeAttributeData Gex::NodeAttributeData::GetIndex(
        unsigned int index,
        Feedback* status
) const
{
    if (!attribute)
    {
        SET_STATUS(status, Status::Failed)

        return NodeAttributeData(node);
    }

    auto indexAttr = attribute->GetIndexAttribute(index);
    if (indexAttr)
    {
        SET_STATUS(status, Status::Success)

        return NodeAttributeData(indexAttr);
    }

    SET_STATUS(status, Status::Failed)

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
