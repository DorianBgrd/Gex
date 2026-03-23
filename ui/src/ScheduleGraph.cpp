#include "ui/include/ScheduleGraph.h"


QRectF Gex::Ui::SchedulePlugItem::boundingRect() const
{
    return {0, 0, 10, 10};
}


void Gex::Ui::SchedulePlugItem::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
         QWidget *widget
)
{
    QRectF rect = boundingRect();

    painter->setBrush(QBrush(QColor(230, 173, 60)));
    painter->setPen(Qt::NoPen);

    painter->drawEllipse(rect.adjusted(1, 1, -1, -1));
}


Gex::Ui::ScheduleNodeItem::ScheduleNodeItem(
        const Gex::ScheduledNodeWkPtr& scheduled,
        QGraphicsItem* parent
): QGraphicsObject(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    text = new QGraphicsTextItem(scheduled->node->Name().c_str(), this);
    text->setDefaultTextColor(Qt::white);

    width = text->boundingRect().width() + 20;
    text->setPos(10, (50 - text->boundingRect().height()) / 2);

    node = scheduled;
    inputPlug = new SchedulePlugItem(this);
    outputPlug = new SchedulePlugItem(this);

    auto rect = getBoundingRect();

    inputPlug->setPos(
            rect.left() - (inputPlug->boundingRect().width()) - 2,
            rect.center().y() - (inputPlug->boundingRect().height() / 2)
    );

    outputPlug->setPos(
            rect.right() + 2,
            rect.center().y() - (inputPlug->boundingRect().height() / 2)
    );
}


QGraphicsItem* Gex::Ui::ScheduleNodeItem::GetInput() const
{
    return inputPlug;
}

QGraphicsItem* Gex::Ui::ScheduleNodeItem::GetOutput() const
{
    return outputPlug;
}


Gex::ScheduledNodeWkPtr Gex::Ui::ScheduleNodeItem::GetScheduledNode() const
{
    return node;
}


QVariant Gex::Ui::ScheduleNodeItem::itemChange(
        QGraphicsItem::GraphicsItemChange change,
        const QVariant &value
)
{
    if (change == QGraphicsItem::ItemPositionChange)
    {
        QPointF pos = value.toPointF();
        pos.setX(itemX);

        return {pos};
    }

    if (change == QGraphicsItem::ItemPositionHasChanged)
    {
        for (const auto& l : links)
        {
            if (l.isNull())
                continue;

            l->update();
        }
    }

    return QGraphicsObject::itemChange(change, value);
}


void Gex::Ui::ScheduleNodeItem::RegisterLink(QSharedPointer<ScheduleLink> l)
{
    links.push_back(l);
}


void Gex::Ui::ScheduleNodeItem::DeregisterLink(QSharedPointer<ScheduleLink> l)
{
    auto iter = std::find(links.begin(), links.end(), l);
    if (iter == links.end())
        return;

    links.erase(iter);
}


void Gex::Ui::ScheduleNodeItem::SetX(qreal x)
{
    itemX = x;
}


void Gex::Ui::ScheduleNodeItem::InitPos()
{
    SetX(pos().x());
}


QRectF Gex::Ui::ScheduleNodeItem::getBoundingRect() const
{
    return {0, 0, width, 50};
}


QRectF Gex::Ui::ScheduleNodeItem::boundingRect() const
{
    return getBoundingRect();
}


void Gex::Ui::ScheduleNodeItem::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget
)
{
    QRectF rect = boundingRect();

    painter->setBrush(
        QBrush(QColor(70, 70, 70))
    );

    painter->setPen(Qt::NoPen);
    if (isSelected())
    {
        painter->setPen(QPen(QColor(230, 173, 60), 1));
        rect.adjust(1, 1, -1, -1);
    }

    painter->drawRoundedRect(rect, 10, 10);
}


Gex::Ui::ScheduleLink::ScheduleLink(
        ScheduleNodeItem* input,
        ScheduleNodeItem* output,
        QGraphicsItem* parent
): QGraphicsObject(parent), pen(
        QColor(230, 173, 60), 2
    )
{
    inputPtr = input;
    outputPtr = output;
}


QRectF Gex::Ui::ScheduleLink::boundingRect() const
{
    auto rect1 = inputPtr->sceneBoundingRect();
    auto rect2 = outputPtr->sceneBoundingRect();

    return Gex::Ui::BoundingRect({rect1, rect2});
}


void Gex::Ui::ScheduleLink::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget
)
{
    path = QPainterPath();

    auto start  = inputPtr->GetOutput()->sceneBoundingRect().center();
    auto end = outputPtr->GetInput()->sceneBoundingRect().center();

    auto middlePoint = start + ((end - start) / 2);

    path.moveTo(start);
//    path.lineTo(middlePoint.x(), start.y());
//    path.moveTo(middlePoint.x(), start.y());
//    path.lineTo(middlePoint.x(), end.y());
//    path.moveTo(middlePoint.x(), end.y());
    path.lineTo(end);

    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);

//    painter->end();
}


QPainterPath Gex::Ui::ScheduleLink::shape() const
{
    QPainterPathStroker stroke(pen);

    return stroke.createStroke(path);
}


Gex::Ui::ScheduleScene::ScheduleScene(QObject *parent):
        QGraphicsScene(parent)
{

}


Gex::ScheduleNodeWkPtrList GatherPreviousNodes(
        const Gex::ScheduledNodeWkPtr& scheduled
)
{
    if (!scheduled)
        return {};

    Gex::ScheduleNodeWkPtrList gatheredNodes;
    for (const auto& previous : scheduled->previousNodes)
    {
        auto previousNodes = GatherPreviousNodes(previous);

        gatheredNodes.push_back(previous);

        gatheredNodes.insert(gatheredNodes.end(),
                             previousNodes.begin(),
                             previousNodes.end());
    }

    return gatheredNodes;
}


void Gex::Ui::ScheduleScene::ScheduleScene::Load(
        const Gex::ScheduleNodePtrList& scheduledNodes
)
{
    clear();

    std::unordered_map<size_t, Gex::ScheduleNodeWkPtrList> nodesPerLevels;

    std::set<size_t> levels;

    for (const auto& scheduledNode : scheduledNodes)
    {
        auto previousNodes = GatherPreviousNodes(scheduledNode);

        auto length = previousNodes.size();
        if (nodesPerLevels.find(length) == nodesPerLevels.end())
        {
            nodesPerLevels[length] = {};
        }

        nodesPerLevels[length].emplace_back(scheduledNode);
        levels.insert(length);
    }

    std::vector<size_t> levelsVec(levels.begin(), levels.end());

    std::sort(levelsVec.begin(), levelsVec.end());
//    std::reverse(levelsVec.begin(), levelsVec.end());

    if(levelsVec.empty())
        return;

//    auto maxLevel = *(std::max_element(levelsVec.begin(), levelsVec.end()));

    qreal itemX = 0;
    qreal lastLevel = 0;
    qreal horizontalSpacing = 200;
    qreal verticalSpacing = 50;

    for (size_t level : levelsVec)
    {
        qreal itemY  = 0;
        itemX += (level - lastLevel) * horizontalSpacing;

        qreal itemWidth = 0;
        for (const auto& node : nodesPerLevels.at(level))
        {
            auto* item = new ScheduleNodeItem(node);

            items.append(item);

            nodes.push_back(node);

            addItem(item);

            item->SetX(itemX);
            item->setPos(itemX, itemY);

            itemY += item->boundingRect().height() + verticalSpacing;

            itemWidth = std::max(itemWidth, item->boundingRect().width());
        }

        itemX += itemWidth;
//        itemX += std::max_element(itemWidths.begin(), itemWidths.end());

        lastLevel = level;
    }

    for (const auto& nodeItem : items)
    {
        for (const auto& previous : nodeItem->GetScheduledNode()->previousNodes)
        {
            Gex::ScheduledNodeWkPtr p = previous;

            for (auto* item : items)
            {
                if (item->GetScheduledNode() == p)
                {
                    auto link = QSharedPointer<ScheduleLink>::create(item, nodeItem);

                    item->RegisterLink(link);
                    nodeItem->RegisterLink(link);

                    addItem(link.get());

                    break;
                }
            }

        }
    }

    QList<QRectF> rects;
    for (const auto* item : items)
    {
        rects.append(item->sceneBoundingRect());
    }

    setSceneRect(Gex::Ui::Scaled(Gex::Ui::BoundingRect(rects), 3));
}


Gex::Ui::ScheduleView::ScheduleView(
        ScheduleScene* scene,
        QWidget* parent
): BaseGraphView(scene, parent, true)
{

}


Gex::Ui::ScheduleViewer::ScheduleViewer(
        QWidget *parent
): QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    scene = new ScheduleScene(this);

    view = new ScheduleView(scene, this);

    layout->addWidget(view);

    auto* reloadButton = view->GetToolbar()->NewButton();
    reloadButton->setIcon(
            Res::UiRes::GetRes()->GetQtAwesome()->icon(
                    fa::fa_solid, fa::fa_sync
            )
    );

    QObject::connect(reloadButton, &QPushButton::clicked,
                     this, &ScheduleViewer::Sync);
}


void Gex::Ui::ScheduleViewer::LoadCompound(
        const Gex::NodeWkPtr& loadCompound
)
{
    compound = loadCompound;

    Sync();
}


void Gex::Ui::ScheduleViewer::Sync()
{
    Gex::ScheduleNodePtrList scheduledNodes;
    if (compound && compound->IsCompound())
    {
        scheduledNodes = dynamic_cast<CompoundNode*>(compound.ToShared().get())->ToScheduledNodes();
    }

    scene->Load(scheduledNodes);
}

