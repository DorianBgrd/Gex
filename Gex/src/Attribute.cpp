#include "Gex/include/Config.h"
#include "Gex/include/Attribute.h"
#include "Gex/include/Node.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "Gex/include/Config.h"

#include "Tsys/include/tsys.h"
#include "Tsys/include/defaultTypes.h"

#include <boost/container_hash/hash.hpp>


#define CHECK_EDITABLE_SKIP() if(!IsEditable()){return;}
#define CHECK_EDITABLE_BOOL() if(!IsEditable()){return false;}


Gex::Attribute::Attribute()
{
    isInternal = false;
	parentAttribute = nullptr;
	attributeNode = nullptr;
	attributeAnyValue = std::make_any<int>(0);
	attributeIsUserDefined = false;
}


Gex::AttrValueType Gex::Attribute::GetAttrValueType() const
{
    return attributeValueType;
}


Gex::AttrType Gex::Attribute::GetAttrType() const
{
    return attributeType;
}


Gex::Attribute::Attribute(std::string name, std::any v, AttrValueType valueType,
			              AttrType type, bool userDefined, Gex::Node* node, Attribute* parent)
{
    isInternal = false;
    attributeNode = node;
    attributeName = name;
    if (parent == nullptr)
        attributeLongname = name;
    else
        attributeLongname = parent->Longname() + Config::GetConfig().attributeSeparator + name;

    attributeAnyValue = v;
    parentAttribute = parent;

    attributeIsUserDefined = userDefined;
    attributeValueType = valueType;
    attributeType = type;
    defaultValue = v;

//    InitDefaultValue();
}


Gex::Attribute::Attribute(const std::string& name, size_t hash, AttrValueType valueType,AttrType type, bool userDefined,
                          Gex::Node* node, Attribute* parent):
                          Attribute(name, TSys::TypeRegistry::GetRegistry()->GetTypeHandle(hash)->InitValue(),
                                    valueType, type, userDefined, node, parent)
{

}


Gex::Attribute::Attribute(const std::string& name, AttrType type, bool multi,
                          bool userDefined, Gex::Node* node, Attribute* parent)
{
    isInternal = false;
    isExternal = true;
    attributeNode = node;
    attributeName = name;
    if (parent == nullptr)
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

    InitDefaultValue();
}


Gex::Attribute::~Attribute()
{
	if (IsMulti())
	{
		for (std::pair<unsigned int, Attribute*> p : multiValues)
		{
			delete p.second;
		}

		multiValues.clear();
	}

    if (IsHolder())
    {
        for (auto h : subAttributes)
        {
            delete h.second;
        }
    }
}


void Gex::Attribute::SignalChange(AttributeChange change)
{
    if (!attributeNode)
    {
        return;
    }

    attributeNode->SignalAttributeChange(this, change);
}


Gex::Attribute* Gex::Attribute::Copy(std::string name, Gex::Node* node, Attribute *parent) const
{
    size_t hash = TypeHash();
    auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(TypeHash());
    if (!handler)
        return nullptr;

    Attribute* newAt = new Attribute(name, handler->InitValue(), attributeValueType,
			                         attributeType, attributeIsUserDefined,
                                     node, parent);
    return newAt;
}


Gex::Attribute* Gex::Attribute::CopyAsChild(Attribute* ref)
{
    Attribute* copied = ref->Copy(ref->Name(), ref->Node(), this);
    AddChildAttribute(copied);
    return copied;
}


void Gex::Attribute::InitDefaultValue()
{
    auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(TypeHash());
    if (!handler)
    {
        return;
    }

    defaultValue = handler->InitValue();
}


void Gex::Attribute::SetParentNode(Gex::Node* node)
{
	attributeNode = node;
}


Gex::Node* Gex::Attribute::Node() const
{
	return attributeNode;
}


Gex::Attribute* Gex::Attribute::ParentAttribute() const
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


size_t Gex::Attribute::TypeHash() const
{
	return attributeAnyValue.type().hash_code();
}


std::any Gex::Attribute::DefaultValue() const
{
    return defaultValue;
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
    auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(TypeHash());
    if(!handler)
    {
        return false;
    }

    return handler->CompareValue(defaultValue, attributeAnyValue);
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


bool Gex::Attribute::IsInternal() const
{
    return isInternal;
}


bool Gex::Attribute::SetInternal(bool internal)
{
    if (!attributeIsUserDefined && !attributeNode->IsInitializing())
        return false;

    isInternal = internal;
    return true;
}


bool Gex::Attribute::IsExternal() const
{
    return isExternal;
}


bool Gex::Attribute::SetExternal(bool external)
{
    if (!attributeIsUserDefined && !attributeNode->IsInitializing())
        return false;

    isExternal = external;
    return true;
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
        for (auto* at : GetChildAttributes())
        {
            boost::hash_combine(res, at->ValueHash());
        }
    }

    if (followCnx && HasSource())
    {
        auto* at = Source();
        if (at)
        {
            auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(at->TypeHash());
            boost::hash_combine(res, handler->ValueHash(at->GetAnyValue()));
        }
    }
    else
    {
        auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(TypeHash());
        boost::hash_combine(res, handler->ValueHash(attributeAnyValue));
    }

    return res;
}



bool Gex::Attribute::HasChildAttributes() const
{
	return (subAttributes.size() > 0);
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


std::vector<Gex::Attribute*> Gex::Attribute::GetChildAttributes() const
{
	std::vector<Attribute*> childs;
	for (std::pair<std::string, Attribute*> p : subAttributes)
	{
		childs.push_back(p.second);
	}

	return childs;
}


Gex::Attribute* Gex::Attribute::GetAttribute(std::string name) const
{
	size_t delimiter = name.find(Config::GetConfig().attributeSeparator);
	std::string shortname = name.substr(0, delimiter);

	Attribute* attribute = nullptr;

    std::smatch results;
    std::regex multiRegex("\\[[0-9]+\\]$");
    std::regex_search(shortname, results, multiRegex);
	if (IsHolder() && results.empty())
	{
		if (subAttributes.find(shortname) == subAttributes.end())
		{
			return nullptr;
		}

		attribute = subAttributes.at(shortname);
	}
	else if (IsMulti())
	{
		if (results.empty())
		{
			return nullptr;
		}

		unsigned int index = std::stoi(results.str().substr(1, results.size() - 2));
		if (!HasIndex(index))
		{
			return nullptr;
		}

		attribute = multiValues.at(index);
	}
	else
	{
		return nullptr;
	}

	if (delimiter == std::string::npos)
	{
		return attribute;
	}

	return attribute->GetAttribute(name.erase(0, delimiter+1));
}


std::vector<Gex::Attribute*> Gex::Attribute::GetAllAttributes() const
{
    std::vector<Gex::Attribute*> attributes;

    for (unsigned int index : ValidIndices())
    {
        Attribute* idxAttr = GetIndexAttribute(index);
        attributes.push_back(idxAttr);

        std::vector<Attribute*> subs = idxAttr->GetAllAttributes();
        for (Attribute* attr : subs)
        {
            attributes.push_back(attr);
        }
    }

    for (Attribute* attr : GetChildAttributes())
    {
        attributes.push_back(attr);

        std::vector<Attribute*> subs = attr->GetAllAttributes();
        for (Attribute* attr : subs)
        {
            attributes.push_back(attr);
        }
    }

    return attributes;
}


bool Gex::Attribute::AddChildAttribute(Attribute* child)
{
//    CHECK_EDITABLE_BOOL()

	if (subAttributes.find(child->Name()) != subAttributes.end())
	{
		return false;
	}

	subAttributes[child->Name()] = child;
	return true;
}


bool Gex::Attribute::HasSource() const
{
    return source.valid();
}


bool Gex::Attribute::HasSource(unsigned int index)
{
	Attribute* indexAttr = GetIndexAttribute(index);
	if (indexAttr == nullptr)
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

    for (unsigned int i = 0; i < dests.size(); i++)
    {
        if(dests.at(i).ptr() != nullptr)
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

    for (auto pair : multiValues)
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

	auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(TypeHash());
	if (!handler)
	{
		return false;
	}

	std::any _v = handler->InitValue();

	AttrValueType indexType = AttrValueType::Single;
	if (IsHolder())
	{
		indexType = AttrValueType::Holder;
	}

	std::string indexName = Name() + "[" + std::to_string(index) + "]";
	Attribute* idxAt = new Attribute(indexName, _v, indexType,
		attributeType, IsUserDefined(), Node(), this);

     multiValues[index] = idxAt;

    if (IsHolder())
    {
        std::vector<Attribute*> childs = GetChildAttributes();
        for (auto at : childs)
        {
            idxAt->CopyAsChild(at);
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

    Attribute* indexAttr = multiValues.at(index);
    multiValues.erase(index);
    delete indexAttr;

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



unsigned int Gex::Attribute::AttributeIndex(Attribute *sub, Feedback* status)
{
    for (auto mattr : multiValues)
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


bool  Gex::Attribute::_CanConnectSource(Attribute* source)
{
    // If source attribute is from a compound AND is internal, check
    // if current node is an internal node.
    if (CompoundNode* sourceCmp = CompoundNode::FromNode(source->Node()))
    {
        if (sourceCmp->HasNode(Node()) && source->IsInternal())
        {
            return (source->IsInput() && IsInput());
        }
    }

    // If current attribute is from a compound AND is internal, check
    // if source node is an internal node.
    else if (CompoundNode* compound = CompoundNode::FromNode(Node()))
    {
        if (compound->HasNode(source->Node()) && IsInternal())
        {
            return (source->IsOutput() && IsOutput());
        }
    }

    else if (!source->IsOutput())
    {
        return false;
    }

    else if (source->Node()->Parent() != Node()->Parent())
    {
        return false;
    }

    if (!isExternal)
    {
        return false;
    }

    if (source->TypeHash() != TypeHash())
    {
        auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(TypeHash());
        if (!handler)
        {
            return false;
        }

        if (!handler->CanConvertFrom(source->GetAnyValue()))
        {
            return false;
        }
    }

    return true;
}


bool  Gex::Attribute::CanConnectSource(Attribute* source)
{
    CHECK_EDITABLE_BOOL()

    if (IsMulti() && !source->IsMulti())
    {
        return false;
    }

    return _CanConnectSource(source);
}


bool Gex::Attribute::CanConnectSourceToIndex(Attribute* source)
{
    CHECK_EDITABLE_BOOL()

    return _CanConnectSource(source);
}


bool Gex::Attribute::ConnectSource(unsigned int index, Attribute* attribute)
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

	Attribute* indexAttr = GetIndexAttribute(index);
	return indexAttr->_ConnectSource(attribute);
}


bool Gex::Attribute::ConnectSource(Attribute* attribute)
{
    CHECK_EDITABLE_BOOL()

	return _ConnectSource(attribute);
}


bool Gex::Attribute::_ConnectSource(Attribute* attribute)
{
    if (!CanConnectSource(attribute))
	{
		return false;
	}

	source = makeSafePtr(attribute);

    SignalChange(AttributeChange::Connected);
	return true;
}


bool Gex::Attribute::ConnectDest(Attribute* attribute)
{
    CHECK_EDITABLE_BOOL()

	return _ConnectDest(attribute);
}


bool Gex::Attribute::_ConnectDest(Attribute *attribute)
{
    if (!attribute->CanConnectSource(this))
	{
		return false;
	}

	attribute->source = makeSafePtr(this);

    dests.push_back(makeSafePtr(attribute));

    SignalChange(AttributeChange::Connected);
	return true;
}


std::vector<Gex::Attribute*> Gex::Attribute::_ValidateDests()
{
    std::vector<Attribute*> results;

    if (dests.empty())
        return results;

    auto itr = dests.end();
    do
    {
        if (itr->ptr() == nullptr)
        {
            dests.erase(itr);
        }
        else
        {
            results.push_back(itr->ptr());
        }

        itr--;
    }
    while (itr != dests.begin());

    return results;
}


bool Gex::Attribute::ConnectDest(unsigned int index, Attribute* attribute)
{
    CHECK_EDITABLE_BOOL()

	if (!attribute->IsMulti())
	{
		return false;
	}

	Attribute* indexAttr = GetIndexAttribute(index);
	if (indexAttr == nullptr)
	{
		return false;
	}

	return indexAttr->_ConnectDest(attribute);
}


bool Gex::Attribute::DisconnectSource(Attribute* attribute)
{
    CHECK_EDITABLE_BOOL()

	return _DisconnectSource(attribute);
}


bool Gex::Attribute::_DisconnectSource(Attribute* attribute)
{
    if (!HasSource())
	{
		return false;
	}

    source.reset();

	attribute->DisconnectDest(this);

    SignalChange(AttributeChange::Disconnected);
	return true;
}


bool Gex::Attribute::DisconnectSource(unsigned int index, Attribute* attribute)
{
    CHECK_EDITABLE_BOOL()

	Attribute* indexAttribute = GetIndexAttribute(index);
    if (!indexAttribute)
    {
        return false;
    }

	return attribute->_DisconnectSource(indexAttribute);
}


bool Gex::Attribute::DisconnectDest(Attribute* attribute)
{
    CHECK_EDITABLE_BOOL()

	return _DisconnectDest(attribute);
}


bool Gex::Attribute::_DisconnectDest(Attribute *attribute)
{
    if (!HasDests())
	{
		return false;
	}

	attribute->DisconnectSource(this);
	return true;
}


bool Gex::Attribute::DisconnectDest(unsigned int index, Attribute* attribute)
{
    CHECK_EDITABLE_BOOL()

	Attribute* indexAttribute = GetIndexAttribute(index);
	if (indexAttribute == nullptr)
	{
		return false;
	}

	return attribute->DisconnectDest(indexAttribute);
}


Gex::Attribute* Gex::Attribute::Source() const
{
    return source.ptr();
}


std::vector<Gex::Attribute*> Gex::Attribute::Sources() const
{
    std::vector<Attribute*> srcs;
    if (!IsInput())
    {
        return srcs;
    }

    if (HasSource())
    {
        if (source.valid())
        {
            srcs.push_back(source.ptr());
        }
    }

	if (IsMulti())
    {
        for (auto attr : multiValues)
        {
            std::vector<Attribute*> indexSrcs = attr.second->Sources();
            if (indexSrcs.empty())
            {
                continue;
            }

            srcs.insert(srcs.end(), indexSrcs.begin(), indexSrcs.end());
        }
    }

    if (IsHolder())
    {
        for (auto attr : subAttributes)
        {
            std::vector<Attribute*> subSrcs = attr.second->Sources();
            if (subSrcs.empty())
            {
                continue;
            }

            srcs.insert(srcs.end(), subSrcs.begin(), subSrcs.end());
        }
    }

	return srcs;
}


std::vector<Gex::Attribute*> Gex::Attribute::Dests()
{
	return _ValidateDests();
}


std::any Gex::Attribute::Convert(std::any value, size_t destHash,
                                 Feedback* success) const
{
    auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(destHash);
    if (!handler)
    {
        if (success)
            success->status = Status::Failed;
        return {};
    }

    std::any cvrtval = handler->ConvertFrom(value, attributeAnyValue);
    if (!cvrtval.has_value() || cvrtval.type().hash_code() != destHash)
    {
        if (success)
            success->status = Status::Failed;
        return {};
    }

    if (success)
        success->status = Status::Success;

    return cvrtval;
}


bool Gex::Attribute::ValueSet(std::any value)
{
    if (attributeType == AttrType::Output || !isExternal)
    {
        return false;
    }

    return SetAnyValue(std::move(value));
}


std::any Gex::Attribute::ValueGet(size_t hash, Feedback* status) const
{
    if (hash != TypeHash())
    {
        std::any cvrtval = Convert(attributeAnyValue, TypeHash());
        if (!cvrtval.has_value())
        {
            if (status)
                status->status = Status::Failed;
        }
        return cvrtval;
    }

    if (status)
        status->status =Status::Success;
    return attributeAnyValue;
}


std::vector<Gex::Attribute*> Gex::Attribute::GetArray() const
{
    std::vector<Attribute*> arr;
    for (std::pair<unsigned int, Attribute*> p : multiValues)
    {
        arr.push_back(p.second);
    }

    return arr;
}


Gex::Attribute* Gex::Attribute::GetIndexAttribute(unsigned int index) const
{
    if (!IsMulti())
    {
        return nullptr;
    }

    if (!HasIndex(index))
    {
        return nullptr;
    }

    return multiValues.at(index);
}


std::any Gex::Attribute::GetAnyValue() const
{
    return attributeAnyValue;
}


bool Gex::Attribute::SetAnyValue(std::any value)
{
    size_t destHash = value.type().hash_code();
    size_t typeHash = TypeHash();

    if (destHash != typeHash)
    {
        auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(TypeHash());
        if (!handler)
        {
            return false;
        }

        std::any dstval = handler->ConvertFrom(value, attributeAnyValue);
        if (!dstval.has_value())
        {
            return false;
        }

        value = dstval;
    }

    attributeAnyValue = value;
    SignalChange(AttributeChange::ValueChanged);
    return true;
}


void Gex::Attribute::SerializeAttribute(rapidjson::Value& value, rapidjson::Document& doc)
{
	// Sent value must be the root dict.
	rapidjson::Value& resultValue = rapidjson::Value().SetObject();

    TSys::TypeHandler* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(TypeHash());

	if (handler)
	{
        rapidjson::Value& apiTypeValue = rapidjson::Value().SetString(
                handler->ApiName().c_str(), doc.GetAllocator());

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
		for (std::pair<std::string, Attribute*> child : subAttributes)
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


Gex::Attribute* Gex::Attribute::DeserializeAttribute(
	const std::string& name, rapidjson::Value& attributes,
    Gex::Node* node, Attribute* parent, bool userDefined)
{
    auto conf = Config::GetConfig();

	rapidjson::Value& serialValues = attributes.GetObject();
	
	std::string attributeValueStr;

	bool success = false;
	size_t attributeValueHash;

    if (serialValues.HasMember(conf.attributeSerializedTypeKey.c_str()))
	{
        std::string apiTypeName = serialValues[conf.attributeSerializedTypeKey.c_str()].GetString();
        attributeValueHash = TSys::TypeRegistry::GetRegistry()->HashFromApiName(
                apiTypeName, success);
	}
	
	if (!success)
	{
		return nullptr;
	}

	auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(attributeValueHash);
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

	auto* result = new Attribute(name, attributeValueHash,
                                 valuetype, type, userDefined,
                                 node, parent);

	if (serialValues.HasMember(Config::GetConfig().attributeChildrenKey.c_str()))
	{
		rapidjson::Value& childAttributes = serialValues[Config::GetConfig().attributeChildrenKey.c_str()];
		for (rapidjson::Value::ConstMemberIterator iter = childAttributes.MemberBegin();
			iter != childAttributes.MemberEnd(); iter++
			)
		{
			std::string name = iter->name.GetString();
			// rapidjson::Value& childConst = childAttributes[name.c_str()];

			Attribute* subAttr = DeserializeAttribute(name, childAttributes, node, result, userDefined);
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
	rapidjson::Value resultValue(rapidjson::kObjectType);
    /*
     * If current attribute node is not dynamic, serialize current value,
     * and store it to document.
     */
    if (!IsMulti())
    {
        /*
         * Only serialize attribute that arenot on default.
         */
        if (!IsDefault())
        {
            auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(TypeHash());
            if (handler)
            {
                rapidjson::Value& resultArrayValue = rapidjson::Value().SetArray();
                handler->SerializeValue(GetAnyValue(), resultArrayValue, doc);

                rapidjson::Value& attributeValueKey = rapidjson::Value().SetString(
                        Config::GetConfig().attributeValueKey.c_str(),
                        doc.GetAllocator());
                value.AddMember(attributeValueKey, resultArrayValue, doc.GetAllocator());
            }
        }
    }

    else
    {
        rapidjson::Value& multiValue = rapidjson::Value().SetObject();
        for (std::pair<unsigned int, Attribute*> p : multiValues)
        {
            rapidjson::Value& subValue = rapidjson::Value().SetObject();

            p.second->Serialize(subValue, doc);

            std::string idx = std::to_string(p.first);
            rapidjson::Value& index = rapidjson::Value().SetString(rapidjson::StringRef(idx.c_str()),
                                                                  doc.GetAllocator());

            multiValue.AddMember(index.Move(), subValue, doc.GetAllocator());
        }

        rapidjson::Value& multiValueKey = rapidjson::Value().SetString(
                Config::GetConfig().attributeMultiValueKey.c_str(),
                doc.GetAllocator());
        value.AddMember(multiValueKey, multiValue, doc.GetAllocator());
    }

	if (HasChildAttributes())
	{
		rapidjson::Value& subAttributesValues = rapidjson::Value().SetObject();
		for (std::pair<std::string, Attribute*> c : subAttributes)
		{
			c.second->Serialize(subAttributesValues, doc);
		}
		value.AddMember(rapidjson::StringRef(Config::GetConfig().attributeChildrenKey.c_str()),
                        subAttributesValues, doc.GetAllocator());
	}

    rapidjson::Value& internal = rapidjson::Value().SetBool(isInternal);
    rapidjson::Value& internalKey = rapidjson::Value().SetString(
            Config::GetConfig().attributeInternalKey.c_str(),
            doc.GetAllocator());
    value.AddMember(internalKey, internal, doc.GetAllocator());
}


void Gex::Attribute::Deserialize(rapidjson::Value& value)
{
    auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(TypeHash());
    if (!handler)
    {
        return;
    }

	if (value.HasMember(Config::GetConfig().attributeValueKey.c_str()))
	{
		std::any v = handler->DeserializeValue(attributeAnyValue, value[Config::GetConfig().attributeValueKey.c_str()]);
		attributeAnyValue = v;
	}
    else
    {
        if (!attributeAnyValue.has_value() && !defaultValue.has_value())
            attributeAnyValue = handler->InitValue();
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
			Attribute* sub = GetAttribute(atName);
			if (sub == nullptr)
			{
				continue;
			}

			sub->Deserialize(value[Config::GetConfig().attributeChildrenKey.c_str()][atName.c_str()]);
		}
	}

    if  (value.HasMember(Config::GetConfig().attributeInternalKey.c_str()))
    {
        bool internal = value[Config::GetConfig().attributeInternalKey.c_str()].GetBool();
        SetInternal(internal);
    }
}


bool Gex::Attribute::PullSource()
{
    if (!HasSource())
    {
        return false;
    }

    Attribute* source_ = Source();
    if (!IsMulti())
    {
        return SetAnyValue(source_->GetAnyValue());
    }
    else
    {
        std::vector<unsigned int> sourceIndices = source_->ValidIndices();
        for (unsigned int index : sourceIndices)
        {
            if (!HasIndex(index))
                _CreateIndex(index);

            Attribute* sourceIndexAttr = source_->GetIndexAttribute(index);
            Attribute* indexAttr = GetIndexAttribute(index);
            indexAttr->SetAnyValue(sourceIndexAttr->GetAnyValue());
        }

        for (unsigned int index : ValidIndices())
        {
            if (std::find(sourceIndices.begin(), sourceIndices.end(), index) == sourceIndices.end())
            {
                Attribute* indexAttr = GetIndexAttribute(index);
                if (indexAttr->HasSource())
                {
                    continue;
                }

                _RemoveIndex(index);
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
        for (auto attr : multiValues)
        {
            attr.second->Pull();
        }
    }

    if (IsHolder())
    {
        for (auto attr : subAttributes)
        {
            attr.second->Pull();
        }
    }

    return true;
}