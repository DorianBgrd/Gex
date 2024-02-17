#ifndef GEX_UI_ATTRIBUTEEDITOR_H
#define GEX_UI_ATTRIBUTEEDITOR_H

#include "api.h"

#include "include/Gex.h"
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
            Gex::Attribute* attribute;
            QPushButton* newIndexButton;
            QPushButton* title;
            QWidget* subAttributeWidget;
            GraphWidget* graph;
        public:
            MultiAttributeWidget(Gex::Attribute* attr,
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
            Gex::Attribute* attribute;
            QHBoxLayout* mainLayout;
            GraphWidget* graph;
        public:
            AttributeWidget(Gex::Attribute* attr,
                            GraphWidget* graphWidget,
                            QWidget* parent=nullptr);

            void OnValueChanged(std::any value);
        };


        class GEX_UI_API ExtraAttributeDialog: public QDialog
        {
        Q_OBJECT
            Gex::Node* targetNode;
            QWidget* updateWidget;
            QLineEdit* attributeName;
            QVBoxLayout* initWidgetLayout;
            UiTSys::TypedInitWidget* initWidget = nullptr;
            GraphWidget* graphWidget;
            QCheckBox* input;
            QCheckBox* output;
            QCheckBox* multi;
            QCheckBox* internal;

        public:
            ExtraAttributeDialog(Gex::Node* node,
                                 GraphWidget* graphWidget,
                                 QWidget* parent=nullptr,
                                 QWidget* updateWidget=nullptr);

            void Setup();

            void SetType(const QString& type);

            void CreateAttribute();
        };


        class GEX_UI_API AttributeTab: public QWidget
        {
        Q_OBJECT
            Gex::Node* node;
            GraphWidget* graph;
            QVBoxLayout* widgetsLayout;
            QVector<QWidget*> widgets;
        public:
            AttributeTab(Gex::Node* node,
                         GraphWidget* graphWidget,
                         QWidget* parent=nullptr);

            void Clear();

            void UpdateAttributes();

            void RebuildAttributes();

            void CreateExtraAttribute();

            void Setup();
        };


        class GEX_UI_API AttributeEditor: public QWidget
        {
        Q_OBJECT
            QStackedWidget* stacked;
            QTabWidget* tab;
            GraphWidget* graph;
            QWidget* emptyWidget;

        public:
            AttributeEditor(GraphWidget* parent=nullptr);

            void SetNodes(std::vector<Gex::Node*> nodes);
        };
    }
}

#endif //GEX_UI_ATTRIBUTEEDITOR_H
