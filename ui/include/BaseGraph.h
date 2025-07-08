#ifndef BASE_GRAPH_H
#define BASE_GRAPH_H

#include "api.h"

#include <QObject>
#include <QLabel>
#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>

#include <iostream>
#include "UiStatus.h"
#include "ViewContexts.h"


namespace Gex::Ui
{
    class GEX_UI_API Toolbar: public QFrame
    {
        Q_OBJECT

        QVector<QWidget*> widgets;

        QFrame* toolbar;
        QVBoxLayout* toolbarLayout;
        QVariantAnimation* animation;

        bool visible;
        bool hiding = false;

    protected:

        int LeavePoint() const;

        int ViewPoint() const;

    public:
        Toolbar(QWidget* parent=nullptr);

        QPushButton* NewButton();

        void AddWidget(QWidget* widget);

        QFrame* AddSeparator();

        void resizeEvent(QResizeEvent* event) override;

        QSize Size() const;

        void SetPos(QVariant x);

        bool IsVisible() const;

        bool IsHiding() const;

        void AnimationFinished();

//        void enterEvent(QEnterEvent* event) override;
//
//        void leaveEvent(QEvent* event) override;
        void Show(bool animate=true);

        void Hide(bool animate=true);

        void UpdateVisible();
    };


    class GEX_UI_API Message: public QFrame
    {
        UiFeedback f;
        QVariantAnimation* animation;

        QLabel* label;

    public:
        Message(QWidget* parent=nullptr);

        void SetFeedback(Gex::Ui::UiFeedback feedback);

        void SetOpacity(QVariant opacity);

        void Hide(int duration);

        void Reset();
    };


    class MoveContext: public ViewContext
    {
        bool zooming = false;
        QPointF zoomingPos;

        bool AcceptBaseEvents() const override;

        void Setup(BaseGraphView* scene) override;

        void Finalize(BaseGraphView *scene) override;

        void OnPressEvent(QMouseEvent* event) override;

        void OnMoveEvent(QMouseEvent* event) override;

        void OnReleaseEvent(QMouseEvent* event) override;
    };


    class MoveEventFilter: public QObject
    {
    public:
        MoveEventFilter(QObject* p): QObject(p)
        {

        }

        bool eventFilter(QObject *watched, QEvent *event) override
        {
            if (event->type() == QEvent::FocusIn ||
                event->type() == QEvent::FocusAboutToChange ||
                event->type() == QEvent::FocusOut ||
                event->type() == QEvent::KeyPress ||
                event->type() == QEvent::KeyRelease)
            {
                std::string typeName;
                if (event->type() == QEvent::FocusIn)
                    typeName = "FocusIn";
                else if (event->type() == QEvent::FocusAboutToChange)
                    typeName = "FocusAboutToChange";
                else
                    typeName = "FocusOut";

                std::cout << "Focus event " << typeName << std::endl;
                return true;
            }

            return false;
        }
    };


    class SelectContext: public ViewContext
    {
        bool AcceptBaseEvents() const override;

        void Setup(BaseGraphView* scene) override;

        void OnPressEvent(QMouseEvent* event) override;

        void OnMoveEvent(QMouseEvent* event) override;

        void OnReleaseEvent(QMouseEvent* event) override;
    };


    class GEX_UI_API BaseGraphView: public QGraphicsView
    {
        Q_OBJECT

        qreal zoomFactor = 0.9;

        Toolbar* toolbar = nullptr;
        Message* message = nullptr;
        QTimer* messageTimer = nullptr;

        ViewContexts* contexts;

        QList<QGraphicsItem*> revertSelectItems;
    public:
        explicit BaseGraphView(QGraphicsScene* scene,
                               QWidget* parent=nullptr,
                               bool toolbar=false);

        virtual ~BaseGraphView();

        ViewContexts* GetEditors() const;

        Toolbar* GetToolbar() const;

        virtual QMenu* GetMenu();

        virtual bool DeleteMenuOnHide() const;

        void ShowMenu(QMenu* menu, const QPoint& pos);

    public:
        virtual QList<QGraphicsItem*> FilterSelectedItems(
                QList<QGraphicsItem*> items) const;

        virtual void ViewUsedSceneRect();

        virtual void ViewSelectionSceneRect();

    public:
        void ZoomIn();

        void ZoomOut();

        virtual void FrameSelection();

        virtual void FrameAll();

        void wheelEvent(QWheelEvent* event) override;

        void mousePressEvent(QMouseEvent* event) override;

        void mouseMoveEvent(QMouseEvent* event) override;

        void mouseReleaseEvent(QMouseEvent* event) override;

        void enterEvent(QEnterEvent* event) override;

        void leaveEvent(QEvent* event) override;

        void keyPressEvent(QKeyEvent* event) override;

        void keyReleaseEvent(QKeyEvent* event) override;

        bool event(QEvent *event) override;

        void resizeEvent(QResizeEvent* event) override;

    public:
        void ShowMessage(Gex::Ui::UiFeedback feedback,
                         int duration=5000);

        void HideMessage();
    };

}


#endif // BASE_GRAPH_H