#ifndef GEX_UI_ATTRIBUTEEDITOR_H
#define GEX_UI_ATTRIBUTEEDITOR_H

#include "api.h"

#include "Gex/include/Gex.h"
#include "BaseGraph.h"

#include "UiTsys/include/uitsys.h"

#include <QVector>
#include <QWidget>
#include <QCheckBox>
#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QRectF>
#include <QMenu>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QPushButton>
#include <QLineEdit>
#include <QStackedWidget>


namespace Gex
{
    namespace Ui
    {
        class GraphWidget;
        class NodeGraphScene;

        class GEX_UI_API MultiAttributeWidget: public QFrame
        {
            Q_OBJECT
            QVBoxLayout* mainLayout;
            QVBoxLayout* subAttributeLayout;
            Gex::AttributeWkPtr attribute;
            QPushButton* newIndexButton;
            QPushButton* title;
            QWidget* subAttributeWidget;
            GraphWidget* graph;
        public:
            MultiAttributeWidget(const Gex::AttributeWkPtr& attr,
                                 GraphWidget* graphWidget,
                                 QWidget* parent=nullptr);

            void AddMultiIndex(bool);

            void SetExpanded(bool expanded);

            void FlushAttributes();

            void CreateAttributes();

            void RebuildAttributes();
        };


        class GEX_UI_API AttributeWidget: public QWidget
        {
        Q_OBJECT
            Gex::AttributeWkPtr attribute;
            QHBoxLayout* mainLayout;
            GraphWidget* graph;
        public:
            AttributeWidget(const Gex::AttributeWkPtr& attr,
                            GraphWidget* graphWidget,
                            QWidget* parent=nullptr);

            void OnValueChanged(std::any value);
        };


        class GEX_UI_API ExtraAttributeDialog: public QDialog
        {
        Q_OBJECT
            Gex::NodePtr targetNode;
            QWidget* updateWidget;
            QLineEdit* attributeName;
            QVBoxLayout* initWidgetLayout;
            UiTSys::TypedInitWidget* initWidget = nullptr;
            std::type_index typeIndex;
            GraphWidget* graphWidget;
            QCheckBox* input;
            QCheckBox* output;
            QCheckBox* multi;
            QCheckBox* internal;

        public:
            ExtraAttributeDialog(const Gex::NodePtr& node,
                                 GraphWidget* graphWidget,
                                 QWidget* parent=nullptr,
                                 QWidget* updateWidget=nullptr);

            void Setup();

            void SetType(const QString& type);

            void CreateAttribute();
        };


        class GEX_UI_API AttributeTypeTitle: public QFrame
        {
        public:
            AttributeTypeTitle(const QString& title, int fa_icon,
                               QWidget* parent=nullptr);
        };


        class GEX_UI_API AttributeTab: public QWidget
        {
        Q_OBJECT
            Gex::NodeWkPtr node;
            GraphWidget* graph;
            QVBoxLayout* inputWidgetsLayout;
            QVBoxLayout* outputWidgetsLayout;
            QVector<QWidget*> widgets;
        public:
            AttributeTab(const Gex::NodePtr& node,
                         GraphWidget* graphWidget,
                         QWidget* parent=nullptr);

            void Clear();

            void UpdateAttributes();

            void RebuildAttributes();

            void CreateExtraAttribute();

        private:
            void Setup(const Gex::NodePtr& node);
        };


        class GEX_UI_API AttributeEditor: public QWidget
        {
        Q_OBJECT
            QStackedWidget* stacked;
            QTabWidget* tab;
            GraphWidget* graph;
            QWidget* emptyWidget;
            Gex::NodeWkList currentNodes;

        public:
            AttributeEditor(GraphWidget* parent=nullptr);

            void SetNodes(const Gex::NodeWkList& nodes);

            void Update();
        };
    }
}

#endif //GEX_UI_ATTRIBUTEEDITOR_H
