
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/variantSets.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/cube.h"
#include "pxr/usd/usdGeom/xform.h"
#include "pxr/usd/sdf/layer.h"
#include "pxr/usd/usd/editContext.h"

#include "pxr/base/gf/half.h"


#include "include/Gex.h"

#include <filesystem>


template<typename Matrix>
void SerializeMatrix(const int rows, const int columns,
                     rapidjson::Value& array,
                     rapidjson::Document& doc,
                     Matrix m)
{
    for (int r = 0; r < 2; r++)
        for (int c = 0; c < 2; c++)
            array.PushBack(m[r][c], doc.GetAllocator());
}


template<typename Matrix>
std::any DeserializeMatrix(int rows, int columns,
                           rapidjson::Value& value)
{
    auto array = value[1].GetArray();

    std::vector<std::vector<double>> doubleArray;
    for (int r = 0; r < rows; rows++)
    {
        std::vector<double> dbls;
        for (int c = 0; c < columns; c++)
        {
            dbls.push_back(array[r + c].GetDouble());
        }
        doubleArray.push_back(dbls);
    }

    return std::make_any<Matrix>(Matrix(doubleArray));
}



#define SERIALIZE_MATRIX(value, document, matrix, rows, columns) \
                         auto array = rapidjson::Value(rapidjson::kArrayType); \
                         SerializeMatrix((rows), (columns), (array), (document), (matrix)); \
                         (value).PushBack(array, document.GetAllocator());


namespace UsdPlugin
{
    // Usd default types ----------------
    template<typename PxrType, typename CppType, typename JsonType>
    struct PxrGfToCppDefault: public PipelineLibrary::TypeHandler
    {
        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            auto gfh = std::any_cast<PxrType>(v);
            auto dbl = static_cast<CppType>(gfh);

            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            value.PushBack<JsonType>(dbl, document.GetAllocator());
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document)
        const override
        {
            return SerializeValue(v, value, document);
        }

        std::any DeserializeConstruction(rapidjson::Value& value)
        const override
        {
            return std::make_any<PxrType>(PxrType());
        }

        std::any InitValue() const override
        {
            return std::make_any<PxrType>(PxrType());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            return (std::any_cast<PxrType>(v1) == std::any_cast<PxrType>(v2));
        }

        std::any FromPython(boost::python::object obj) const override
        {
            auto value = boost::python::extract<PxrType>(obj);
            return std::make_any<PxrType>(value);
        }

        boost::python::object ToPython(std::any v) const override
        {
            return boost::python::object(std::any_cast<PxrType>(v));
        }

        std::any CopyValue(std::any source) const override
        {
            return std::make_any<PxrType>(std::any_cast<PxrType>(source));
        }

        size_t Hash() const override
        {
            return typeid(PxrType).hash_code();
        }

        std::string Name() const override
        {
            return typeid(PxrType).name();
        }

        size_t ValueHash(std::any val) const override
        {
            auto h = std::hash<CppType>();
            return h(static_cast<CppType>(std::any_cast<PxrType>(val)));
        }
    };


    struct GfHalfHandler: public PxrGfToCppDefault<pxr::GfHalf, double, double>
    {
        std::string ApiName() const override
        {
            return "Half";
        }

        std::string PythonName() const override
        {
            return "Half";
        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            return std::make_any<pxr::GfHalf>(static_cast<pxr::GfHalf>(value.GetDouble()));
        }
    };


    struct SdfTimeCodeHandler: public PxrGfToCppDefault<pxr::SdfTimeCode, double, double>
    {
        std::string ApiName() const override
        {
            return "TimeCode";
        }

        std::string PythonName() const override
        {
            return "TimeCode";
        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            return std::make_any<pxr::SdfTimeCode>(pxr::SdfTimeCode(value.GetDouble()));
        }
    };


    struct TfTokenHandler: public PipelineLibrary::TypeHandler
    {
        std::string ApiName() const override
        {
            return "Token";
        }

        std::string PythonName() const override
        {
            return "Token";
        }

        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            auto gfh = std::any_cast<pxr::TfToken>(v);

            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            value.PushBack(rapidjson::StringRef(gfh.GetString().c_str()),
                           document.GetAllocator());
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document)
        const override
        {
            return SerializeValue(v, value, document);
        }

        std::any DeserializeConstruction(rapidjson::Value& value)
        const override
        {
            return std::make_any<pxr::TfToken>(pxr::TfToken());
        }

        std::any InitValue() const override
        {
            return std::make_any<pxr::TfToken>(pxr::TfToken());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            return (std::any_cast<pxr::TfToken>(v1) == std::any_cast<pxr::TfToken>(v2));
        }

        std::any FromPython(boost::python::object obj) const override
        {
            auto value = boost::python::extract<pxr::TfToken>(obj);
            return std::make_any<pxr::TfToken>(value);
        }

        boost::python::object ToPython(std::any v) const override
        {
            return boost::python::object(std::any_cast<pxr::TfToken>(v));
        }

        std::any CopyValue(std::any source) const override
        {
            return std::make_any<pxr::TfToken>(std::any_cast<pxr::TfToken>(source));
        }

        size_t Hash() const override
        {
            return typeid(pxr::TfToken).hash_code();
        }

        std::string Name() const override
        {
            return typeid(pxr::TfToken).name();
        }

        size_t ValueHash(std::any val) const override
        {
            auto h = std::hash<std::string>();
            return h(static_cast<std::string>(std::any_cast<pxr::TfToken>(val)));
        }
    };


    // Matrix -----------------------

    template<typename Matrix>
    struct GfMatrixHandler: public PipelineLibrary::TypeHandler
    {
        virtual const int Rows() const
        {
            return 2;
        }

        virtual const int Columns() const
        {
            return 2;
        }

        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document) const override
        {
            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            SERIALIZE_MATRIX(value, document, std::any_cast<Matrix>(v),
                             Rows(), Columns())
        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            return DeserializeMatrix<Matrix>(Rows(), Columns(), value);
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override
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
            return std::make_any<Matrix>(Matrix());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            return (std::any_cast<Matrix>(v1) == std::any_cast<Matrix>(v2));
        }

        std::any FromPython(boost::python::object python) const  override
        {
            Matrix value = boost::python::extract<Matrix>(python);
            return std::make_any<Matrix>(value);
        }

        boost::python::object ToPython(std::any value) const override
        {
            return boost::python::object(std::any_cast<Matrix>(value));
        }

        std::any CopyValue(std::any source) const override
        {
            return std::make_any<Matrix>(std::any_cast<Matrix>(source));
        }

        size_t Hash() const override
        {
            return typeid(Matrix).hash_code();
        }

        size_t ValueHash(std::any val) const override
        {
            return 0;
        }

    };


#define GF_MATRIX_HANDLER(structname, matrix, classname, pythonName, rows, columns) \
    struct structname: public GfMatrixHandler<matrix> \
    { \
        std::string Name() const override \
        { \
            return typeid(pxr::classname).name(); \
        }\
        std::string PythonName() const override \
        { \
            return #pythonName; \
        } \
        std::string ApiName() const override \
        { \
            return #classname; \
        } \
    };

    GF_MATRIX_HANDLER(GfMatrix2dHandler, pxr::GfMatrix2d, GfMatrix2d, Matrix2d, 2, 2)

    GF_MATRIX_HANDLER(GfMatrix3dHandler, pxr::GfMatrix3d, GfMatrix3d, Matrix3d, 3, 3)

    GF_MATRIX_HANDLER(GfMatrix4dHandler, pxr::GfMatrix4d, GfMatrix4d, Matrix4d, 4, 4)


    // Quats -----------------------

    template<typename Quat, typename Def>
    struct GfQuatHandler: public PipelineLibrary::TypeHandler
    {
        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document) const override
        {
            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            Quat quat = std::any_cast<Quat>(v);
            rapidjson::Value& array = rapidjson::Value().GetArray();

            array.PushBack<Def>(quat.GetReal(), document.GetAllocator());
            array.PushBack<Def>(quat.GetImaginary()[0], document.GetAllocator());
            array.PushBack<Def>(quat.GetImaginary()[1], document.GetAllocator());
            array.PushBack<Def>(quat.GetImaginary()[2], document.GetAllocator());

            value.PushBack(array, document.GetAllocator());
        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            return std::make_any<Quat>(Quat(value[0].Get<Def>(), value[1].Get<Def>(),
                                            value[2].Get<Def>(), value[3].Get<Def>()));
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override
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
            return std::make_any<Quat>(Quat());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            return (std::any_cast<Quat>(v1) == std::any_cast<Quat>(v2));
        }

        std::any FromPython(boost::python::object python) const  override
        {
            Quat value = boost::python::extract<Quat>(python);
            return std::make_any<Quat>(value);
        }

        boost::python::object ToPython(std::any value) const override
        {
            return boost::python::object(std::any_cast<Quat>(value));
        }

        std::any CopyValue(std::any source) const override
        {
            return std::make_any<Quat>(std::any_cast<Quat>(source));
        }

        size_t Hash() const override
        {
            return typeid(Quat).hash_code();
        }

        size_t ValueHash(std::any val) const override
        {
            return 0;
        }
    };


#define GF_QUAT_HANDLER(typeName, defType, pxrType, pythonName) \
    struct typeName: public GfQuatHandler<pxr::pxrType, defType> { \
        std::string Name() const override \
        { \
            return #pxrType; \
        } \
        std::string PythonName() const override \
        { \
            return #pythonName; \
        } \
        std::string ApiName() const override \
        { \
            return #pxrType; \
        } \
    };

    GF_QUAT_HANDLER(GfQuatdHandler, double, GfQuatd, Quatd)

    GF_QUAT_HANDLER(GfQuatfHandler, float, GfQuatf, Quatf)

    GF_QUAT_HANDLER(GfQuathHandler, float, GfQuath, Quath)


    // Vectors -----------------------
    template<typename pxrtype, typename cpptype>
    struct BuildVector
    {
        virtual  pxrtype Create(rapidjson::Value& array) const = 0;
    };


    template<typename pxrtype, typename cpptype>
    struct BuildVec2: public BuildVector<pxrtype, cpptype>
    {
        pxrtype Create(rapidjson::Value& array) const override
        {
            return pxrtype(array[0].Get<cpptype>(),
                           array[1].Get<cpptype>());
        }
    };

    template<typename pxrtype, typename cpptype>
    struct BuildVec3: public BuildVector<pxrtype, cpptype>
    {
        pxrtype Create(rapidjson::Value& array) const override
        {
            return pxrtype(array[0].Get<cpptype>(),
                           array[1].Get<cpptype>(),
                           array[2].Get<cpptype>());
        }
    };

    template<typename pxrtype, typename cpptype>
    struct BuildVec4: public BuildVector<pxrtype, cpptype>
    {
        pxrtype Create(rapidjson::Value& array) const override
        {
            return pxrtype(array[0].Get<cpptype>(),
                           array[1].Get<cpptype>(),
                           array[2].Get<cpptype>(),
                           array[3].Get<cpptype>());
        }
    };


    template<typename pxrtype, typename cpptype, template<typename t1, typename t2> class Bld>
    pxrtype MakeVecBuilder(rapidjson::Value& array)
    {
        Bld<pxrtype, cpptype> builder;

        return builder.Create(array);
    }

    template<typename Vec, typename Def, template<typename t1, typename t2> class Bld>
    struct GfVecHandler: public PipelineLibrary::TypeHandler
    {
         virtual const unsigned int Count() const
         {
            return 2;
         }

        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document) const override
        {
            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            Vec vec = std::any_cast<Vec>(v);
            rapidjson::Value& array = rapidjson::Value().GetArray();

            for (unsigned int i = 0; i < Count(); i++) {
                array.PushBack<Def>(vec[i], document.GetAllocator());
            }

            value.PushBack(array, document.GetAllocator());
        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            rapidjson::Value& jsonArray = value[1].GetArray();
            return std::make_any<Vec>(MakeVecBuilder<Vec, Def, Bld>(jsonArray));
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override
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
            return std::make_any<Vec>(Vec());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            return (std::any_cast<Vec>(v1) == std::any_cast<Vec>(v2));
        }

        std::any FromPython(boost::python::object python) const  override
        {
            Vec value = boost::python::extract<Vec>(python);
            return std::make_any<Vec>(value);
        }

        boost::python::object ToPython(std::any value) const override
        {
            return boost::python::object(std::any_cast<Vec>(value));
        }

        std::any CopyValue(std::any source) const override
        {
            return std::make_any<Vec>(std::any_cast<Vec>(source));
        }

        size_t Hash() const override
        {
            return typeid(Vec).hash_code();
        }

        size_t ValueHash(std::any val) const override
        {
            return 0;
        }
    };


#define GF_VEC_HANDLER(typeName, defType, builder, pxrShortName, apiName, pythonName, count) \
    struct typeName: public GfVecHandler<pxr::pxrShortName, defType, builder> \
    { \
        std::string Name() const override \
        { \
            return #pxrShortName; \
        } \
        std::string PythonName() const override \
        { \
            return #pythonName; \
        } \
        std::string ApiName() const override \
        { \
            return #apiName; \
        } \
        const unsigned int Count() const override \
        { \
            return count; \
        } \
    };


    GF_VEC_HANDLER(GfVec2fHandler, float, BuildVec2, GfVec2f, Float2, Vec2f, 2)

    GF_VEC_HANDLER(GfVec2dHandler, double, BuildVec2, GfVec2d, Double2, Vec2d, 2)

    GF_VEC_HANDLER(GfVec2iHandler, int, BuildVec2, GfVec2i, Vec2i, Int2, 2)

    GF_VEC_HANDLER(GfVec3fHandler, float, BuildVec3, GfVec3f, Vec3f, Float3, 3)

    GF_VEC_HANDLER(GfVec3dHandler, double, BuildVec3, GfVec3d, Vec3d, Double3, 3)

    GF_VEC_HANDLER(GfVec3iHandler, int, BuildVec3, GfVec3i, Vec3i, Int3, 3)

    GF_VEC_HANDLER(GfVec4fHandler, float, BuildVec4, GfVec4f, Vec4f, Float4, 4)

    GF_VEC_HANDLER(GfVec4dHandler, double, BuildVec4, GfVec4d, Vec4d, Double4, 4)

    GF_VEC_HANDLER(GfVec4iHandler, int, BuildVec4, GfVec4i, Vec4i, Int4, 4)


    // Specific treatment for half vectors.
    template<typename pxrtype, typename cpptype>
    struct BuildVecH2: public BuildVector<pxrtype, cpptype>
    {
        pxrtype Create(rapidjson::Value& array) const override
        {
            return pxrtype(array[0].Get<float>(),
                           array[1].Get<float>());
        }
    };

    template<typename pxrtype, typename cpptype>
    struct BuildVecH3: public BuildVector<pxrtype, cpptype>
    {
        pxrtype Create(rapidjson::Value& array) const override
        {
            return pxrtype(array[0].Get<float>(),
                           array[1].Get<float>(),
                           array[2].Get<float>());
        }
    };

    template<typename pxrtype, typename cpptype>
    struct BuildVecH4: public BuildVector<pxrtype, cpptype>
    {
        pxrtype Create(rapidjson::Value& array) const override
        {
            return pxrtype(array[0].Get<float>(),
                           array[1].Get<float>(),
                           array[2].Get<float>(),
                           array[3].Get<float>());
        }
    };


    GF_VEC_HANDLER(GfVec2hHandler, float, BuildVecH2, GfVec2h, Vec2h, Half2, 2)

    GF_VEC_HANDLER(GfVec3hHandler, float, BuildVecH3, GfVec3h, Vec3h, Half3, 3)

    GF_VEC_HANDLER(GfVec4hHandler, float, BuildVecH4, GfVec4h, Vec4h, Half4, 4)




    // ----------------------------------


    struct UsdStageHandler: public PipelineLibrary::TypeHandler
    {
        std::string ApiName() const override
        {
            return "UsdStage";
        }

        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
                            const override
        {
            auto stage = std::any_cast<pxr::UsdStageRefPtr>(v);

            std::string stageStr = "";
            if (stage)
                stageStr = stage->GetRootLayer()->GetIdentifier();

            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            value.PushBack(rapidjson::Value().SetString(stageStr.c_str(), document.GetAllocator()),
                           document.GetAllocator());

        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            std::string stageStr = value[1].GetString();

            pxr::SdfLayerRefPtr layer = pxr::SdfLayer::FindOrOpen(stageStr);
            return std::make_any<pxr::UsdStageRefPtr>(pxr::UsdStage::Open(layer));
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document)
                                   const override
        {
            return SerializeValue(v, value, document);
        }

        std::any DeserializeConstruction(rapidjson::Value& value)
                                         const override
        {
            return std::make_any<pxr::UsdStageRefPtr>(pxr::UsdStage::CreateInMemory());
        }

        std::any InitValue() const override
        {
            return std::make_any<pxr::UsdStageRefPtr>(pxr::UsdStage::CreateInMemory());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            auto ptr1 = std::any_cast<pxr::UsdStageRefPtr>(v1);
            auto ptr2 = std::any_cast<pxr::UsdStageRefPtr>(v2);

            return (ptr1 == ptr2);
        }

        std::any FromPython(boost::python::object obj) const override
        {
            pxr::UsdStageRefPtr stage = boost::python::extract<pxr::UsdStageRefPtr>(obj);
            return std::make_any<pxr::UsdStageRefPtr>(stage);
        }

        boost::python::object ToPython(std::any v) const override
        {
            return boost::python::object(std::any_cast<pxr::UsdStageRefPtr>(v));
        }

        std::any CopyValue(std::any source) const override
        {
            return source;
        }

        size_t Hash() const override
        {
            return typeid(pxr::UsdStageRefPtr).hash_code();
        }

        std::string Name() const override
        {
            return typeid(pxr::UsdStageRefPtr).name();
        }

        std::string PythonName() const override
        {
            return "Stage";
        }

        size_t ValueHash(std::any val) const override
        {
            auto st = std::any_cast<pxr::UsdStageRefPtr>(val);

            std::string str;
            st->GetRootLayer()->ExportToString(&str);

            auto h = std::hash<std::string>();
            return h(str);
        }
    };


    struct UsdStagePrimHandler: public PipelineLibrary::TypeHandler
    {
        std::string ApiName() const override
        {
            return "UsdPrim";
        }

        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            auto prim = std::any_cast<pxr::UsdPrim>(v);

            pxr::SdfPath stageStr = prim.GetPath();

            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            value.PushBack(rapidjson::Value().SetString(stageStr.GetString().c_str(), document.GetAllocator()),
                           document.GetAllocator());

        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            return std::make_any<pxr::UsdPrim>(pxr::UsdPrim());
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document)
        const override
        {
            return SerializeValue(v, value, document);
        }

        std::any DeserializeConstruction(rapidjson::Value& value)
        const override
        {
            return std::make_any<pxr::UsdPrim>(pxr::UsdPrim());
        }

        std::any InitValue() const override
        {
            return std::make_any<pxr::UsdPrim>(pxr::UsdPrim());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            auto ptr1 = std::any_cast<pxr::UsdPrim>(v1);
            auto ptr2 = std::any_cast<pxr::UsdPrim>(v2);

            return (ptr1 == ptr2);
        }

        std::any FromPython(boost::python::object obj) const override
        {
            pxr::UsdPrim prim = boost::python::extract<pxr::UsdPrim>(obj);
            return std::make_any<pxr::UsdPrim>(prim);
        }

        boost::python::object ToPython(std::any v) const override
        {
            return boost::python::object(std::any_cast<pxr::UsdPrim>(v));
        }

        std::any CopyValue(std::any source) const override
        {
            return source;
        }

        size_t Hash() const override
        {
            return typeid(pxr::UsdPrim).hash_code();
        }

        std::string Name() const override
        {
            return typeid(pxr::UsdPrim).name();
        }

        std::string PythonName() const override
        {
            return "Prim";
        }

        size_t ValueHash(std::any val) const override
        {
            auto st = std::any_cast<pxr::UsdPrim>(val);

            std::string str = st.GetPath().GetString();

            auto h = std::hash<std::string>();
            return h(str);
        }
    };


    struct UsdStagePrimAttributeHandler: public PipelineLibrary::TypeHandler
    {
        std::string ApiName() const override
        {
            return "UsdAttribute";
        }

        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            auto attr = std::any_cast<pxr::UsdAttribute>(v);

            pxr::SdfPath stageStr = attr.GetPath();

            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            value.PushBack(rapidjson::Value().SetString(stageStr.GetString().c_str(), document.GetAllocator()),
                           document.GetAllocator());

        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            return std::make_any<pxr::UsdAttribute>(pxr::UsdAttribute());
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document)
        const override
        {
            return SerializeValue(v, value, document);
        }

        std::any DeserializeConstruction(rapidjson::Value& value)
        const override
        {
            return std::make_any<pxr::UsdAttribute>(pxr::UsdAttribute());
        }

        std::any InitValue() const override
        {
            return std::make_any<pxr::UsdAttribute>(pxr::UsdAttribute());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            auto ptr1 = std::any_cast<pxr::UsdAttribute>(v1);
            auto ptr2 = std::any_cast<pxr::UsdAttribute>(v2);

            return (ptr1 == ptr2);
        }

        std::any FromPython(boost::python::object obj) const override
        {
            pxr::UsdAttribute prim = boost::python::extract<pxr::UsdAttribute>(obj);
            return std::make_any<pxr::UsdAttribute>(prim);
        }

        boost::python::object ToPython(std::any v) const override
        {
            return boost::python::object(std::any_cast<pxr::UsdAttribute>(v));
        }

        std::any CopyValue(std::any source) const override
        {
            return source;
        }

        size_t Hash() const override
        {
            return typeid(pxr::UsdAttribute).hash_code();
        }

        std::string Name() const override
        {
            return typeid(pxr::UsdAttribute).name();
        }

        std::string PythonName() const override
        {
            return "Prim";
        }

        size_t ValueHash(std::any val) const override
        {
            auto st = std::any_cast<pxr::UsdAttribute>(val);

            std::string str = st.GetPath().GetString();

            auto h = std::hash<std::string>();
            return h(str);
        }
    };


    class OpenUsdStage: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::SdfLayerRefPtr>("OpenLayer", PipelineLibrary::AttrValueType::Single,
                                            PipelineLibrary::AttrType::Input);

            CreateAttribute<pxr::UsdStageRefPtr>("Stage", PipelineLibrary::AttrValueType::Single,
                                            PipelineLibrary::AttrType::Output);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
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


    class GetUsdStageRootLayer: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdStageRefPtr>("Stage", PipelineLibrary::AttrValueType::Single,
                                                 PipelineLibrary::AttrType::Static);

            CreateAttribute<pxr::SdfLayerRefPtr>("RootLayer", PipelineLibrary::AttrValueType::Single,
                                                 PipelineLibrary::AttrType::Output);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
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


    class GetUsdStagePrim: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdStageRefPtr>("Stage", PipelineLibrary::AttrValueType::Single,
                                                 PipelineLibrary::AttrType::Static);

            CreateAttribute<pxr::UsdPrim>("PrimPath", PipelineLibrary::AttrValueType::Single,
                                                 PipelineLibrary::AttrType::Output);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
        override
        {
            auto stage  = context.GetAttribute("Stage").GetValue<pxr::UsdStageRefPtr>();

            if (!stage)
                return false;

            context.GetAttribute("RootLayer").SetValue(stage->GetRootLayer());
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(GetUsdStagePrimBuilder, GetUsdStagePrim)


    class GetUsdPrimStage: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdPrim>("Prim", PipelineLibrary::AttrValueType::Single,
                                          PipelineLibrary::AttrType::Static);

            CreateAttribute<pxr::UsdStageRefPtr>("Stage", PipelineLibrary::AttrValueType::Single,
                                                 PipelineLibrary::AttrType::Output);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
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


    class GetUsdStagePrimAttribute: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdPrim>("Prim", PipelineLibrary::AttrValueType::Single,
                                          PipelineLibrary::AttrType::Input);

            CreateAttribute<std::string>("AttributeName", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Input);

            CreateAttribute<pxr::UsdAttribute>("Attribute", PipelineLibrary::AttrValueType::Single,
                                              PipelineLibrary::AttrType::Output);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
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


    struct SdfLayerHandler: public PipelineLibrary::TypeHandler
    {
        std::string ApiName() const override
        {
            return "SdfLayer";
        }

        void SerializeValue(std::any v, rapidjson::Value& value,
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

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            std::string stageStr = value[1].GetString();

            pxr::SdfLayerRefPtr layer = pxr::SdfLayer::CreateAnonymous();
            layer->ImportFromString(stageStr);

            return std::make_any<pxr::SdfLayerRefPtr>(layer);
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
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

        bool CompareValue(std::any v1, std::any v2) const override
        {
            auto ptr1 = std::any_cast<pxr::SdfLayerRefPtr>(v1);
            auto ptr2 = std::any_cast<pxr::SdfLayerRefPtr>(v2);

            return (ptr1 == ptr2);
        }

        std::any FromPython(boost::python::object obj) const override
        {
            pxr::SdfLayerRefPtr layer = boost::python::extract<pxr::SdfLayerRefPtr>(obj);
            return std::make_any<pxr::SdfLayerRefPtr>(layer);
        }

        boost::python::object ToPython(std::any v) const override
        {
            return boost::python::object(std::any_cast<pxr::SdfLayerRefPtr>(v));
        }

        std::any CopyValue(std::any source) const override
        {
            return source;
        }

        size_t Hash() const override
        {
            return typeid(pxr::SdfLayer).hash_code();
        }

        std::string Name() const override
        {
            return typeid(pxr::SdfLayer).name();
        }

        std::string PythonName() const override
        {
            return "Layer";
        }

        size_t ValueHash(std::any val) const override
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


    struct SdfPathToStr: public PipelineLibrary::TypeConverter
    {
        [[nodiscard]]
        std::any Convert(std::any from, std::any to) const override
        {
            return std::make_any<std::string>(std::any_cast<pxr::SdfPath>(from).GetString());
        }
    };


    struct StrToSdfPath: public PipelineLibrary::TypeConverter
    {
        [[nodiscard]]
        std::any Convert(std::any from, std::any to) const override
        {
            return std::make_any<pxr::SdfPath>(pxr::SdfPath(std::any_cast<std::string>(from)));
        }
    };


    struct SdfPathHandler: public PipelineLibrary::TypeHandler
    {
        std::string ApiName() const override
        {
            return "SdfPath";
        }

        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            auto path = std::any_cast<pxr::SdfPath>(v);


            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            value.PushBack(rapidjson::Value().SetString(path.GetString().c_str(), document.GetAllocator()),
                           document.GetAllocator());

        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            std::string stageStr = value[1].GetString();

            pxr::SdfPath path(stageStr);

            return std::make_any<pxr::SdfPath>(path);
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
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

        bool CompareValue(std::any v1, std::any v2) const override
        {
            auto p1 = std::any_cast<pxr::SdfPath>(v1);
            auto p2 = std::any_cast<pxr::SdfPath>(v2);

            return (p1 == p2);
        }

        std::any FromPython(boost::python::object obj) const override
        {
            pxr::SdfPath layer = boost::python::extract<pxr::SdfPath>(obj);
            return std::make_any<pxr::SdfPath>(layer);
        }

        boost::python::object ToPython(std::any v) const override
        {
            return boost::python::object(std::any_cast<pxr::SdfPath>(v));
        }

        std::any CopyValue(std::any source) const override
        {
            return source;
        }

        size_t Hash() const override
        {
            return typeid(pxr::SdfPath).hash_code();
        }

        std::string Name() const override
        {
            return typeid(pxr::SdfPath).name();
        }

        std::string PythonName() const override
        {
            return "Layer";
        }

        size_t ValueHash(std::any val) const override
        {
            auto st = std::any_cast<pxr::SdfPath>(val);

            auto h = std::hash<std::string>();
            return h(st.GetString());
        }
    };


    class CreateSdfLayer: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Identifier", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Input);

            CreateAttribute<pxr::SdfLayerRefPtr>("Layer", PipelineLibrary::AttrValueType::Single,
                                            PipelineLibrary::AttrType::Output);
            CreateAttribute<std::string>("RootPath", PipelineLibrary::AttrValueType::Single,
                                          PipelineLibrary::AttrType::Output);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
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


    class OpenSdfLayer: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Filepath", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Input);

            CreateAttribute<pxr::SdfLayerRefPtr>("Layer", PipelineLibrary::AttrValueType::Single,
                                                 PipelineLibrary::AttrType::Output);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
        override
        {
            auto fp  = context.GetAttribute("Filepath").GetValue<std::string>();

            pxr::SdfLayerRefPtr layer  = pxr::SdfLayer::FindOrOpen(fp);

            context.GetAttribute("Layer").SetValue(layer);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(OpenSdfLayerBuilder, OpenSdfLayer)


    class DefinePrim: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Parent", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Input);
            CreateAttribute<std::string>("Name", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Input);
            CreateAttribute<pxr::UsdStageRefPtr>("Stage", PipelineLibrary::AttrValueType::Single,
                                             PipelineLibrary::AttrType::Static);

            CreateAttribute<pxr::UsdPrim>("OutPrim", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Output);
        }


        virtual pxr::UsdPrim DoDefinePrim(pxr::UsdStageRefPtr stage, pxr::SdfPath path)
        {
            return stage->DefinePrim(path);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
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


    class DefineXform: public DefinePrim
    {
    public:
        pxr::UsdPrim DoDefinePrim(pxr::UsdStageRefPtr stage, pxr::SdfPath path) override
        {
            return pxr::UsdGeomXform::Define(stage, path).GetPrim();
        }
    };


    GENERATE_DEFAULT_BUILDER(DefineXformBuilder, DefineXform)


    class DefineSphere: public DefinePrim
    {
    public:
        pxr::UsdPrim DoDefinePrim(pxr::UsdStageRefPtr stage, pxr::SdfPath path) override
        {
            return pxr::UsdGeomSphere::Define(stage, path).GetPrim();
        }
    };


    GENERATE_DEFAULT_BUILDER(DefineSphereBuilder, DefineSphere)


    class DefineCube: public DefinePrim
    {
    public:
        pxr::UsdPrim DoDefinePrim(pxr::UsdStageRefPtr stage, pxr::SdfPath path) override
        {
            return pxr::UsdGeomCube::Define(stage, path).GetPrim();
        }
    };


    GENERATE_DEFAULT_BUILDER(DefineCubeBuilder, DefineCube)


    class GetPrimPath: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdPrim>("Prim", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Static);
            CreateAttribute<std::string>("Path", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Output);
        }

        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
        override
        {
            auto prim  = context.GetAttribute("Prim").GetValue<pxr::UsdPrim>();

            context.GetAttribute("Path").SetValue<std::string>(prim.GetPath().GetString());
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(GetPrimPathBuilder, GetPrimPath)


    class OverridePrim: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Parent", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Input);
            CreateAttribute<std::string>("Name", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Input);
            CreateAttribute<pxr::UsdStageRefPtr>("Stage", PipelineLibrary::AttrValueType::Single,
                                                 PipelineLibrary::AttrType::Static);
*
            CreateAttribute<pxr::UsdPrim>("Prim", PipelineLibrary::AttrValueType::Single,
                                          PipelineLibrary::AttrType::Output);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
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


    class AddReference: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("ReferencePath", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Input);
            CreateAttribute<pxr::UsdPrim>("OverridePrim", PipelineLibrary::AttrValueType::Single,
                                          PipelineLibrary::AttrType::Static);
        }

        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
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


    class CreateVariantSet: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<pxr::UsdPrim>("Prim", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Static);

            CreateAttribute<std::string>("VariantSet", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Static);

            CreateAttribute<std::string>("Variants", PipelineLibrary::AttrValueType::Multi,
                                         PipelineLibrary::AttrType::Static);
        }

        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
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


    class EditVariantSet: public PipelineLibrary::CompoundNode
    {
    public:
        void InitAttributes() override
        {
            CreateInternalAttribute<pxr::UsdPrim>("Prim", PipelineLibrary::AttrValueType::Single,
                                                PipelineLibrary::AttrType::Static);

            CreateInternalAttribute<std::string>("VariantSet", PipelineLibrary::AttrValueType::Single,
                                            PipelineLibrary::AttrType::Input);

            CreateInternalAttribute<std::string>("Variant", PipelineLibrary::AttrValueType::Single,
                                            PipelineLibrary::AttrType::Input);
        }

        bool Evaluate(PipelineLibrary::NodeAttributeData &ctx,
                      PipelineLibrary::GraphContext &graphContext)
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

                        result = PipelineLibrary::CompoundNode::Evaluate(ctx, graphContext);
                    }
                }
            }

            return result;
        }
    };


    GENERATE_DEFAULT_BUILDER(EditVariantSetBuilder, EditVariantSet)


    class SaveStage: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Filepath", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Static);

            CreateAttribute<pxr::UsdStageRefPtr>("Stage", PipelineLibrary::AttrValueType::Single,
                                            PipelineLibrary::AttrType::Input);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
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
    class DefaultConstantNode: public PipelineLibrary::Node
    {
        void InitAttributes() override
        {
            CreateAttribute<UsdType>("Value", PipelineLibrary::AttrValueType::Single,
                                     PipelineLibrary::AttrType::Static);
        }
    };


#define GENERATE_CONSTANT_NODE(UsdType, NodeType, BuilderName) \
    class NodeType: public DefaultConstantNode<UsdType>{}; \
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


    class UsdStagePrimCreateAttribute: public PipelineLibrary::Node
    {
    public:
        void InitAttributes() override
        {
            PipelineLibrary::Enum values;
            for (auto pair: _valueMap) {
                values.AddValue(pair.first, pair.second.typeName);
            }
            auto vals = std::make_any<PipelineLibrary::Enum>(values);

            CreateAttribute<pxr::UsdPrim>("Prim", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Static);
            CreateAttribute<std::string>("AttributeName", PipelineLibrary::AttrValueType::Single,
                                         PipelineLibrary::AttrType::Static);
            CreateAttributeFromValue("AttributeType", vals, PipelineLibrary::AttrValueType::Single,
                                     PipelineLibrary::AttrType::Input);

            CreateAttribute<pxr::UsdAttribute>("Attribute", PipelineLibrary::AttrValueType::Single,
                                              PipelineLibrary::AttrType::Output);

        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
                      override
        {
            auto prim = context.GetAttribute("Prim").GetValue<pxr::UsdPrim>();
            if (!prim)
                return false;

            std::string name = context.GetAttribute("AttributeName").GetValue<std::string>();
            int index = context.GetAttribute("AttributeType").GetValue<PipelineLibrary::Enum>().CurrentIndex();

            auto vtn  = _valueMap.at(index);

            auto attr = prim.CreateAttribute(pxr::TfToken(name), vtn.valueTypeName);
            if (!attr)
                return false;

            context.GetAttribute("Attribute").SetValue<pxr::UsdAttribute>(attr);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(UsdStagePrimCreateAttributeBuilder, UsdStagePrimCreateAttribute)


    class UsdStagePrimSetAttribute: public PipelineLibrary::Node
    {
        void InitAttributes()
        {
            CreateAttribute<pxr::UsdAttribute>("Attribute", PipelineLibrary::AttrValueType::Single,
                                                 PipelineLibrary::AttrType::Static);
            CreateAttribute<PipelineLibrary::AnyValue>("Value", PipelineLibrary::AttrValueType::Single,
                                                       PipelineLibrary::AttrType::Input);
        }


        bool Evaluate(PipelineLibrary::NodeAttributeData &context,
                      PipelineLibrary::GraphContext& graphContext)
                      override
        {
            auto value = context.GetAttribute("Value").GetValue<PipelineLibrary::AnyValue>();
            std::any inputValue = value.InputValue();

            bool result = false;
            auto attribute = context.GetAttribute("Attribute").GetValue<pxr::UsdAttribute>();
            if (attribute)
            {
                auto hash = inputValue.type().hash_code();
                auto typeHandle = PipelineLibrary::Context::GetAttributeConfig()->GetTypeHandle(hash);
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


    class UsdViewOpen: public PipelineLibrary::OpenCommand
    {
        using PipelineLibrary::OpenCommand::OpenCommand;
    public:
        int OpenFilepath(const std::string& filepath) const override
        {
            std::string command = "usdview";

            char* env = std::getenv("USD_VIEW");
            if (env)
                command = env;

            return std::system((command + " " + filepath).c_str());
        }
    };


    class UsdViewOpenBuilder: public PipelineLibrary::CommandBuilder
    {
        PipelineLibrary::OpenCommand* GenerateCommand(const std::string& identifier,
                                                      const std::string& filename)
                                                      const override
        {
            return new UsdViewOpen(identifier, filename);
        }
    };
}


REGISTER_PLUGIN(PipelineLibrary::PluginLoader* loader)
{
    loader->InitializeContext();



    loader->RegisterTypeHandler<pxr::UsdStageRefPtr, UsdPlugin::UsdStageHandler>();

    loader->RegisterTypeHandler<pxr::SdfPath, UsdPlugin::SdfPathHandler>();

    loader->GetContext()->GetAttributeConfig()->GetTypeHandleFromApiName("String")
            ->RegisterConverter<pxr::SdfPath, UsdPlugin::SdfPathToStr>();

    loader->GetContext()->GetAttributeConfig()->GetTypeHandleFromApiName("SdfPath")
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




    loader->RegisterSoftwareOpenCommand("usdview", new UsdPlugin::UsdViewOpenBuilder());
}