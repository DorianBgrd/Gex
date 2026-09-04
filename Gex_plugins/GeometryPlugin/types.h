#ifndef GEX_GEOMPLUGIN_TYPES_H
#define GEX_GEOMPLUGIN_TYPES_H


#include "Gex_plugins/MathPlugin/vector.h"
#include "Gex_plugins/MathPlugin/point.h"

#include "Tsys/tsys.h"


namespace Geometry::Types
{
    typedef Math::Point3 Point3;
    typedef Math::Vector3 Vector3;

    typedef size_t Index;


    void SerializeIndex(const TSys::TypedValuePtr& v,
                        rapidjson::Value& value,
                        rapidjson::Document& document);

    void DeserializeIndex(const TSys::TypedValuePtr& v,
                          rapidjson::Value& value,
                          rapidjson::Document& document);

    bool CompareValue(const TSys::TypedValuePtr&,
                      const TSys::TypedValuePtr&);


    class Face: public std::vector<Index>
    {
    public:
        using std::vector<Index>::vector;

        std::vector<Face> ToTriangles() const;
    };


    void SerializeFace(const TSys::TypedValuePtr & v,
                       rapidjson::Value& value,
                       rapidjson::Document& document);


    void DeserializeFace(const TSys::TypedValuePtr & v,
                         rapidjson::Value& value,
                         rapidjson::Document& document);


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

        size_t GetPointCount() const;

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


    void SerializeMesh(const TSys::TypedValuePtr& v,
                       rapidjson::Value& value,
                       rapidjson::Document& document);

    void DeserializeMesh(const TSys::TypedValuePtr& v,
                         rapidjson::Value& value,
                         rapidjson::Document& document);

}

#endif //GEX_GEOMPLUGIN_TYPES_H
