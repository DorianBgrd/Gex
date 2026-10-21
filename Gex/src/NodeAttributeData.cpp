#include "Gex/include/Gex/Node.h"
#include "Gex/include/Gex/NodeAttributeData.h"


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


TSys::TypePtr Gex::NodeAttributeData::GetAttributeType() const
{
    return attribute->Type();
}


TSys::TypedValuePtr Gex::NodeAttributeData::GetAttributeValue() const
{
    return attribute->GetResolvedValue();
}


std::vector<TSys::TypedValuePtr> Gex::NodeAttributeData::GetAttributeArrayValues() const
{
    return attribute->GetResolvedArrayValues();
}


void Gex::NodeAttributeData::SetAttributeValue(const TSys::TypedValuePtr& value) const
{
    attribute->SetValue(value);
}


void Gex::NodeAttributeData::SetAttributeIndexValue(
        unsigned int index, const TSys::TypedValuePtr& value
) const
{
    attribute->SetIndexValue(index, value);
}


bool Gex::NodeAttributeData::CreateIndexValue(
        unsigned int index, const TSys::TypedValuePtr& value
) const
{
    auto indexAttribute = attribute->GetOrCreateIndex(index);

    indexAttribute->SetValue(value);

    return true;
}


bool Gex::NodeAttributeData::IsInput() const
{
    return attribute->IsInput();
}


bool Gex::NodeAttributeData::IsOutput() const
{
    return attribute->IsOutput();
}


bool Gex::NodeAttributeData::IsExclusiveOutput() const
{
    return attribute->GetAttrType() == AttrType::Output;
}


std::vector<unsigned int> Gex::NodeAttributeData::GetIndices(Feedback* status)
{
    if (!attribute)
    {
        SET_STATUS_MSG(status, Status::Failed, "No attribute to query.")

        return {};
    }

    if (!attribute->IsArray())
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
