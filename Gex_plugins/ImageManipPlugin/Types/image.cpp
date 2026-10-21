#include "image.h"


void ImageManip::Types::SerializeImage(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document& document)
{

}


void ImageManip::Types::DeserializeImage(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document&
)
{
    v->Set(QImage());
}