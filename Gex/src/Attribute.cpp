#include "Gex/include/Config.h"
#include "Gex/include/Attribute.h"
#include "Gex/include/Node.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "Gex/include/Config.h"

#include "Tsys/include/tsys.h"
#include "Tsys/include/defaultTypes.h"

#include <boost/container_hash/hash.hpp>

#include "Gex/include/UndoCommands.h"


#define CHECK_EDITABLE_SKIP() if(!IsEditable()){return;}
#define CHECK_EDITABLE_BOOL() if(!IsEditable()){return false;}


Gex::Attribute::Attribute(): typeIndex(typeid(TSys::None))
{
	attributeAnyValue = std::make_any<TSys::None>(TSys::None());
	attributeIsUserDefined = false;

    typeHandle = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(typeIndex);
}


Gex::AttrValueType Gex::Attribute::GetAttrValueType() const
{
    return attributeValueType;
}


Gex::AttrType Gex::Attribute::GetAttrType() const
{
    return attributeType;
}


Gex::Attribute::Attribute(const std::string& name, const std::any& v, const AttrValueType& valueType,
			              const AttrType& type, bool userDefined, const NodePtr& node,
                          const AttributeWkPtr& parent): typeIndex(v.type())
{
    attributeName = name;
    if (!parent)
        attributeLongname = name;
    else
        attributeLongname = parent->Longname() + Config::GetConfig().attributeSeparator + name;

    attributeAnyValue = v;
    attributeNode = node;
    parentAttribute = parent;

    attributeIsUserDefined = userDefined;
    attributeValueType = valueType;
    attributeType = type;
    defaultValue = v;

    typeHandle = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(typeIndex);
}


Gex::Attribute::Attribute(const std::string& name, const std::type_index& vt, const AttrValueType& valueType,
                          const AttrType& type, bool userDefined, const NodePtr& node,
                          const AttributeWkPtr& parent):
                          Attribute(name, TSys::TypeRegistry::GetRegistry()->GetTypeHandle(vt)->InitValue(),
                                    valueType, type, userDefined, node, parent)
{

}


Gex::Attribute::Attribute(const std::string& name, const std::string& apiType,
                          const AttrValueType& valueType, const AttrType& type,
                          bool userDefined, const NodePtr& node,
                          const AttributeWkPtr& parent):
        Attribute(name, TSys::TypeRegistry::GetRegistry()->GetTypeHandle(apiType)->InitValue(),
                  valueType, type, userDefined, node, parent)
{

}


Gex::Attribute::Attribute(const std::string& name, const AttrType& type, bool multi,
                          bool userDefined, const NodePtr& node,
                          const AttributeWkPtr& parent):
                          typeIndex(typeid(TSys::None))
{
    attributeNode = node;
    attributeName = name;
    if (!parent)
        attributeLongname = name;
    else
        attributeLongname = parent->Longname() + Config::GetConfig().attributeSeparator + name;
    attributeAnyValue = std::make_any<TSys::None>(TSys::None());
    parentAttribute = parent;

    attributeIsUserDefined = userDefined;
    if (multi)
        attributeValueType = AttrValueType::MultiHolder;
    else
        attributeValueType = AttrValueType::Holder;

    attributeType = type;

    typeHandle = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(typeIndex);
    InitDefaultValue();
}


Gex::Attribute::~Attribute()
{
	if (IsMulti())
	{
		multiValues.clear();
	}

    if (IsHolder())
    {
        subAttributes.clear();
    }
}


void Gex::Attribute::SignalChange(AttributeChange change)
{
    if (!attributeNode)
    {
        return;
    }

    attributeNode->SignalAttributeChange(shared_from_this(), change);
}


Gex::AttributePtr Gex::Attribute::Copy(const std::string& name, const NodePtr& node,
                                       const AttributePtr& parent) const
{
    return std::make_shared<Attribute>(
            name, defaultValue, attributeValueType,
            attributeType, attributeIsUserDefined,
            node, parent);
}


Gex::AttributePtr Gex::Attribute::CopyAsChild(const AttributePtr& ref)
{
    AttributePtr copied = ref->Copy(ref->Name(), ref->Node().ToShared(),
                                    shared_from_this());
    AddChildAttribute(copied);
    return copied;
}


void Gex::Attribute::InitDefaultValue()
{
    defaultValue = typeHandle->InitValue();
}


void Gex::Attribute::SetParentNode(const NodePtr& node)
{
	attributeNode = node;
}


Gex::NodeWkPtr Gex::Attribute::Node() const
{
	return attributeNode;
}


Gex::AttributeWkPtr Gex::Attribute::ParentAttribute() const
{
    return parentAttribute;
}


std::string Gex::Attribute::Name() const
{
	return attributeName;
}


std::string Gex::Attribute::Longname() const
{
	return attributeLongname;
}



void Gex::Attribute::SetName(std::string name)
{
	attributeName = name;
}


std::string Gex::Attribute::TypeName() const
{
	return attributeAnyValue.type().name();
}


std::type_index Gex::Attribute::Type() const
{
    return typeIndex;
}


TSys::TypeHandlerPtr Gex::Attribute::TypeHandle() const
{
    return typeHandle;
}


std::any Gex::Attribute::DefaultValue() const
{
    return defaultValue;
}


bool Gex::Attribute::SetDefaultAnyValue(const std::any& v)
{
    bool isDefault = IsDefault();

    if (std::type_index(v.type()) != Type())
    {
        return false;
    }

    defaultValue = v;

    if (isDefault)
    {
        Reset();
    }

    return true;
}


bool Gex::Attribute::SetAttributeType(AttrType type)
{
    CHECK_EDITABLE_BOOL()

    attributeType = type;
    return true;
}


void Gex::Attribute::Reset()
{
    CHECK_EDITABLE_SKIP()

    _Reset();
}


void Gex::Attribute::_Reset()
{
    attributeAnyValue = defaultValue;
}


void Gex::Attribute::FactoryReset()
{
    _Reset();

    if (IsMulti())
    {
        for (unsigned int index : ValidIndices())
        {
            _RemoveIndex(index);
        }
    }
}


bool Gex::Attribute::IsDefault() const
{
    return typeHandle->CompareValue(defaultValue, attributeAnyValue);
}


bool Gex::Attribute::IsUserDefined() const
{
	return attributeIsUserDefined;
}


bool Gex::Attribute::IsInput() const
{
	return (attributeType == AttrType::Input || attributeType == AttrType::Static);
}


bool Gex::Attribute::IsOutput() const
{
	return (attributeType == AttrType::Output || attributeType == AttrType::Static);
}


bool Gex::Attribute::IsStatic() const
{
	return (attributeType == AttrType::Static);
}


bool Gex::Attribute::IsMulti() const
{
	return(attributeValueType == AttrValueType::Multi ||
           attributeValueType == AttrValueType::MultiHolder);
}


bool Gex::Attribute::IsHolder() const
{
    return(attributeValueType == AttrValueType::Holder ||
           attributeValueType == AttrValueType::MultiHolder);
}


bool Gex::Attribute::IsEditable() const
{
    if (!attributeNode)
    {
        return true;
    }

    return attributeNode->IsEditable();
}


size_t Gex::Attribute::ValueHash(bool followCnx)
{
    size_t res = 0;

    if (IsMulti())
    {
        for (unsigned int i : ValidIndices())
        {
            boost::hash_combine(res, GetIndexAttribute(i)->ValueHash());
        }
    }
    else if (IsHolder())
    {
        for (const auto& at : GetChildAttributes())
        {
            boost::hash_combine(res, at->ValueHash());
        }
    }

    if (followCnx && HasSource())
    {
        if (auto at = Source())
        {
            auto handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(at->Type());
            boost::hash_combine(res, handler->ValueHash(at->GetAnyValue()));
        }
    }
    else
    {
        boost::hash_combine(res, typeHandle->ValueHash(attributeAnyValue));
    }

    return res;
}



bool Gex::Attribute::HasChildAttributes() const
{
	return (!subAttributes.empty());
}


std::vector<std::string> Gex::Attribute::ChildAttributesNames() const
{
    std::vector<std::string> names;
    for (auto p : subAttributes)
    {
        names.push_back(p.first);
    }

    return names;
}


bool Gex::Attribute::HasChildAttribute(const std::string& name) const
{
    return (subAttributes.find(name) != subAttributes.end());
}


std::vector<Gex::AttributeWkPtr> Gex::Attribute::GetChildAttributes() const
{
	std::vector<AttributeWkPtr> childs;
	for (std::pair<std::string, AttributePtr> p : subAttributes)
	{
		childs.emplace_back(p.second);
	}

	return childs;
}


Gex::AttributeWkPtr Gex::Attribute::GetAttribute(std::string name) const
{
	size_t delimiter = name.find(Config::GetConfig().attributeSeparator);
	std::string shortname = name.substr(0, delimiter);

	AttributePtr attribute;

    std::smatch results;
    std::regex multiRegex("\\[[0-9]+\\]$");
    std::regex_search(shortname, results, multiRegex);
	if (IsHolder() && results.empty())
	{
		if (subAttributes.find(shortname) == subAttributes.end())
		{
			return {};
		}

		attribute = subAttributes.at(shortname);
	}
	else if (IsMulti())
	{
		if (results.empty())
		{
			return {};
		}

		unsigned int index = std::stoi(results.str().substr(1, results.size() - 2));
		if (!HasIndex(index))
		{
			return {};
		}

		attribute = multiValues.at(index);
	}
	else
	{
		return {};
	}

	if (delimiter == std::string::npos)
	{
		return attribute;
	}

	return attribute->GetAttribute(name.erase(0, delimiter+1));
}


std::vector<Gex::AttributeWkPtr> Gex::Attribute::GetAllAttributes() const
{
    std::vector<Gex::AttributeWkPtr> attributes;

    for (unsigned int index : ValidIndices())
    {
        AttributeWkPtr idxAttr = GetIndexAttribute(index);
        attributes.push_back(idxAttr);

        std::vector<AttributeWkPtr> subs = idxAttr->GetAllAttributes();
        for (const AttributeWkPtr& attr : subs)
        {
            attributes.push_back(attr);
        }
    }

    for (const AttributeWkPtr& attrptr : GetChildAttributes())
    {
        attributes.push_back(attrptr);

        std::vector<AttributeWkPtr> subs = attrptr->GetAllAttributes();
        for (const AttributeWkPtr& attr : subs)
        {
            attributes.push_back(attr);
        }
    }

    return attributes;
}


bool Gex::Attribute::AddChildAttribute(const AttributePtr& child)
{
//    CHECK_EDITABLE_BOOL()

	if (subAttributes.find(child->Name()) != subAttributes.end())
	{
		return false;
	}

	subAttributes[child->Name()] = child;
	return true;
}


Gex::AttributeWkPtr Gex::Attribute::CreateChildAttribute(
        const std::string& name, const std::any& value,
        const AttrValueType& vType, const AttrType& type,
        bool userdefined)
{
    auto child = std::make_shared<Attribute>(
            name, value, vType, type, userdefined,
            attributeNode.lock(), parentAttribute.lock());

    bool success = AddChildAttribute(child);
    if (!success)
    {
        return {};
    }

    return child;
}


bool Gex::Attribute::HasSource() const
{
    return source;
}


bool Gex::Attribute::HasSource(unsigned int index)
{
	AttributeWkPtr indexAttr = GetIndexAttribute(index);
	if (!indexAttr)
	{
		return false;
	}

	return indexAttr->HasSource();
}


bool Gex::Attribute::HasSources() const
{
    if(HasSource())
    {
        return true;
    }

    if(IsMulti())
    {
        for (auto attr : multiValues)
        {
            if (attr.second->HasSource())
            {
                return true;
            }
        }
    }

    if(IsHolder())
    {
        for (auto attr : subAttributes)
        {
            if (attr.second->HasSources())
            {
                return true;
            }
        }
    }

    return false;
}


/**
 * @brief 
 * @return 
*/
bool Gex::Attribute::HasDests() const
{
    if (dests.empty())
    {
        return false;
    }

    for (const auto& dest : dests)
    {
        if(dest)
        {
            return true;
        }
    }
    return false;
}


bool Gex::Attribute::HasIndex(unsigned int index) const
{
	return (multiValues.find(index) != multiValues.end());
}


std::vector<unsigned int> Gex::Attribute::ValidIndices() const
{
    std::vector<unsigned int> indices;
    if (multiValues.empty())
        return indices;

    for (const auto& pair : multiValues)
    {
        indices.push_back(pair.first);
    }

    return indices;
}


bool Gex::Attribute::CreateIndex(unsigned int index)
{
    CHECK_EDITABLE_BOOL()

	return _CreateIndex(index);
}


bool Gex::Attribute::_CreateIndex(unsigned int index)
{
    if (HasIndex(index))
	{
		return false;
	}

	std::any _v = typeHandle->InitValue();

	AttrValueType indexType = AttrValueType::Single;
	if (IsHolder())
	{
		indexType = AttrValueType::Holder;
	}

	std::string indexName = Name() + "[" + std::to_string(index) + "]";
	AttributePtr idxAt = std::make_shared<Attribute>(indexName, _v, indexType,
		attributeType, IsUserDefined(), Node().ToShared(),
        shared_from_this());

     multiValues[index] = idxAt;

    if (IsHolder())
    {
        std::vector<AttributeWkPtr> childs = GetChildAttributes();
        for (const auto& at : childs)
        {
            idxAt->CopyAsChild(at.ToShared());
        }
    }

    SignalChange(AttributeChange::IndexAdded);
	return true;
}


bool Gex::Attribute::RemoveIndex(unsigned int index)
{
    CHECK_EDITABLE_BOOL()

    return _RemoveIndex(index);
}


bool Gex::Attribute::_RemoveIndex(unsigned int index)
{
    if (!HasIndex(index))
    {
        return false;
    }

    multiValues.erase(index);

    SignalChange(AttributeChange::IndexRemoved);
    return true;
}


bool Gex::Attribute::ClearIndices()
{
    CHECK_EDITABLE_BOOL()

    return _ClearIndices();
}


bool Gex::Attribute::_ClearIndices()
{
    if (!IsMulti())
        return false;

    for (unsigned int index : ValidIndices())
    {
        _RemoveIndex(index);
    }

    return true;
}



unsigned int Gex::Attribute::AttributeIndex(const AttributePtr& sub, Feedback* status)
{
    for (const auto& mattr : multiValues)
    {
        if (mattr.second == sub)
        {
            if (status)
            {
                status->status = Status::Success;
            }

            return mattr.first;
        }
    }

    if (status)
    {
        status->status = Status::Failed;
    }

    return 0;
}


bool  Gex::Attribute::_CanConnectSource(const AttributePtr& source)
{
    // If source attribute is from a compound AND is internal, check
    // if current node is an internal node.
    if (CompoundNodePtr sourceCmp = CompoundNode::FromNode(source->Node()))
    {
        return false;
    }

    // If current attribute is from a compound AND is internal, check
    // if source node is an internal node.
    if (CompoundNodePtr compound = CompoundNode::FromNode(Node()))
    {
        if (compound->HasNode(source->Node()))
        {
            return false;
        }
    }

    if (!source->IsOutput())
    {
        return false;
    }

    if (source->Node()->Parent() != Node()->Parent())
    {
        return false;
    }

    if (source->Type() != Type())
    {
        if (!typeHandle->CanConvertFrom(source->GetAnyValue()))
        {
            return false;
        }
    }

    return true;
}


bool Gex::Attribute::CanConnectSource(const AttributeWkPtr& source)
{
    if (!source)
        return false;

    return CanConnectSource(source.ToShared());
}


bool Gex::Attribute::CanConnectSource(const AttributePtr& source)
{
    CHECK_EDITABLE_BOOL()

    if (IsMulti() && !source->IsMulti())
    {
        return false;
    }

    return _CanConnectSource(source);
}


bool Gex::Attribute::CanConnectSourceToIndex(const AttributePtr& source)
{
    CHECK_EDITABLE_BOOL()

    return _CanConnectSource(source);
}


bool Gex::Attribute::ConnectSource(unsigned int index, const AttributeWkPtr& attribute)
{
    if (!attribute)
        return false;

    return ConnectSource(index, attribute.ToShared());
}


bool Gex::Attribute::ConnectSource(unsigned int index, const AttributePtr& attribute)
{
	CHECK_EDITABLE_BOOL()

	if (!IsMulti())
	{
		return false;
	}

    if (!HasIndex(index))
    {
       if (!CreateIndex(index))
       {
           return false;
       }
    }

	AttributeWkPtr indexAttr = GetIndexAttribute(index);
	return indexAttr->_ConnectSource(attribute);
}


bool Gex::Attribute::ConnectSource(const AttributeWkPtr& attribute)
{
    if (!attribute)
        return false;

    return ConnectSource(attribute.ToShared());
}


bool Gex::Attribute::ConnectSource(const AttributePtr& attribute)
{
    CHECK_EDITABLE_BOOL()

	return _ConnectSource(attribute);
}


bool Gex::Attribute::_ConnectSource(const AttributePtr& attribute)
{
    if (!CanConnectSource(attribute))
	{
		return false;
	}

    auto prev = source;
	source = attribute;
    source->dests.push_back(weak_from_this());
    SignalChange(AttributeChange::Connected);

    if (!Undo::UndoStack::IsDoing())
        Undo::UndoStack::AddUndo(
                Undo::CreateUndo<Undo::ConnectAttr>(
                    attribute, shared_from_this(),
                    prev.ToShared()));

	return true;
}


bool Gex::Attribute::ConnectDest(const AttributePtr& attribute)
{
    CHECK_EDITABLE_BOOL()

	return _ConnectDest(attribute);
}


bool Gex::Attribute::_ConnectDest(const AttributePtr& attribute)
{
    if (!attribute->CanConnectSource(shared_from_this()))
	{
		return false;
	}

	attribute->source = shared_from_this();

    dests.emplace_back(attribute);

    SignalChange(AttributeChange::Connected);
	return true;
}


void Gex::Attribute::_ValidateDests()
{
    if (dests.empty())
        return;

    std::vector<Gex::AttributeWkPtr> results;
    for (const Gex::AttributeWkPtr& dest : dests)
    {
        if (!dest)
            continue;

        results.push_back(dest);
    }

    dests = results;
}


bool Gex::Attribute::ConnectDest(unsigned int index, const AttributePtr& attribute)
{
    CHECK_EDITABLE_BOOL()

	if (!attribute->IsMulti())
	{
		return false;
	}

	AttributeWkPtr indexAttr = GetIndexAttribute(index);
	if (!indexAttr)
	{
		return false;
	}

	return indexAttr->_ConnectDest(attribute);
}


bool Gex::Attribute::DisconnectSource(const AttributeWkPtr& attribute)
{
    if (!attribute)
        return false;

    return DisconnectSource(attribute.ToShared());
}


bool Gex::Attribute::DisconnectSource(const AttributePtr& attribute)
{
    CHECK_EDITABLE_BOOL()

	return _DisconnectSource(attribute);
}


bool Gex::Attribute::_DisconnectSource(const AttributePtr& attribute)
{
    if (!HasSource())
	{
		return false;
	}

    auto destIter = std::find(
            source->dests.begin(), source->dests.end(),
            weak_from_this());

    if (destIter != source->dests.end())
        source->dests.erase(destIter);

    source.reset();

	attribute->DisconnectDest(shared_from_this());
    SignalChange(AttributeChange::Disconnected);

    if (!Undo::UndoStack::IsDoing())
        Undo::UndoStack::AddUndo(
            Undo::CreateUndo<Undo::DisconnectAttr>(
                    attribute, shared_from_this())
                );

	return true;
}


bool Gex::Attribute::DisconnectSource(unsigned int index, const AttributeWkPtr& attribute)
{
    if (!attribute)
        return false;

    return DisconnectSource(index, attribute.ToShared());
}


bool Gex::Attribute::DisconnectSource(unsigned int index, const AttributePtr& attribute)
{
    CHECK_EDITABLE_BOOL()

	AttributeWkPtr indexAttribute = GetIndexAttribute(index);
    if (!indexAttribute)
    {
        return false;
    }

	return attribute->_DisconnectSource(indexAttribute.ToShared());
}


bool Gex::Attribute::ClearSource()
{
    if (!HasSource())
        return false;

    auto src = Source();
    if (!src)
        return false;

    return DisconnectSource(src.ToShared());
}

bool Gex::Attribute::ClearSource(unsigned int index)
{
    if (!HasSource(index))
        return false;

    auto src = GetIndexAttribute(index)->Source();
    if (!src)
        return false;

    return DisconnectSource(src.ToShared());
}


bool Gex::Attribute::DisconnectDest(const AttributeWkPtr& attribute)
{
    if (!attribute)
        return false;

    return DisconnectDest(attribute.ToShared());
}


bool Gex::Attribute::DisconnectDest(const AttributePtr& attribute)
{
    CHECK_EDITABLE_BOOL()

	return _DisconnectDest(attribute);
}


bool Gex::Attribute::_DisconnectDest(const AttributePtr& attribute)
{
    if (!HasDests())
	{
		return false;
	}

	attribute->DisconnectSource(shared_from_this());
	return true;
}


bool Gex::Attribute::DisconnectDest(unsigned int index, const AttributeWkPtr& attribute)
{
    if (!attribute)
        return false;

    return DisconnectDest(index, attribute.ToShared());
}


bool Gex::Attribute::DisconnectDest(unsigned int index, const AttributePtr& attribute)
{
    CHECK_EDITABLE_BOOL()

	AttributeWkPtr indexAttribute = GetIndexAttribute(index);
	if (!indexAttribute)
	{
		return false;
	}

	return attribute->DisconnectDest(indexAttribute);
}


bool Gex::Attribute::ClearDests()
{
    auto destinations = Dests();

    bool success = true;
    for (const auto& destination : destinations)
    {
        if (!DisconnectDest(destination))
            success = false;
    }

    return success;
}


Gex::AttributeWkPtr Gex::Attribute::Source() const
{
    return source;
}


std::vector<Gex::AttributeWkPtr> Gex::Attribute::Sources() const
{
    std::vector<AttributeWkPtr> srcs;
    if (!IsInput())
    {
        return srcs;
    }

    if (HasSource())
    {
        srcs.push_back(source);
    }

	if (IsMulti())
    {
        for (const auto& attr : multiValues)
        {
            std::vector<AttributeWkPtr> indexSrcs = attr.second->Sources();
            if (indexSrcs.empty())
            {
                continue;
            }

            srcs.insert(srcs.end(), indexSrcs.begin(), indexSrcs.end());
        }
    }

    if (IsHolder())
    {
        for (const auto& attr : subAttributes)
        {
            std::vector<AttributeWkPtr> subSrcs = attr.second->Sources();
            if (subSrcs.empty())
            {
                continue;
            }

            srcs.insert(srcs.end(), subSrcs.begin(), subSrcs.end());
        }
    }

	return srcs;
}


std::vector<Gex::AttributeWkPtr> Gex::Attribute::Dests()
{
	_ValidateDests();

    return dests;
}


bool Gex::Attribute::ValueSet(const std::any& value)
{
    if (attributeType == AttrType::Output)
    {
        return false;
    }

    if (HasSource())
    {
        return false;
    }

    return SetAnyValue(value);
}


std::any Gex::Attribute::ValueGet(const std::type_info& t, Feedback* status) const
{
    if (std::type_index(t) != Type())
    {
        auto handle = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(t);
        if (!handle)
        {
            if (status)
            {
                status->Set(
                        Status::Failed,
                        "Could not find type handle."
                );
            }

            return {};
        }

        std::any cvrtval = handle->ConvertFrom(attributeAnyValue, attributeAnyValue);
        if (!cvrtval.has_value())
        {
            if (status)
                status->Set(
                    Status::Failed,
                    "Conversion failed."
                );
        }

        return cvrtval;
    }

    if (status)
        status->status =Status::Success;

    return attributeAnyValue;
}


std::vector<Gex::AttributeWkPtr> Gex::Attribute::GetArray() const
{
    std::vector<AttributeWkPtr> arr;
    for (std::pair<unsigned int, AttributePtr> p : multiValues)
    {
        arr.emplace_back(p.second);
    }

    return arr;
}


Gex::AttributeWkPtr Gex::Attribute::GetIndexAttribute(unsigned int index) const
{
    if (!IsMulti())
    {
        return {};
    }

    if (!HasIndex(index))
    {
        return {};
    }

    return multiValues.at(index);
}


std::any Gex::Attribute::GetAnyValue() const
{
    return attributeAnyValue;
}


bool Gex::Attribute::_SetAnyValue(const std::any& value)
{
    if (std::type_index(value.type()) != Type())
    {
        std::any dstval = typeHandle->ConvertFrom(value, attributeAnyValue);
        std::string tn = dstval.type().name();
        if (!dstval.has_value() || std::type_index(dstval.type()) != Type())
        {
            return false;
        }

        if (attributeNode)
            if (!typeHandle->CompareValue(attributeAnyValue, dstval))
            {
                attributeNode->SetModified();
            }

        attributeAnyValue = dstval;
        return true;
    }

    if (attributeNode)
        if (!typeHandle->CompareValue(attributeAnyValue, value))
        {
            attributeNode->SetModified();
        }

    attributeAnyValue = value;
    return true;
}


bool Gex::Attribute::SetAnyValue(const std::any& value)
{
    std::any previous = GetAnyValue();

    if (!_SetAnyValue(value))
    {
        return false;
    }

    SignalChange(AttributeChange::ValueChanged);

    if (!Undo::UndoStack::IsDoing())
        Undo::UndoStack::AddUndo(
            Undo::CreateUndo<Undo::SetAttr>(
                shared_from_this(),
                GetAnyValue(),
                previous
            )
        );

    return true;
}


void Gex::Attribute::SerializeAttribute(rapidjson::Value& value, rapidjson::Document& doc)
{
	// Sent value must be the root dict.
	rapidjson::Value& resultValue = rapidjson::Value().SetObject();

	if (typeHandle)
	{
        rapidjson::Value& apiTypeValue = rapidjson::Value().SetString(
                typeHandle->ApiName().c_str(), doc.GetAllocator());

        rapidjson::Value& attrSerializedTypeKey = rapidjson::Value().SetString(
                Config::GetConfig().attributeSerializedTypeKey.c_str(),
                doc.GetAllocator());
		resultValue.AddMember(attrSerializedTypeKey.Move(),
                              apiTypeValue.Move(), doc.GetAllocator());
	}

	rapidjson::Value& typeValue = rapidjson::Value().SetInt((int)attributeType);
    rapidjson::Value& typeKey = rapidjson::Value().SetString(
            Config::GetConfig().attributeTypeKey.c_str(),
            doc.GetAllocator());
	resultValue.AddMember(typeKey.Move(), typeValue.Move(), doc.GetAllocator());

	rapidjson::Value& valueTypeValue = rapidjson::Value().SetInt((int)attributeValueType);
    rapidjson::Value& valueTypeKey = rapidjson::Value().SetString(
            Config::GetConfig().attributeValueTypeKey.c_str(),
            doc.GetAllocator());
	resultValue.AddMember(valueTypeKey.Move(), valueTypeValue.Move(), doc.GetAllocator());

	if (IsHolder())
	{
		rapidjson::Value& childAttributes = rapidjson::Value().SetObject();
		for (std::pair<std::string, AttributePtr> child : subAttributes)
		{
			child.second->SerializeAttribute(childAttributes, doc);
		}
        rapidjson::Value& attrChildrenKey = rapidjson::Value().SetString(
                Config::GetConfig().attributeChildrenKey.c_str(),
                doc.GetAllocator());
		resultValue.AddMember(attrChildrenKey.Move(), childAttributes.Move(), doc.GetAllocator());
	}

	rapidjson::Value& attributeNameKey = rapidjson::Value().SetString(
            attributeName.c_str(), doc.GetAllocator());
	value.AddMember(attributeNameKey.Move(), resultValue.Move(), doc.GetAllocator());
}


Gex::AttributePtr Gex::Attribute::DeserializeAttribute(
	const std::string& name, rapidjson::Value& attributes,
    const Gex::NodePtr& node, const AttributePtr& parent,
    bool userDefined)
{
    auto conf = Config::GetConfig();

	rapidjson::Value& serialValues = attributes.GetObject();
	
	std::string attributeValueStr;

	size_t attributeValueHash;
    std::string apiTypeName;

    if (serialValues.HasMember(conf.attributeSerializedTypeKey.c_str()))
	{
        apiTypeName = serialValues[conf.attributeSerializedTypeKey.c_str()].GetString();
	}

	auto handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(apiTypeName);
    if (!handler)
	{
		return nullptr;
	}

	AttrType type = AttrType::Static;
	if (serialValues.HasMember(Config::GetConfig().attributeTypeKey.c_str()))
	{
		type = AttrType(serialValues[Config::GetConfig().attributeTypeKey.c_str()].GetInt());
	}

	AttrValueType valuetype = AttrValueType::Single;
	if (serialValues.HasMember(Config::GetConfig().attributeValueTypeKey.c_str()))
	{
		valuetype = AttrValueType(serialValues[Config::GetConfig().attributeValueTypeKey.c_str()].GetInt());
	}

	auto result = std::make_shared<Attribute>(
            name, handler->InitValue(),
            valuetype, type, userDefined,
            node, parent
    );

	if (serialValues.HasMember(Config::GetConfig().attributeChildrenKey.c_str()))
	{
		rapidjson::Value& childAttributes = serialValues[Config::GetConfig().attributeChildrenKey.c_str()];
		for (rapidjson::Value::ConstMemberIterator iter = childAttributes.MemberBegin();
			iter != childAttributes.MemberEnd(); iter++
			)
		{
			AttributePtr subAttr = DeserializeAttribute(
                    iter->name.GetString(),
                    childAttributes, node,
                    result, userDefined
            );

			if (subAttr == nullptr)
				continue;

			result->AddChildAttribute(subAttr);
		}
	}

	return result;
}


// Serializing only saves value, not connections.
// This serialized data will be used to set value of
// unconnected attributes. This means that an attribute
// that is connected should not be serialized.
void Gex::Attribute::Serialize(rapidjson::Value& value, rapidjson::Document& doc)
{
    /*
     * If current attribute node is not dynamic, serialize current value,
     * and store it to document.
     */
    if (!IsMulti())
    {
        /*
         * Only serialize attribute that are not on default.
         */
        if (!IsDefault() && !HasSource())
        {
            if (typeHandle)
            {
                rapidjson::Value resultAttrValue;
                typeHandle->SerializeValue(GetAnyValue(), resultAttrValue, doc);

                rapidjson::Value attributeValueKey(rapidjson::kStringType);
                attributeValueKey.SetString(
                        Config::GetConfig().attributeValueKey.c_str(),
                        doc.GetAllocator()
                );

                value.AddMember(attributeValueKey.Move(), resultAttrValue, doc.GetAllocator());
            }
        }
    }

    else
    {
        rapidjson::Value multiValue(rapidjson::kObjectType);
        for (std::pair<unsigned int, AttributePtr> p : multiValues)
        {
            rapidjson::Value subValue(rapidjson::kObjectType);

            p.second->Serialize(subValue, doc);

            std::string idx = std::to_string(p.first);
            rapidjson::Value& index = rapidjson::Value().SetString(
                    rapidjson::StringRef(idx.c_str()),
                    doc.GetAllocator()
            );

            multiValue.AddMember(index.Move(), subValue.Move(), doc.GetAllocator());
        }

        // Only save if some value were stored.
        if (multiValue.MemberCount())
        {
            rapidjson::Value multiValueKey(rapidjson::kStringType);
            multiValueKey.SetString(
                    Config::GetConfig().attributeMultiValueKey.c_str(),
                    doc.GetAllocator()
            );

            value.AddMember(multiValueKey.Move(), multiValue.Move(), doc.GetAllocator());
        }
    }

	if (HasChildAttributes())
	{
		rapidjson::Value subAttributesData(rapidjson::kObjectType);
		for (std::pair<std::string, AttributePtr> subAttribute : subAttributes)
		{
            rapidjson::Value subAttributeValues(rapidjson::kObjectType);
            subAttribute.second->Serialize(subAttributeValues, doc);

            subAttributesData.AddMember(
                    rapidjson::Value(rapidjson::kStringType).SetString(
                        subAttribute.first.c_str(), doc.GetAllocator()
                    ), subAttributeValues, doc.GetAllocator()
            );
		}

		value.AddMember(
                rapidjson::Value(rapidjson::kStringType).SetString(
                    Config::GetConfig().attributeChildrenKey.c_str(),
                    doc.GetAllocator()
                ),subAttributesData.Move(), doc.GetAllocator());
	}
}


void Gex::Attribute::Deserialize(rapidjson::Value& value)
{
    if (value.IsNull())
        return;

	if (value.HasMember(Config::GetConfig().attributeValueKey.c_str()))
	{
        auto& attrValue = value[Config::GetConfig().attributeValueKey.c_str()];

        if (!attrValue.IsNull())
        {
            attributeAnyValue = typeHandle->DeserializeValue(
                    attributeAnyValue, attrValue

            );
        }

	}
    else
    {
        if (!attributeAnyValue.has_value() && !defaultValue.has_value())
            attributeAnyValue = typeHandle->InitValue();
        else
            attributeAnyValue = defaultValue;
    }

	if (value.HasMember(Config::GetConfig().attributeMultiValueKey.c_str()) && IsMulti())
	{
		rapidjson::Value& multiValue = value[Config::GetConfig().attributeMultiValueKey.c_str()].GetObject();
		for (rapidjson::Value::ConstMemberIterator itr = multiValue.MemberBegin();
			 itr != multiValue.MemberEnd(); ++itr)
		{
			const rapidjson::Value& index = itr->name;

			rapidjson::Value& multi = multiValue[index.GetString()];
			unsigned int idx = std::stoi(index.GetString());

			if (!_CreateIndex(idx))
            {
                continue;
            }

            multiValues.at(idx)->Deserialize(multi);
		}
	}

	if (value.HasMember(Config::GetConfig().attributeChildrenKey.c_str()))
	{
		rapidjson::Value& childAttributes = value[Config::GetConfig().attributeChildrenKey.c_str()];
		for (rapidjson::Value::ConstMemberIterator iter = childAttributes.MemberBegin();
			 iter != childAttributes.MemberEnd(); ++iter)
		{
			std::string atName = iter->name.GetString();
            AttributeWkPtr sub = GetAttribute(atName);
			if (!sub)
			{
				continue;
			}

			sub->Deserialize(value[Config::GetConfig().attributeChildrenKey.c_str()][atName.c_str()]);
		}
	}
}


bool Gex::Attribute::PullSource()
{
    if (!HasSource())
    {
        return false;
    }

    if (AttributeWkPtr source_ = Source())
    {
        if (!IsMulti())
        {
            return _SetAnyValue(source_->GetAnyValue());
        }
        else
        {
            std::vector<unsigned int> sourceIndices = source_->ValidIndices();
            for (unsigned int index : sourceIndices)
            {
                if (!HasIndex(index))
                    _CreateIndex(index);

                AttributeWkPtr sourceIndexAttr = source_->GetIndexAttribute(index);
                AttributeWkPtr indexAttr = GetIndexAttribute(index);
                indexAttr->_SetAnyValue(sourceIndexAttr->GetAnyValue());
            }

            for (unsigned int index : ValidIndices())
            {
                if (std::find(sourceIndices.begin(), sourceIndices.end(), index) == sourceIndices.end())
                {
                    AttributeWkPtr indexAttr = GetIndexAttribute(index);
                    if (indexAttr->HasSource())
                    {
                        continue;
                    }

                    _RemoveIndex(index);
                }
            }
        }
    }


    return true;
}


bool Gex::Attribute::Pull()
{
    PullSource();

    if (IsMulti())
    {
        for (const auto& attr : multiValues)
        {
            attr.second->Pull();
        }
    }

    if (IsHolder())
    {
        for (const auto& attr : subAttributes)
        {
            attr.second->Pull();
        }
    }

    return true;
}


Gex::AttributeList::const_iterator Gex::FindAttributeItByName(const AttributeList& l,  const std::string& n)
{
    auto pred = [n](const Gex::AttributePtr& a)
    {
        return (a && a->Name() == n);
    };

    return std::find_if(l.begin(), l.end(), pred);
}


Gex::AttributeList::const_iterator Gex::FindAttributeItByPath(const AttributeList& l,  const std::string& n)
{
    auto pred = [n](const Gex::AttributePtr& a)
    {
        return (a && a->Longname() == n);
    };

    return std::find_if(l.begin(), l.end(), pred);
}


Gex::AttributePtr Gex::FindAttributeByName(const AttributeList& l,  const std::string& n)
{
    auto iter = FindAttributeItByName(l, n);
    if (iter == l.end())
        return {};

    return *iter;
}


Gex::AttributePtr Gex::FindAttributeByPath(const AttributeList& l,  const std::string& n)
{
    auto iter = FindAttributeItByPath(l, n);
    if (iter == l.end())
        return {};

    return *iter;
}


bool Gex::ContainsAttributeByName(
        const AttributeList& list,
        const std::string& name)
{
    return (FindAttributeItByName(list, name) != list.end());
}


bool Gex::ContainsAttributeByPath(
        const AttributeList& list,
        const std::string& name)
{
    return (FindAttributeItByPath(list, name) != list.end());
}
