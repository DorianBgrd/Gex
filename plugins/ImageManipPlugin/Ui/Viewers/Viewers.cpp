#include "Viewers.h"

#include "Types/geometry.h"
#include "Types/resolution.h"


void ImageManip::Viewers::ImageNodeViewer::Setup()
{
    auto* layout = new QVBoxLayout();
    setLayout(layout);

    viewer = new ImageManip::Ui::Types::ImageViewer(
            QImage(), this);
    layout->addWidget(viewer);
}

void ImageManip::Viewers::ImageNodeViewer::OnNodeUpdated(Gex::NodeWkPtr node)
{
    auto imageAttr = node->GetAttribute("Image");
    if (!imageAttr)
    {
        viewer->SetImage(QImage());
        return;
    }

    QImage img = imageAttr->Get<QImage>();
    viewer->SetImage(img);
}


void ImageManip::Viewers::ImageNodeViewer::OnAttributeUpdated(
        const Gex::AttributePtr& attribute,
        const Gex::AttributeChange& change)
{
    OnNodeUpdated(CurrentNode());
}


ImageManip::Viewers::PolygonEditor::PolygonEditor(QWidget* parent): QWidget(parent)
{

}


Gex::NodeWkPtr ImageManip::Viewers::PolygonEditor::Node() const
{
    return node;
}

void ImageManip::Viewers::PolygonEditor::SetNode(Gex::NodeWkPtr node_)
{
    node = node_;
    update();
}


int ImageManip::Viewers::PolygonEditor::HandleIndex(QPoint pos) const
{
    for (int i = 0; i < handles.size(); i++)
    {
        QRect rect = handles[i];
        if (rect.contains(pos))
        {
            return i;
        }
    }

    return -1;
}


void ImageManip::Viewers::PolygonEditor::enterEvent(QEnterEvent *event)
{

}


void ImageManip::Viewers::PolygonEditor::moveEvent(QMoveEvent *event)
{

}

void ImageManip::Viewers::PolygonEditor::leaveEvent(QEvent *event)
{

}


void ImageManip::Viewers::PolygonEditor::mousePressEvent(QMouseEvent* event)
{
    if (!node)
        return;

    auto pointAttr = node->GetAttribute("Points");
    if (event->modifiers() == Qt::ControlModifier)
    {
        // Delete Handle and point.
        currentHandleIndex = HandleIndex(event->pos());
        currentAction = Action::Remove;
    }
    else if (event->modifiers() == Qt::ShiftModifier)
    {
        // Add a new point.
        currentAction = Action::Add;
    }
    else
    {
        // Try selecting a point.
        currentHandleIndex = HandleIndex(event->pos());
        if (currentHandleIndex >= 0)
            currentHandlePos = handles[currentHandleIndex].center();
    }
}

void ImageManip::Viewers::PolygonEditor::mouseMoveEvent(QMouseEvent* event)
{
    if (!node)
        return;

    if (currentHandleIndex >= 0)
    {
        currentHandlePos = event->pos();
    }

    update();
}

void ImageManip::Viewers::PolygonEditor::mouseReleaseEvent(QMouseEvent* event)
{
    if (node)
    {
        auto pointAttr = node->GetAttribute("Points");

        if (currentAction == Action::Remove)
        {
            auto idx = pointAttr->ValidIndices().at(currentHandleIndex);
            pointAttr->RemoveIndex(idx);
        }
        else if (currentAction == Action::Add)
        {
            auto indices = pointAttr->ValidIndices();

            int newIndex = 0;
            if (!indices.empty())
            {
                newIndex = indices.back() + 1;
            }

            if (pointAttr->CreateIndex(newIndex))
            {
                auto res = node->GetAttribute("Resolution")->Get<Types::Resolution>();

                QPoint pos = event->pos();
                pointAttr->GetIndexAttribute(newIndex)->Set<Types::Point>(
                        Types::Point(
                                (pos.x() / static_cast<double>(rect().width())) * res.at(0),
                                (pos.y() /  static_cast<double>(rect().height())) * res.at(1))
                                );
            }
        }
        else if (currentHandleIndex >= 0)
        {
            auto indexAt = pointAttr->GetIndexAttribute(
                    pointAttr->ValidIndices().at(currentHandleIndex));

            if (indexAt)
            {
                auto res = node->GetAttribute("Resolution")->Get<Types::Resolution>();

                QPoint pos = event->pos();
                indexAt->Set(Types::Point(
                        (pos.x() / static_cast<double>(rect().width())) * res.at(0),
                        (pos.y() /  static_cast<double>(rect().height())) * res.at(1))
                );
            }
        }
    }

    currentHandlePos = QPoint();
    currentHandleIndex = -1;
    currentAction = Action::Default;
    update();
}

void ImageManip::Viewers::PolygonEditor::paintEvent(QPaintEvent* event)
{
    handles.clear();

    QPainter painter(this);

    QPen pen(Qt::white);
    pen.setWidth(1);

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect());

    if (!node)
    {
        painter.end();
        return;
    }

    auto color = node->GetAttribute("Color")->Get<QColor>();
    auto resolution = node->GetAttribute("Resolution")->Get<Types::Resolution>();

    auto points = node->GetAttribute("Points")->GetArrayValues<Types::Point>();

    QPolygon polygon;
    for (const Types::Point& point :  points)
    {
        QPoint pnt(rect().width() * (point.x / resolution[0]),
                   rect().height() * (point.y / resolution[1]));
        polygon.push_back(pnt);

        QRect handle(pnt.x() - 4, pnt.y() - 4, 8, 8);
        handles.push_back(handle);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawPolygon(polygon);

    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    for (const auto& handle : handles)
    {
        painter.drawRect(handle);
    }

    if (currentHandleIndex >= 0)
    {
        painter.drawRect(QRect(currentHandlePos.x() - 4, currentHandlePos.y() - 4, 8, 8));
        painter.drawLine(polygon.point(currentHandleIndex), currentHandlePos);
    }

    painter.end();
}


void ImageManip::Viewers::PolygonNodeEditor::Setup()
{
    auto* layout = new QVBoxLayout();
    setLayout(layout);

    editor = new PolygonEditor(this);
    layout->addWidget(editor);
}

void ImageManip::Viewers::PolygonNodeEditor::OnNodeUpdated(Gex::NodeWkPtr node)
{
    editor->SetNode(node);
}

void ImageManip::Viewers::PolygonNodeEditor::OnAttributeUpdated(
        const Gex::AttributePtr& attribute,
        const Gex::AttributeChange& change)
{
    editor->update();
}


