#ifndef GEX_MESH_H
#define GEX_MESH_H

#include <vector>

#include "Gex_plugins/MathPlugin/matrix.h"
#include "xform.h"

namespace Geometry
{
    struct MeshData
    {
        std::vector<double> vertices;
        std::vector<int> faceVerticesCount;
        std::vector<int> faceVertices;
    };


    struct MeshRenderData
    {
        std::vector<double> points;
    };


    struct Vertex
    {

    };

    struct Face
    {

    };


    class Mesh: public Xform
    {
        MeshData data;

        Matrix::Matrix4 transform;

    public:
        Mesh(const MeshData& data);

        MeshData GetData() const;

        void SetData(const MeshData& data);

        MeshRenderData GetRenderData() const;
    };
}


#endif //GEX_MESH_H
