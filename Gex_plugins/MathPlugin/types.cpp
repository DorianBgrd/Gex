#include "types.h"

#include "vector.h"
#include "point.h"



template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}


void Math::Types::SerializePoint2(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document& document
)
{
    auto pnt2 = v->Get<Point2>();

    auto& val = rapidjson::Value().SetArray();
    val.PushBack(pnt2.x, document.GetAllocator());
    val.PushBack(pnt2.y, document.GetAllocator());
}


void Math::Types::DeserializePoint2(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document&
)
{
    Point2 pnt2 = {value[0].GetDouble(), value[1].GetDouble()};

    v->Set(pnt2);
}


void Math::Types::SerializeVector2(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document& document
)
{
    auto vec2 = v->Get<Math::Vector2>();

    auto& val = rapidjson::Value().SetArray();

    val.PushBack(vec2.x, document.GetAllocator());
    val.PushBack(vec2.y, document.GetAllocator());
}


void Math::Types::DeserializeVector2(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document&)
{
    Math::Vector2 vec2 = {value[0].GetDouble(),
                          value[1].GetDouble()};

    v->Set(vec2);
}


void Math::Types::SerializePoint3(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document& document
)
{
    auto pnt = v->Get<Point3>();

    auto& val = rapidjson::Value().SetArray();

    val.PushBack(pnt.x, document.GetAllocator());
    val.PushBack(pnt.y, document.GetAllocator());
    val.PushBack(pnt.z, document.GetAllocator());
}


void Math::Types::DeserializePoint3(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document&)
{
    Point3 pnt = {value[0].GetDouble(),
                  value[1].GetDouble(),
                  value[2].GetDouble()};

    v->Set(pnt);
}


void Math::Types::SerializeVector3(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document& document
)
{
    auto vec3 = v->Get<Math::Vector3>();

    auto& val = rapidjson::Value().SetArray();

    val.PushBack(vec3.x, document.GetAllocator());
    val.PushBack(vec3.y, document.GetAllocator());
    val.PushBack(vec3.z, document.GetAllocator());
}


void Math::Types::DeserializeVector3(
        const TSys::TypedValuePtr& v,
        rapidjson::Value& value,
        rapidjson::Document&)
{

    Math::Vector3 vec3 = {
            value[0].GetDouble(),
            value[1].GetDouble(),
            value[2].GetDouble()
    };

    v->Set(vec3);
}