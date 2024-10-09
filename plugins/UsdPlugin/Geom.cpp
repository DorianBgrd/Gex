#include "Geom.h"

#include <regex>
#include <math.h>

#include "pxr/usd/usdGeom/primvarsAPI.h"


UsdPlugin::UsdGeom::UsdGeomMeshType::UsdGeomMeshType(): UnsavableHandler()
{
    RegisterConverter<UsdTriangulatedMesh, UsdTriangulatedMeshToMesh>();
}


std::any UsdPlugin::UsdGeom::UsdGeomMeshType::InitValue() const
{
    return pxr::UsdGeomMesh();
}

bool UsdPlugin::UsdGeom::UsdGeomMeshType::CompareValue(std::any v1, std::any v2) const
{
        return (std::any_cast<pxr::UsdGeomMesh>(v1).GetPrim() ==
                std::any_cast<pxr::UsdGeomMesh>(v2).GetPrim());
}

std::any UsdPlugin::UsdGeom::UsdGeomMeshType::FromPython(boost::python::object o) const
{
    pxr::UsdGeomMesh fpy = boost::python::extract<pxr::UsdGeomMesh>(o);
    return fpy;
}


boost::python::object UsdPlugin::UsdGeom::UsdGeomMeshType::ToPython(std::any o) const
{
    return boost::python::object(std::any_cast<pxr::UsdGeomMesh>(o));
}


std::any UsdPlugin::UsdGeom::UsdGeomMeshType::CopyValue(std::any source) const
{
    return pxr::UsdGeomMesh(std::any_cast<pxr::UsdGeomMesh>(source));
}


size_t UsdPlugin::UsdGeom::UsdGeomMeshType::Hash() const
{
    return typeid(pxr::UsdGeomMesh).hash_code();
}


std::string UsdPlugin::UsdGeom::UsdGeomMeshType::Name() const
{
    return typeid(pxr::UsdGeomMesh).name();
}

std::string UsdPlugin::UsdGeom::UsdGeomMeshType::PythonName() const
{
    return "GeomMesh";
}

std::string UsdPlugin::UsdGeom::UsdGeomMeshType::ApiName() const
{
    return "pxr::UsdGeomMesh";
}

size_t UsdPlugin::UsdGeom::UsdGeomMeshType::ValueHash(std::any val) const
{
    return 0;
}


UsdPlugin::UsdGeom::Triangle::Triangle(const Triangle& other)
{
    pointsIndices = other.pointsIndices;
    pointsFaceIndices = other.pointsFaceIndices;
    points = other.points;
    uvs = other.uvs;
    uvMap = other.uvMap;
}


std::string UsdPlugin::UsdGeom::Triangle::UVMapName() const
{
    return uvMap;
}


bool UsdPlugin::UsdGeom::Triangle::ComputeUvs(const pxr::UsdGeomPrimvarsAPI& api,
                                              const std::string& uvPrimvarName)
{
    pxr::TfToken tok(uvPrimvarName);
    if (!api.HasPrimvar(tok))
        return false;

    auto primvar = api.GetPrimvar(tok);

    pxr::VtArray<pxr::GfVec2f> primvarUvs;
    if (!primvar.Get(&primvarUvs))
        return false;

    if (primvarUvs.empty())
        return false;

    uvs = {primvarUvs[pointsFaceIndices[0]],
           primvarUvs[pointsFaceIndices[1]],
           primvarUvs[pointsFaceIndices[2]]};

    uvMap = uvPrimvarName;
    return true;
}


float Vec2DLength(pxr::GfVec2f p1, pxr::GfVec2f p2)
{
    return std::sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) +
                     (p1[1] - p2[1]) * (p1[1] - p2[1]));
}


float TArea2D(pxr::GfVec2f p1, pxr::GfVec2f p2, pxr::GfVec2f p3)
{
    float a = Vec2DLength(p1, p2);
    float b = Vec2DLength(p2, p3);
    float c = Vec2DLength(p3, p1);
    float d = (a + b + c) / 2.0f;

    return std::sqrt(d * (d - a) * (d - b) * (d - c));
}


pxr::GfVec3f BCoord2D(pxr::GfVec2f point, pxr::GfVec2f p1, pxr::GfVec2f p2, pxr::GfVec2f p3)
{
    float mainArea = TArea2D(p1, p2, p3);

    float area1 = TArea2D(point, p2, p3);
    float area2 = TArea2D(p1, point, p3);
    float area3 = TArea2D(p1, p2, point);

    float darea1 = area1 / mainArea;
    float darea2 = area2 / mainArea;
    float darea3 = area3 / mainArea;

    return {darea1, darea2, darea3};
}


pxr::GfVec3f FromBCoord2D(pxr::GfVec3f bary, pxr::GfVec3f p1, pxr::GfVec3f p2, pxr::GfVec3f p3)
{
    return {
            (bary[0] * p1[0]) + (bary[1] * p2[0]) + (bary[2] * p3[0]),
            (bary[0] * p1[1]) + (bary[1] * p2[1]) + (bary[2] * p3[1]),
            (bary[0] * p1[2]) + (bary[1] * p2[2]) + (bary[2] * p3[2])
    };
}


pxr::GfVec3f UsdPlugin::UsdGeom::Triangle::UVsBaryCentricCoord(pxr::GfVec2f uv) const
{
    return BCoord2D(uv, uvs[0], uvs[1], uvs[2]);
}


float Round(float value, unsigned int decimals)
{
    int exp = std::pow(10, decimals);
    return std::round(value * exp) / exp;
}


bool IsIn2DTriangle(pxr::GfVec3f bary)
{
    return (bary[0] >= 0 && bary[0] <= 1 &&
            bary[1] >= 0 && bary[1] <= 1 &&
            bary[2] >= 0 && bary[2] <= 1 &&
            Round(bary[0] + bary[1] + bary[2], 3) == 1.0);
}


bool UsdPlugin::UsdGeom::Triangle::IsInUVsTriangle(pxr::GfVec2f uv) const
{
    return IsIn2DTriangle(UVsBaryCentricCoord(uv));
}


pxr::GfVec3f UsdPlugin::UsdGeom::Triangle::FromBaryCentricCoord(
        pxr::GfVec3f barycentric) const
{
    return FromBCoord2D(barycentric, points[0],
                        points[1], points[2]);
}


pxr::GfVec3f UsdPlugin::UsdGeom::Triangle::UvToMesh(pxr::GfVec2f uv) const
{
    return FromBaryCentricCoord(UVsBaryCentricCoord(uv));
}


pxr::GfVec3f UsdPlugin::UsdGeom::Triangle::GetNormal() const
{
    /*
    Nx = Ay * Bz - Az * By
    Ny = Az * Bx - Ax * Bz
    Nz = Ax * By - Ay * Bx
    */
    pxr::GfVec3f A = points[1] - points[0];
    pxr::GfVec3f B = points[2] - points[0];

    pxr::GfVec3f nrml = {(A[1] * B[2]) - (A[2] * B[1]),
                         (A[2] * B[0]) - (A[0] * B[2]),
                         (A[0] * B[1]) - (A[1] * B[0])};

    return nrml.GetNormalized();
}


UsdPlugin::UsdGeom::UsdTriangulatedMesh::UsdTriangulatedMesh()
{
    isValid = false;
}



UsdPlugin::UsdGeom::UsdTriangulatedMesh::UsdTriangulatedMesh(
        pxr::UsdGeomMesh mesh)
{
    msh = mesh;
    isValid = (bool)msh;

    if (isValid)
        ComputeMesh();
}


UsdPlugin::UsdGeom::UsdTriangulatedMesh::UsdTriangulatedMesh(
        pxr::UsdPrim prim): UsdTriangulatedMesh(pxr::UsdGeomMesh(prim))
{

}


UsdPlugin::UsdGeom::UsdTriangulatedMesh::UsdTriangulatedMesh(
        const UsdTriangulatedMesh& other)
{
    msh = other.msh;
    triangles = other.triangles;
    isValid = other.isValid;
    uvMap = other.uvMap;
}


void UsdPlugin::UsdGeom::UsdTriangulatedMesh::ComputeMesh()
{
    pxr::VtArray<pxr::GfVec3f> mshPoints;
    pxr::VtArray<int> faceIndices;
    pxr::VtArray<int> facesLength;

    msh.GetPointsAttr().Get(&mshPoints);
    msh.GetFaceVertexIndicesAttr().Get(&faceIndices);
    msh.GetFaceVertexCountsAttr().Get(&facesLength);

    int p = 0;
    for (int faceLength : facesLength)
    {
        if (faceLength > 3)
        {
            std::vector<std::array<int, 3>> pts;
//            pts.push_back({faceIndices[p], faceIndices[p + 1], faceIndices[p + 2]});
            pts.push_back({0, 1, 2});
            for (int i = 3; i < faceLength; i++)
            {
                pts.push_back({0, i - 1, i});
            }

            for (std::array<int, 3> idcs : pts)
            {
                int v1 = faceIndices[p + idcs[0]];
                int v2 = faceIndices[p + idcs[1]];
                int v3 = faceIndices[p + idcs[2]];

                Triangle t{};
                t.pointsFaceIndices = {v1, v2, v3};
                t.points = {mshPoints[v1], mshPoints[v2], mshPoints[v3]};
                t.pointsFaceIndices = {p + idcs[0], p + idcs[1], p + idcs[2]};

                triangles.push_back(t);
            }
        }
        else
        {
            int v1 = faceIndices[p];
            int v2 = faceIndices[p + 1];
            int v3 = faceIndices[p + 2];

            Triangle t{};
            t.pointsFaceIndices = {v1, v2, v3};
            t.points = {mshPoints[v1], mshPoints[v2], mshPoints[v3]};
            t.pointsFaceIndices = {p, p + 1, p + 2};

            triangles.push_back(t);
        }

        p += faceLength;
    }

}


pxr::UsdGeomMesh UsdPlugin::UsdGeom::UsdTriangulatedMesh::GetMesh() const
{
    return msh;
}


std::string UsdPlugin::UsdGeom::UsdTriangulatedMesh::UVMapName() const
{
    return uvMap;
};


bool UsdPlugin::UsdGeom::UsdTriangulatedMesh::IsValid() const
{
    return isValid;
}


UsdPlugin::UsdGeom::UsdTriangulatedMesh::operator bool()
{
    return isValid;
}


bool UsdPlugin::UsdGeom::UsdTriangulatedMesh::ComputeUvs(
        const std::string& uvPrimvarName)
{
    bool success = true;

    if (!msh)
        return false;

    pxr::UsdGeomPrimvarsAPI api(msh);
    for (auto& t : triangles)
    {
        if (!t.ComputeUvs(api, uvPrimvarName))
            success = false;
    }

    if (success)
        uvMap = uvPrimvarName;

    return success;
}


std::vector<UsdPlugin::UsdGeom::Triangle> UsdPlugin::UsdGeom::UsdTriangulatedMesh::GetTriangles() const
{
    return triangles;
}


std::any UsdPlugin::UsdGeom::MeshToUsdTriangulatedMesh::Convert(
        std::any from, std::any to) const
{
    return std::make_any<UsdTriangulatedMesh>(
            std::any_cast<pxr::UsdGeomMesh>(from));
}


std::any UsdPlugin::UsdGeom::UsdTriangulatedMeshToMesh::Convert(
        std::any from, std::any to) const
{
    return std::make_any<pxr::UsdGeomMesh>(
            std::any_cast<UsdTriangulatedMesh>(from)
                    .GetMesh());
}



UsdPlugin::UsdGeom::UsdTriangulatedMeshType::UsdTriangulatedMeshType():
        UnsavableHandler()
{
    RegisterConverter<pxr::UsdGeomMesh, MeshToUsdTriangulatedMesh>();
}


std::any UsdPlugin::UsdGeom::UsdTriangulatedMeshType::InitValue() const
{
    return std::make_any<UsdTriangulatedMesh>(
            UsdTriangulatedMesh());
}


std::string UsdPlugin::UsdGeom::UsdTriangulatedMeshType::ApiName() const
{
    return "UsdTriangulatedMesh";
}


bool UsdPlugin::UsdGeom::UsdTriangulatedMeshType::CompareValue(
        std::any v1, std::any v2) const
{
    return (std::any_cast<UsdTriangulatedMesh>(v1).GetMesh().GetPrim() ==
            std::any_cast<UsdTriangulatedMesh>(v2).GetMesh().GetPrim());
}


std::any UsdPlugin::UsdGeom::UsdTriangulatedMeshType::FromPython(
        boost::python::object o) const
{
    return {};
}


boost::python::object UsdPlugin::UsdGeom::UsdTriangulatedMeshType::ToPython(std::any) const
{
    return {};
}


std::any UsdPlugin::UsdGeom::UsdTriangulatedMeshType::CopyValue(std::any source) const
{
    return std::make_any<UsdTriangulatedMesh>(UsdTriangulatedMesh(
            std::any_cast<UsdTriangulatedMesh>(source)));
}


size_t UsdPlugin::UsdGeom::UsdTriangulatedMeshType::Hash() const
{
    auto h = typeid(UsdTriangulatedMesh).hash_code();
    return h;
}


std::string UsdPlugin::UsdGeom::UsdTriangulatedMeshType::Name() const
{
    return typeid(UsdTriangulatedMesh).name();
}


std::string UsdPlugin::UsdGeom::UsdTriangulatedMeshType::PythonName() const
{
    return ApiName();
}


size_t UsdPlugin::UsdGeom::UsdTriangulatedMeshType::ValueHash(std::any val) const
{
    return 0;
}



void UsdPlugin::UsdGeom::UsdGeomMakeTriangulatedMesh::InitAttributes()
{
    CreateAttribute<pxr::UsdGeomMesh>("Mesh", Gex::AttrValueType::Single,
                                      Gex::AttrType::Input);

    CreateAttribute<std::string>("UVMap", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);

    CreateAttribute<UsdTriangulatedMesh>("TriangulatedMesh", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
}


bool UsdPlugin::UsdGeom::UsdGeomMakeTriangulatedMesh::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto mesh = context.GetAttribute("Mesh").GetValue<pxr::UsdGeomMesh>();
    auto uvmap = context.GetAttribute("UVMap").GetValue<std::string>();

    UsdTriangulatedMesh tmesh(mesh);
    if (!uvmap.empty())
    {
        tmesh.ComputeUvs(uvmap);
    }

    return context.GetAttribute("TriangulatedMesh").SetValue(tmesh);
}



std::string UsdPlugin::UsdGeom::UsdGeomPoints::ApiName() const
{
    return "PointList";
}


std::string UsdPlugin::UsdGeom::UsdGeomPoints::PythonName() const
{
    return "";
}


void UsdPlugin::UsdGeom::UsdGeomPoints::SerializeValue(
        std::any v, rapidjson::Value& value,
        rapidjson::Document& document) const
{
    PointList pl = std::any_cast<PointList>(v);

    std::string apiname = ApiName();
    auto& namevalue = rapidjson::Value().SetString(apiname.c_str(), document.GetAllocator());
    value.PushBack(namevalue.Move(),document.GetAllocator());

    rapidjson::Value list(rapidjson::kArrayType);
    for (const auto v : pl)
    {
        list.PushBack(rapidjson::Value().SetInt(v).Move(),
                      document.GetAllocator());
    }
    value.PushBack(list.Move(),document.GetAllocator());
}


std::any UsdPlugin::UsdGeom::UsdGeomPoints::DeserializeValue(
        std::any v, rapidjson::Value& value) const
{
    PointList points;

    rapidjson::Value& list = value[1];
    for (unsigned int i = 0; i < list.Size(); i++)
    {
        points.push_back(list[i].GetInt());
    }

    return points;
}


void UsdPlugin::UsdGeom::UsdGeomPoints::SerializeConstruction(
        std::any v, rapidjson::Value& value,
        rapidjson::Document& document) const
{
    value.PushBack(
            rapidjson::Value().SetString(
                    ApiName().c_str(),
                    document.GetAllocator()
            ).Move(),
            document.GetAllocator()
    );
}


std::any UsdPlugin::UsdGeom::UsdGeomPoints::DeserializeConstruction(
        rapidjson::Value& value) const
{
    return InitValue();
}


std::any UsdPlugin::UsdGeom::UsdGeomPoints::InitValue() const
{
    return std::make_any<PointList>(PointList());
}


std::any UsdPlugin::UsdGeom::UsdGeomPoints::FromPython(
        boost::python::object o) const
{
    boost::python::list l(o);

    PointList pl;
    for (unsigned int i = 0; i < boost::python::len(l); i++)
    {
        int v = boost::python::extract<int>(l[i]);
        pl.push_back(v);
    }

    return pl;
}


boost::python::object UsdPlugin::UsdGeom::UsdGeomPoints::ToPython(
        std::any value) const
{
    PointList l = std::any_cast<PointList>(value);

    boost::python::list pyl;
    for (auto v : l)
    {
        pyl.append(v);
    }

    return pyl;
}


std::any UsdPlugin::UsdGeom::UsdGeomPoints::CopyValue(
        std::any source) const
{
    return std::make_any<PointList>(PointList(std::any_cast<PointList>(source)));
}



void UsdPlugin::UsdGeom::UsdGeomIsMesh::InitAttributes()
{
    CreateAttribute<pxr::UsdPrim>("Prim", Gex::AttrValueType::Single,
                                  Gex::AttrType::Input);
    CreateAttribute<bool>("IsAMesh", Gex::AttrValueType::Single,
                          Gex::AttrType::Output);
}


bool UsdPlugin::UsdGeom::UsdGeomIsMesh::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto prim = context.GetAttribute("Prim").GetValue<pxr::UsdPrim>();
    if (!prim)
    {
        return false;
    }

    context.GetAttribute("IsAMesh").SetValue((bool)pxr::UsdGeomMesh(prim));
    return true;
}



void UsdPlugin::UsdGeom::UsdGeomPrimToMesh::InitAttributes()
{
    CreateAttribute<pxr::UsdPrim>("Prim", Gex::AttrValueType::Single,
                                  Gex::AttrType::Input);
    CreateAttribute<pxr::UsdGeomMesh>("Mesh", Gex::AttrValueType::Single,
                                      Gex::AttrType::Output);
}


bool UsdPlugin::UsdGeom::UsdGeomPrimToMesh::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto prim = context.GetAttribute("Prim").GetValue<pxr::UsdPrim>();
    if (!prim)
    {
        return false;
    }

    context.GetAttribute("Mesh").SetValue(pxr::UsdGeomMesh(prim));
    return true;
}




void UsdPlugin::UsdGeom::UsdGeomGetPoints::InitAttributes()
{
    CreateAttribute<std::string>("Pattern", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);

    CreateAttribute<pxr::UsdGeomMesh>("Mesh", Gex::AttrValueType::Single,
                                 Gex::AttrType::Static);

    CreateAttribute<PointList>("Points", Gex::AttrValueType::Single,
                               Gex::AttrType::Output);
}


bool UsdPlugin::UsdGeom::UsdGeomGetPoints::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    std::string pattern = context.GetAttribute("Pattern")
            .GetValue<std::string>();

    pxr::UsdGeomMesh mesh = context.GetAttribute("Mesh")
            .GetValue<pxr::UsdGeomMesh>();

    auto pointAttr = mesh.GetPointsAttr();
    if (!pointAttr)
        return false;

    std::regex ranges("(?P<start>[0-9]+)( )-( )(?P<end>[0-9]+)");
    std::regex numbers("(?P<n>[0-9]+)[,$]");
    std::regex all("( )*( )");

    pxr::VtArray<pxr::GfVec3f> pnts;

    if (!pointAttr.Get(&pnts))
        return false;

    PointList vtx;
    if (std::regex_match(pattern, all))
    {
        for (unsigned int i = 0; i < pnts.size(); i++)
        {
            vtx.push_back(i);
        }

        return true;
    }

//    std::match_results<std::string> res;
//    auto searchNumbers = std::regex_search(pattern, res, numbers);
    return false;
}


void UsdPlugin::UsdGeom::UsdGeomMovePointsAlongNormals::InitAttributes()
{
    CreateAttribute<double>("Distance", Gex::AttrValueType::Single,
                            Gex::AttrType::Input);

    CreateAttribute<pxr::UsdGeomMesh>("Mesh", Gex::AttrValueType::Single,
                                      Gex::AttrType::Static);
}


bool UsdPlugin::UsdGeom::UsdGeomMovePointsAlongNormals::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto distance = context.GetAttribute("Distance").GetValue<double>();
    auto mesh = context.GetAttribute("Mesh").GetValue<pxr::UsdGeomMesh>();

    if (!mesh)
        return false;

    pxr::VtArray<pxr::GfVec3f> pnts;
    if (!mesh.CreatePointsAttr().Get(&pnts))
        return false;

    auto faceCnt = mesh.GetFaceCount();
    if (!faceCnt)
        return false;

    pxr::VtArray<int> faceVtxIdx;
    mesh.GetFaceVertexIndicesAttr().Get(&faceVtxIdx);

    pxr::VtArray<int> faceVtxCount;
    mesh.GetFaceVertexCountsAttr().Get(&faceVtxCount);

    pxr::VtArray<pxr::GfVec3f> faceVtxNormals;
    mesh.GetNormalsAttr().Get(&faceVtxNormals);

    // Computes normals for points.
    std::map<int, std::vector<pxr::GfVec3f>> nrmls;
    for (unsigned int i = 0; i < faceVtxIdx.size(); i++)
    {
        int idx = faceVtxIdx[i];
        pxr::GfVec3f nrml = faceVtxNormals[i];

        if (nrmls.find(idx) == nrmls.end())
        {
            nrmls[idx] = std::vector<pxr::GfVec3f>();
        }

        nrmls.at(idx).push_back(nrml);
    }

    pxr::VtArray<pxr::GfVec3f> destPnts;
    for (auto k : nrmls)
    {
        float sumX = 0;
        float sumY = 0;
        float sumZ = 0;
        for (auto n : k.second)
        {
            sumX += n[0];
            sumY += n[1];
            sumZ += n[2];
        }

        pxr::GfVec3f pntNrml(sumX / 3.0f, sumY / 3.0f, sumZ / 3.0f);

        destPnts.push_back(pnts[k.first] + (distance * pntNrml.GetNormalized()));
    }

    mesh.CreatePointsAttr().Set(destPnts);
    return true;
}



void UsdPlugin::UsdGeom::UsdGeomUvToPoint::InitAttributes()
{
    CreateAttribute<float>("U", Gex::AttrValueType::Single,
                           Gex::AttrType::Input);
    CreateAttribute<float>("V", Gex::AttrValueType::Single,
                           Gex::AttrType::Input);
    CreateAttribute<std::string>("UVPrimvar", Gex::AttrValueType::Single,
                                  Gex::AttrType::Input);
    CreateAttribute<UsdTriangulatedMesh>("Mesh", Gex::AttrValueType::Single,
                                      Gex::AttrType::Static);
    CreateAttribute<pxr::GfVec3f>("Position", Gex::AttrValueType::Single,
                                  Gex::AttrType::Output);

}


float Vec2DAngle(pxr::GfVec2f v1, pxr::GfVec2f v2)
{
    return std::acos((v1[0] * v2[0]) + (v2[1] * v2[1]));
}


bool UsdPlugin::UsdGeom::UsdGeomUvToPoint::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto mesh = context.GetAttribute("Mesh").GetValue<UsdTriangulatedMesh>();
    auto u = context.GetAttribute("U").GetValue<float>();
    auto v = context.GetAttribute("V").GetValue<float>();
    pxr::GfVec2f uvPoint(u, v);

    auto uvmap = context.GetAttribute("UVPrimvar").GetValue<std::string>();

    // Compute uv only if the uvmap is not the same.
    // TODO : Add a 'force' parameter ?
    if (mesh.UVMapName() != uvmap)
        mesh.ComputeUvs(uvmap);

    // Check for each faces if it contains the uv point.
    for (const Triangle& triangle : mesh.GetTriangles())
    {
        if (triangle.IsInUVsTriangle(uvPoint))
        {
            context.GetAttribute("Position").SetValue(
                    triangle.UvToMesh(uvPoint));
            return true;
        }
    }

    context.GetAttribute("Position").SetValue(pxr::GfVec3f(0, 0, 0));
    return true;
}


void UsdPlugin::UsdGeom::UsdGeomUVNormal::InitAttributes()
{
    CreateAttribute<UsdTriangulatedMesh>("Mesh", Gex::AttrValueType::Single,
                                      Gex::AttrType::Static);

    CreateAttribute<float>("U", Gex::AttrValueType::Single,
                           Gex::AttrType::Input);

    CreateAttribute<float>("V", Gex::AttrValueType::Single,
                           Gex::AttrType::Input);

    CreateAttribute<std::string>("UVPrimvar", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);

    CreateAttribute<pxr::GfVec3f>("Normal", Gex::AttrValueType::Single,
                                  Gex::AttrType::Output);
}


float Vec3fSum(const std::vector<pxr::GfVec3f>& vectors, int index)
{
    float value = 0;
    for (const pxr::GfVec3f& vec : vectors)
    {
        value += vec[index];
    }

    return value;
}


pxr::GfVec3f Vec3fAverage(std::vector<pxr::GfVec3f> vectors)
{
    float div = vectors.size();
    return {
        Vec3fSum(vectors, 0) / div,
        Vec3fSum(vectors, 1) / div,
        Vec3fSum(vectors, 2) / div
    };
}


bool UsdPlugin::UsdGeom::UsdGeomUVNormal::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    auto mesh = context.GetAttribute("Mesh").GetValue<UsdTriangulatedMesh>();
    auto u = context.GetAttribute("U").GetValue<float>();
    auto v = context.GetAttribute("V").GetValue<float>();
    pxr::GfVec2f uvPoint(u, v);

    auto uvmap = context.GetAttribute("UVPrimvar").GetValue<std::string>();

    if (mesh.UVMapName() != uvmap)
        mesh.ComputeUvs(uvmap);

    // Check for each faces if it contains the uv point,
    // if it does, then store the normal vector of the
    // analysed face.
    std::vector<pxr::GfVec3f> normals;
    auto triangles = mesh.GetTriangles();
    for (auto triangle : triangles)
    {
        if (triangle.IsInUVsTriangle(uvPoint))
        {
            normals.push_back(triangle.GetNormal());
        }
    }

    if (normals.empty())
    {
        context.GetAttribute("Normal").SetValue(
                pxr::GfVec3f(0, 0, 0));
        return true;
    }

    // Once the normals were stored, make an average
    // of those normals.
    context.GetAttribute("Normal").SetValue(
            Vec3fAverage(normals));
    return true;
}


void UsdPlugin::UsdGeom::UsdGeomRayIntersect::InitAttributes()
{
    CreateAttribute<pxr::UsdGeomMesh>("Mesh", Gex::AttrValueType::Single,
                                      Gex::AttrType::Static);

    CreateAttribute<pxr::GfVec3f>("Position", Gex::AttrValueType::Single,
                                  Gex::AttrType::Input);

    CreateAttribute<pxr::GfVec3f>("Direction", Gex::AttrValueType::Single,
                                  Gex::AttrType::Input);

    CreateAttribute<pxr::GfVec3f>("Intersection", Gex::AttrValueType::Single,
                                  Gex::AttrType::Output);
}


bool UsdPlugin::UsdGeom::UsdGeomRayIntersect::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler)
{
    return true;
}


