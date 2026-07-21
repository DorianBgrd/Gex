#include "types.h"

#include "vector.h"
#include "point.h"



template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}


Math::Types::Point2Handle::Point2Handle()
{
    RegisterConverter<Vector2>(
            [](const std::any& vector, const std::any&)
            {
                return std::make_any<Point2>(
                        Point2::FromVector2(
                                std::any_cast<Vector2>(vector)
                        )
                );
            }
    );
}


void Math::Types::Point2Handle::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{
    auto pnt2 = std::any_cast<Point2>(v);

    auto& val = rapidjson::Value().SetArray();
    val.PushBack(pnt2.x, document.GetAllocator());
    val.PushBack(pnt2.y, document.GetAllocator());
}

std::any Math::Types::Point2Handle::DeserializeValue(
        const std::any& v,
        rapidjson::Value& value) const
{
    Point2 pnt2 = {value[0].GetDouble(), value[1].GetDouble()};
    return std::make_any<Point2>(pnt2);
}

void Math::Types::Point2Handle::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document) const
{

}

std::any Math::Types::Point2Handle::DeserializeConstruction(
        rapidjson::Value& value) const
{
    Point2 pnt2 = {0, 0};
    return std::make_any<Point2>(pnt2);
}

std::any Math::Types::Point2Handle::InitValue() const
{
    Point2 pnt2 = {0, 0};
    return std::make_any<Point2>(pnt2);
}

bool Math::Types::Point2Handle::CompareValue(
        const std::any& v1,
        const std::any& v2
) const
{
    return std::any_cast<Point2>(v1) == std::any_cast<Point2>(v2);
}

std::any Math::Types::Point2Handle::FromPython(const pybind11::object& l) const
{
    pybind11::list list(l);

    Point2 pnt2 = {
            list[0].cast<double>(),
            list[1].cast<double>()
    };

    return std::make_any<Point2>(pnt2);
}

pybind11::object Math::Types::Point2Handle::ToPython(const std::any& v) const
{
    pybind11::list l;

    auto pnt2 = std::any_cast<Point2>(v);

    l.append(pnt2.x);
    l.append(pnt2.y);

    return l;
}

std::any Math::Types::Point2Handle::CopyValue(const std::any& source) const
{
    return Point2(std::any_cast<Point2>(source));
}

size_t Math::Types::Point2Handle::Hash() const
{
    return typeid(Point2).hash_code();

}

std::string Math::Types::Point2Handle::ApiName() const
{
    return "Math::Point2";
}


size_t Math::Types::Point2Handle::ValueHash(const std::any& val) const
{
    auto vec2 = std::any_cast<Math::Types::Point2>(val);

    std::hash<double> hash;

    auto hashValue = hash(vec2.x);

    hash_combine(hashValue, vec2.y);

    return hashValue;
}









Math::Types::Vector2Handle::Vector2Handle()
{
    RegisterConverter<Point2>(
            [](const std::any& vector, const std::any&)
            {
                return std::make_any<Vector2>(
                        Vector2::FromPoint2(
                                std::any_cast<Point2>(vector)
                        )
                );
            }
    );
}


void Math::Types::Vector2Handle::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{
    auto vec2 = std::any_cast<Vector2>(v);

    auto& val = rapidjson::Value().SetArray();

    val.PushBack(vec2.x, document.GetAllocator());
    val.PushBack(vec2.y, document.GetAllocator());
}

std::any Math::Types::Vector2Handle::DeserializeValue(
        const std::any& v,
        rapidjson::Value& value) const
{
    Vector2 vec2 = {value[0].GetDouble(), value[1].GetDouble()};
    return std::make_any<Vector2>(vec2);
}

void Math::Types::Vector2Handle::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
       rapidjson::Document& document) const
{

}

std::any Math::Types::Vector2Handle::DeserializeConstruction(
        rapidjson::Value& value) const
{
    Vector2 vec2 = {0, 0};
    return std::make_any<Vector2>(vec2);
}

std::any Math::Types::Vector2Handle::InitValue() const
{
    Vector2 vec2 = {0, 0};
    return std::make_any<Vector2>(vec2);
}

bool Math::Types::Vector2Handle::CompareValue(
        const std::any& v1,
        const std::any& v2) const
{
    return std::any_cast<Vector2>(v1) == std::any_cast<Vector2>(v2);
}

std::any Math::Types::Vector2Handle::FromPython(const pybind11::object& l) const
{
    pybind11::list list(l);

    Vector2 vec2 = {
            list[0].cast<double>(),
            list[1].cast<double>()
    };

    return std::make_any<Vector2>(vec2);
}

pybind11::object Math::Types::Vector2Handle::ToPython(const std::any& v) const
{
    pybind11::list l;

    auto vec2 = std::any_cast<Vector2>(v);

    l.append(vec2.x);
    l.append(vec2.y);

    return l;
}

std::any Math::Types::Vector2Handle::CopyValue(const std::any& source) const
{
    return Vector2(std::any_cast<Vector2>(source));
}

size_t Math::Types::Vector2Handle::Hash() const
{
    return typeid(Vector2).hash_code();

}

std::string Math::Types::Vector2Handle::ApiName() const
{
    return "Math::Vector2";
}


size_t Math::Types::Vector2Handle::ValueHash(const std::any& val) const
{
    auto vec2 = std::any_cast<Math::Types::Vector2>(val);

    std::hash<double> hash;

    auto hashValue = hash(vec2.x);

    hash_combine(hashValue, vec2.y);

    return hashValue;
}




struct PointToVector3: public TSys::Converter
{

};


Math::Types::Point3Handle::Point3Handle()
{
    RegisterConverter<Vector3>(
            [](const std::any& vector, const std::any&)
            {
                return std::make_any<Point3>(
                        Point3::FromVector3(
                                std::any_cast<Vector3>(vector)
                        )
                );
            }
    );
}


void Math::Types::Point3Handle::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{
    auto pnt = std::any_cast<Point3>(v);

    auto& val = rapidjson::Value().SetArray();

    val.PushBack(pnt.x, document.GetAllocator());
    val.PushBack(pnt.y, document.GetAllocator());
    val.PushBack(pnt.z, document.GetAllocator());
}

std::any Math::Types::Point3Handle::DeserializeValue(
        const std::any& v,
        rapidjson::Value& value) const
{
    Point3 pnt = {value[0].GetDouble(),
                  value[1].GetDouble(),
                  value[2].GetDouble()};

    return std::make_any<Point3>(pnt);
}

void Math::Types::Point3Handle::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document) const
{

}

std::any Math::Types::Point3Handle::DeserializeConstruction(
        rapidjson::Value& value) const
{
    Point3 pnt = {0, 0, 0};
    return std::make_any<Point3>(pnt);
}

std::any Math::Types::Point3Handle::InitValue() const
{
    Point3 pnt = {0, 0, 0};
    return std::make_any<Point3>(pnt);
}

bool Math::Types::Point3Handle::CompareValue(
        const std::any& v1,
        const std::any& v2
) const
{
    return std::any_cast<Point3>(v1) == std::any_cast<Point3>(v2);
}

std::any Math::Types::Point3Handle::FromPython(const pybind11::object& l) const
{
    pybind11::list list(l);

    Point3 pnt = {
            list[0].cast<double>(),
            list[1].cast<double>(),
            list[2].cast<double>()
    };

    return std::make_any<Point3>(pnt);
}

pybind11::object Math::Types::Point3Handle::ToPython(const std::any& v) const
{
    pybind11::list l;

    auto pnt = std::any_cast<Point3>(v);

    l.append(pnt.x);
    l.append(pnt.y);
    l.append(pnt.z);

    return l;
}

std::any Math::Types::Point3Handle::CopyValue(const std::any& source) const
{
    return Point3(std::any_cast<Point3>(source));
}

size_t Math::Types::Point3Handle::Hash() const
{
    return typeid(Point3).hash_code();

}

std::string Math::Types::Point3Handle::ApiName() const
{
    return "Math::Point3";
}

size_t Math::Types::Point3Handle::ValueHash(const std::any& val) const
{
    auto vec3 = std::any_cast<Point3>(val);

    std::hash<double> hash;

    auto hashValue = hash(vec3.x);

    hash_combine(hashValue, vec3.y);

    hash_combine(hashValue, vec3.z);

    return hashValue;
}






Math::Types::Vector3Handle::Vector3Handle()
{
    RegisterConverter<Point3>(
            [](const std::any& vector, const std::any&)
            {
                return std::make_any<Vector3>(
                        Vector3::FromPoint3(
                                std::any_cast<Point3>(vector)
                        )
                );
            }
    );
}


void Math::Types::Vector3Handle::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{
    auto vec2 = std::any_cast<Vector3>(v);

    auto& val = rapidjson::Value().SetArray();

    val.PushBack(vec2.x, document.GetAllocator());
    val.PushBack(vec2.y, document.GetAllocator());
    val.PushBack(vec2.z, document.GetAllocator());
}

std::any Math::Types::Vector3Handle::DeserializeValue(
        const std::any& v,
        rapidjson::Value& value) const
{
    Vector3 vec3 = {value[0].GetDouble(), value[1].GetDouble(), value[2].GetDouble()};
    return std::make_any<Vector3>(vec3);
}

void Math::Types::Vector3Handle::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document) const
{

}

std::any Math::Types::Vector3Handle::DeserializeConstruction(
        rapidjson::Value& value) const
{
    Vector3 vec3 = {0, 0, 0};
    return std::make_any<Vector3>(vec3);
}

std::any Math::Types::Vector3Handle::InitValue() const
{
    Vector3 vec3 = {0, 0, 0};
    return std::make_any<Vector3>(vec3);
}

bool Math::Types::Vector3Handle::CompareValue(
        const std::any& v1,
        const std::any& v2) const
{
    return std::any_cast<Vector3>(v1) == std::any_cast<Vector3>(v2);
}

std::any Math::Types::Vector3Handle::FromPython(const pybind11::object& l) const
{
    pybind11::list list(l);

    Vector3 vec3 = {
            list[0].cast<double>(),
            list[1].cast<double>(),
            list[2].cast<double>()
    };

    return std::make_any<Vector3>(vec3);
}

pybind11::object Math::Types::Vector3Handle::ToPython(const std::any& v) const
{
    pybind11::list l;

    auto vec3 = std::any_cast<Vector3>(v);

    l.append(vec3.x);
    l.append(vec3.y);
    l.append(vec3.z);

    return l;
}

std::any Math::Types::Vector3Handle::CopyValue(const std::any& source) const
{
    return Vector3(std::any_cast<Vector3>(source));
}

size_t Math::Types::Vector3Handle::Hash() const
{
    return typeid(Vector3).hash_code();

}

std::string Math::Types::Vector3Handle::ApiName() const
{
    return "Math::Vector3";
}


size_t Math::Types::Vector3Handle::ValueHash(const std::any& val) const
{
    auto vec3 = std::any_cast<Vector3>(val);

    std::hash<double> hash;

    auto hashValue = hash(vec3.x);

    hash_combine(hashValue, vec3.y);

    hash_combine(hashValue, vec3.z);

    return hashValue;
}