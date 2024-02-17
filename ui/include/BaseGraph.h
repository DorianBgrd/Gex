#ifndef BASE_GRAPH_H
#define BASE_GRAPH_H

#include "api.h"

#include <QObject>
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

    protected:

        int LeavePoint() const;

        int ViewPoint() const;

    public:
        Toolbar(QWidget* parent=nullptr);

        QPushButton* NewButton();

        QFrame* AddSeparator();

        void resizeEvent(QResizeEvent* event) override;

        QSize Size() const;

        void SetPos(QVariant x);

        bool IsVisible() const;

//        void enterEvent(QEnterEvent* event) override;
//
//        void leaveEvent(QEvent* event) override;
        void Show(bool animate=true);

        void Hide(bool animate=true);

        void UpdateVisible();
    };

    class GEX_UI_API Message: public QDialog
    {
        UiFeedback f;
        int duration;
        int fadeDuration;

    public:
        Message(Gex::Ui::UiFeedback feedback, int duration,
                int fadeDuration, QWidget* parent=nullptr);
    };

    class GEX_UI_API BaseGraphView: public QGraphicsView
    {
    Q_OBJECT
        qreal zoomInFactor = 1.1;
        qreal zoomOutFactor = 0.9;
        bool zooming = false;
        bool dragMode = false;
        QPoint zoomingPos;
        QPoint dragPos;
        QPoint sceneDragPos;
        QRectF sceneDragRect;

        QCursor mainCursor;
        QCursor pressCursor;
        bool pressed = false;

        Toolbar* toolbar = nullptr;

        void SetMainCursor(QCursor c);

        void SetPressCursor(QCursor c);

        void RestoreCursor();

        QList<QGraphicsItem*> revertSelectItems;
    public:
        explicit BaseGraphView(QGraphicsScene* scene, QWidget* parent=nullptr,
                               bool toolbar=false);

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

        void resizeEvent(QResizeEvent* event) override;

    private:
        Message* current = nullptr;

    public:
        void ShowMessage(Gex::Ui::UiFeedback feedback);
    };

}


#endif // BASE_GRAPH_H