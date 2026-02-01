#ifndef GEX_GEOMPLUGIN_TYPES_H
#define GEX_GEOMPLUGIN_TYPES_H


#include "MathPlugin/types.h"

#include "Tsys/include/tsys.h"


namespace Geometry::Types
{
    typedef Math::Types::Point3 Point3;
    typedef Math::Types::Vector3 Vector3;

    typedef size_t Index;


    class IndexHandler: public TSys::TypeHandler
    {
        std::string ApiName() const override;

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(const std::any&, const std::any&) const override;

        std::any FromPython(const boost::python::object&) const override;

        boost::python::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        size_t ValueHash(const std::any& val) const override;
    };


    class Face: public std::vector<Index>
    {
    public:
        using std::vector<Index>::vector;

        std::vector<Face> ToTriangles() const;
    };


    class FaceHandler: public TSys::TypeHandler
    {
        std::string ApiName() const override;

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(const std::any&, const std::any&) const override;

        std::any FromPython(const boost::python::object&) const override;

        boost::python::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        size_t ValueHash(const std::any& val) const override;
    };


    typedef std::vector<Point3> PointArray;
    typedef std::vector<Face> FaceArray;


    struct Distance
    {
        double dist;
    };


    class Mesh
    {
    private:
        PointArray points;
        FaceArray faces;

    public:
        Mesh() = default;

        Mesh(const PointArray& points,
             const FaceArray& triangles);

        Mesh(const Mesh& other);

        PointArray GetPoints() const;

        void SetPoints(const PointArray& points);

        FaceArray GetFaces() const;

        void SetFaces(const FaceArray& faces);

        Point3& GetPoint(Index index);

        bool IsPointInFace(Index pointIndex, Index faceIndex);

        std::vector<Face> GetPointFaces(Index index);

        Face& GetFace(Index index);

        std::vector<Point3> GetFacePoints(Index index);

        Vector3 GetFaceNormal(Index index);

        Vector3 GetPointNormal(Index pointIndex);

        Point3 GetClosestPoint(const Point3& point) const;

        double GetFaceClosestPoint(const Point3& point, Index face,
                                   Point3* result);

        bool Intersects(const Vector3& vector);

//        void Extrude(std::vector<int> triangleIndices, double distance);

        bool operator==(const Mesh& other) const;
    };


    class MeshHandler: public TSys::TypeHandler
    {
        std::string ApiName() const override;

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(const std::any&, const std::any&) const override;

        std::any FromPython(const boost::python::object&) const override;

        boost::python::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        size_t ValueHash(const std::any& val) const override;
    };
}

#endif //GEX_GEOMPLUGIN_TYPES_H
