#include "options.h"

#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/variantSets.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/cube.h"
#include "pxr/usd/usdGeom/xform.h"
#include "pxr/usd/sdf/layer.h"
#include "pxr/usd/usd/editContext.h"

#include "pxr/base/gf/half.h"

#include "Geom.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include "Tsys/tsys.h"
#include "Tsys/defaultTypes.h"
#include "Gex/include/Gex.h"

#include <filesystem>

#include "plugins/export.h"

#include "UsdTypes.h"


namespace UsdPlugin
{
    class Plugin_API OpenUsdStage: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::SdfLayerRefPtr>("OpenLayer", Gex::AttrValueType::Single,
                                            Gex::AttrType::Input);

            CreateAttribute<pxr::UsdStageRefPtr>("Stage", Gex::AttrValueType::Single,
                                            Gex::AttrType::Output);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
                      override
        {
            auto root  = context.GetAttribute("OpenLayer").GetValue<pxr::SdfLayerRefPtr>();

            if (!root)
                root = pxr::SdfLayer::CreateAnonymous();

            pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(root);

            context.GetAttribute("Stage").SetValue(stage);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(OpenUsdStageBuilder, OpenUsdStage)


    class Plugin_API GetUsdStageRootLayer: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdStageRefPtr>("Stage", Gex::AttrValueType::Single,
                                                 Gex::AttrType::Static);

            CreateAttribute<pxr::SdfLayerRefPtr>("RootLayer", Gex::AttrValueType::Single,
                                                 Gex::AttrType::Output);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
        override
        {
            auto stage  = context.GetAttribute("Stage").GetValue<pxr::UsdStageRefPtr>();

            if (!stage)
                return false;

            context.GetAttribute("RootLayer").SetValue(stage->GetRootLayer());
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(GetUsdStageRootLayerBuilder, GetUsdStageRootLayer)


    class Plugin_API GetUsdStagePrim: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdStageRefPtr>("Stage", Gex::AttrValueType::Single,
                                                 Gex::AttrType::Static);

            CreateAttribute<std::string>("Path", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);

            CreateAttribute<pxr::UsdPrim>("Prim", Gex::AttrValueType::Single,
                                          Gex::AttrType::Output);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
        override
        {
            auto stage  = context.GetAttribute("Stage").GetValue<pxr::UsdStageRefPtr>();
            auto path = context.GetAttribute("Path").GetValue<std::string>();
            if (!stage || path.empty())
                return false;

            pxr::SdfPath sdfp(path);
            pxr::UsdPrim prim = stage->GetPrimAtPath(sdfp);
            context.GetAttribute("Prim").SetValue(prim);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(GetUsdStagePrimBuilder, GetUsdStagePrim)


    class Plugin_API GetUsdPrimStage: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdPrim>("Prim", Gex::AttrValueType::Single,
                                          Gex::AttrType::Static);

            CreateAttribute<pxr::UsdStageRefPtr>("Stage", Gex::AttrValueType::Single,
                                                 Gex::AttrType::Output);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
        override
        {
            auto prim  = context.GetAttribute("Prim").GetValue<pxr::UsdPrim>();

            if (!prim)
                return false;

            context.GetAttribute("Stage").SetValue(prim.GetStage());
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(GetUsdPrimStageBuilder, GetUsdPrimStage)


    class Plugin_API GetUsdStagePrimAttribute: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdPrim>("Prim", Gex::AttrValueType::Single,
                                          Gex::AttrType::Input);

            CreateAttribute<std::string>("AttributeName", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);

            CreateAttribute<pxr::UsdAttribute>("Attribute", Gex::AttrValueType::Single,
                                              Gex::AttrType::Output);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
        override
        {
            auto prim  = context.GetAttribute("Prim").GetValue<pxr::UsdPrim>();

            if (!prim)
                return false;

            auto name = context.GetAttribute("AttributeName").GetValue<std::string>();
            auto attribute = prim.GetAttribute(pxr::TfToken(name));
            if (!attribute)
                return false;

            context.GetAttribute("Attribute").SetValue(attribute);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(GetUsdStagePrimAttributeBuilder, GetUsdStagePrimAttribute)


    struct SdfLayerHandler: public TSys::TypeHandler
    {
        std::string ApiName() const override
        {
            return "SdfLayer";
        }

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            auto stage = std::any_cast<pxr::SdfLayerRefPtr>(v);

            std::string stageStr;
            stage->ExportToString(&stageStr);

            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            value.PushBack(rapidjson::Value().SetString(stageStr.c_str(), document.GetAllocator()),
                           document.GetAllocator());

        }

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            std::string stageStr = value[1].GetString();

            pxr::SdfLayerRefPtr layer = pxr::SdfLayer::CreateAnonymous();
            layer->ImportFromString(stageStr);

            return std::make_any<pxr::SdfLayerRefPtr>(layer);
        }

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document)
        const override
        {
            return SerializeValue(v, value, document);
        }

        std::any DeserializeConstruction(rapidjson::Value& value)
        const override
        {
            return std::make_any<pxr::SdfLayerRefPtr>(pxr::SdfLayer::CreateAnonymous());
        }

        std::any InitValue() const override
        {
            pxr::SdfLayerRefPtr layer;
            return std::make_any<pxr::SdfLayerRefPtr>(layer);
        }

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            auto ptr1 = std::any_cast<pxr::SdfLayerRefPtr>(v1);
            auto ptr2 = std::any_cast<pxr::SdfLayerRefPtr>(v2);

            return (ptr1 == ptr2);
        }

        std::any FromPython(const pybind11::object& obj) const override
        {
            pxr::SdfLayerRefPtr layer = obj.cast<pxr::SdfLayerRefPtr>();
            return std::make_any<pxr::SdfLayerRefPtr>(layer);
        }

        pybind11::object ToPython(const std::any& v) const override
        {
            return pybind11::cast(std::any_cast<pxr::SdfLayerRefPtr>(v));
        }

        std::any CopyValue(const std::any& source) const override
        {
            return source;
        }

        size_t Hash() const override
        {
            return typeid(pxr::SdfLayerRefPtr).hash_code();
        }

        size_t ValueHash(const std::any& val) const override
        {
            auto st = std::any_cast<pxr::SdfLayerRefPtr>(val);

            std::string str;
            if (st)
            {
                st->ExportToString(&str);
            }

            auto h = std::hash<std::string>();
            return h(str);
        }
    };


    struct SdfPathToStr
    {
        [[nodiscard]]
        std::any operator()(const std::any& from, const std::any& to) const
        {
            return std::make_any<std::string>(std::any_cast<pxr::SdfPath>(from).GetString());
        }
    };


    struct StrToSdfPath
    {
        [[nodiscard]]
        std::any operator()(const std::any& from, const std::any& to) const
        {
            return std::make_any<pxr::SdfPath>(pxr::SdfPath(std::any_cast<std::string>(from)));
        }
    };


    struct SdfPathHandler: public TSys::TypeHandler
    {
        std::string ApiName() const override
        {
            return "SdfPath";
        }

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            auto path = std::any_cast<pxr::SdfPath>(v);


            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            value.PushBack(rapidjson::Value().SetString(path.GetString().c_str(), document.GetAllocator()),
                           document.GetAllocator());

        }

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            std::string stageStr = value[1].GetString();

            pxr::SdfPath path(stageStr);

            return std::make_any<pxr::SdfPath>(path);
        }

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document)
        const override
        {
            return SerializeValue(v, value, document);
        }

        std::any DeserializeConstruction(rapidjson::Value& value)
        const override
        {
            return InitValue();
        }

        std::any InitValue() const override
        {
            return std::make_any<pxr::SdfPath>(pxr::SdfPath());
        }

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            auto p1 = std::any_cast<pxr::SdfPath>(v1);
            auto p2 = std::any_cast<pxr::SdfPath>(v2);

            return (p1 == p2);
        }

        std::any FromPython(const pybind11::object& obj) const override
        {
            pxr::SdfPath layer = obj.cast<pxr::SdfPath>();
            return std::make_any<pxr::SdfPath>(layer);
        }

        pybind11::object ToPython(const std::any& v) const override
        {
            return pybind11::cast(std::any_cast<pxr::SdfPath>(v));
        }

        std::any CopyValue(const std::any& source) const override
        {
            return source;
        }

        size_t Hash() const override
        {
            return typeid(pxr::SdfPath).hash_code();
        }

        size_t ValueHash(const std::any& val) const override
        {
            auto st = std::any_cast<pxr::SdfPath>(val);

            auto h = std::hash<std::string>();
            return h(st.GetString());
        }
    };


    class Plugin_API CreateSdfLayer: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Identifier", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);

            CreateAttribute<pxr::SdfLayerRefPtr>("Layer", Gex::AttrValueType::Single,
                                            Gex::AttrType::Output);
            CreateAttribute<std::string>("RootPath", Gex::AttrValueType::Single,
                                          Gex::AttrType::Output);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
                      override
        {
            auto identifier  = context.GetAttribute("Identifier").GetValue<std::string>();

            pxr::SdfLayerRefPtr layer;
            if (identifier.empty()) {
                layer = pxr::SdfLayer::CreateAnonymous();
            }
            else {
                layer = pxr::SdfLayer::CreateNew(identifier);
            }

            context.GetAttribute("Layer").SetValue(layer);
            context.GetAttribute("RootPath").SetValue(std::string("/"));
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(CreateSdfLayerBuilder, CreateSdfLayer)


    class Plugin_API OpenSdfLayer: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Filepath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);

            CreateAttribute<pxr::SdfLayerRefPtr>("Layer", Gex::AttrValueType::Single,
                                                 Gex::AttrType::Output);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
        override
        {
            auto fp  = context.GetAttribute("Filepath").GetValue<std::string>();

            pxr::SdfLayerRefPtr layer  = pxr::SdfLayer::FindOrOpen(fp);

            context.GetAttribute("Layer").SetValue(layer);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(OpenSdfLayerBuilder, OpenSdfLayer)


    class Plugin_API DefinePrim: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Parent", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Name", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<pxr::UsdStageRefPtr>("Stage", Gex::AttrValueType::Single,
                                             Gex::AttrType::Static);

            CreateAttribute<pxr::UsdPrim>("OutPrim", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }


        virtual pxr::UsdPrim DoDefinePrim(pxr::UsdStageRefPtr stage, pxr::SdfPath path)
        {
            return stage->DefinePrim(path);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
        override
        {
            auto parent  = context.GetAttribute("Parent").GetValue<std::string>();
            auto name  = context.GetAttribute("Name").GetValue<std::string>();

            pxr::UsdStageRefPtr stage = context.GetAttribute("Stage").GetValue<pxr::UsdStageRefPtr>();

            pxr::SdfPath p(parent);
            p = p.AppendPath(pxr::SdfPath(name));

            auto prim = DoDefinePrim(stage, p);

            context.GetAttribute("OutPrim").SetValue<pxr::UsdPrim>(prim);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(DefinePrimBuilder, DefinePrim)


    class Plugin_API DefineXform: public DefinePrim
    {
    public:
        pxr::UsdPrim DoDefinePrim(pxr::UsdStageRefPtr stage, pxr::SdfPath path) override
        {
            return pxr::UsdGeomXform::Define(stage, path).GetPrim();
        }
    };


    GENERATE_DEFAULT_BUILDER(DefineXformBuilder, DefineXform)


    class Plugin_API DefineSphere: public DefinePrim
    {
    public:
        pxr::UsdPrim DoDefinePrim(pxr::UsdStageRefPtr stage, pxr::SdfPath path) override
        {
            return pxr::UsdGeomSphere::Define(stage, path).GetPrim();
        }
    };


    GENERATE_DEFAULT_BUILDER(DefineSphereBuilder, DefineSphere)


    class Plugin_API DefineCube: public DefinePrim
    {
    public:
        pxr::UsdPrim DoDefinePrim(pxr::UsdStageRefPtr stage, pxr::SdfPath path) override
        {
            return pxr::UsdGeomCube::Define(stage, path).GetPrim();
        }
    };


    GENERATE_DEFAULT_BUILDER(DefineCubeBuilder, DefineCube)


    class Plugin_API GetPrimPath: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdPrim>("Prim", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);
            CreateAttribute<std::string>("Path", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
        override
        {
            auto prim  = context.GetAttribute("Prim").GetValue<pxr::UsdPrim>();

            context.GetAttribute("Path").SetValue<std::string>(prim.GetPath().GetString());
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(GetPrimPathBuilder, GetPrimPath)


    class Plugin_API OverridePrim: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Parent", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Name", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<pxr::UsdStageRefPtr>("Stage", Gex::AttrValueType::Single,
                                                 Gex::AttrType::Static);

            CreateAttribute<pxr::UsdPrim>("Prim", Gex::AttrValueType::Single,
                                          Gex::AttrType::Output);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
        override
        {
            auto path  = context.GetAttribute("Parent").GetValue<std::string>();
            auto name  = context.GetAttribute("Name").GetValue<std::string>();

            pxr::SdfPath sdfp(path);
            sdfp = sdfp.AppendPath(pxr::SdfPath(name));

            std::string rp = sdfp.GetString();
            auto stage = context.GetAttribute("Stage").GetValue<pxr::UsdStageRefPtr>();
            if (!stage)
                return false;

            auto prim = stage->OverridePrim(sdfp);
            if (!prim)
                return false;

            context.GetAttribute("Prim").SetValue<pxr::UsdPrim>(prim);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(OverridePrimBuilder, OverridePrim)


    class Plugin_API AddReference: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("ReferencePath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<pxr::UsdPrim>("OverridePrim", Gex::AttrValueType::Single,
                                          Gex::AttrType::Static);
        }

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
                      override
        {
            auto prim = context.GetAttribute("OverridePrim").GetValue<pxr::UsdPrim>();
            auto reference = context.GetAttribute("ReferencePath").GetValue<std::string>();

            auto references = prim.GetReferences();
            references.AddReference(reference);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(AddReferenceBuilder, AddReference)


    class Plugin_API CreateVariantSet: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdPrim>("Prim", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);

            CreateAttribute<std::string>("VariantSet", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);

            CreateAttribute<std::string>("Variants", Gex::AttrValueType::Multi,
                                         Gex::AttrType::Static);
        }

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
                      override
        {
            auto primpath = context.GetAttribute("PrimPath").GetValue<std::string>();
            auto sets = context.GetAttribute("VariantSet").GetValue<std::string>();
            auto variants = context.GetAttribute("Variants");

            auto prim =  context.GetAttribute("Prim").GetValue<pxr::UsdPrim>();
            auto variantSet = prim.GetVariantSets().AddVariantSet(sets);

            for (unsigned int index : variants.Indices())
            {
                variantSet.AddVariant(variants.GetIndexValue<std::string>(index));
            }

            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(CreateVariantSetBuilder, CreateVariantSet)


    class Plugin_API EditVariantSet: public Gex::CompoundNode
    {
    public:
        void InitAttributes() override
        {
            auto primat = CreateAttribute<pxr::UsdPrim>("Prim", Gex::AttrValueType::Single,
                                                         Gex::AttrType::Static);

            auto variantsetat = CreateAttribute<std::string>("VariantSet", Gex::AttrValueType::Single,
                                                                  Gex::AttrType::Input);

            auto variantat = CreateAttribute<std::string>("Variant", Gex::AttrValueType::Single,
                                                      Gex::AttrType::Input);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler& profiler)
                      override
        {
            auto variantSetName = ctx.GetAttribute("VariantSet").GetValue<std::string>();
            auto variantName = ctx.GetAttribute("Variant").GetValue<std::string>();

            bool result = false;

            auto prim = ctx.GetAttribute("Prim").GetValue<pxr::UsdPrim>();
            if (prim)
            {
                auto variantSet = prim.GetVariantSets().GetVariantSet(variantSetName);

                if (variantSet)
                {
                    variantSet.SetVariantSelection(variantName);
                    {
                        pxr::UsdEditContext editContext(variantSet.GetVariantEditContext());

                        result = Gex::CompoundNode::Evaluate(ctx, graphContext, profiler);
                    }
                }
            }

            return result;
        }
    };


    GENERATE_DEFAULT_BUILDER(EditVariantSetBuilder, EditVariantSet)


    class Plugin_API SaveStage: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Filepath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);

            CreateAttribute<pxr::UsdStageRefPtr>("Stage", Gex::AttrValueType::Single,
                                            Gex::AttrType::Input);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
        override
        {
            auto filepath  = context.GetAttribute("Filepath").GetValue<std::string>();

            auto stage = context.GetAttribute("Stage").GetValue<pxr::UsdStageRefPtr>();

            std::filesystem::path directory = std::filesystem::path(filepath).parent_path();
            if (!std::filesystem::exists(directory))
                std::filesystem::create_directories(directory);

            stage->GetRootLayer()->Export(filepath);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(SaveStageBuilder, SaveStage)

    template<typename UsdType>
    class DefaultConstantNode: public Gex::Node
    {
        void InitAttributes() override
        {
            CreateAttribute<UsdType>("Value", Gex::AttrValueType::Single,
                                     Gex::AttrType::Static);
        }
    };


#define GENERATE_CONSTANT_NODE(UsdType, NodeType, BuilderName) \
    class Plugin_API NodeType: public DefaultConstantNode<UsdType>{}; \
    GENERATE_DEFAULT_BUILDER(BuilderName, NodeType)


    GENERATE_CONSTANT_NODE(pxr::GfHalf, GfHalfConstant, GfHalfConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::SdfTimeCode, SdfTimeCode, SdfTimeCodeBuilder)

    GENERATE_CONSTANT_NODE(pxr::TfToken, TfTokenConstant, TfTokenConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfMatrix2d, GfMatrix2dConstant, GfMatrix2dConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfMatrix3d, GfMatrix3dConstant, GfMatrix3dConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfMatrix4d, GfMatrix4dConstant, GfMatrix4dConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfQuatd, GfQuatdConstant, GfQuatdConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfQuatf, GfQuatfConstant, GfQuatfConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfQuath, GfQuathConstant, GfQuathConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec2f, GfVec2fConstant, GfVec2fConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec2d, GfVec2dConstant, GfVec2dConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec2i, GfVec2iConstant, GfVec2iConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec2h, GfVec2hConstant, GfVec2hConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec3f, GfVec3fConstant, GfVec3fConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec3d, GfVec3dConstant, GfVec3dConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec3i, GfVec3iConstant, GfVec3iConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec3h, GfVec3hConstant, GfVec3hConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec4f, GfVec4fConstant, GfVec4fConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec4d, GfVec4dConstant, GfVec4dConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec4i, GfVec4iConstant, GfVec4iConstantBuilder)

    GENERATE_CONSTANT_NODE(pxr::GfVec4h, GfVec4hConstant, GfVec4hConstantBuilder)


    template<typename T>
    bool ApplyValue(pxr::UsdAttribute& attribute, std::any value,
                    pxr::UsdTimeCode timecode=pxr::UsdTimeCode::Default())
    {
        try
        {
            const T set = std::any_cast<T>(value);
            return attribute.Set<T>(set, timecode);
        }
        catch(std::bad_any_cast&)
        {
            return false;
        }
    }


    bool NoOp(pxr::UsdAttribute& attribute, std::any value,
              pxr::UsdTimeCode timecode=pxr::UsdTimeCode::Default())
    {
        return true;
    }


    struct _SdfValueType
    {
        std::string typeName;
        pxr::SdfValueTypeName valueTypeName;
        std::function<bool(pxr::UsdAttribute&, std::any,
                           pxr::UsdTimeCode timecode)> attrApply;

        _SdfValueType(std::string sn, pxr::SdfValueTypeName vtn,
                      std::function<bool(pxr::UsdAttribute&, std::any,
                                         pxr::UsdTimeCode timecode)> aa)
        {
            typeName = sn;
            valueTypeName = vtn;
            attrApply = aa;
        }
    };


    std::map<unsigned int, _SdfValueType> _valueMap = {
            {0, _SdfValueType("Bool", pxr::SdfValueTypeNames->Bool,
                              ApplyValue<bool>)},
            {1, _SdfValueType("Int", pxr::SdfValueTypeNames->Int,
                              ApplyValue<int>)},
            {2, _SdfValueType("Float", pxr::SdfValueTypeNames->Float,
                              ApplyValue<float>)},
            {3, _SdfValueType("Double", pxr::SdfValueTypeNames->Double,
                              ApplyValue<double>)},
            {4, _SdfValueType("Timecode", pxr::SdfValueTypeNames->TimeCode,
                              NoOp)},
            {5, _SdfValueType("String", pxr::SdfValueTypeNames->String,
                              ApplyValue<std::string>)},
            {6, _SdfValueType("Token", pxr::SdfValueTypeNames->Token,
                              ApplyValue<pxr::TfToken>)},
            {7, _SdfValueType("Matrix2d", pxr::SdfValueTypeNames->Matrix2d,
                              ApplyValue<pxr::GfMatrix2d>)},
            {8, _SdfValueType("Matrix3d", pxr::SdfValueTypeNames->Matrix3d,
                               ApplyValue<pxr::GfMatrix3d>)},
            {9, _SdfValueType("Matrix4d", pxr::SdfValueTypeNames->Matrix4d,
                               ApplyValue<pxr::GfMatrix4d>)},
            {10, _SdfValueType("Quatd", pxr::SdfValueTypeNames->Quatd,
                               ApplyValue<pxr::GfQuatd>)},
            {11, _SdfValueType("Quatf", pxr::SdfValueTypeNames->Quatf,
                               ApplyValue<pxr::GfQuatf>)},
            {12, _SdfValueType("Quath", pxr::SdfValueTypeNames->Quath,
                               ApplyValue<pxr::GfQuath>)},
            {13, _SdfValueType("Double2", pxr::SdfValueTypeNames->Double2,
                               ApplyValue<pxr::GfVec2d>)},
            {14, _SdfValueType("Float2", pxr::SdfValueTypeNames->Float2,
                               ApplyValue<pxr::GfVec2f>)},
            {15, _SdfValueType("Half2", pxr::SdfValueTypeNames->Half2,
                               ApplyValue<pxr::GfVec2h>)},
            {16, _SdfValueType("Int2", pxr::SdfValueTypeNames->Int2,
                               ApplyValue<pxr::GfVec2i>)},
            {17, _SdfValueType("Double3", pxr::SdfValueTypeNames->Double3,
                               ApplyValue<pxr::GfVec3d>)},
            {18, _SdfValueType("Float3", pxr::SdfValueTypeNames->Float3,
                               ApplyValue<pxr::GfVec3f>)},
            {19, _SdfValueType("Half3", pxr::SdfValueTypeNames->Half3,
                               ApplyValue<pxr::GfVec3h>)},
            {20, _SdfValueType("Int3", pxr::SdfValueTypeNames->Int3,
                               ApplyValue<pxr::GfVec3i>)},
            {21, _SdfValueType("Double4", pxr::SdfValueTypeNames->Double4,
                               ApplyValue<pxr::GfVec4d>)},
            {22, _SdfValueType("Float4", pxr::SdfValueTypeNames->Float4,
                               ApplyValue<pxr::GfVec4f>)},
            {23, _SdfValueType("Half4", pxr::SdfValueTypeNames->Half4,
                               ApplyValue<pxr::GfVec4h>)},
            {24, _SdfValueType("Int4", pxr::SdfValueTypeNames->Int4,
                               ApplyValue<pxr::GfVec4i>)},
    };


    class Plugin_API UsdStagePrimCreateAttribute: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            TSys::Enum values;
            for (auto pair: _valueMap) {
                values.AddValue(pair.first, pair.second.typeName);
            }
            auto vals = std::make_any<TSys::Enum>(values);

            CreateAttribute<pxr::UsdPrim>("Prim", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);
            CreateAttribute<std::string>("AttributeName", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);
            CreateAttributeFromValue("AttributeType", vals, Gex::AttrValueType::Single,
                                     Gex::AttrType::Input);

            CreateAttribute<pxr::UsdAttribute>("Attribute", Gex::AttrValueType::Single,
                                              Gex::AttrType::Output);

        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
                      override
        {
            auto prim = context.GetAttribute("Prim").GetValue<pxr::UsdPrim>();
            if (!prim)
                return false;

            std::string name = context.GetAttribute("AttributeName").GetValue<std::string>();
            int index = context.GetAttribute("AttributeType").GetValue<TSys::Enum>().CurrentIndex();

            auto vtn  = _valueMap.at(index);

            auto attr = prim.CreateAttribute(pxr::TfToken(name), vtn.valueTypeName);
            if (!attr)
                return false;

            context.GetAttribute("Attribute").SetValue<pxr::UsdAttribute>(attr);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(UsdStagePrimCreateAttributeBuilder, UsdStagePrimCreateAttribute)


    class Plugin_API UsdStagePrimSetAttribute: public Gex::Node
    {
        void InitAttributes()
        {
            CreateAttribute<pxr::UsdAttribute>("Attribute", Gex::AttrValueType::Single,
                                                 Gex::AttrType::Static);
            CreateAttribute<TSys::AnyValue>("Value", Gex::AttrValueType::Single,
                                                       Gex::AttrType::Input);
        }


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext& graphContext,
                      Gex::NodeProfiler& profiler)
                      override
        {
            auto value = context.GetAttribute("Value").GetValue<TSys::AnyValue>();
            std::any inputValue = value.InputValue();

            bool result = false;
            auto attribute = context.GetAttribute("Attribute").GetValue<pxr::UsdAttribute>();
            if (attribute)
            {
                auto hash = inputValue.type().hash_code();
                auto typeHandle = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(hash);
                if (typeHandle)
                {
                    for (auto pair : _valueMap)
                    {
                        if (pair.second.typeName == typeHandle->ApiName())
                        {
                            result = pair.second.attrApply(attribute, inputValue,
                                                            pxr::UsdTimeCode::Default());
                            break;
                        }
                    }
                }

            }

            return result;
        }
    };


    GENERATE_DEFAULT_BUILDER(UsdStagePrimSetAttributeBuilder, UsdStagePrimSetAttribute)

//
//    class UsdViewOpen: public Gex::OpenCommand
//    {
//        using Gex::OpenCommand::OpenCommand;
//    public:
//        int OpenFilepath(const std::string& filepath) const override
//        {
//            std::string command = "usdview";
//
//            char* env = std::getenv("USD_VIEW");
//            if (env)
//                command = env;
//
//            return std::system((command + " " + filepath).c_str());
//        }
//    };
//
//
//    class UsdViewOpenBuilder: public Gex::CommandBuilder
//    {
//        Gex::OpenCommand* GenerateCommand(const std::string& identifier,
//                                                      const std::string& filename)
//                                                      const override
//        {
//            return new UsdViewOpen(identifier, filename);
//        }
//    };
}


REGISTER_PLUGIN(Gex::PluginLoader* loader)
{
    loader->RegisterTypeHandler<pxr::UsdStageRefPtr, UsdPlugin::UsdStageHandler>();

    loader->RegisterTypeHandler<pxr::SdfPath, UsdPlugin::SdfPathHandler>();

    TSys::TypeRegistry::GetRegistry()->GetTypeHandle("String")
            ->RegisterConverter<pxr::SdfPath, UsdPlugin::SdfPathToStr>();

    TSys::TypeRegistry::GetRegistry()->GetTypeHandle("SdfPath")
            ->RegisterConverter<std::string, UsdPlugin::StrToSdfPath>();

    loader->RegisterTypeHandler<pxr::SdfLayerRefPtr, UsdPlugin::SdfLayerHandler>();

    loader->RegisterTypeHandler<pxr::UsdPrim, UsdPlugin::UsdStagePrimHandler>();

    loader->RegisterTypeHandler<pxr::UsdAttribute, UsdPlugin::UsdStagePrimAttributeHandler>();

    loader->RegisterTypeHandler<pxr::SdfPath, UsdPlugin::SdfPathHandler>();

    loader->RegisterTypeHandler<pxr::GfHalf , UsdPlugin::GfHalfHandler>();

    loader->RegisterTypeHandler<pxr::SdfTimeCode, UsdPlugin::SdfTimeCodeHandler>();

    loader->RegisterTypeHandler<pxr::GfMatrix2d, UsdPlugin::GfMatrix2dHandler>();

    loader->RegisterTypeHandler<pxr::GfMatrix3d, UsdPlugin::GfMatrix3dHandler>();

    loader->RegisterTypeHandler<pxr::GfMatrix4d, UsdPlugin::GfMatrix4dHandler>();

    loader->RegisterTypeHandler<pxr::GfQuatd, UsdPlugin::GfQuatdHandler>();

    loader->RegisterTypeHandler<pxr::GfQuatf, UsdPlugin::GfQuatfHandler>();

    loader->RegisterTypeHandler<pxr::GfQuath, UsdPlugin::GfQuathHandler>();

    loader->RegisterTypeHandler<pxr::GfVec2d, UsdPlugin::GfVec2dHandler>();

    loader->RegisterTypeHandler<pxr::GfVec3d, UsdPlugin::GfVec3dHandler>();

    loader->RegisterTypeHandler<pxr::GfVec4d, UsdPlugin::GfVec4dHandler>();

    loader->RegisterTypeHandler<pxr::GfVec2f, UsdPlugin::GfVec2fHandler>();

    loader->RegisterTypeHandler<pxr::GfVec3f, UsdPlugin::GfVec3fHandler>();

    loader->RegisterTypeHandler<pxr::GfVec4f, UsdPlugin::GfVec4fHandler>();

    loader->RegisterTypeHandler<pxr::GfVec2i, UsdPlugin::GfVec2iHandler>();

    loader->RegisterTypeHandler<pxr::GfVec3i, UsdPlugin::GfVec3iHandler>();

    loader->RegisterTypeHandler<pxr::GfVec4i, UsdPlugin::GfVec4iHandler>();

    loader->RegisterTypeHandler<pxr::GfVec2h, UsdPlugin::GfVec2hHandler>();

    loader->RegisterTypeHandler<pxr::GfVec3h, UsdPlugin::GfVec3hHandler>();

    loader->RegisterTypeHandler<pxr::GfVec4h, UsdPlugin::GfVec4hHandler>();


    // Layer operations.
    loader->RegisterNode<UsdPlugin::OpenUsdStageBuilder>("Usd/Stage/OpenStage");

    loader->RegisterNode<UsdPlugin::GetUsdStageRootLayerBuilder>("Usd/Stage/GetRootLayer");

    loader->RegisterNode<UsdPlugin::GetUsdStagePrimBuilder>("Usd/Stage/GetPrim");

    loader->RegisterNode<UsdPlugin::GetPrimPathBuilder>("Usd/Prim/GetPath");

    loader->RegisterNode<UsdPlugin::GetUsdPrimStageBuilder>("Usd/Prim/GetStage");

    loader->RegisterNode<UsdPlugin::DefinePrimBuilder>("Usd/Prim/DefinePrim");

    loader->RegisterNode<UsdPlugin::DefineXformBuilder>("Usd/Prim/DefineXform");

    loader->RegisterNode<UsdPlugin::DefineSphereBuilder>("Usd/Prim/DefineSphere");

    loader->RegisterNode<UsdPlugin::GetUsdStagePrimAttributeBuilder>("Usd/Prim/GetAttribute");

    loader->RegisterNode<UsdPlugin::UsdStagePrimCreateAttributeBuilder>("Usd/Prim/CreateAttributeFromValue");

    loader->RegisterNode<UsdPlugin::UsdStagePrimSetAttributeBuilder>("Usd/Prim/SetAttribute");

    loader->RegisterNode<UsdPlugin::CreateSdfLayerBuilder>("Usd/Layers/CreateLayer");

    loader->RegisterNode<UsdPlugin::OpenSdfLayerBuilder>("Usd/Layers/OpenLayer");

    loader->RegisterNode<UsdPlugin::DefineCubeBuilder>("Usd/Prim/DefineCube");

    loader->RegisterNode<UsdPlugin::AddReferenceBuilder>("Usd/Stage/AddReference");

    loader->RegisterNode<UsdPlugin::OverridePrimBuilder>("Usd/Prim/OverridePrim");

    loader->RegisterNode<UsdPlugin::CreateVariantSetBuilder>("Usd/Prim/CreateVariantSet");

    loader->RegisterNode<UsdPlugin::EditVariantSetBuilder>("Usd/Prim/EditVariantSet");

    loader->RegisterNode<UsdPlugin::SaveStageBuilder>("Usd/Stage/SaveLayer");

    // Attribute operations.
    loader->RegisterNode<UsdPlugin::GfHalfConstantBuilder>("Usd/Constants/Half");

    loader->RegisterNode<UsdPlugin::SdfTimeCodeBuilder>("Usd/Constants/TimeCode");

    loader->RegisterNode<UsdPlugin::TfTokenConstantBuilder>("Usd/Constants/Token");

    loader->RegisterNode<UsdPlugin::GfMatrix2dConstantBuilder>("Usd/Constants/Matrix2d");

    loader->RegisterNode<UsdPlugin::GfMatrix3dConstantBuilder>("Usd/Constants/Matrix3d");

    loader->RegisterNode<UsdPlugin::GfMatrix4dConstantBuilder>("Usd/Constants/Matrix4d");

    loader->RegisterNode<UsdPlugin::GfQuatdConstantBuilder>("Usd/Constants/Quatd");

    loader->RegisterNode<UsdPlugin::GfQuatfConstantBuilder>("Usd/Constants/Quatf");

    loader->RegisterNode<UsdPlugin::GfQuathConstantBuilder>("Usd/Constants/Quath");

    loader->RegisterNode<UsdPlugin::GfVec2fConstantBuilder>("Usd/Constants/Float2");

    loader->RegisterNode<UsdPlugin::GfVec2dConstantBuilder>("Usd/Constants/Double2");

    loader->RegisterNode<UsdPlugin::GfVec2iConstantBuilder>("Usd/Constants/Int2");

    loader->RegisterNode<UsdPlugin::GfVec2hConstantBuilder>("Usd/Constants/Half2");

    loader->RegisterNode<UsdPlugin::GfVec3fConstantBuilder>("Usd/Constants/Float3");

    loader->RegisterNode<UsdPlugin::GfVec3dConstantBuilder>("Usd/Constants/Double3");

    loader->RegisterNode<UsdPlugin::GfVec3iConstantBuilder>("Usd/Constants/Int3");

    loader->RegisterNode<UsdPlugin::GfVec3hConstantBuilder>("Usd/Constants/Half3");

    loader->RegisterNode<UsdPlugin::GfVec4fConstantBuilder>("Usd/Constants/Float4");

    loader->RegisterNode<UsdPlugin::GfVec4dConstantBuilder>("Usd/Constants/Double4");

    loader->RegisterNode<UsdPlugin::GfVec4iConstantBuilder>("Usd/Constants/Int4");

    loader->RegisterNode<UsdPlugin::GfVec4hConstantBuilder>("Usd/Constants/Half4");



    loader->RegisterTypeHandler<pxr::UsdGeomMesh, UsdPlugin::UsdGeom::UsdGeomMeshType>();

    loader->RegisterTypeHandler<UsdPlugin::UsdGeom::PointList, UsdPlugin::UsdGeom::UsdGeomPoints>();

    loader->RegisterTypeHandler<UsdPlugin::UsdGeom::UsdTriangulatedMesh, UsdPlugin::UsdGeom::UsdTriangulatedMeshType>();



    loader->RegisterNode<UsdPlugin::UsdGeom::UsdGeomIsMeshBuilder>("Usd/Geom/IsMesh");

    loader->RegisterNode<UsdPlugin::UsdGeom::UsdGeomPrimToMeshBuilder>("Usd/Geom/PrimToMesh");

    loader->RegisterNode<UsdPlugin::UsdGeom::UsdGeomMakeTriangulatedMeshBuilder>("Usd/Geom/MakeTriangulatedMesh");

    loader->RegisterNode<UsdPlugin::UsdGeom::UsdGeomMovePointsBuilder>("Usd/Geom/MovePointsAlongNormal");

    loader->RegisterNode<UsdPlugin::UsdGeom::UvMapBuilder>("Usd/Geom/UvMap");

    loader->RegisterNode<UsdPlugin::UsdGeom::UsdGeomUvToPointBuilder>("Usd/Geom/UvToPoint");

    loader->RegisterNode<UsdPlugin::UsdGeom::UsdGeomUVNormalBuilder>("Usd/Geom/UsdGeomUVNormal");

    loader->RegisterNode<UsdPlugin::UsdGeom::UsdGeomRayIntersectBuilder>("Usd/Geom/UsdGeomRayIntersect");

//    loader->RegisterAppOpenCommand("usdview", new UsdPlugin::UsdViewOpenBuilder());
}