#include <any>
#include <Tsys/typeSystem.h>
#include "rapidjson/document.h"


TSys::TypedValuePtr TSys::DefaultCreator(const TypePtr& type)
{
    return std::make_shared<TypedValue>(type);
}


std::any TSys::UnuseFromPython(const pybind11::object&)
{
    return {};
}


pybind11::object TSys::UnuseToPython(const std::any&)
{
    return {};
}


TSys::Type::Type(
        TypeValueRegistry* registry,
        const std::string& name,
        const std::type_index& index,
        const ValueCreator& creator,
        const Serializer& serializer,
        const Deserializer& deserializer,
        const Compare& compare,
        const InitDefault& initDefault,
        const TSys::FromPython& fromPython,
        const TSys::ToPython& toPython
): _name(name), _index(index), _creator(creator),
   _serializer(serializer), _deserializer(deserializer),
   _compare(compare), _initDefault(initDefault),
   _fromPython(fromPython), _toPython(toPython)
{
    _registry = registry;
}


bool TSys::Type::operator==(const std::any &other) const
{
    return std::type_index(other.type()) == _index;
}


bool TSys::Type::operator==(const Type& other) const
{
    return _index == other._index;
}


bool TSys::Type::operator==(const std::type_index& other) const
{
    return _index == other;
}


bool TSys::Type::IsOfType(const std::any& value) const
{
    return std::type_index(value.type()) == _index;
}


TSys::Converter TSys::Type::GetConverter(std::type_index type) const
{
    auto iter = converters.find(type);
    if (iter == converters.end())
    {
        return iter->second;
    }

    return {};
}


void TSys::Type::RegisterConverter(
        const std::type_index& sourceType,
        const Converter& converter
)
{
    converters[sourceType] = converter;
}


bool TSys::Type::IsConvertibleTo(const TypePtr& other) const
{
    return other && (other.get() == this || bool(other->GetConverter(GetIndex())));
}


bool TSys::Type::IsConvertibleFrom(const TypePtr& other) const
{
    return other && (other.get() == this || bool(GetConverter(other->GetIndex())));
}


TSys::TypeValueRegistry* TSys::Type::GetRegistry() const
{
    return _registry;
}


std::type_index TSys::Type::GetIndex() const
{
    return _index;
}


std::string TSys::Type::GetName() const
{
    return _name;
}


TSys::ValueCreator TSys::Type::GetCreator() const
{
    return _creator;
}


TSys::Serializer TSys::Type::GetSerializer() const
{
    return _serializer;
}


TSys::Deserializer TSys::Type::GetDeserializer() const
{
    return _deserializer;
}


bool TSys::Type::CompareValues(
        const TypedValuePtr& value1,
        const TypedValuePtr& value2
) const
{
    return CompareValues(
            value1->GetAny(),
            value2->GetAny()
    );
}


bool TSys::Type::CompareValues(
        const std::any& value1,
        const std::any& value2
) const
{
    return _compare(
            CastFrom(value1, value1),
            CastFrom(value2, value2)
    );
}


std::any TSys::Type::InitDefaultValue() const
{
    return _initDefault();
}


std::any TSys::Type::FromPython(const pybind11::object& pythonValue)
{
    return _fromPython(pythonValue);
}

pybind11::object TSys::Type::ToPython(const std::any& cppValue)
{
    return _toPython(cppValue);
}


TSys::TypedValue::TypedValue(
        const TypePtr& type
): _type(type)
{
    _value = type->InitDefaultValue();
}


TSys::TypedValue::TypedValue(
        const TypePtr& type,
        const std::any& value
): _type(type), _value(value)
{

}


TSys::TypedValue::TypedValue(
        const TypedValue& other
): _type(other.GetType()),
   _value(other.GetAny())
{

}


const TSys::TypePtr TSys::TypedValue::GetType() const
{
    return _type;
}


pybind11::object TSys::TypedValue::Get() const
{
    return GetType()->ToPython(GetAny());
}


bool TSys::TypedValue::Set(const pybind11::object& pythonObject)
{
    _value = GetType()->FromPython(pythonObject);
    return true;
}


std::any TSys::TypedValue::GetAny() const
{
    return _value;
}


bool TSys::TypedValue::SetAny(const std::any& value)
{
    if (_type->IsOfType(value))
    {
        return false;
    }

    _value = value;
    return true;
}


std::type_index TSys::TypedValue::GetIndex() const
{
    return _type->GetIndex();
}


bool TSys::TypedValue::IsOfType(const std::type_index& otherIndex) const
{
    return otherIndex == GetIndex();
}


bool TSys::TypedValue::IsOfType(const TypedValue& other) const
{
    return IsOfType(other.GetIndex());
}


TSys::TypedValue& TSys::TypedValue::operator=(const TypedValue& other)
{
    if (IsOfType(other))
    {
        _value = other._value;
    }

    else if (auto converter = _type->GetConverter(other.GetIndex()))
    {
        _value = converter(other.GetAny(), _value);
    }

    return *this;
}


bool TSys::TypedValue::operator==(const TypedValuePtr& other) const
{
    return GetType()->CompareValues(
            GetAny(), other->GetAny()
    );
}

bool TSys::TypedValue::IsValid() const
{
    return (_type && _value.has_value());
}


TSys::TypedValue::operator bool() const
{
    return IsValid();
}



TSys::TypeValueRegistry* TSys::TypeValueRegistry::instance;



void TSys::TypeValueRegistry::RegisterType(
        const std::type_info& typeInfo,
        const std::string& name,
        const ValueCreator& creator,
        const Serializer& serializer,
        const Deserializer& deserializer,
        const Compare& compare,
        const InitDefault& initDefault,
        const FromPython& fromPython,
        const ToPython& toPython
) noexcept
{
    std::type_index index(typeInfo);

    typeNames.emplace(name, typeInfo);

    types[index] = std::make_shared<Type>(
            this, name, index, creator,
            serializer, deserializer,
            compare, initDefault,
            fromPython, toPython
    );
}


void TSys::TypeValueRegistry::RegisterCtorConverter(
        const std::type_index& source,
        const std::type_index& dest,
        const Converter& converter
) const
{
    auto destType = GetType(source);
    auto sourceType = GetType(dest);
    if (!destType || !sourceType)
    {
        return;
    }

    destType->RegisterConverter(sourceType->GetIndex(), converter);

}


TSys::TypePtr  TSys::TypeValueRegistry::GetType(const std::type_index& index) const
{
    auto iter = types.find(index);
    if (iter != types.end())
    {
        return (iter->second);
    }

    return {};
}


TSys::TypePtr  TSys::TypeValueRegistry::GetType(const std::string& name) const
{
    auto nameIter = typeNames.find(name);
    if (nameIter != typeNames.end())
    {
        return GetType(nameIter->second);
    }

    return {};
}


TSys::TypePtr TSys::TypeValueRegistry::GetType(const std::any& value) const
{
    return GetType(std::type_index(value.type()));
}


TSys::TypedValuePtr TSys::TypeValueRegistry::CreateValue(const std::string& name) const
{
    return CreateValue(GetType(name));
}


TSys::TypedValuePtr TSys::TypeValueRegistry::CreateValue(const std::type_index& index) const
{
    return CreateValue(GetType(index));
}


TSys::TypedValuePtr TSys::TypeValueRegistry::CreateValue(const TypePtr& type) const
{
    if (!type)
    {
        return {};
    }

    return type->GetCreator()(type);
}


TSys::TypedValuePtr TSys::TypeValueRegistry::WrapValue(const std::any& value)
{
    auto typedValue = CreateValue(GetType(std::type_index(value.type())));

    typedValue->SetAny(value);

    return typedValue;
}


void TSys::TypeValueRegistry::SerializeValue(
        const TypedValuePtr& value,
        JsonValue& jsonValue,
        JsonObject& jsonObject
)
{
    value->GetType()->GetSerializer()(
            value, jsonValue, jsonObject
    );
}


void TSys::TypeValueRegistry::DeserializeValue(
        TypedValuePtr& value,
        JsonValue& jsonValue,
        JsonObject& jsonObject
)
{
    value->GetType()->GetDeserializer()(
            value, jsonValue, jsonObject
    );
}


void TSys::TypeValueRegistry::Serialize(
        const TypedValuePtr& value,
        JsonValue& jsonValue,
        JsonObject& jsonObject
)
{
    auto type = GetType(value->GetIndex());

    auto& dict = jsonValue.SetObject();

    dict.AddMember(
            rapidjson::StringRef("Type"),
            rapidjson::Value(rapidjson::kStringType).SetString(
                    rapidjson::StringRef(
                            type->GetName().c_str(),
                            type->GetName().size()
                    ), jsonObject.GetAllocator()
            ), jsonObject.GetAllocator()
    );

    rapidjson::Value serializedValue;

    SerializeValue(value, serializedValue, jsonObject);

    dict.AddMember(rapidjson::StringRef("Value"),
                   serializedValue, jsonObject.GetAllocator());
}


TSys::TypedValuePtr TSys::TypeValueRegistry::Deserialize(
        JsonValue& jsonValue, JsonObject& jsonObject
)
{
    std::string typeName = jsonValue["Type"].GetString();

    auto value = CreateValue(typeName);

    DeserializeValue(value, jsonValue, jsonObject);

    return value;
}


TSys::TypeValueRegistry* TSys::TypeValueRegistry::Get(TSys::InitCallback callback)
{
    if (!instance)
    {
        instance = new TypeValueRegistry();

        if (callback)
        {
            callback(instance);
        }
    }

    return instance;
}

