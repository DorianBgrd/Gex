#include "Geom.h"

#include <regex>


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