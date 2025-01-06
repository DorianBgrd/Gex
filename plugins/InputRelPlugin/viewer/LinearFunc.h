#ifndef GEX_LINEARFUNC_H
#define GEX_LINEARFUNC_H


#include "plugins/InputRelPlugin/Func.h"
#include "plugins/InputRelPlugin/Nodes.h"
#include "Gex/include/Gex.h"

#include "plugins/export.h"

#include "UiTsys/include/uitsys.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>

#include "FuncViewer.h"


namespace Gex::InputRel
{
    class LinearFuncHandle: public FuncViewerHandle
    {
    public:
        using FuncViewerHandle::FuncViewerHandle;

        QRectF boundingRect() const override;

        void paintHandle(
                QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                QWidget *widget=nullptr)
                override;
    };


    class LinearFuncDelegate: public FuncViewerDelegate
    {
    private:
        LinearFuncPtr func;

    public:
        void AddPoint(double x, double y) override;

        void InitFunction(const std::any& f) override;

        PointsSet GetPoints() const override;

        FuncViewerHandle* CreateHandle(
                const CurvePointWk& p,
                const FuncDelegatePtr& d,
                FuncScene* scene)
                override;

        bool EditPoint(
                const CurvePointWk& p,
                const QPointF& previous,
                const QPointF& next,
                int decimals
        ) override;

        void DrawCurve(
                QGraphicsPathItem* item,
                FuncScene* scene,
                int decimals
        ) const override;
    };


    struct LinearFuncDelegateCreator: public TypedDelegateCreator<LinearFuncDelegate> {};
}


#endif //GEX_LINEARFUNC_H
