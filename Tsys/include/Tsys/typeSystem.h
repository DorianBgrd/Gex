#ifndef GEX_TYPESYSTEM_H
#define GEX_TYPESYSTEM_H

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
    typedef std::function<std::any(const std::any&, const std::any&)> Operator;
    typedef Operator Add;
    typedef Operator Substract;
    typedef Operator Multiply;
    typedef Operator Divide;

    typedef std::function<void(size_t index, std::any&, const std::any&)> ArrayIndexValueEditOperator;
    typedef std::function<void(size_t index, std::any&)> ArrayIndexEditOperator;
    typedef std::function<void(size_t index, std::any&, const std::any&)> Insert;

    typedef std::function<void(size_t index, std::any&)> Remove;

    typedef std::function<void(std::any&, const std::any&)> ArrayEditOperator;
    typedef ArrayEditOperator Append;
    typedef ArrayEditOperator Erase;

    typedef std::function<std::any(const std::any&, size_t index)> Get;
    typedef std::function<size_t(const std::any&)> Count;
    typedef std::function<void(std::any&)> Clear;

    typedef std::function<std::any()> InitDefault;
    typedef std::function<std::any(const pybind11::object&)> FromPython;
    typedef std::function<pybind11::object(const std::any&)> ToPython;

    typedef std::function<std::any(const std::any&, const std::any&)> Converter;

    template<class Source, class Dest>
    static std::any CtorConvert(const std::any& value, const std::any& current)
    {
        return std::make_any<Dest>(Dest(std::any_cast<Source>(value)));
    }

    TSYS_API
    TypedValuePtr DefaultCreator(const TypePtr& type);

    template<class T>
    std::any UseDefaultCtor()
    {
        return std::make_any<T>(T());
    }

    template<class T>
    std::any DefaultAddOp(const std::any& v1, const std::any& v2)
    {
        T v1Value = std::any_cast<T>(v1);
        T v2Value = std::any_cast<T>(v2);
        return std::make_any<T>(v1Value + v2Value);
    }

    template<class T>
    std::any DefaultSubstractOp(const std::any& v1, const std::any& v2)
    {
        return std::any_cast<T>(v1) - std::any_cast<T>(v2);
    }

    template<class T>
    std::any DefaultMultiplyOp(const std::any& v1, const std::any& v2)
    {
        return std::any_cast<T>(v1) * std::any_cast<T>(v2);
    }

    template<class T>
    std::any DefaultDivideOp(const std::any& v1, const std::any& v2)
    {
        return std::any_cast<T>(v1) / std::any_cast<T>(v2);
    }

    template<class T>
    void DefaultInsert(size_t index, std::any& v1, const std::any& v2)
    {
        auto* arrayValue = std::any_cast<std::vector<T>>(&v1);
        auto singleValue = std::any_cast<T>(v2);

        auto iterator = arrayValue->begin();
        std::advance(iterator, index);

        arrayValue->insert(iterator, singleValue);
    }

    template<class T>
    void DefaultAppend(std::any& v1, const std::any& v2)
    {
        std::vector<T>* arrayValue = std::any_cast<std::vector<T>>(&v1);
        T singleValue = std::any_cast<T>(v2);
        arrayValue->push_back(singleValue);
    }

    template<class T>
    void DefaultErase(std::any& v1, const std::any& v2)
    {
        auto* arrayValue = std::any_cast<std::vector<T>>(&v1);
        auto singleValue = std::any_cast<T>(v2);

        auto iterator = std::find(arrayValue->begin(), arrayValue->end(), singleValue);
        if (iterator == arrayValue->end())
            return;

        arrayValue->erase(iterator);
    }

    template<class T>
    void DefaultRemove(size_t index, std::any& v1)
    {
        auto arrayValue = std::any_cast<std::vector<T>>(&v1);

        auto iterator = arrayValue->begin();
        std::advance(iterator, index);

        arrayValue->erase(iterator);
    }

    template<class T>
    std::any DefaultGet(const std::any& v1, size_t index)
    {
        const std::vector<T>* arrayValue = std::any_cast<std::vector<T>>(&v1);

        T singleValue = arrayValue->at(index);

        return std::make_any<T>(singleValue);
    }

    template<class T>
    size_t DefaultCount(const std::any& v1)
    {
        return std::any_cast<std::vector<T>>(&v1)->size();
    }

    template<class T>
    void DefaultClear(std::any& v1)
    {
        std::any_cast<std::vector<T>>(&v1)->clear();
    }


    template<class T>
    bool DefaultEqualOp(const std::any& v1, const std::any& v2)
    {
        return std::any_cast<T>(v1) == std::any_cast<T>(v2);
    }

    TSYS_API
    std::any UnuseFromPython(const pybind11::object&);

    TSYS_API
    pybind11::object UnuseToPython(const std::any&);

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
    Converter  MakeConverter() {
        return CtorConvert<Source, Dest>;
    }


    class TSYS_API TypeValueRegistry;


    class TSYS_API Type
    {
        TypeValueRegistry* _registry;

        std::string _name;
        std::type_index _index;

        ValueCreator _creator;
        Serializer _serializer;
        Deserializer _deserializer;
        Compare _compare;

        Add _add;
        Substract _substract;
        Multiply _multiply;
        Divide _divide;

        Append _append;
        Remove _remove;
        Insert _insert;
        Erase _erase;
        Get _get;
        Count _count;
        Clear _clear;

        InitDefault  _initDefault;
        FromPython _fromPython;
        ToPython _toPython;

        std::unordered_map<std::type_index, Converter> converters;

    public:
        Type(TypeValueRegistry* registry,
             const std::string& name, const std::type_index& index,
             const ValueCreator& creator, const Serializer& serializer,
             const Deserializer& deserializer, const Compare& compare,
             const Add& add, const Substract& substract,
             const Multiply& multiply, const Divide& divide,
             const Insert& insert, const Append& append,
             const Erase& pop, const Remove& remove,
             const Get& get, const Count& count, const Clear& clear,
             const InitDefault& initDefault, const FromPython& fromPython,
             const ToPython& toPython);

        bool operator==(const std::any& other) const;

        bool operator==(const Type& other) const;

        bool operator==(const std::type_index& other) const;

        bool IsOfType(const std::type_info& type) const;

        template<class T>
        bool IsOfType() const
        {
            return IsOfType(typeid(T));
        }

        template<class T>
        bool IsOfType(const T& v) const
        {
            return IsOfType<T>();
        }

        bool IsOfType(const std::any& value) const;

        Converter GetConverter(const std::type_index& type) const;

        template<class T>
        Converter GetConverter() const
        {
            return GetConverter(std::type_index(typeid(T)));
        }

        bool IsConvertibleTo(const TypePtr& other) const;

        bool IsConvertibleTo(const std::type_info& other) const;

        template<class T>
        bool IsConvertibleTo() const
        {
            return IsConvertibleTo(typeid(T));
        }

        bool IsConvertibleFrom(const TypePtr& other) const;

        bool IsConvertibleFrom(const std::type_info& other) const;

        template<class T>
        bool IsConvertibleFrom() const
        {
            return IsConvertibleFrom(typeid(T));
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
            if (!IsOfType(typeid(T)))
            {
                if (auto destType = GetRegistry()->GetType(typeid(T)))
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

    public:

        Add GetAddOperator() const;

        Substract GetSubstractOperator() const;

        Multiply GetMultiplyOperator() const;

        Divide GetDivideOperator() const;

        Insert GetInsertOperator() const;

        Append GetAppendOperator() const;

        Remove GetRemoveOperator() const;

        Erase GetEraseOperator() const;

        Get GetGetOperator() const;

        Count GetCountOperator() const;

        Clear GetClearOperator() const;

        std::any InitDefaultValue() const;

        std::any FromPython(const pybind11::object& pythonValue);

        pybind11::object ToPython(const std::any& cppValue);
    };


    class TSYS_API TypedValue: public std::enable_shared_from_this<TypedValue>
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

        bool Set(const TSys::TypedValuePtr& value);

        pybind11::object Get() const;

        bool Set(const pybind11::object& pythonObject);

        std::any GetAny() const;

        bool SetAny(const std::any& value);

        std::type_index GetIndex() const;

        bool IsOfType(const std::type_index& otherIndex) const;

        bool IsOfType(const TypedValue& other) const;

        TypedValue& operator=(const TypedValue& other);

        bool operator==(const TypedValuePtr& other) const;

    protected:
        TypedValuePtr CallOperator(const Operator& op, const TypedValuePtr& other) const;

    public:

        TypedValuePtr operator + (const TypedValuePtr& other) const;

        TypedValuePtr operator - (const TypedValuePtr& other) const;

        TypedValuePtr operator * (const TypedValuePtr& other) const;

        TypedValuePtr operator / (const TypedValuePtr& other) const;

    protected:
        bool CallArrayOperator(
                const ArrayEditOperator& op,
                const TypedValuePtr& other
        );

        bool CallArrayIndexValueOperator(
                size_t index, const ArrayIndexValueEditOperator& op,
                const TypedValuePtr& other
        );

        bool CallArrayIndexOperator(
                size_t index, const ArrayIndexEditOperator& op
        );

    public:

        bool Append(const TypedValuePtr& other);

        template<class T>
        bool Append(const T& value)
        {
            return Append(_type->GetRegistry()->WrapValue(value));
        }

        bool Erase(const TypedValuePtr& other);

        template<class T>
        bool Erase(const T& value)
        {
            return Erase(_type->GetRegistry()->WrapValue(value));
        }

        bool Insert(size_t index, const TypedValuePtr& other);

        template<class T>
        bool Insert(size_t index, const T& value)
        {
            return Insert(index, _type->GetRegistry()->WrapValue(value));
        }

        bool Remove(size_t index);

        TypedValuePtr At(size_t index) const;

        TypedValuePtr operator[](size_t index) const;

        size_t Count() const;

        void Clear();

    public:

        bool IsValid() const;

        operator bool() const;
    };

    typedef std::shared_ptr<TypedValue> TypedValuePtr;


    TSYS_API
    TSys::TypedValuePtr operator+(
            const TSys::TypedValuePtr &v,
            const TSys::TypedValuePtr &other
    );

    TSYS_API
    TSys::TypedValuePtr operator-(
            const TSys::TypedValuePtr &v,
            const TSys::TypedValuePtr &other
    );

    TSYS_API
    TSys::TypedValuePtr operator*(
            const TSys::TypedValuePtr &v,
            const TSys::TypedValuePtr &other
    );

    TSYS_API
    TSys::TypedValuePtr operator/(
            const TSys::TypedValuePtr &v,
            const TSys::TypedValuePtr &other
    );

    template<class T>
    TypedValuePtr CreateNew(const TypePtr type)
    {
        return std::make_shared<TypedValue>(type);
    }


    typedef void(*InitCallback)(TypeValueRegistry*);


    class TSYS_API TypeValueRegistry
    {
        static TypeValueRegistry* instance;

        std::unordered_map<std::string, std::type_index> typeNames;
        std::unordered_map<std::type_index, TypePtr> types;
        std::unordered_map<std::type_index, std::type_index> typeToArray;

    public:
        void RegisterType(
                const std::type_index& typeIndex,
                const std::string& name,
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
        ) noexcept;

        template<class T>
        void RegisterType(
                const std::string& name,
                const ValueCreator& creator,
                const Serializer& serializer,
                const Deserializer& deserializer,
                const FromPython& fromPython=UnuseFromPython,
                const ToPython& toPython=UnuseToPython,
                const InitDefault& initDefault=UseDefaultCtor<T>,
                const Compare& compare=DefaultEqualOp<T>,
                const Add& add=DefaultAddOp<T>,
                const Substract& substract=DefaultSubstractOp<T>,
                const Multiply& multiply=DefaultMultiplyOp<T>,
                const Divide& divide=DefaultDivideOp<T>
        ) noexcept
        {
            return RegisterType(
                    std::type_index(typeid(T)),
                    name, creator,
                    serializer, deserializer,
                    fromPython, toPython,
                    initDefault, compare,
                    add, substract,
                    multiply, divide,
                    nullptr, nullptr,
                    nullptr, nullptr,
                    nullptr, nullptr,
                    nullptr
            );
        }

    protected:
        void RegisterSingleToArrayTypes(
            const std::type_info& singleType,
            const std::type_info& arrayType
        );

        template<class Single, class Array>
        void RegisterSingleToArrayTypes()
        {
            RegisterSingleToArrayTypes(
                    typeid(Single), typeid(Array)
            );
        }

    public:
        template<class T>
        void RegisterArrayType(
                const std::string& name,
                const ValueCreator& creator,
                const Serializer& serializer,
                const Deserializer& deserializer,
                const FromPython& fromPython=UnuseFromPython,
                const ToPython& toPython=UnuseToPython,
                const InitDefault& initDefault=UseDefaultCtor<std::vector<T>>,
                const Compare& compare=DefaultEqualOp<std::vector<T>>,
                const Insert& insert=DefaultInsert<T>,
                const Append& append=DefaultAppend<T>,
                const Erase& erase=DefaultErase<T>,
                const Remove& remove=DefaultRemove<T>,
                const Get& get=DefaultGet<T>,
                const Count& count=DefaultCount<T>,
                const Clear& clear=DefaultClear<T>
        )
        {
            auto arrayTypeIndex = std::type_index(typeid(std::vector<T>));
            RegisterType(
                    arrayTypeIndex, name, creator,
                    serializer, deserializer,
                    fromPython, toPython,
                    initDefault, compare,
                    nullptr, nullptr,
                    nullptr, nullptr,
                    insert, append,
                    erase, remove,
                    get, count, clear
            );

            RegisterSingleToArrayTypes<T, std::vector<T>>();
        }


        TypePtr GetArrayType(const TypePtr& singleType) const;


        TypePtr GetSingleType(const TypePtr& arrayType) const;


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
            return GetType(typeid(T));
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

        std::vector<std::string> GetTypeNames() const;

    public:
        static TypeValueRegistry* GetInstance(InitCallback callback=nullptr);
    };


    template<class T>
    class ArraySerializer
    {
        const std::type_info& _tplType = typeid(T);
        TypePtr _valueType;

    public:
        ArraySerializer(const TypeValueRegistry* registry)
        {
            _valueType = registry->GetType(_tplType);
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
        const std::type_info& _tplType = typeid(T);
        TypePtr _valueType;

    public:
        ArrayDeserializer(
                const TypeValueRegistry* registry
        )
        {
            _valueType = registry->GetType(_tplType);
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


}


#endif //GEX_TYPESYSTEM_H
