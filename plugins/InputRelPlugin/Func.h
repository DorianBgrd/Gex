#ifndef GEX_FUNC_H
#define GEX_FUNC_H

#include "plugins/export.h"

#include "Tsys/include/tsys.h"
#include "rapidjson/document.h"
#include "Gex/include/ptrs.h"


namespace Gex::InputRel
{
    class PointDelta;
    class Func;

    class Plugin_API CurvePoint: public std::enable_shared_from_this<CurvePoint>
    {
        double x = 0;
        double y = 0;
        Func* func;

    public:
        CurvePoint() = default;

        CurvePoint(Func* f);

        CurvePoint(double px, double py, Func* f);

        virtual ~CurvePoint() = default;

        double GetX() const;

        double GetY() const;

        void SetX(double x);

        void SetY(double y);

        Func* GetFunc() const;

        void SetFunc(Func* f);

        CurvePoint Move(double x, double y) const;

        bool operator <(const CurvePoint& other) const;

        bool operator >(const CurvePoint& other) const;

        CurvePoint operator +(const CurvePoint& other) const;

        virtual void Serialize(rapidjson::Value& value, rapidjson::Document& json) const;

        virtual void Deserialize(rapidjson::Value& value);

        virtual void Edit(const PointDelta* delta);

    protected:
        void RefreshFunc();
    };


    typedef std::shared_ptr<CurvePoint> CurvePointPtr;
    typedef Gex::BaseWkPtr<CurvePoint> CurvePointWk;


    struct Plugin_API PointCmp
    {
        bool operator()(
                const CurvePointPtr& l,
                const CurvePointPtr& r
        ) const;
    };


    typedef std::set<CurvePointPtr, PointCmp> PointsSet;


    class Plugin_API PointDelta
    {
        double x = 0;
        double y = 0;

    public:
        PointDelta() = default;

        double DeltaX() const;

        void SetDeltaX(double dx);

        double DeltaY() const;

        void SetDeltaY(double dy);

        void Apply(Func* f, const CurvePointPtr& point) const;

        virtual void EditPoint(CurvePoint* point) const;
    };


    class Plugin_API Func
    {
        friend CurvePoint;
        friend PointDelta;

        PointsSet points;

    public:
        Func() = default;

        CurvePointPtr PreviousPoint(double x) const;

        CurvePointPtr NextPoint(double x) const;

        virtual double Interpolate(double x, const CurvePointPtr& previous,
                                   const CurvePointPtr& next) const
                                   {return 0;};

        virtual double GetValue(double x) const;

        virtual CurvePointPtr CreatePoint(double x, double y);

        void AddPoint(double x, double y);

        void AddPoint(double x);

        void AddPoint(const CurvePointPtr& pnt);

        void AddPoint(const CurvePointWk& pnt);

        bool EditPoint(double x, double nx, double ny);

        bool EditPoint(double x, const PointDelta* delta);

        bool EditPoint(const CurvePointWk& p,
                       const PointDelta* delta);

        CurvePointPtr GetPoint(double x) const;

        bool RemovePoint(double x);

        CurvePointPtr TakePoint(double x);

        PointsSet Points() const;

        virtual void Serialize(rapidjson::Value& dict,
                               rapidjson::Document& json)
                               const;

        virtual void Deserialize(rapidjson::Value& dict);

    protected:
        bool Refresh(const CurvePointPtr& pnt);

    private:
        static bool registered;

    public:
        static bool RegisterPythonWrapper();
    };


    template<typename T>
    struct FuncWrapper: public TSys::TypeHandler
    {
        virtual std::string PythonName() const override
        {
            return ApiName();
        }

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document)
                            const override
        {
            std::shared_ptr<T> func = std::any_cast<std::shared_ptr<T>>(v);
            func->Serialize(value, document);
        }

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            std::shared_ptr<T> func = std::make_shared<T>();
            func->Deserialize(value);

            return std::make_any<std::shared_ptr<T>>(func);
        }

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document)
                                   const override
        {

        }

        std::any DeserializeConstruction(rapidjson::Value& value) const override
        {
            return {};
        }

        std::any InitValue() const override
        {
            return std::make_any<std::shared_ptr<T>>(std::make_shared<T>());
        }

        bool CompareValue(const std::any&, const std::any&) const override
        {
            return false;
        }

        std::any FromPython(const boost::python::object& v) const override
        {
//            std::shared_ptr<T> value = boost::python::extract<std::shared_ptr<T>>(v);
//            return std::make_any<std::shared_ptr<T>>(value);
            std::shared_ptr<T> p = boost::python::extract<std::shared_ptr<T>>(v);

            return std::make_any<std::shared_ptr<T>>(p);
        }

        boost::python::object ToPython(const std::any& v) const override
        {
            T::RegisterPythonWrapper();

            std::shared_ptr<T> p = std::any_cast<std::shared_ptr<T>>(v);

            return boost::python::object(p);
        }

        virtual std::any CopyValue(const std::any& source) const override
        {
            return std::make_any<std::shared_ptr<T>>(std::any_cast<std::shared_ptr<T>>(source));
        }

        virtual size_t Hash() const override
        {
            return typeid(std::shared_ptr<T>).hash_code();
        }

        virtual size_t ValueHash(const std::any& val) const override
        {
            return 0;
        }
    };


    class Plugin_API LinearFunc: public Func
    {
        double Interpolate(double x, const CurvePointPtr& previous,
                           const CurvePointPtr& next)
                           const override;
    };


    typedef std::shared_ptr<LinearFunc> LinearFuncPtr;


    struct LinearFuncHandler: public FuncWrapper<LinearFunc>
    {
        virtual std::string ApiName() const override
        {
            return "LinearFunc";
        }
    };

}

#endif //GEX_FUNC_H
