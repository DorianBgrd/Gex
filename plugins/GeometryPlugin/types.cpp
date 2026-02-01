#include "types.h"



std::string Geometry::Types::IndexHandler::ApiName() const
{
    return "Geometry::Index";
}

void Geometry::Types::IndexHandler::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{
    auto& val = rapidjson::Value().SetArray();

    val.PushBack(
            std::any_cast<Index>(v),
            document.GetAllocator()
    );
}

std::any Geometry::Types::IndexHandler::DeserializeValue(
        const std::any& v, rapidjson::Value& value
) const
{
    return {};
}

void Geometry::Types::IndexHandler::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{

}

std::any Geometry::Types::IndexHandler::DeserializeConstruction(
        rapidjson::Value& value
) const
{
    return InitValue();
}

std::any Geometry::Types::IndexHandler::InitValue() const
{
    return Index(0);
}

bool Geometry::Types::IndexHandler::CompareValue(
        const std::any& v1, const std::any& v2
) const
{
    return std::any_cast<Index>(v1) == std::any_cast<Index>(v2);
}

std::any Geometry::Types::IndexHandler::FromPython(
        const boost::python::object& v
) const
{
    Index index = boost::python::extract<Index>(v);
    return std::make_any<Index>(index);
}

boost::python::object Geometry::Types::IndexHandler::ToPython(
        const std::any& value
) const
{
    boost::python::object i(std::any_cast<Index>(value));

    return i;
}

std::any Geometry::Types::IndexHandler::CopyValue(
        const std::any& source
) const
{
    return std::any_cast<Index>(source);
}

size_t Geometry::Types::IndexHandler::Hash() const
{
    return typeid(Index).hash_code();
}

size_t Geometry::Types::IndexHandler::ValueHash(
        const std::any& val
) const
{
    return std::any_cast<Index>(val);
}


std::vector<Geometry::Types::Face> Geometry::Types::Face::ToTriangles() const
{
    if (size() <= 3)
    {
        return {Face(*this)};
    }

    auto first = at(0);
    auto second = at(1);

    std::vector<Geometry::Types::Face> faces;

    for (unsigned int i = 2; i < size(); i++)
    {
        auto f = at(i);
        Face face;
        face.push_back(first);
        face.push_back(second);
        face.push_back(f);

        faces.push_back({});

        second = f;
    }

    return faces;
}



std::string Geometry::Types::FaceHandler::ApiName() const
{
    return "Mesh::Face";
}

void Geometry::Types::FaceHandler::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{
    auto face = std::any_cast<Face>(v);

    auto& val = rapidjson::Value().SetArray();

//    val.PushBack(triangle[0], document.GetAllocator());
//    val.PushBack(triangle[1], document.GetAllocator());
//    val.PushBack(triangle[2], document.GetAllocator());

    for (auto index : face)
    {
        val.PushBack(index, document.GetAllocator());
    }

    value.PushBack(val, document.GetAllocator());
}

std::any Geometry::Types::FaceHandler::DeserializeValue(
        const std::any& v,
        rapidjson::Value& value
) const
{
//    Triangle triangle = {
//            (Index)value[0].GetInt(),
//            (Index)value[1].GetInt(),
//            (Index)value[2].GetInt(),
//    };
    Face face;

    for (unsigned int i = 0; i < value.Size(); i ++)
    {
        face.push_back(value[i].GetInt());
    }

    return std::make_any<Face>(face);
}

void Geometry::Types::FaceHandler::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{

}

std::any Geometry::Types::FaceHandler::DeserializeConstruction(
        rapidjson::Value& value
) const
{
    return {};
}

std::any Geometry::Types::FaceHandler::InitValue() const
{
    Face t;
    return std::make_any<Face>(t);
}

bool Geometry::Types::FaceHandler::CompareValue(
        const std::any& v1, const std::any& v2
) const
{
    return std::any_cast<Face>(v1) == std::any_cast<Face>(v2);
}

std::any Geometry::Types::FaceHandler::FromPython(
        const boost::python::object&
) const
{
    return {};
}

boost::python::object Geometry::Types::FaceHandler::ToPython(
        const std::any&
) const
{
    return {};
}

std::any Geometry::Types::FaceHandler::CopyValue(
        const std::any& source
) const
{
    Face t(std::any_cast<Face>(source));

    return std::make_any<Face>(t);
}

size_t Geometry::Types::FaceHandler::Hash() const
{
    return typeid(Face).hash_code();
}

size_t Geometry::Types::FaceHandler::ValueHash(
        const std::any& val
) const
{
    return 0;
}


double GetTriangleArea(const Geometry::Types::Vector3& v1,
                       const Geometry::Types::Vector3& v2)
{
    return (v1.Magnitude() * v2.Magnitude() * std::sin(v1.Angle(v2))) / 2.0;
}


Geometry::Types::Point3 BarycentricCoord(
        const Geometry::Types::Point3& point,
        const Geometry::Types::Point3& p1,
        const Geometry::Types::Point3& p2,
        const Geometry::Types::Point3& p3
)
{
    double mainArea = GetTriangleArea(
        Geometry::Types::Vector3::FromPoints(p1, p2),
        Geometry::Types::Vector3::FromPoints(p1, p3)
    );

    double area1 = GetTriangleArea(
        Geometry::Types::Vector3::FromPoints(p1, p2),
        Geometry::Types::Vector3::FromPoints(p1, point)
    );

    double area2 = GetTriangleArea(
            Geometry::Types::Vector3::FromPoints(p2, p3),
            Geometry::Types::Vector3::FromPoints(p2, point)
    );

    double area3 = GetTriangleArea(
            Geometry::Types::Vector3::FromPoints(p3, p1),
            Geometry::Types::Vector3::FromPoints(p3, point)
    );

    return {area1 / mainArea, area2 / mainArea, area3 / mainArea};
}


bool IsInTriangle(const Geometry::Types::Point3 bary)
{
    return (bary.x <= 1.0 && bary.y <= 1.0 && bary.z <= 1.0);
}


Geometry::Types::Point3 FromBarycentric(
        const Geometry::Types::Point3& bary,
        const Geometry::Types::Point3& p1,
        const Geometry::Types::Point3& p2,
        const Geometry::Types::Point3& p3,
        bool clamp=false
)
{
    double x = bary.x;
    double y = bary.y;
    double z = bary.z;
    if (clamp)
    {
        x = std::clamp(x, 0., 1.);
        y = std::clamp(y, 0., 1.);
        z = std::clamp(z, 0., 1.);
    }

    return p1 * x + p2 * y + p3 * z;
}


double Geometry::Types::Mesh::GetFaceClosestPoint(
        const Point3& point, Index faceIndex, Point3* resultPoint
)
{
//    auto facePoints = GetFacePoints(faceIndex);
    auto face = GetFace(faceIndex);

    std::vector<Geometry::Types::Point3> closestPoints;
    for (auto triangle : face.ToTriangles())
    {
        auto p1 = GetPoint(triangle[0]);
        auto p2 = GetPoint(triangle[1]);
        auto p3 = GetPoint(triangle[2]);

        auto baryCoord = BarycentricCoord(
            point, p1, p2, p3
        );

        if (IsInTriangle(baryCoord))
        {
            resultPoint->Swap(point);
            return 0.;
        }

        closestPoints.push_back(
            FromBarycentric(
                baryCoord,
                p1, p2, p3
            )
        );
    }

    unsigned int index = 0;
    double distance = closestPoints[0].Distance(point);
    for (unsigned int i = 1; i < closestPoints.size(); i++)
    {
        double pdist = closestPoints[i].Distance(point);
        if (pdist < distance)
        {
            index = i;
            distance = pdist;
        }
    }

    resultPoint->Swap(closestPoints[index]);
    return distance;
}


Geometry::Types::Mesh::Mesh(
        const PointArray& pnts,
        const FaceArray& fcs
)
{
    points = pnts;
    faces = fcs;
}


Geometry::Types::Mesh::Mesh(const Mesh& other)
{
    points = other.points;
    faces = other.faces;
}


Geometry::Types::PointArray Geometry::Types::Mesh::GetPoints() const
{
    return points;
}


void Geometry::Types::Mesh::SetPoints(const PointArray& pnts)
{
    points = pnts;
}


Geometry::Types::FaceArray Geometry::Types::Mesh::GetFaces() const
{
    return faces;
}


void Geometry::Types::Mesh::SetFaces(const FaceArray& f)
{
    faces = f;
}


Math::Types::Point3& Geometry::Types::Mesh::GetPoint(Index index)
{
    return points.at(index);
}


bool Geometry::Types::Mesh::IsPointInFace(
        Index pointIndex, Index triangleIndex
)
{
    auto t = GetFace(triangleIndex);

    return std::find(t.begin(), t.end(), pointIndex) != t.end();
}


Geometry::Types::Face& Geometry::Types::Mesh::GetFace(Index index)
{
    return faces.at(index);
}


std::vector<Geometry::Types::Point3> Geometry::Types::Mesh::GetFacePoints(Index index)
{
    std::vector<Geometry::Types::Point3> facePoints;

    auto face = faces.at(index);

    for (auto pointIndex : face)
    {
        facePoints.push_back(points.at(pointIndex));
    }

    return facePoints;
}


std::vector<Geometry::Types::Face> Geometry::Types::Mesh::GetPointFaces(Index index)
{
    std::vector<Geometry::Types::Face> foundTriangles;

    for (auto t : GetFaces())
    {
        if (std::find(t.begin(), t.end(), index) != t.end())
        {
            foundTriangles.push_back(t);
        }
    }

    return foundTriangles;
}


Geometry::Types::Vector3 Geometry::Types::Mesh::GetFaceNormal(Index index)
{
    auto face = GetFace(index);

    auto triangles = face.ToTriangles();

    double sumAreas = 0.;
    Vector3 sumNormal;

    for (auto t : triangles)
    {
        auto p1 = GetPoint(t[0]);
        auto p2 = GetPoint(t[1]);
        auto p3 = GetPoint(t[2]);

        auto v1 = Vector3::FromPoints(p1, p2);
        auto v2 = Vector3::FromPoints(p2, p3);
        auto v3 = Vector3::FromPoints(p3, p1);

        auto cross1 = Vector3::CrossProduct(v1, v2);

        auto cross2 = Vector3::CrossProduct(v2, v3);

        auto cross3 = Vector3::CrossProduct(v3, v1);

        Vector3 normal = {
                (cross1.x + cross2.x + cross3.x) / 3.0,
                (cross1.y + cross2.y + cross3.y) / 3.0,
                (cross1.z + cross2.z + cross3.z) / 3.0,
        };

        double area = GetTriangleArea(Vector3::FromPoints(p1, p2),
                                      Vector3::FromPoints(p1, p3));

        sumAreas += area;

        sumNormal.Add(normal.Multiplied(area));
    }

    return sumNormal.Divided(sumAreas);
}


Geometry::Types::Vector3 Geometry::Types::Mesh::GetPointNormal(Index pointIndex)
{
    std::vector<Vector3> triangleNormals;

    for (Index triangleIndex = 0; triangleIndex < faces.size(); triangleIndex++)
    {
        if (IsPointInFace(pointIndex, triangleIndex))
        {
            triangleNormals.push_back(GetFaceNormal(triangleIndex));
        }
    }

    double x = 0;
    double y = 0;
    double z = 0;

    for (const auto& triangleNormal : triangleNormals)
    {
        x += triangleNormal.x;
        y += triangleNormal.y;
        z += triangleNormal.z;
    }

    auto count = (double)triangleNormals.size();

    Vector3 normal = {x / count, y / count, z / count};

    return normal.Normalized();
}


bool Geometry::Types::Mesh::operator==(const Mesh& other) const
{
    return points == other.points && faces == other.faces;
}





std::string Geometry::Types::MeshHandler::ApiName() const
{
    return "Geometry::Mesh";
}

void Geometry::Types::MeshHandler::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{
    auto mesh = std::any_cast<Mesh>(v);

    auto& val = value.SetArray();

    rapidjson::Value pnts(rapidjson::kArrayType);

    for (auto point : mesh.GetPoints())
    {
        rapidjson::Value pnt(rapidjson::kArrayType);
        pnt.PushBack(point.x, document.GetAllocator());
        pnt.PushBack(point.y, document.GetAllocator());
        pnt.PushBack(point.z, document.GetAllocator());

        pnts.PushBack(pnt, document.GetAllocator());
    }

    rapidjson::Value trngls(rapidjson::kArrayType);
    for (auto triangle : mesh.GetFaces())
    {
        rapidjson::Value tri(rapidjson::kArrayType);

        rapidjson::Value index0(rapidjson::kNumberType);
        index0.SetUint64(triangle[0]);

        rapidjson::Value index1(rapidjson::kNumberType);
        index1.SetUint64(triangle[1]);

        rapidjson::Value index2(rapidjson::kNumberType);
        index2.SetUint64(triangle[2]);

        tri.PushBack(index0, document.GetAllocator());
        tri.PushBack(index1, document.GetAllocator());
        tri.PushBack(index2, document.GetAllocator());

        trngls.PushBack(tri, document.GetAllocator());
    }

    val.PushBack(pnts, document.GetAllocator());
    val.PushBack(trngls, document.GetAllocator());
}

std::any Geometry::Types::MeshHandler::DeserializeValue(
        const std::any& v,
        rapidjson::Value& value
) const
{
    rapidjson::Value& pnts = value[0];
    rapidjson::Value& tris = value[1];

    PointArray points;
    for (size_t i = 0; i < pnts.Size(); i++)
    {
        auto& pnt = pnts[i];

        Point3 point = {
                pnt[0].GetDouble(),
                pnt[1].GetDouble(),
                pnt[2].GetDouble()
        };

        points.push_back(point);
    }

    FaceArray faces;
    for (size_t i = 0; i < tris.Size(); i++)
    {
        auto& tri = pnts[i];

        Face face;

        for (unsigned int idx = 0; idx < tri.Size(); idx++)
        {
            face.push_back(tri[idx].GetInt());
        }

        faces.push_back(face);
    }

    Mesh mesh(points, faces);

    return std::make_any<Mesh>(mesh);
}

void Geometry::Types::MeshHandler::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document
) const
{

}

std::any Geometry::Types::MeshHandler::DeserializeConstruction(
        rapidjson::Value& value
) const
{
    return InitValue();
}

std::any Geometry::Types::MeshHandler::InitValue() const
{
    Mesh mesh;
    return std::make_any<Mesh>(mesh);
}

bool Geometry::Types::MeshHandler::CompareValue(
        const std::any& v1, const std::any& v2
) const
{
    return std::any_cast<Mesh>(v1) == std::any_cast<Mesh>(v2);
}

std::any Geometry::Types::MeshHandler::FromPython(
        const boost::python::object& value
) const
{
    return InitValue();
}

boost::python::object Geometry::Types::MeshHandler::ToPython(
        const std::any& value
) const
{
    return {};
}

std::any Geometry::Types::MeshHandler::CopyValue(
        const std::any& source
) const
{
    return std::make_any<Mesh>(Mesh(std::any_cast<Mesh>(source)));
}

size_t Geometry::Types::MeshHandler::Hash() const
{
    return typeid(Mesh).hash_code();
}

size_t Geometry::Types::MeshHandler::ValueHash(
        const std::any& val
) const
{
    return 0;
}