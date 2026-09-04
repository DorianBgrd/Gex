#include "color.h"


void ImageManip::Types::SerializeColor(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document& document
)
{
    auto color = v->Get<QColor>();

    value.PushBack(rapidjson::Value().SetInt(color.red()),
                   document.GetAllocator());
    value.PushBack(rapidjson::Value().SetInt(color.green()),
                   document.GetAllocator());
    value.PushBack(rapidjson::Value().SetInt(color.blue()),
                   document.GetAllocator());
    value.PushBack(rapidjson::Value().SetInt(color.alpha()),
                   document.GetAllocator());
}


void ImageManip::Types::DeserializeColor(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document&
)
{
    QColor color(
            value[0].GetInt(),
            value[1].GetInt(),
            value[2].GetInt(),
            value[3].GetInt()
    );

    v->Set(color);
}