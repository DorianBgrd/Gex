#include "reader.h"


void JsonPlugin::Reader::SetParent(const ReaderPtr& prnt)
{
    parent = prnt;
}


JsonPlugin::ReaderPtr JsonPlugin::Reader::GetParent() const
{
    return parent;
}


rapidjson::Value& JsonPlugin::Reader::Read(
        rapidjson::Value& value
        ) const
{
    if (parent)
    {
        value = parent->Read(value);
    }

    return ReadValue(value);
}


std::any JsonPlugin::Reader::ExtractValue() const
{
    return {};
}


std::any JsonPlugin::Reader::Extract(
        rapidjson::Value& value
) const
{
    return Extract(Read(value));
}


JsonPlugin::DocumentReader::DocumentReader(
        rapidjson::Document& json
): Reader(), doc(json)
{

}


rapidjson::Value& JsonPlugin::DocumentReader::ReadValue(
        rapidjson::Value& value
) const
{
    return doc.GetObject();
}



JsonPlugin::DictValue::DictValue(const std::string& k): Reader()
{
    key = k;
}


rapidjson::Value& JsonPlugin::DictValue::ReadValue(
        rapidjson::Value& value
        ) const
{
    return value[key.c_str()];
}

bool JsonPlugin::DictValue::IsExtractable() const
{
    return false;
}


bool JsonPlugin::DocumentReader::IsExtractable() const
{
    return false;
}


JsonPlugin::ListValue::ListValue(unsigned int idx): Reader()
{
    index = idx;
}


bool JsonPlugin::ListValue::IsExtractable() const
{
    return false;
}


rapidjson::Value& JsonPlugin::ListValue::ReadValue(
        rapidjson::Value& value
        ) const
{
    return value[index];
}

rapidjson::Value &JsonPlugin::Extractor::ReadValue(
        rapidjson::Value &value
) const {
    return value;
}

std::any JsonPlugin::Extractor::Extract(
        rapidjson::Value& value
) const
{
    switch (value.GetType())
    {
        case rapidjson::kStringType:
            return std::make_any<std::string>(
                    std::string(value.GetString())
            );

        case rapidjson::kTrueType:
            return std::make_any<bool>(true);

        case rapidjson::kFalseType:
            return std::make_any<bool>(false);

        case rapidjson::kNumberType:
            if (value.IsDouble())
                return std::make_any<double>(value.GetDouble());
            else if (value.IsFloat())
                return std::make_any<float>(value.GetFloat());
            else if (value.IsInt())
                return std::make_any<int>(value.GetInt());
            else if (value.IsInt64())
                return std::make_any<int>(value.GetInt64());
        default:
            return {};
    }
}
