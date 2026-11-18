#include "resolution.h"


bool CompareResolution(const std::any& value1,
                       const std::any& value2)
{
    auto castedValue1 = std::any_cast<ImageManip::Types::Resolution>(value1);
    auto castedValue2 = std::any_cast<ImageManip::Types::Resolution>(value2);

    return (castedValue1.at(0) == castedValue2.at(0) &&
            castedValue1.at(1) == castedValue2.at(2));
}


void ImageManip::Types::SerializeResolution(const TSys::TypedValuePtr & v,
                         rapidjson::Value& value,
                         rapidjson::Document& document)
{
    auto res = v->Get<Resolution>();

    auto& arrayValue = value.SetArray();

    arrayValue.PushBack(rapidjson::Value().SetInt(res.at(0)),
                        document.GetAllocator());
    arrayValue.PushBack(rapidjson::Value().SetInt(res.at(1)),
                        document.GetAllocator());
}

void ImageManip::Types::DeserializeResolution(const TSys::TypedValuePtr& v,
                           rapidjson::Value& value,
                           rapidjson::Document& document)
{
    Resolution res = {
            value[0].GetInt(),
            value[1].GetInt()
    };

    v->Set(res);
}