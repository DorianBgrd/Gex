#ifndef GEX_IMGMANIP_VIEWERS_H
#define GEX_IMGMANIP_VIEWERS_H

#include "ui/include/ui.h"
#include "Ui/Types.h"

namespace ImageManip::Viewers
{
    struct ImageNodeViewer: public Gex::Ui::NodeViewer
    {
        ImageManip::Ui::Types::ImageViewer* viewer;

    public:
        void Setup() override;

        void OnNodeUpdated(Gex::NodeWkPtr node) override;

        void OnAttributeUpdated(
                const Gex::AttributePtr& attribute,
                const Gex::AttributeChange& change)
                override;
    };


    GENERATE_DEFAULT_VIEWER_CREATOR(
            ImageNodeViewerBuilder,
            ImageNodeViewer)


    class PolygonEditor: public QWidget
    {
        Gex::NodeWkPtr node;
        QVector<QRect> handles;

        int currentHandleIndex = -1;
        QPoint currentHandlePos;

        enum class Action
        {
            Default,
            Add,
            Remove
        };

        Action currentAction = Action::Default;

    public:
        PolygonEditor(QWidget* parent=nullptr);

        Gex::NodeWkPtr Node() const;

        int HandleIndex(QPoint pos) const;

        void SetNode(Gex::NodeWkPtr node);

        void enterEvent(QEnterEvent *event) override;

        void moveEvent(QMoveEvent *event) override;

        void leaveEvent(QEvent *event) override;

        void mousePressEvent(QMouseEvent *event) override;

        void mouseMoveEvent(QMouseEvent *event) override;

        void mouseReleaseEvent(QMouseEvent *event) override;

        void paintEvent(QPaintEvent *) override;

    };


    struct PolygonNodeEditor: public Gex::Ui::NodeViewer
    {
        PolygonEditor* editor;

    public:
        void Setup() override;

        void OnNodeUpdated(Gex::NodeWkPtr node) override;

        void OnAttributeUpdated(
                const Gex::AttributePtr& attribute,
                const Gex::AttributeChange& change)
        override;
    };


    GENERATE_DEFAULT_VIEWER_CREATOR(
            PolygonNodeEditorBuilder,
            PolygonNodeEditor)
}

#endif //GEX_IMGMANIP_VIEWERS_H
