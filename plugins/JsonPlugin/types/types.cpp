#include "types.h"
#include "editor.h"


JsonPlugin::JsonValueHandler::JsonValueHandler(): TSys::TypeHandler()
{
    RegisterConverter<std::string>(
            [](const std::any& v, const std::any& c)
            {
                JsonValuePtr val = std::make_shared<JsonValue>(
                        rapidjson::kStringType
                );

                std::string str = std::any_cast<std::string>(v);
                val->SetString(str.c_str(), str.size());

                return std::make_any<JsonValuePtr>(val);
            }
    );

    RegisterConverter<int>(
            [](const std::any& v, const std::any& c)
            {
                JsonValuePtr val = std::make_shared<JsonValue>(
                        rapidjson::kNumberType
                );

                int jv = std::any_cast<int>(v);
                val->SetInt64(jv);

                return std::make_any<JsonValuePtr>(val);
            }
    );

    RegisterConverter<float>(
            [](const std::any& v, const std::any& c)
            {
                JsonValuePtr val = std::make_shared<JsonValue>(
                        rapidjson::kNumberType
                );

                float jv = std::any_cast<float>(v);
                val->SetFloat(jv);

                return std::make_any<JsonValuePtr>(val);
            }
    );

    RegisterConverter<double>(
            [](const std::any& v, const std::any& c)
            {
                JsonValuePtr val = std::make_shared<JsonValue>(
                        rapidjson::kNumberType
                );

                double jv = std::any_cast<double>(v);
                val->SetDouble(jv);

                return std::make_any<JsonValuePtr>(val);
            }
    );
}


void JsonPlugin::JsonValueHandler::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{

}


std::any JsonPlugin::JsonValueHandler::DeserializeValue(
        const std::any& v,
        rapidjson::Value& value
) const
{
    return InitValue();
}


void JsonPlugin::JsonValueHandler::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{

}


std::any JsonPlugin::JsonValueHandler::DeserializeConstruction(
        rapidjson::Value& value
) const
{
    return InitValue();
}


std::any JsonPlugin::JsonValueHandler::InitValue() const
{
    return std::make_shared<rapidjson::Value>();
}


bool JsonPlugin::JsonValueHandler::CompareValue(const std::any& v1, const std::any& v2) const
{
    return std::any_cast<JsonValuePtr>(v1) == std::any_cast<JsonValuePtr>(v2);
}


std::any JsonPlugin::JsonValueHandler::FromPython(const boost::python::object&) const
{
    return InitValue();
}


boost::python::object JsonPlugin::JsonValueHandler::ToPython(const std::any&) const
{
    return {};
}


std::any JsonPlugin::JsonValueHandler::CopyValue(const std::any& source) const
{
    return source;
}


size_t JsonPlugin::JsonValueHandler::Hash() const
{
    return typeid(JsonValuePtr).hash_code();
}


std::string JsonPlugin::JsonValueHandler::ApiName() const
{
    return "JsonValue";
}


size_t JsonPlugin::JsonValueHandler::ValueHash(const std::any& val) const
{
    return 0;
}



template<typename EditorType, typename... Args>
JsonPlugin::EditorPtr MakeEditorPtr(Args... args)
{
    return std::make_shared<EditorType>(args...);
}


template<typename EditorType, typename... Args>
std::any MakeEditorAny(Args... args)
{
    return std::make_any<JsonPlugin::EditorPtr>(
            MakeEditorPtr<EditorType>(args...)
    );
}


template<typename EditorType, typename SourceType>
std::any MakeEditorFromAny(const std::any& value)
{
    return MakeEditorAny<EditorType>(
            std::any_cast<SourceType>(
                    value
            )
    );
}


JsonPlugin::JsonEditorHandler::JsonEditorHandler(): TSys::TypeHandler()
{
    RegisterConverter<std::string>(
            [](const std::any& v, const std::any& c)
            {
                return MakeEditorFromAny<StringEditor, std::string>(v);
            }
    );

    RegisterConverter<int>(
            [](const std::any& v, const std::any& c)
            {
                return MakeEditorFromAny<IntEditor, int>(v);
            }
    );

    RegisterConverter<float>(
            [](const std::any& v, const std::any& c)
            {
                return MakeEditorFromAny<FloatEditor, float>(v);
            }
    );

    RegisterConverter<double>(
            [](const std::any& v, const std::any& c)
            {
                return MakeEditorFromAny<DoubleEditor, double>(v);
            }
    );
}


void JsonPlugin::JsonEditorHandler::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{

}


std::any JsonPlugin::JsonEditorHandler::DeserializeValue(
        const std::any& v,
        rapidjson::Value& value
) const
{
    return InitValue();
}


void JsonPlugin::JsonEditorHandler::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{

}


std::any JsonPlugin::JsonEditorHandler::DeserializeConstruction(
        rapidjson::Value& value
) const
{
    return InitValue();
}


std::any JsonPlugin::JsonEditorHandler::InitValue() const
{
    EditorPtr editor = std::make_shared<EmptyEditor>();
    return editor;
}


bool JsonPlugin::JsonEditorHandler::CompareValue(const std::any& v1, const std::any& v2) const
{
//    return std::any_cast<JsonEditorPtr>(v1) == std::any_cast<JsonEditorPtr>(v2);
    return false;
}


std::any JsonPlugin::JsonEditorHandler::FromPython(const boost::python::object&) const
{
    return InitValue();
}


boost::python::object JsonPlugin::JsonEditorHandler::ToPython(const std::any&) const
{
    return {};
}


std::any JsonPlugin::JsonEditorHandler::CopyValue(const std::any& source) const
{
    return source;
}


size_t JsonPlugin::JsonEditorHandler::Hash() const
{
    return typeid(EditorPtr).hash_code();
}


std::string JsonPlugin::JsonEditorHandler::ApiName() const
{
    return "JsonEditor";
}


size_t JsonPlugin::JsonEditorHandler::ValueHash(const std::any& val) const
{
    return 0;
}