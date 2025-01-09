#ifndef GEX_BEZIERFUNC_H
#define GEX_BEZIERFUNC_H
#include "Func.h"


namespace Gex::InputRel
{
    class BezierPoint: public CurvePoint
    {
    public:
        struct BezierHandle
        {
            BezierHandle() = default;

            BezierHandle(double x, double y)
            {
                hx = x;
                hy = y;
            }

            double hx = 0;
            double hy = 0;
        };

        typedef std::shared_ptr<BezierHandle> BezierHandlePtr;
        typedef Gex::BaseWkPtr<BezierHandle> BezierHandleWk;

    private:
        BezierHandlePtr leftHandle;
        BezierHandlePtr rightHandle;

    public:
        BezierPoint();

        BezierPoint(Func* f);

        BezierPoint(double x, double y,
                    double lhx, double lhy,
                    double rhx, double rhy,
                    Func* f
                    );

        BezierHandleWk LeftHandle() const;

        BezierHandleWk RightHandle() const;
    };


    typedef std::shared_ptr<BezierPoint> BezierPointPtr;
    typedef Gex::BaseWkPtr<BezierPoint> BezierPointWk;


    class BezierFunc: public Func
    {
        CurvePointPtr CreatePoint(double x, double y) override;

        double Interpolate(double x, const CurvePointPtr& previous,
                           const CurvePointPtr& next)
                           const override;
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
