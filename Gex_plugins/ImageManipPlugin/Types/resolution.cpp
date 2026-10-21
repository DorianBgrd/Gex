#include "resolution.h"


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