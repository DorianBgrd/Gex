#ifndef GEX_TYPESYSTEM_H
#define GEX_TYPESYSTEM_H

#pragma once

//#ifndef BOOST_PYTHON_STATIC_LIB
//#define BOOST_PYTHON_STATIC_LIB
//#endif
#include <map>
#include <vector>
#include <set>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <any>
#include <functional>

#include "rapidjson/document.h"
#include "pybind11/pybind11.h"

#include "api.h"


namespace TSys
{

    typedef rapidjson::Value JsonValue;
    typedef rapidjson::Document JsonObject;


    class Type;
    typedef std::shared_ptr<Type> TypePtr;

    class TypedValue;
    typedef std::shared_ptr<TypedValue> TypedValuePtr;

    typedef std::function<TypedValuePtr(const TypePtr&)> ValueCreator;
    typedef std::function<void(const TypedValuePtr&, JsonValue&, JsonObject&)> Serializer;
    typedef std::function<void(TypedValuePtr&, JsonValue&, JsonObject&)> Deserializer;
    typedef std::function<bool(const std::any&, const std::any&)> Compare;
    typedef std::function<std::any()> InitDefault;
    typedef std::function<std::any(const pybind11::object&)> FromPython;
    typedef std::function<pybind11::object(const std::any&)> ToPython;

    typedef std::function<std::any(const std::any&, const std::any&)> Converter;

    template<class Source, class Dest>
    std::any CtorConvert(const std::any& value, const std::any& current)
    {
        return std::make_any<Source>(Source(std::any_cast<Dest>(value)));
    }

    TypedValuePtr DefaultCreator(const TypePtr& type)
    {
        return std::make_shared<TypedValue>(type);
    }

    template<class T>
    std::any UseDefaultCtor()
    {
        return std::make_any<T>(T());
    }

    template<class T>
    bool UseEqualOp(const std::any& v1, const std::any& v2)
    {
        return std::any_cast<T>(v1) == std::any_cast<T>(v2);
    }

    std::any UnuseFromPython(const pybind11::object&)
    {
        return {};
    }

    pybind11::object UnuseToPython(const std::any&)
    {
        return {};
    }

    template<class T>
    std::any DefaultPyBindToAnyCast(const pybind11::object& obj)
    {
        return std::make_any<T>(obj.cast<T>());
    }


    template<class T>
    pybind11::object DefaultAnyToPyBindCast(const std::any& obj)
    {
        return pybind11::cast(std::any_cast<T>(obj));
    }


    template<class Source, class Dest>
    Converter MakeConverter() {
        return CtorConvert<Source, Dest>;
    }


    class TypeValueRegistry;


    class Type
    {
        TypeValueRegistry* _registry;

        std::string _name;
        std::type_index _index;

        ValueCreator _creator;
        Serializer _serializer;
        Deserializer _deserializer;
        Compare _compare;
        InitDefault  _initDefault;
        FromPython _fromPython;
        ToPython _toPython;

        std::unordered_map<std::type_index, Converter> converters;

    public:
        Type(TypeValueRegistry* registry,
             const std::string& name, const std::type_index& index,
             const ValueCreator& creator, const Serializer& serializer,
             const Deserializer& deserializer, const Compare& compare,
             const InitDefault& initDefault, const FromPython& fromPython,
             const ToPython& toPython);

        bool operator==(const std::any& other) const;

        bool operator==(const Type& other) const;

        bool operator==(const std::type_index& other) const;

        template<class T>
        bool IsOfType() const
        {
            return std::type_index(typeid(T)) == _index;
        }

        template<class T>
        bool IsOfType(const T& v) const
        {
            return IsOfType<T>();
        }

        bool IsOfType(const std::any& value) const;

        Converter GetConverter(std::type_index type) const;

        template<class T>
        Converter GetConverter() const
        {
            return GetConverter(std::type_index(typeid(T)));
        }

        bool IsConvertibleTo(const TypePtr& other) const;

        template<class T>
        bool IsConvertibleTo() const
        {
            auto destinationType = _registry->GetType<T>();
            if (!destinationType)
                return false;

            return IsConvertibleTo(destinationType);
        }

        bool IsConvertibleFrom(const TypePtr& other) const;

        template<class T>
        bool IsConvertibleFrom() const
        {
            return IsConvertibleFrom(_registry->GetType<T>());
        }

        void RegisterConverter(const std::type_index& sourceType,
                               const Converter& converter);

        template<class T>
        void RegisterConverter(const Converter& converter)
        {
            RegisterConverter(std::type_index(typeid(T)),
                              converter);
        }

        TypeValueRegistry* GetRegistry() const;

        std::type_index GetIndex() const;

        std::string GetName() const;

        ValueCreator GetCreator() const;

        Serializer GetSerializer() const;

        Deserializer GetDeserializer() const;

        template<class T>
        T CastTo(const std::any& value, const std::any& current)
        {
            if (!IsOfType<T>())
            {
                if (auto destType = GetRegistry()->GetType<T>())
                {
                    if (auto cvrtr = destType->GetConverter(GetIndex()))
                    {
                        return std::any_cast<T>(cvrtr(value, current));
                    }
                }
            }

            return std::any_cast<T>(value);
        }

        std::any CastFrom(const std::any& source, const std::any& current) const
        {
            if (IsOfType(source))
            {
                return source;
            }

            if (auto cvrtr = GetConverter(source.type()))
            {
                return cvrtr(source, current);
            }

            return {};
        }

        bool CompareValues(const TypedValuePtr& value1,
                           const TypedValuePtr& value2) const;

        bool CompareValues(const std::any& value1,
                           const std::any& value2) const;

        std::any InitDefaultValue() const;

        std::any FromPython(const pybind11::object& pythonValue);

        pybind11::object ToPython(const std::any& cppValue);
    };


    class TypedValue: public std::enable_shared_from_this<TypedValue>
    {
        const TypePtr _type;
        std::any _value;

    public:
        TypedValue() = default;

        explicit
        TypedValue(const TypePtr& type);

        virtual ~TypedValue() = default;

        explicit
        TypedValue(const TypePtr& type, const std::any& value);

        TypedValue(const TypedValue& other);

        const TypePtr GetType() const;

        template <class T>
        T CastTo() const
        {
            return GetType()->CastTo<T>(_value, _value);
        }

        template<class T>
        std::any CastFrom(T value)
        {
            return GetType()->CastFrom(value, _value);
        }

        template<class T>
        T* GetPtr() const {
            return CastTo<T*>();
        }

        template<class T>
        T Get() const {
            return CastTo<T>();
        }

        template<class T>
        T& GetRef() const {
            return CastTo<T&>();
        }

        template<class T>
        const T* GetConstPtr() const {
            return CastTo<const T*>();
        }

        template<class T>
        bool SetRef(T& value) {
            std::any anyValue = CastFrom(value);
            if (!anyValue.has_value())
                return false;

            _value = anyValue;
            return true;
        }

        template<class T>
        bool Set(T value) {
            std::any anyValue = CastFrom(value);
            if (!anyValue.has_value())
                return false;

            _value = anyValue;
            return true;
        }

        template<class T>
        bool SetConstPtr(const T* value)
        {
            std::any anyValue = CastFrom(value);
            if (!anyValue.has_value())
                return false;

            _value = anyValue;
            return true;
        }

        pybind11::object Get() const;

        bool Set(const pybind11::object& pythonObject);

        std::any GetAny() const;

        bool SetAny(const std::any& value);

        std::type_index GetIndex() const;

        bool IsOfType(const std::type_index& otherIndex) const;

        bool IsOfType(const TypedValue& other) const;

        TypedValue& operator=(const TypedValue& other);

        bool operator==(const TypedValuePtr& other) const;

        bool IsValid() const;

        operator bool() const;
    };


    typedef std::shared_ptr<TypedValue> TypedValuePtr;


    template<class T>
    TypedValuePtr CreateNew(const TypePtr type)
    {
        return std::make_shared<TypedValue>(type);
    }


    template<class T>
    class ArraySerializer
    {
        TypePtr _valueType;

    public:
        ArraySerializer(const TypeValueRegistry* registry)
        {
            _valueType = registry->GetType<T>();
        }

        void operator()(
                const TSys::TypedValuePtr& value,
                rapidjson::Value& obj,
                rapidjson::Document& json
        ) const
        {
            auto& arrayValue = obj.SetArray();

            for (auto item : value->Get<std::vector<T>>())
            {
                rapidjson::Value serializedTypeValue;

                _valueType->GetSerializer()(
                        _valueType->GetRegistry()->WrapValue(item),
                        serializedTypeValue, json
                );

                arrayValue.PushBack(
                        serializedTypeValue,
                        json.GetAllocator()
                );
            }
        }
    };


    template<class T>
    class ArrayDeserializer
    {
        TypePtr _valueType;

    public:
        ArrayDeserializer(
                const TypeValueRegistry* registry
        )
        {
            _valueType = registry->GetType<T>();
        }

        void operator()(
                const TSys::TypedValuePtr& value,
                rapidjson::Value& obj,
                rapidjson::Document& json
        ) const
        {
            std::vector<T> resultingArray;

            for (unsigned int i = 0; i < obj.Size(); i++)
            {
                TypedValuePtr indexValue =_valueType->GetRegistry()->CreateValue(_valueType);

                _valueType->GetDeserializer()(indexValue, obj[i], json);

                T deserializedValue = indexValue->Get<T>();

                resultingArray.push_back(deserializedValue);
            }

            value->Set<std::vector<T>>(resultingArray);
        }
    };


    typedef void(*InitCallback)(TypeValueRegistry*);


    class TypeValueRegistry
    {
        static TypeValueRegistry* instance;

        std::unordered_map<std::string, std::type_index> typeNames;
        std::unordered_map<std::type_index, TypePtr> types;

    public:
        void RegisterType(
                const std::type_info& typeInfo,
                const std::string& name,
                const ValueCreator& creator,
                const Serializer& serializer,
                const Deserializer& deserializer,
                const Compare& compare,
                const InitDefault& initDefault,
                const FromPython& fromPython=UnuseFromPython,
                const ToPython& toPython=UnuseToPython
        ) noexcept;

        template<class T>
        void RegisterType(
                const std::string& name,
                const ValueCreator& creator,
                const Serializer& serializer,
                const Deserializer& deserializer,
                const Compare& compare=UseEqualOp<T>,
                const InitDefault& initDefault=UseDefaultCtor<T>,
                const FromPython& fromPython=UnuseFromPython,
                const ToPython& toPython=UnuseToPython
        ) noexcept
        {
            return RegisterType(
                    typeid(T), name, creator,
                    serializer, deserializer,
                    compare, initDefault
            );
        }

        template<class Source, class Dest>
        void RegisterCtorConverter(const Converter& converter) const
        {
            RegisterCtorConverter(typeid(Source), typeid(Dest), converter);
        }

        void RegisterCtorConverter(const std::type_index& source,
                                   const std::type_index& dest,
                                   const Converter& converter) const;

        template<class Source, class Dest>
        void RegisterDefaultCtorConverter() const
        {
            return RegisterCtorConverter<Source, Dest>(
                    CtorConvert<Source, Dest>
            );
        }

        TypePtr GetType(const std::type_index& index) const;

        TypePtr GetType(const std::string& name) const;

        TypePtr GetType(const std::any& value) const;

        template<class T>
        TypePtr GetType() const
        {
            return GetType(std::type_index(typeid(T)));
        }

        template<class T>
        TypedValuePtr CreateValue() const
        {
            return CreateValue(std::type_index(typeid(T)));
        }

        TypedValuePtr CreateValue(const std::string& name) const;

        TypedValuePtr CreateValue(const std::type_index& index) const;

        TypedValuePtr CreateValue(const TypePtr& index) const;

        TypedValuePtr WrapValue(const std::any& value);

        template<class T>
        TypedValuePtr WrapValue(T value)
        {
            return WrapValue(std::make_any<T>(value));
        }

        void SerializeValue(const TypedValuePtr& value, JsonValue& jsonValue, JsonObject& jsonObject);

        void DeserializeValue(TypedValuePtr& value, JsonValue& jsonValue, JsonObject& jsonObject);

        void Serialize(const TypedValuePtr& value, JsonValue& jsonValue, JsonObject& jsonObject);

        TypedValuePtr Deserialize(JsonValue& jsonValue, JsonObject& jsonObject);

    public:
        static TypeValueRegistry* Get(InitCallback callback=nullptr);
    };
}

#endif //GEX_TYPESYSTEM_H
