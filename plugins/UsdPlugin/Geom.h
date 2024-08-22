#include "Tsys/include/tsys.h"
#include "Gex/include/Gex.h"

#include "pxr/usd/usdGeom/mesh.h"

#include "plugins/export.h"


namespace UsdPlugin
{

    struct UnsavableHandler: public TSys::TypeHandler
    {
        // Disable saving options.
        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override {};

        // Disable loading options.
        std::any DeserializeValue(std::any v,
                                  rapidjson::Value& value)
        const override
        {
            return InitValue();
        };


        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document)
        const override {};

        std::any DeserializeConstruction(
                rapidjson::Value& value)
        const override
        {
            return {};
        };
    };


    namespace UsdGeom
    {
        struct UsdGeomMeshType: public UnsavableHandler
        {
            // Initializes the value to an invalid Geom
            // mesh prim api.
            std::any InitValue() const override;

            // Checks whether the two usd geom mesh api
            // operates on the same prims.
            bool CompareValue(std::any v1, std::any v2) const override;

            //
            std::any FromPython(boost::python::object o) const override;

            boost::python::object ToPython(std::any) const override;

            std::any CopyValue(std::any source) const override;

            size_t Hash() const override;

            std::string Name() const override;

            std::string PythonName() const override;

            std::string ApiName() const override;

            size_t ValueHash(std::any val) const override;
        };


        struct Triangle
        {
            // Point index in the mesh points definitions.
            std::array<int, 3> pointsIndices;

            // Points within the faces vertices list
            // indices, that helps retrieving primvars
            // values that are stored in faces indices
            // such as uvs.
            std::array<int, 3> pointsFaceIndices;

            // Points position values.
            std::array<pxr::GfVec3f, 3> points;

            // Points uvs values.
            std::array<pxr::GfVec2f, 3> uvs;

            bool ComputeUvs(const pxr::UsdGeomPrimvarsAPI& api,
                            const std::string& uvPrimvarName);
        };


        class Plugin_API UsdTriangulatedMesh
        {
            pxr::UsdGeomMesh msh;
            std::vector<Triangle> triangles;
            bool isValid;

        public:
            UsdTriangulatedMesh();

            UsdTriangulatedMesh(pxr::UsdGeomMesh mesh);

            UsdTriangulatedMesh(pxr::UsdPrim prim);

            UsdTriangulatedMesh(const UsdTriangulatedMesh& other);

        private:
            void ComputeMesh();

        public:
            pxr::UsdGeomMesh GetMesh() const;

            bool IsValid() const;

            operator bool();

            bool ComputeUvs(const pxr::UsdGeomPrimvarsAPI& api,
                            const std::string& uvPrimvarName);
        };


        struct MeshToUsdTriangulatedMesh: public TSys::TypeConverter
        {
            std::any Convert(std::any from, std::any to) const;
        };


        struct UsdTriangulatedMeshType: public UnsavableHandler
        {
            UsdTriangulatedMeshType();

            // Initializes the value to an invalid Geom
            // mesh prim api.
            std::any InitValue() const override;

            // Checks whether the two usd geom mesh api
            // operates on the same prims.
            bool CompareValue(std::any v1, std::any v2) const override;

            //
            std::any FromPython(boost::python::object o) const override;

            boost::python::object ToPython(std::any) const override;

            std::any CopyValue(std::any source) const override;

            size_t Hash() const override;

            std::string Name() const override;

            std::string PythonName() const override;

            std::string ApiName() const override;

            size_t ValueHash(std::any val) const override;
        };


        class Plugin_API UsdGeomMakeTriangulatedMesh: public Gex::Node
        {
            void InitAttributes() override;

            bool Evaluate(Gex::NodeAttributeData &context,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler)
                          override;
        };


        GENERATE_DEFAULT_BUILDER(UsdGeomMakeTriangulatedMeshBuilder,
                                 UsdGeomMakeTriangulatedMesh)


        struct Face
        {
            pxr::VtArray<pxr::GfVec3f> points;
            pxr::VtArray<pxr::GfVec2f> uvs;
        };


        typedef std::vector<int> PointList;

        struct UsdGeomPoints: public TSys::TypeHandler
        {
            std::string ApiName() const override;

            std::string PythonName() const override;

            void SerializeValue(std::any v, rapidjson::Value& value,
                                rapidjson::Document& document) const override;

            std::any DeserializeValue(std::any v, rapidjson::Value& value) const override;

            void SerializeConstruction(std::any v, rapidjson::Value& value,
                                       rapidjson::Document& document) const override;

            std::any DeserializeConstruction(rapidjson::Value& value) const override;

            std::any InitValue() const override;

            std::any FromPython(boost::python::object) const override;

            boost::python::object ToPython(std::any) const override;

            std::any CopyValue(std::any source) const override;

            size_t ValueHash(std::any val) const override
            {
                return 0;
            }

            size_t Hash() const override
            {
                return typeid(PointList).hash_code();
            }

            std::string Name() const override
            {
                return typeid(PointList).name();
            }

            bool CompareValue(std::any, std::any) const override
            {
                return false;
            }
        };


        class Plugin_API UvMap: public Gex::Node
        {
            std::string Description() const override
            {
                return "Defines an uv map range "
                       "from specified Udim.";
            }

            void InitAttributes() override
            {
                CreateAttribute<int>("Udim", Gex::AttrValueType::Single,
                                     Gex::AttrType::Input);

                CreateAttribute<float>("MinU", Gex::AttrValueType::Single,
                                       Gex::AttrType::Output);

                CreateAttribute<float>("MinV", Gex::AttrValueType::Single,
                                       Gex::AttrType::Output);

                CreateAttribute<float>("MaxU", Gex::AttrValueType::Single,
                                       Gex::AttrType::Output);

                CreateAttribute<float>("MaxU", Gex::AttrValueType::Single,
                                       Gex::AttrType::Output);
            }

            bool Evaluate(Gex::NodeAttributeData &context,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override
            {
                float start = context.GetAttribute("Udim").GetValue<int>();

                context.GetAttribute("MinU").SetValue(start);
                context.GetAttribute("MinV").SetValue(start);
                context.GetAttribute("MaxU").SetValue(start + 1.0f);
                context.GetAttribute("MaxV").SetValue(start + 1.0f);

                return true;
            }
        };


        GENERATE_DEFAULT_BUILDER(UvMapBuilder,
                                 UvMap)


        class Plugin_API UsdGeomIsMesh: public Gex::Node
        {
            std::string Description() const override
            {
                return "Check whether specified prim "
                       "is a valid mesh prim.";
            }

            void InitAttributes() override;

            bool Evaluate(Gex::NodeAttributeData &context,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override;
        };


        GENERATE_DEFAULT_BUILDER(UsdGeomIsMeshBuilder,
                                 UsdGeomIsMesh)


        class Plugin_API UsdGeomPrimToMesh: public Gex::Node
        {
            std::string Description() const override
            {
                return "Converts specified prim to a Mesh prim.";
            }

            void InitAttributes() override;

            bool Evaluate(Gex::NodeAttributeData &context,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override;
        };


        GENERATE_DEFAULT_BUILDER(UsdGeomPrimToMeshBuilder,
                                 UsdGeomPrimToMesh)


        class Plugin_API UsdGeomGetPoints: public Gex::Node
        {


            void InitAttributes() override;

            bool Evaluate(Gex::NodeAttributeData &context,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override;
        };


        GENERATE_DEFAULT_BUILDER(UsdGeomGetPointsBuilder,
                                 UsdGeomGetPoints)


        class Plugin_API UsdGeomMovePointsAlongNormals: public Gex::Node
        {
            std::string Description() const override
            {
                return "Moves specified prim points along "
                       "normal on specified distance.";
            }

            void InitAttributes() override;

            bool Evaluate(Gex::NodeAttributeData &context,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override;
        };


        GENERATE_DEFAULT_BUILDER(UsdGeomMovePointsBuilder,
                                 UsdGeomMovePointsAlongNormals)


        class Plugin_API UsdGeomUvToPoint: public Gex::Node
        {
            std::string Description() const override
            {
                return "Converts specified uv coordinates "
                       "to local mesh point position. This "
                       "node only considers triangulated "
                       "faces at the moment.";
            }

            void InitAttributes() override;

            bool Evaluate(Gex::NodeAttributeData &context,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override;
        };


        GENERATE_DEFAULT_BUILDER(UsdGeomUvToPointBuilder,
                                 UsdGeomUvToPoint)


        class Plugin_API UsdGeomUVNormal: public Gex::Node
        {
            void InitAttributes() override;

            bool Evaluate(Gex::NodeAttributeData &context,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override;
        };


        GENERATE_DEFAULT_BUILDER(UsdGeomUVNormalBuilder,
                                 UsdGeomUVNormal)


        class Plugin_API UsdGeomRayIntersect: public Gex::Node
        {
            void InitAttributes() override;

            bool Evaluate(Gex::NodeAttributeData &context,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override;
        };


        GENERATE_DEFAULT_BUILDER(UsdGeomRayIntersectBuilder,
                                 UsdGeomRayIntersect)
    }

}
