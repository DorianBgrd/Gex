#include "editor.h"


std::unordered_map<std::type_index, JsonPlugin::Generator> JsonPlugin::ValueGenerator::valueGenerators;


void JsonPlugin::ValueGenerator::RegisterGenerator(
        const std::type_index& index,
        const Generator& generator
)
{
    valueGenerators[index] = generator;
}


bool JsonPlugin::ValueGenerator::Generate(
        const std::any& value,
        rapidjson::Value& json,
        rapidjson::Document& jsonDoc
)
{
    auto genM = valueGenerators.find(value.type());
    if (genM == valueGenerators.end())
    {
        return false;
    }

    genM->second(value, json, jsonDoc);
    return true;
}


JsonPlugin::JsonValuePtr JsonPlugin::EmptyEditor::EditValue(JsonDoc& json) const
{
    return std::make_shared<JsonValue>();
}

JsonPlugin::IntEditor::IntEditor(int v)
{
    value = v;
}


JsonPlugin::JsonValuePtr JsonPlugin::IntEditor::EditValue(JsonDoc& json) const
{
    JsonValuePtr jsonValue = std::make_shared<JsonValue>(
            rapidjson::kNumberType
    );

    jsonValue->SetInt64(value);

    return jsonValue;
}


JsonPlugin::FloatEditor::FloatEditor(float v)
{
    value = v;
}


JsonPlugin::JsonValuePtr JsonPlugin::FloatEditor::EditValue(JsonDoc& json) const
{
    JsonValuePtr jsonValue = std::make_shared<JsonValue>(
            rapidjson::kNumberType
    );

    jsonValue->SetFloat(value);

    return jsonValue;
}



JsonPlugin::DoubleEditor::DoubleEditor(double v)
{
    value = v;
}


JsonPlugin::JsonValuePtr JsonPlugin::DoubleEditor::EditValue(JsonDoc& json) const
{
    JsonValuePtr jsonValue = std::make_shared<JsonValue>(
            rapidjson::kNumberType
    );

    jsonValue->SetDouble(value);

    return jsonValue;
}


JsonPlugin::StringEditor::StringEditor(const std::string& v)
{
    value = v;
}


JsonPlugin::JsonValuePtr JsonPlugin::StringEditor::EditValue(JsonDoc& json) const
{
    JsonValuePtr jsonValue = std::make_shared<JsonValue>(
            rapidjson::kNumberType
    );

    jsonValue->SetString(value.c_str(), value.size(),
            json.GetAllocator());

    return jsonValue;
}


JsonPlugin::DictEditor::DictEditor(const EditorMap& values)
{
    editors = values;
}



void JsonPlugin::DictEditor::AddEditor(
        const std::string& key,
        const EditorPtr& value
)
{
    editors[key] = value;
}


void JsonPlugin::DictEditor::RemoveEditor(
        const std::string& key
)
{
    auto index = editors.find(key);
    if (index == editors.end())
    {
        return;
    }

    editors.erase(index);
}


JsonPlugin::JsonValuePtr  JsonPlugin::DictEditor::EditValue(JsonDoc& json) const
{
    JsonValuePtr jsonValue = std::make_shared<JsonValue>(
            rapidjson::kObjectType
    );

    for (const auto& kv : editors)
    {
        std::string key = kv.first;
        EditorPtr value = kv.second;

        JsonValuePtr jvalue = value->EditValue(json);

        jsonValue->AddMember(
            rapidjson::Value().SetString(key.c_str(), json.GetAllocator()).Move(),
            *jvalue, json.GetAllocator()
        );
    }

    return jsonValue;
}


JsonPlugin::ListEditor::ListEditor(const EditorList& values)
{
    editors = values;
}


void JsonPlugin::ListEditor::AddEditor(const EditorPtr& value)
{
    editors.push_back(value);
}


void JsonPlugin::ListEditor::RemoveEditor(unsigned int index)
{
    if (index >= editors.size())
    {
        return;
    }

    editors.erase(editors.begin() + (index - 1));
}


JsonPlugin::JsonValuePtr JsonPlugin::ListEditor::EditValue(JsonDoc& json) const
{
    JsonValuePtr jsonValue = std::make_shared<JsonValue>(
            rapidjson::kArrayType
    );

    for (const auto& editor : editors)
    {
        JsonValuePtr jvalue = editor->EditValue(json);
        jsonValue->PushBack((*jvalue).Move(), json.GetAllocator());
    }

    return jsonValue;
}

