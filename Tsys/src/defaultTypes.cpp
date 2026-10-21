#include <Tsys/defaultTypes.h>
#include <Tsys/typeSystem.h>

#include <pybind11/pybind11.h>

#include <any>
#include <string>
#include <vector>
#include "rapidjson/document.h"


TSys::Enum::Enum(const std::vector<EnumValue>& values)
{
    _enumValues = values;
}


TSys::Enum::Enum(const std::vector<std::string>& values)
{
    unsigned int index = 0;

    for (const auto& value : values)
    {
        _enumValues.emplace_back(value, index);
    }
}


TSys::Enum::Enum(const std::unordered_map<std::string, unsigned int>& values)
{
    for (const auto& value : values)
    {
        _enumValues.emplace_back(value);
    }
}


void TSys::Enum::AddValue(const EnumValue& value)
{
    _enumValues.push_back(value);
}


unsigned int TSys::Enum::GetIndex(
        const std::string& stringValue
) const
{
    for (const EnumValue& val : _enumValues)
    {
        if (val.first == stringValue)
            return val.second;
    }

    return {};
}


std::string TSys::Enum::GetString(unsigned int index) const
{
    for (const EnumValue& val : _enumValues)
    {
        if (val.second == index)
            return val.first;
    }

    return {};
}


std::vector<std::string> TSys::Enum::GetStrings() const
{
    std::vector<std::string> values;

    for (const EnumValue& val : _enumValues)
    {
        values.push_back(val.first);
    }

    return values;
}


std::vector<unsigned int> TSys::Enum::GetIndices() const
{
    std::vector<unsigned int> values;

    for (const EnumValue& val : _enumValues)
    {
        values.push_back(val.second);
    }

    return values;
}


bool TSys::Enum::Set(const std::string& stringValue)
{
    for (const EnumValue& val : _enumValues)
    {
        if (val.first == stringValue)
        {
            _current = val;
            return true;
        }
    }

    return false;
}


bool TSys::Enum::Set(unsigned int indexValue)
{
    for (const EnumValue& val : _enumValues)
    {
        if (val.second == indexValue)
        {
            _current = val;
            return true;
        }
    }

    return false;
}


std::string TSys::Enum::GetCurrentString() const
{
    return _current.first;
}

unsigned int TSys::Enum::GetCurrentIndex() const
{
    return _current.second;
}


TSys::Enum::operator unsigned int() const
{
    return GetCurrentIndex();
}


TSys::Enum::operator std::string() const
{
    return GetCurrentString();
}


TSys::Enum& TSys::Enum::operator=(const Enum& other)
{
    _enumValues = other._enumValues;
    _current = other._current;

    return *this;
}


bool TSys::Enum::operator==(const Enum& other) const
{
    return _current == other._current;
}



void TSys::RegisterDefaultTypes(TSys::TypeValueRegistry* registry)
{
    auto intSerializer = [](
            const TSys::TypedValuePtr& value, rapidjson::Value& obj,
            rapidjson::Document& json
    ) {obj.SetInt(value->Get<int>());};

    auto intDeserializer = [](
            TSys::TypedValuePtr& value, rapidjson::Value& obj,
            rapidjson::Document& json
    ) {value->Set<int>(obj.GetInt());};

    registry->RegisterType<int>(
            "int",
            DefaultCreator,
            intSerializer,
            intDeserializer,
            DefaultPyBindToAnyCast<int>,
            DefaultAnyToPyBindCast<int>
    );


    auto floatSerializer = [](
            const TSys::TypedValuePtr& value, rapidjson::Value& obj,
            rapidjson::Document& json
    ) {obj.SetFloat(value->Get<float>());};

    auto floatDeserializer = [](
            TSys::TypedValuePtr& value, rapidjson::Value& obj,
            rapidjson::Document& json
    ) {value->Set<float>(obj.GetFloat());};

    registry->RegisterType<float>(
            "float",
            DefaultCreator,
            floatSerializer,
            floatDeserializer,
            DefaultPyBindToAnyCast<float>,
            DefaultAnyToPyBindCast<float>
    );


    auto doubleSerializer = [](
            const TSys::TypedValuePtr& value, rapidjson::Value& obj,
            rapidjson::Document& json
    ) {obj.SetDouble(value->Get<double>());};

    auto doubleDeserializer = [](
            TSys::TypedValuePtr& value, rapidjson::Value& obj,
            rapidjson::Document& json
    ) {value->Set<double>(obj.GetDouble());};

    registry->RegisterType<double>(
            "double",
            DefaultCreator,
            doubleSerializer,
            doubleDeserializer,
            DefaultPyBindToAnyCast<double>,
            DefaultAnyToPyBindCast<double>
    );


    auto stringSerializer = [](
            const TSys::TypedValuePtr& value, rapidjson::Value& obj,
            rapidjson::Document& json
    ) {auto str = value->Get<std::string>();obj.SetString(str.c_str(), str.size());};

    auto stringDeserializer = [](
            TSys::TypedValuePtr& value, rapidjson::Value& obj,
            rapidjson::Document& json
    ) {value->Set<std::string>(obj.GetString());};

    registry->RegisterType<std::string>(
            "string",
            DefaultCreator,
            stringSerializer,
            stringDeserializer,
            DefaultPyBindToAnyCast<std::string>,
            DefaultAnyToPyBindCast<std::string>,
            UseDefaultCtor<std::string>,
            DefaultEqualOp<std::string>,
            DefaultAddOp<std::string>,
            nullptr,
            nullptr,
            nullptr
    );


    auto enumSerializer = [](
            const TSys::TypedValuePtr& value, rapidjson::Value& obj,
            rapidjson::Document& json
    ) {auto en = value->Get<Enum>(); auto crt = en.GetCurrentString();
             obj.SetString(crt.c_str(), crt.size());};

    auto enumDeserializer = [](
            TSys::TypedValuePtr& value, rapidjson::Value& obj,
            rapidjson::Document& json
    ) {value->Get<Enum>().Set(obj.GetString());};


    registry->RegisterType<std::string>(
            "enum",
            DefaultCreator,
            enumSerializer,
            enumDeserializer,
            DefaultPyBindToAnyCast<Enum>,
            DefaultAnyToPyBindCast<Enum>,
            UseDefaultCtor<Enum>,
            DefaultEqualOp<Enum>,
            nullptr,
            nullptr,
            nullptr,
            nullptr
    );



    registry->RegisterArrayType<int>(
            "intArray", DefaultCreator,
            TSys::ArraySerializer<int>(registry),
            TSys::ArrayDeserializer<int>(registry),
            DefaultPyBindToAnyCast<std::vector<int>>,
            DefaultAnyToPyBindCast<std::vector<int>>
    );


    registry->RegisterArrayType<float>(
            "floatArray",
            DefaultCreator,
            ArraySerializer<float>(registry),
            ArrayDeserializer<float>(registry),
            DefaultPyBindToAnyCast<std::vector<float>>,
            DefaultAnyToPyBindCast<std::vector<float>>
    );


    registry->RegisterArrayType<double>(
            "doubleArray",
            DefaultCreator,
            ArraySerializer<double>(registry),
            ArrayDeserializer<double>(registry),
            DefaultPyBindToAnyCast<std::vector<double>>,
            DefaultAnyToPyBindCast<std::vector<double>>
    );


    registry->RegisterArrayType<std::string>(
            "stringArray",
            DefaultCreator,
            ArraySerializer<std::string>(registry),
            ArrayDeserializer<std::string>(registry),
            DefaultPyBindToAnyCast<std::vector<std::string>>,
            DefaultAnyToPyBindCast<std::vector<std::string>>
    );


    registry->RegisterDefaultCtorConverter<int, float>();
    registry->RegisterDefaultCtorConverter<int, double>();

    registry->RegisterDefaultCtorConverter<float, int>();
    registry->RegisterDefaultCtorConverter<float, double>();

    registry->RegisterDefaultCtorConverter<double, int>();
    registry->RegisterDefaultCtorConverter<double, float>();

    registry->RegisterCtorConverter<std::string, int>(
        [](const std::any& value, const std::any&)
        {
            try
            {
                return std::make_any<int>(
                    std::stoi(
                        std::any_cast<std::string>(value)
                    )
                );
            }
            catch (std::invalid_argument&)
            {
                return std::make_any<int>(0);
            }
        }
    );
    registry->RegisterCtorConverter<std::string, float>(
            [](const std::any& value, const std::any&)
            {
                try
                {
                    return std::make_any<float>(
                            std::stof(
                                    std::any_cast<std::string>(value)
                            )
                    );
                }
                catch (std::invalid_argument&)
                {
                    return std::make_any<float>(0.0f);
                }
            }
    );
    registry->RegisterCtorConverter<std::string, double>(
            [](const std::any& value, const std::any&)
            {
                try
                {
                    return std::make_any<double>(
                            std::stoi(
                                    std::any_cast<std::string>(value)
                            )
                    );
                }
                catch (std::invalid_argument&)
                {
                    return std::make_any<double>(0.0);
                }
            }
    );

    registry->RegisterCtorConverter<int, std::string>(
            [](const std::any& value, const std::any&)
            {
                return std::to_string(std::any_cast<int>(value));
            }
    );

    registry->RegisterCtorConverter<float, std::string>(
            [](const std::any& value, const std::any&)
            {
                return std::to_string(std::any_cast<float>(value));
            }
    );

    registry->RegisterCtorConverter<double, std::string>(
            [](const std::any& value, const std::any&)
            {
                return std::to_string(std::any_cast<double>(value));
            }
    );

    registry->RegisterCtorConverter<int, Enum>(
            [](const std::any& value, const std::any& current)
            {
                return std::any_cast<Enum>(current).Set(
                        (unsigned int)std::any_cast<int>(value)
                );
            }
    );

    registry->RegisterCtorConverter<std::string, Enum>(
            [](const std::any& value, const std::any& current)
            {
                return std::any_cast<Enum>(current).Set(
                        std::any_cast<std::string>(value)
                );
            }
    );

    registry->RegisterCtorConverter<Enum, std::string>(
            [](const std::any& value, const std::any& current)
            {
                return std::any_cast<Enum>(value).GetCurrentString();
            }
    );

    registry->RegisterCtorConverter<Enum, int>(
            [](const std::any& value, const std::any& current)
            {
                return (int)std::any_cast<Enum>(value).GetCurrentIndex();
            }
    );
}