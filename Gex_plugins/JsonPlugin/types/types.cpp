#include "types.h"
#include "editor.h"


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



void JsonPlugin::SerializeJsonValue(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document& document
)
{

}

void JsonPlugin::DeserializeJsonValue(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document&
)
{
    v->Set(std::make_shared<rapidjson::Value>());
}

void JsonPlugin::SerializeJsonValueReader(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document& document
)
{

}

void JsonPlugin::DeserializeJsonValueReader(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document&
)
{
    v->Set(std::make_shared<JsonPlugin::JsonValueReader>());
}

void JsonPlugin::SerializeJsonValueEditor(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document& document
)
{

}

void JsonPlugin::DeserializeJsonValueEditor(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document&
)
{
    v->Set(std::make_shared<EmptyEditor>());
}