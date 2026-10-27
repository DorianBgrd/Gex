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
        bool array,
        const ValueCreator& creator,
        const Serializer& serializer,
        const Deserializer& deserializer,
        const Compare& compare,

        const Add& add,
        const Substract& substract,
        const Multiply& multiply,
        const Divide& divide,

        const Insert& insert,
        const Append& append,
        const Erase& pop,
        const Remove& remove,
        const Get& get,
        const Count& count,
        const Clear& clear,

        const InitDefault& initDefault,
        const TSys::FromPython& fromPython,
        const TSys::ToPython& toPython
): _name(name), _index(index), _isArray(array), _creator(creator),
   _serializer(serializer), _deserializer(deserializer),
   _compare(compare), _add(add), _substract(substract),
   _multiply(multiply), _divide(divide),
   _insert(insert), _append(append),
   _erase(pop), _remove(remove),
   _get(get), _count(count), _clear(clear),
   _initDefault(initDefault),
   _fromPython(fromPython),
   _toPython(toPython)
{
    _registry = registry;
}


bool TSys::Type::IsArray() const
{
    return _isArray;
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


bool TSys::Type::IsOfType(const std::type_info& type) const
{
    return std::type_index(type) == _index;
}


bool TSys::Type::IsOfType(const std::any& value) const
{
    return std::type_index(value.type()) == _index;
}


TSys::Converter TSys::Type::GetConverter(const std::type_index& type) const
{
    auto iter = converters.find(type);
    if (iter != converters.end())
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


bool TSys::Type::IsConvertibleTo(const std::type_info& other) const
{
    return IsConvertibleTo(_registry->GetType(other));
}


bool TSys::Type::IsConvertibleFrom(const TypePtr& other) const
{
    return other && (other.get() == this || bool(GetConverter(other->GetIndex())));
}


bool TSys::Type::IsConvertibleFrom(const std::type_info& other) const
{
    return IsConvertibleFrom(_registry->GetType(other));
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


TSys::Add TSys::Type::GetAddOperator() const
{
    return _add;
}


TSys::Substract TSys::Type::GetSubstractOperator() const
{
    return _substract;
}


TSys::Multiply TSys::Type::GetMultiplyOperator() const
{
    return _multiply;
}


TSys::Divide TSys::Type::GetDivideOperator() const
{
    return _divide;
}

TSys::Insert TSys::Type::GetInsertOperator() const
{
    return _insert;
}

TSys::Append TSys::Type::GetAppendOperator() const
{
    return _append;
}

TSys::Remove TSys::Type::GetRemoveOperator() const
{
    return _remove;
}

TSys::Erase TSys::Type::GetEraseOperator() const
{
    return _erase;
}

TSys::Get TSys::Type::GetGetOperator() const
{
    return _get;
}

TSys::Count TSys::Type::GetCountOperator() const
{
    return _count;
}

TSys::Clear TSys::Type::GetClearOperator() const
{
    return _clear;
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


bool TSys::TypedValue::Set(const TSys::TypedValuePtr& value)
{
    auto casted = GetType()->CastFrom(value->GetAny(), _value);

    if (!casted.has_value())
        return false;

    _value = casted;
    return true;
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
    if (!_type->IsOfType(value))
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


bool TSys::TypedValue::operator!=(const TypedValuePtr& other) const
{
    return !GetType()->CompareValues(
            GetAny(), other->GetAny()
    );
}


bool TSys::TypedValue::operator==(const TypedValuePtr& other) const
{
    return GetType()->CompareValues(
            GetAny(), other->GetAny()
    );
}


TSys::TypedValuePtr TSys::TypedValue::CallOperator(
        const Operator& op, const TypedValuePtr& other
) const
{
    if (!op)
    {
        return {};
    }

    auto otherRawAny = other->GetAny();

    std::any otherAny = _type->CastFrom(
            otherRawAny,otherRawAny
    );

    return std::make_shared<TypedValue>(
            _type, op(GetAny(), otherAny)
    );
}


TSys::TypedValuePtr TSys::TypedValue::operator + (const TypedValuePtr& other) const
{
   return CallOperator(_type->GetAddOperator(), other);
}

TSys::TypedValuePtr TSys::TypedValue::operator - (const TypedValuePtr& other) const
{
    return CallOperator(_type->GetSubstractOperator(), other);
}

TSys::TypedValuePtr TSys::TypedValue::operator * (const TypedValuePtr& other) const
{
    return CallOperator(_type->GetMultiplyOperator(), other);
}

TSys::TypedValuePtr TSys::TypedValue::operator / (const TypedValuePtr& other) const
{
    return CallOperator(_type->GetDivideOperator(), other);
}


bool TSys::TypedValue::CallArrayOperator(
        const ArrayEditOperator& op,
        const TypedValuePtr& other
)
{
    if (!op)
    {
        return false;
    }

    auto thisSingleType = _type->GetRegistry()->GetSingleType(_type);
    if (!thisSingleType)
        return false;

    std::any otherAny = other->GetAny();
    thisSingleType->CastFrom(otherAny, otherAny);

    op(_value, otherAny);
    return true;
}

bool TSys::TypedValue::CallArrayIndexValueOperator(
        size_t index, const ArrayIndexValueEditOperator& op,
        const TypedValuePtr& other
)
{
    if (!op)
    {
        return false;
    }

    auto thisSingleType = _type->GetRegistry()->GetSingleType(_type);
    if (!thisSingleType)
        return false;

    std::any otherAny = other->GetAny();
    thisSingleType->CastFrom(otherAny, otherAny);

    op(index, _value, otherAny);
    return true;
}


bool TSys::TypedValue::CallArrayIndexOperator(
        size_t index, const ArrayIndexEditOperator& op
)
{
    if (!op)
    {
        return false;
    }

    op(index, _value);
    return true;
}


bool TSys::TypedValue::Append(const TypedValuePtr& other)
{
    return CallArrayOperator(_type->GetAppendOperator(), other);
}

bool TSys::TypedValue::Insert(size_t index, const TypedValuePtr& other)
{
    return CallArrayIndexValueOperator(index, _type->GetInsertOperator(), other);
}

bool TSys::TypedValue::Remove(size_t index)
{
    return CallArrayIndexOperator(index, _type->GetRemoveOperator());
}

bool TSys::TypedValue::Erase(const TypedValuePtr& other)
{
    return CallArrayOperator(_type->GetEraseOperator(), other);
}

TSys::TypedValuePtr TSys::TypedValue::At(size_t index) const
{
    auto op = _type->GetGetOperator();
    if (!op)
    {
        return {};
    }

    return std::make_shared<TypedValue>(
        _type->GetRegistry()->GetSingleType(_type),
        op(_value, index)
    );
}

TSys::TypedValuePtr TSys::TypedValue::operator[](size_t index) const
{
    return At(index);
}

size_t TSys::TypedValue::Count() const
{
    auto op = _type->GetCountOperator();
    if (!op)
    {
        return {};
    }

    return op(_value);
}


void TSys::TypedValue::Clear()
{
    auto op = _type->GetClearOperator();
    if (!op)
    {
        return;
    }

    return op(_value);
}


bool TSys::TypedValue::IsValid() const
{
    return (_type && _value.has_value());
}


TSys::TypedValue::operator bool() const
{
    return IsValid();
}



TSys::TypedValuePtr TSys::operator + (
        const TSys::TypedValuePtr &value1,
        const TSys::TypedValuePtr &value2
)
{
    bool value1Valid = value1->IsValid();
    bool value2Valid = value1->IsValid();

    if (value1Valid && value2Valid)
    {
        return *value1.get() + value2;
    }
    else if (!value1Valid)
    {
        return value2;
    }
    else
    {
        return value1;
    }
}

TSys::TypedValuePtr TSys::operator - (
        const TSys::TypedValuePtr &value1,
        const TSys::TypedValuePtr &value2
)
{
    bool value1Valid = value1->IsValid();
    bool value2Valid = value2->IsValid();

    if (value1Valid && value2Valid)
    {
        return *value1.get() - value2;
    }
    else if (!value1Valid)
    {
        return value2;
    }
    else
    {
        return value1;
    }
}

TSys::TypedValuePtr TSys::operator * (
        const TSys::TypedValuePtr &value1,
        const TSys::TypedValuePtr &value2
)

{
    bool value1Valid = value1->IsValid();
    bool value2Valid = value2->IsValid();

    if (value1Valid && value2Valid)
    {
        return *value1.get() * value2;
    }
    else if (!value1Valid)
    {
        return value2;
    }
    else
    {
        return value1;
    }
}

TSys::TypedValuePtr TSys::operator / (
        const TSys::TypedValuePtr &value1,
        const TSys::TypedValuePtr &value2
)
{
    bool value1Valid = value1->IsValid();
    bool value2Valid = value2->IsValid();

    if (value1Valid && value2Valid)
    {
        return *value1.get() / value2;
    }
    else if (!value1Valid)
    {
        return value2;
    }
    else
    {
        return value1;
    }
}


bool TSys::operator == (
        const TSys::TypedValuePtr &value1,
        const TSys::TypedValuePtr &value2
)
{
    bool value1Valid = value1->IsValid();
    bool value2Valid = value2->IsValid();

    if (value1Valid && value2Valid)
    {
        return *value1.get() == value2;
    }

    return false;
}


bool TSys::operator != (
        const TSys::TypedValuePtr &value1,
        const TSys::TypedValuePtr &value2
)
{
    bool value1Valid = value1->IsValid();
    bool value2Valid = value2->IsValid();

    if (value1Valid && value2Valid)
    {
        return *value1.get() != value2;
    }

    return false;
}



TSys::TypeValueRegistry* TSys::TypeValueRegistry::instance;



TSys::TypePtr TSys::TypeValueRegistry::RegisterType(
        const std::type_index& typeIndex,
        const std::string& name,
        bool array,
        const ValueCreator& creator,
        const Serializer& serializer,
        const Deserializer& deserializer,
        const FromPython& fromPython,
        const ToPython& toPython,
        const InitDefault& initDefault,
        const Compare& compare,
        const Add& add,
        const Substract& substract,
        const Multiply& multiply,
        const Divide& divide,
        const Insert& insert,
        const Append& append,
        const Erase& erase,
        const Remove& remove,
        const Get& get,
        const Count& count,
        const Clear& clear
) noexcept
{
    auto type = std::make_shared<Type>(
            this, name,
            typeIndex, array,
            creator,
            serializer, deserializer,
            compare,
            add, substract,
            multiply, divide,
            insert, append,
            erase, remove,
            get, count, clear,
            initDefault,
            fromPython,
            toPython
    );

    types[typeIndex] = type;

    typeNames.emplace(name, typeIndex);

    return type;
}


void TSys::TypeValueRegistry::RegisterCtorConverter(
        const std::type_index& source,
        const std::type_index& dest,
        const Converter& converter
) const
{
    auto sourceType = GetType(source);
    auto destType = GetType(dest);
    if (!destType || !sourceType)
    {
        return;
    }

    destType->RegisterConverter(sourceType->GetIndex(), converter);

}


TSys::TypePtr TSys::TypeValueRegistry::GetArrayType(
        const TypePtr& singleType
) const
{
    auto iter = typeToArray.find(
            singleType->GetIndex()
    );

    if (iter == typeToArray.end())
        return {};

    return GetType(iter->second);
}


TSys::TypePtr TSys::TypeValueRegistry::GetSingleType(
        const TypePtr& arrayType
) const
{
    for (const auto& typePair : typeToArray)
    {
        if (typePair.second == arrayType->GetIndex())
        {
            return GetType(typePair.first);
        }
    }

    return {};
}


bool TSys::TypeValueRegistry::HasArrayType(const TypePtr& type) const
{
    return typeToArray.find(type->GetIndex()) != typeToArray.end();
}


bool TSys::TypeValueRegistry::HasSingleType(const TypePtr& type) const
{
    return static_cast<bool>(GetSingleType(type));
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


std::vector<std::string> TSys::TypeValueRegistry::GetTypeNames() const
{
    std::vector<std::string> computedTypeNames;

    for (const auto& type : typeNames)
    {
        computedTypeNames.push_back(type.first);
    }

    return computedTypeNames;
}

void TSys::TypeValueRegistry::RegisterSingleToArrayTypes(
        const std::type_info &singleType,
        const std::type_info &arrayType
)
{
    typeToArray.emplace(singleType, arrayType);

}


TSys::TypeValueRegistry* TSys::TypeValueRegistry::GetInstance(TSys::InitCallback callback)
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

