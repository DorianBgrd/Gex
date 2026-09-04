#ifndef GEX_FUNC_H
#define GEX_FUNC_H

#include "Gex_plugins/export.h"

#include "Tsys/tsys.h"
#include "rapidjson/document.h"
#include "Gex/include/Gex/ptrs.h"


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
    void SerializeFunc(const TSys::TypedValuePtr& v,
                       rapidjson::Value& value,
                       rapidjson::Document& doc)
    {
        v->Get<std::shared_ptr<T>>()->Serialize(value, doc);
    }


    template<typename T>
    void DeserializeFunc(const TSys::TypedValuePtr& v,
                         rapidjson::Value& value,
                         rapidjson::Document& doc)
    {
       auto func =  std::make_shared<T>();

       func->Deserialize(value);

       v->Set(func);
    }



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
