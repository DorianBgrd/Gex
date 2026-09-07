#include <Tsys/defaultTypes.h>
#include <Tsys/typeSystem.h>

#include <pybind11/pybind11.h>

#include <any>
#include <string>
#include <map>
#include <vector>
#include "rapidjson/reader.h"
#include "rapidjson/document.h"


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
            UseEqualOp<int>,
            UseDefaultCtor<int>,
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
            UseEqualOp<float>,
            UseDefaultCtor<float>,
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
            UseEqualOp<double>,
            UseDefaultCtor<double>,
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
            UseEqualOp<std::string>,
            UseDefaultCtor<std::string>,
            DefaultPyBindToAnyCast<std::string>,
            DefaultAnyToPyBindCast<std::string>
    );


    registry->RegisterType<std::vector<int>>(
            "intArray",
            DefaultCreator,
            ArraySerializer<int>(registry),
            ArrayDeserializer<int>(registry),
            UseEqualOp<std::vector<int>>,
            UseDefaultCtor<std::vector<int>>,
            DefaultPyBindToAnyCast<std::vector<int>>,
            DefaultAnyToPyBindCast<std::vector<int>>
    );


    registry->RegisterType<std::vector<float>>(
            "floatArray",
            DefaultCreator,
            ArraySerializer<float>(registry),
            ArrayDeserializer<float>(registry),
            UseEqualOp<std::vector<float>>,
            UseDefaultCtor<std::vector<float>>,
            DefaultPyBindToAnyCast<std::vector<float>>,
            DefaultAnyToPyBindCast<std::vector<float>>
    );


    registry->RegisterType<std::vector<double>>(
            "doubleArray",
            DefaultCreator,
            ArraySerializer<double>(registry),
            ArrayDeserializer<double>(registry),
            UseEqualOp<std::vector<double>>,
            UseDefaultCtor<std::vector<double>>,
            DefaultPyBindToAnyCast<std::vector<double>>,
            DefaultAnyToPyBindCast<std::vector<double>>
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
}