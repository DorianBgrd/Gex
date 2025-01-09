#ifndef GEX_FUNC_VIEWER_H
#define GEX_FUNC_VIEWER_H


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
#include <QGraphicsSceneMouseEvent>

#include "ui/include/ui.h"
#include "Func.h"
#include "ui/include/BaseGraph.h"

#include <unordered_map>


#define USER_TYPE_START 10000


namespace Gex::InputRel
{

    double FromViewX(double value, int decimals);


    double FromViewY(double value, int decimals);


    double FromViewLength(double value, int decimals);


    double ToViewX(double value, int decimals);


    double ToViewY(double value, int decimals);


    double ToViewLength(double value, int decimals);


    class FuncViewerHandle;


    class FuncViewerDelegate;


    typedef std::shared_ptr<FuncViewerDelegate> FuncDelegatePtr;


    class FuncScene;
    class FuncViewerHandle;


    typedef QList<FuncViewerHandle*> HandleList;


    class Plugin_API FuncViewerDelegate:
            public std::enable_shared_from_this<FuncViewerDelegate>
    {
        std::any function;
        int decimals = 3;

    public:
        FuncViewerDelegate() = default;

        std::any Function() const;

        void SetFunction(const std::any& f);

        int Decimals() const;

        void SetDecimals(int decimals);

        virtual void AddPoint(double x, double y) = 0;

        virtual void InitFunction(const std::any& f);

        virtual PointsSet GetPoints() const = 0;

        FuncViewerHandle* CreateHandle(
                const CurvePointWk& p,
                FuncScene* scene);

        virtual FuncViewerHandle* CreateHandle(
                const CurvePointWk& p,
                const FuncDelegatePtr& delegate,
                FuncScene* scene) = 0;

        bool EditPoint(
                const CurvePointWk& p,
                const QPointF& previous,
                const QPointF& next);

        virtual bool EditPoint(
                const CurvePointWk& p,
                const QPointF& previous,
                const QPointF& next,
                int decimals
                ) = 0;

        void DrawPoints(
                FuncScene* scene,
                HandleList& outPoints
        );

        virtual void DrawPoints(
                FuncScene* scene,
                int decimals,
                HandleList& outPoints
                );

        virtual void DrawCurve(
                QPainterPath& path,
                FuncScene* scene,
                int decimals
                ) const = 0;

        void DrawCurve(
                QPainterPath& path,
                FuncScene* scene
                ) const;
    };


    typedef std::shared_ptr<FuncViewerDelegate> FuncDelegatePtr;


    class Plugin_API FuncViewerHandle: public QGraphicsObject
    {
        Q_OBJECT

        CurvePointWk point;
        FuncDelegatePtr delegate;
        FuncScene* scene;

        QPointF sourcePos;
        bool initialized = false;

    public:
        FuncViewerHandle(const CurvePointWk& point,
                         const FuncDelegatePtr& delegate,
                         FuncScene* scene);

        void Initialize();

        FuncDelegatePtr Delegate() const;

        CurvePointWk Point() const;

        void Update();

        QVariant itemChange(GraphicsItemChange change,
                            const QVariant &value) override;

        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget=nullptr) override;

        virtual void paintCoordinates(QPainter* painter);

        virtual void paintHandle(QPainter* painter,
                                 const QStyleOptionGraphicsItem *option,
                                 QWidget *widget=nullptr) = 0;

        void OnModified();
//        void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
//
//        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    };


    typedef QPointer<FuncViewerHandle> FuncViewerHandlePtr;


    struct Plugin_API FuncDelegateCreator
    {
        virtual FuncDelegatePtr CreateDelegate() const = 0;
    };


    template<class T>
    struct Plugin_API TypedDelegateCreator: public FuncDelegateCreator
    {
        FuncDelegatePtr CreateDelegate() const override
        {
            return std::make_shared<T>();
        }
    };


    typedef std::shared_ptr<FuncDelegateCreator> FuncDelegateCreatorPtr;


    class Plugin_API FuncDelegateFactory
    {
        std::unordered_map<size_t, FuncDelegateCreatorPtr> delegates;

        static FuncDelegateFactory* instance;

        static FuncDelegateCreatorPtr GetFactory(size_t hash);

        static FuncDelegateCreatorPtr GetFactory(const std::any& func)
        {
            return GetFactory(func.type().hash_code());
        }

    public:
        static FuncDelegatePtr GetDelegate(const std::any& func);

        static void RegisterDelegate(size_t hash, const FuncDelegateCreatorPtr& delegate);

        template<typename F, typename D>
        static void RegisterDelegate()
        {
            RegisterDelegate(typeid(F).hash_code(),
                             std::make_shared<D>());
        }

    };


    typedef std::function<void(const Gex::Ui::UiFeedback&)> UiFeedbackCb;


    class Plugin_API FuncScene: public QGraphicsScene
    {
        Q_OBJECT

        bool createMode = false;
        int decimals = 3;

        unsigned int feedbackIndex = 0;
        std::map<unsigned int, UiFeedbackCb> feedbackCallbacks;

        friend FuncViewerHandle;

    public:
        FuncScene(qreal x, qreal y, qreal width,
                  qreal height, QObject* parent);

        ~FuncScene() override;

        unsigned int AddFeedbackCallback(const UiFeedbackCb& cb);

        bool RemoveFeedbackCallback(unsigned int index);

        void EmitMessage(const Gex::Ui::UiFeedback& m);

        bool CreateMode() const;

        void SetCreateMode(bool mode);

        int Decimals() const;

        void SetDecimals(int decimals);

        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

        Q_SIGNAL void PointRequested(QPointF point);

        QList<FuncViewerHandlePtr> SelectedHandles() const;

    protected:
        void OnSelectedPointsModified();

    public:
        Q_SIGNAL void PointsModified(const QList<FuncViewerHandlePtr>& pnts);

//        void drawBackground(QPainter *painter, const QRectF &rect) override;
    };


    class Plugin_API FuncView: public Gex::Ui::BaseGraphView
    {
        Q_OBJECT

        double tick = 0;
        QPainterPath path;
        QGraphicsPathItem* curveItem;
        FuncDelegatePtr delegate;
        int decimals = 3;
        bool createMode = false;

        HandleList handles;

        FuncScene* funcScene;
    public:
        FuncView(FuncScene* scene, QWidget* parent, bool toolbar);

        ~FuncView() override;

        double Tick() const;

        void SetTick(double t);

        void AddPoint(QPointF point);

        void SetFunction(const std::any& func);

        bool CreateMode() const;

        void SetCreateMode(bool mode);

        void ActivateCreateMode();

        void Draw();

        void DrawPoints();

        void DrawCurve();

        void AutoFrame();

    protected:
        void DrawAdaptativeGrid(QPainter* painter);

    public:
        void drawBackground(QPainter *painter, const QRectF &rect) override;

        void paintEvent(QPaintEvent* event) override;

        void drawForeground(QPainter *painter, const QRectF &rect) override;

        FuncDelegatePtr Delegate() const;
    protected:
        void OnSceneSelection();

    public:
        Q_SIGNAL void PointsSelected(QList<FuncViewerHandlePtr> pnts);

        Q_SLOT void OnPointsModified(QList<FuncViewerHandlePtr> pnts);

        Q_SIGNAL void PointsModified(QList<FuncViewerHandlePtr> pntsX);
    };


    class Plugin_API FuncViewWidget: public QWidget
    {
        Q_OBJECT

        bool initialized = false;
        int decimals = 3;

        QPushButton* addPointBtn;
        QPushButton* delPointBtn;

        QDoubleSpinBox* xValue;
        QDoubleSpinBox* yValue;

        FuncScene* scene;
        FuncView* view;

        QList<FuncViewerHandlePtr> editing;
    public:
        FuncViewWidget(QWidget* parent=nullptr);

        ~FuncViewWidget() override;

        void SetFunction(const std::any& func);

        Q_SLOT void OnPointSelected(const QList<FuncViewerHandlePtr>& pnts);

        double Tick() const;

        void SetTick(double t);

        void showEvent(QShowEvent* event) override;

    protected:
        void EditPoints(double y);
    };
}

#endif