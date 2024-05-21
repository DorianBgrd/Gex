#ifndef GEX_BEZIERFUNC_H
#define GEX_BEZIERFUNC_H

#include <map>
#include <set>
#include <vector>
#include <memory>

#include "plugins/export.h"
#include "plugins/InputRel/Func.h"

namespace Gex::InputRel
{
    struct Point;
    typedef std::shared_ptr<Point> PointPtr;
    typedef std::weak_ptr<Point> PointRef;

    struct BezierPoint;
    typedef std::shared_ptr<BezierPoint> BezierPointPtr;
    typedef std::weak_ptr<BezierPoint> BezierPointRef;


    struct Plugin_API Point
    {
    private:
        double x=0;
        double y=0;

    public:
        Point(double x, double y);

        void operator=(Point other);

        void Swap(const Point& other);

        double GetX() const;

        void SetX(double x);

        double GetY() const;

        void SetY(double y);

        void Move(double dx, double dy);

        static PointPtr NewPoint(double x, double y);
    };


    static Point operator*(double f, const Point& p);

    static Point operator+(const Point& p1, const Point& p2);

    static bool operator==(const Point& p1, const Point& p2);


    struct Plugin_API BezierPoint: public Point
    {
        PointPtr leftHandle;
        PointPtr rightHandle;

    public:
        BezierPoint(double x, double y, double lx, double ly,
                    double rx, double ry);

        PointRef LeftHandle() const;

        PointRef RightHandle() const;

        static BezierPointPtr NewBezierPoint(
                double x, double y, double lx, double ly,
                double rx, double ry);
    };


    struct Plugin_API BezierPointCompare
    {
        bool operator()(const BezierPointPtr& point1,
                        const BezierPointPtr& point2)
                        const
        {
            return (point1->GetX() < point2->GetX());
        }
    };

    typedef std::set<BezierPointPtr, BezierPointCompare> BezierPoints;

    struct Plugin_API BezierPointRefCompare
    {
        bool operator()(const BezierPointRef& point1,
                        const BezierPointRef& point2)
        const
        {
            return (point1.lock()->GetX() < point2.lock()->GetX());
        }
    };

    typedef std::set<BezierPointRef, BezierPointRefCompare> BezierPointRefs;


    enum class Side
    {
        Left,
        Right
    };


    class Plugin_API BezierFunc: public Func
    // Bezier curve acts as bezier curve segments of 3 points.
    // This means that adding a point automatically adds a
    // control point between the previous point and the
    // added one.
    {
        // Curve point list.
        BezierPoints points;

    public:
        BezierFunc();

        // Returns all control points.
        BezierPoints CurvePoints() const;

        BezierPointRefs CurvePointRefs() const;

        BezierPointRef PointAt(double x);

        BezierPointRef AddPoint(double x);

        void RemovePoint(double x);

    public:
        BezierPoints AdjacentPoints(const double& x, bool& foundPrev,
                                    bool& foundPost) const;

        double y(double x) const override;

        bool Serialize(rapidjson::Value& dict,
                       rapidjson::Document& json)
                       const override;

        bool Deserialize(rapidjson::Value& dict) override;
    };


    typedef std::shared_ptr<BezierFunc> BezierFuncPtr;


    struct BezierFuncHandler: public FuncWrapper<BezierFunc>
    {
        virtual std::string ApiName() const override
        {
            return "BezierFunc";
        }
    };
}

#endif //GEX_BEZIERFUNC_H
