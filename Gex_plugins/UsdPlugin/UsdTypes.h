#ifndef GEX_USDTYPES_H
#define GEX_USDTYPES_H


#include "Tsys/tsys.h"


#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/variantSets.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/cube.h"
#include "pxr/usd/usdGeom/xform.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/sdf/layer.h"
#include "pxr/usd/usd/editContext.h"


#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"


#include <any>
#include <string>
#include <vector>


namespace UsdPlugin
{
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


    // Usd default types ----------------
    template<typename PxrType, typename CppType, typename JsonType>
    struct PxrGfToCppDefault: public TSys::TypeHandler
    {
        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            auto gfh = std::any_cast<PxrType>(v);
            auto dbl = static_cast<CppType>(gfh);

            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            value.PushBack<JsonType>(dbl, document.GetAllocator());
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
            return std::make_any<PxrType>(PxrType());
        }

        std::any InitValue() const override
        {
            return std::make_any<PxrType>(PxrType());
        }

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            return (std::any_cast<PxrType>(v1) == std::any_cast<PxrType>(v2));
        }

        std::any FromPython(const pybind11::object& obj) const override
        {
            auto value = obj.cast<PxrType>();
            return std::make_any<PxrType>(value);
        }

        pybind11::object ToPython(const std::any& v) const override
        {
            return pybind11::cast(std::any_cast<PxrType>(v));
        }

        std::any CopyValue(const std::any& source) const override
        {
            return std::make_any<PxrType>(std::any_cast<PxrType>(source));
        }

        size_t Hash() const override
        {
            return typeid(PxrType).hash_code();
        }

        size_t ValueHash(const std::any& val) const override
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

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
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

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            return std::make_any<pxr::SdfTimeCode>(pxr::SdfTimeCode(value.GetDouble()));
        }
    };


    struct TfTokenHandler: public TSys::TypeHandler
    {
        std::string ApiName() const override
        {
            return "Token";
        }

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            auto gfh = std::any_cast<pxr::TfToken>(v);

            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            value.PushBack(rapidjson::StringRef(gfh.GetString().c_str()),
                           document.GetAllocator());
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
            return std::make_any<pxr::TfToken>(pxr::TfToken());
        }

        std::any InitValue() const override
        {
            return std::make_any<pxr::TfToken>(pxr::TfToken());
        }

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            return (std::any_cast<pxr::TfToken>(v1) == std::any_cast<pxr::TfToken>(v2));
        }

        std::any FromPython(const pybind11::object& obj) const override
        {
            auto value = obj.cast<pxr::TfToken>();
            return std::make_any<pxr::TfToken>(value);
        }

        pybind11::object ToPython(const std::any& v) const override
        {
            return pybind11::cast(std::any_cast<pxr::TfToken>(v));
        }

        std::any CopyValue(const std::any& source) const override
        {
            return std::make_any<pxr::TfToken>(std::any_cast<pxr::TfToken>(source));
        }

        size_t Hash() const override
        {
            return typeid(pxr::TfToken).hash_code();
        }

        size_t ValueHash(const std::any& val) const override
        {
            auto h = std::hash<std::string>();
            return h(static_cast<std::string>(std::any_cast<pxr::TfToken>(val)));
        }
    };


    // Matrix -----------------------

    template<typename Matrix>
    struct GfMatrixHandler: public TSys::TypeHandler
    {
        virtual const int Rows() const
        {
            return 2;
        }

        virtual const int Columns() const
        {
            return 2;
        }

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override
        {
            value.PushBack(rapidjson::Value().SetString(ApiName().c_str(), document.GetAllocator()),
                           document.GetAllocator());

            SERIALIZE_MATRIX(value, document, std::any_cast<Matrix>(v),
                             Rows(), Columns())
        }

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            return DeserializeMatrix<Matrix>(Rows(), Columns(), value);
        }

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
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

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            return (std::any_cast<Matrix>(v1) == std::any_cast<Matrix>(v2));
        }

        std::any FromPython(const pybind11::object& python) const  override
        {
            Matrix value = python.cast<Matrix>();
            return std::make_any<Matrix>(value);
        }

        pybind11::object ToPython(const std::any& value) const override
        {
            return pybind11::cast(std::any_cast<Matrix>(value));
        }

        std::any CopyValue(const std::any& source) const override
        {
            return std::make_any<Matrix>(std::any_cast<Matrix>(source));
        }

        size_t Hash() const override
        {
            return typeid(Matrix).hash_code();
        }

        size_t ValueHash(const std::any& val) const override
        {
            return 0;
        }

    };


#define GF_MATRIX_HANDLER(structname, matrix, classname, pythonName, rows, columns) \
struct structname: public GfMatrixHandler<matrix> \
{ \
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
    struct GfQuatHandler: public TSys::TypeHandler
    {
        void SerializeValue(const std::any& v, rapidjson::Value& value,
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

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            return std::make_any<Quat>(Quat(value[0].Get<Def>(), value[1].Get<Def>(),
                                            value[2].Get<Def>(), value[3].Get<Def>()));
        }

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
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

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            return (std::any_cast<Quat>(v1) == std::any_cast<Quat>(v2));
        }

        std::any FromPython(const pybind11::object& python) const  override
        {
            Quat value = python.cast<Quat>();
            return std::make_any<Quat>(value);
        }

        pybind11::object ToPython(const std::any& value) const override
        {
            return pybind11::cast(std::any_cast<Quat>(value));
        }

        std::any CopyValue(const std::any& source) const override
        {
            return std::make_any<Quat>(std::any_cast<Quat>(source));
        }

        size_t Hash() const override
        {
            return typeid(Quat).hash_code();
        }

        size_t ValueHash(const std::any& val) const override
        {
            return 0;
        }
    };


#define GF_QUAT_HANDLER(typeName, defType, pxrType, pythonName) \
struct typeName: public GfQuatHandler<pxr::pxrType, defType> { \
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
    struct GfVecHandler: public TSys::TypeHandler
    {
        virtual const unsigned int Count() const
        {
            return 2;
        }

        void SerializeValue(const std::any& v, rapidjson::Value& value,
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

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            rapidjson::Value& jsonArray = value[1].GetArray();
            return std::make_any<Vec>(MakeVecBuilder<Vec, Def, Bld>(jsonArray));
        }

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
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

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            return (std::any_cast<Vec>(v1) == std::any_cast<Vec>(v2));
        }

        std::any FromPython(const pybind11::object& python) const  override
        {
            Vec value = python.cast<Vec>();
            return std::make_any<Vec>(value);
        }

        pybind11::object ToPython(const std::any& value) const override
        {
            return pybind11::cast(std::any_cast<Vec>(value));
        }

        std::any CopyValue(const std::any& source) const override
        {
            return std::make_any<Vec>(std::any_cast<Vec>(source));
        }

        size_t Hash() const override
        {
            return typeid(Vec).hash_code();
        }

        size_t ValueHash(const std::any& val) const override
        {
            return 0;
        }
    };


#define GF_VEC_HANDLER(typeName, defType, builder, pxrShortName, apiName, pythonName, count) \
struct typeName: public GfVecHandler<pxr::pxrShortName, defType, builder> \
{ \
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


    struct UsdStageHandler: public TSys::TypeHandler
    {
        std::string ApiName() const override
        {
            return "UsdStage";
        }

        void SerializeValue(const std::any& v, rapidjson::Value& value,
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

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            std::string stageStr = value[1].GetString();

            pxr::SdfLayerRefPtr layer = pxr::SdfLayer::FindOrOpen(stageStr);
            return std::make_any<pxr::UsdStageRefPtr>(pxr::UsdStage::Open(layer));
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
            return std::make_any<pxr::UsdStageRefPtr>(pxr::UsdStage::CreateInMemory());
        }

        std::any InitValue() const override
        {
            return std::make_any<pxr::UsdStageRefPtr>(pxr::UsdStage::CreateInMemory());
        }

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            auto ptr1 = std::any_cast<pxr::UsdStageRefPtr>(v1);
            auto ptr2 = std::any_cast<pxr::UsdStageRefPtr>(v2);

            return (ptr1 == ptr2);
        }

        std::any FromPython(const pybind11::object& obj) const override
        {
            pxr::UsdStageRefPtr stage = obj.cast<pxr::UsdStageRefPtr>();
            return std::make_any<pxr::UsdStageRefPtr>(stage);
        }

        pybind11::object ToPython(const std::any& v) const override
        {
            return pybind11::cast(std::any_cast<pxr::UsdStageRefPtr>(v));
        }

        std::any CopyValue(const std::any& source) const override
        {
            return source;
        }

        size_t Hash() const override
        {
            return typeid(pxr::UsdStageRefPtr).hash_code();
        }

        size_t ValueHash(const std::any& val) const override
        {
            auto st = std::any_cast<pxr::UsdStageRefPtr>(val);

            std::string str;
            st->GetRootLayer()->ExportToString(&str);

            auto h = std::hash<std::string>();
            return h(str);
        }
    };


    struct MeshToPrim
    {
        std::any operator()(const std::any& from, const std::any& to) const
        {
            return std::any_cast<pxr::UsdPrim>(
                    std::any_cast<pxr::UsdGeomMesh>(
                            from).GetPrim());
        }
    };


    struct UsdStagePrimHandler: public TSys::TypeHandler
    {
        UsdStagePrimHandler(): TSys::TypeHandler()
        {
            RegisterConverter<pxr::UsdGeomMesh, MeshToPrim>();
        }

        std::string ApiName() const override
        {
            return "UsdPrim";
        }

        void SerializeValue(const std::any& v, rapidjson::Value& value,
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

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            return std::make_any<pxr::UsdPrim>(pxr::UsdPrim());
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
            return std::make_any<pxr::UsdPrim>(pxr::UsdPrim());
        }

        std::any InitValue() const override
        {
            return std::make_any<pxr::UsdPrim>(pxr::UsdPrim());
        }

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            auto ptr1 = std::any_cast<pxr::UsdPrim>(v1);
            auto ptr2 = std::any_cast<pxr::UsdPrim>(v2);

            return (ptr1 == ptr2);
        }

        std::any FromPython(const pybind11::object& obj) const override
        {
            pxr::UsdPrim prim = obj.cast<pxr::UsdPrim>();
            return std::make_any<pxr::UsdPrim>(prim);
        }

        pybind11::object ToPython(const std::any& v) const override
        {
            return pybind11::cast(std::any_cast<pxr::UsdPrim>(v));
        }

        std::any CopyValue(const std::any& source) const override
        {
            return source;
        }

        size_t Hash() const override
        {
            return typeid(pxr::UsdPrim).hash_code();
        }

        size_t ValueHash(const std::any& val) const override
        {
            auto st = std::any_cast<pxr::UsdPrim>(val);

            std::string str = st.GetPath().GetString();

            auto h = std::hash<std::string>();
            return h(str);
        }
    };


    struct UsdStagePrimAttributeHandler: public TSys::TypeHandler
    {
        std::string ApiName() const override
        {
            return "UsdAttribute";
        }

        void SerializeValue(const std::any& v, rapidjson::Value& value,
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

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            return std::make_any<pxr::UsdAttribute>(pxr::UsdAttribute());
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
            return std::make_any<pxr::UsdAttribute>(pxr::UsdAttribute());
        }

        std::any InitValue() const override
        {
            return std::make_any<pxr::UsdAttribute>(pxr::UsdAttribute());
        }

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            auto ptr1 = std::any_cast<pxr::UsdAttribute>(v1);
            auto ptr2 = std::any_cast<pxr::UsdAttribute>(v2);

            return (ptr1 == ptr2);
        }

        std::any FromPython(const pybind11::object& obj) const override
        {
            pxr::UsdAttribute prim = obj.cast<pxr::UsdAttribute>();
            return std::make_any<pxr::UsdAttribute>(prim);
        }

        pybind11::object ToPython(const std::any& v) const override
        {
            return pybind11::cast(std::any_cast<pxr::UsdAttribute>(v));
        }

        std::any CopyValue(const std::any& source) const override
        {
            return source;
        }

        size_t Hash() const override
        {
            return typeid(pxr::UsdAttribute).hash_code();
        }

        size_t ValueHash(const std::any& val) const override
        {
            auto st = std::any_cast<pxr::UsdAttribute>(val);

            std::string str = st.GetPath().GetString();

            auto h = std::hash<std::string>();
            return h(str);
        }
    };


}

#endif //GEX_USDTYPES_H
