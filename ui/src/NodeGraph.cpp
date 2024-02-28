#include "Gex/include/Config.h"
#include "ui/include/NodeGraph.h"

#include "UiTsys/include/uitsys.h"
#include "UiRes/include/uires.h"

#include "ui/include/AttributeEditor.h"
#include "ui/include/commands.h"

#include <QGridLayout>
#include <QSplitter>
#include <QSpinBox>
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>
#include <QShortcut>
#include <QMenu>
#include <QAction>
#include <QWidgetAction>
#include <QPushButton>
#include <QTextDocument>
#include <QStyle>
#include <QApplication>
#include <QThread>
#include <QStyleOptionGraphicsItem>

#include "Gex/include/Evaluation.h"

Gex::Ui::PlugItem::PlugItem(AttributeItem* attribute,
                                      bool input): QGraphicsEllipseItem(attribute)
{
    attr = attribute;
    isInput = input;
}


Gex::Ui::AttributeItem* Gex::Ui::PlugItem::Attribute() const
{
    return attr;
}


bool Gex::Ui::PlugItem::IsInputAnchor() const
{
    return isInput;
}


bool Gex::Ui::PlugItem::IsOutputAnchor() const
{
    return (!isInput);
}


Gex::Ui::MultiAttributeItem::MultiAttributeItem(AttributeItem* attribute): QGraphicsItem(attribute)
{
    attributeItem = attribute;
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
}


bool Gex::Ui::MultiAttributeItem::Collapsed() const
{
    return attributeItem->Collapsed();
}


QRectF Gex::Ui::MultiAttributeItem::boundingRect() const
{
    return QRectF(0, 0, size, size);
}


void Gex::Ui::MultiAttributeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                                  QWidget *widget)
{
    painter->setPen(Qt::NoPen);

    QBrush backgroundBrush(backgroundColor);
    QBrush iconBrush(iconColor);
    if (hover)
    {
        backgroundBrush = hoverBackgroundColor;
        iconBrush = hoverIconColor;
    }
    else if (pressed)
    {
        backgroundBrush = pressedBackgroundColor;
        iconBrush = pressedIconColor;
    }

    painter->setBrush(backgroundBrush);
    QRectF rect = boundingRect();
    painter->drawRoundedRect(rect, radius, radius);

    painter->setBrush(iconBrush);

    QRectF minusRect(rect);
    minusRect.setX(margins);
    qreal minusRectY = (rect.height() - rect.height() / 4) / 2;
    minusRect.setY(minusRectY);

    minusRect.setWidth(rect.width() - margins * 2);
    minusRect.setHeight(rect.height() / 4);

    painter->drawRoundedRect(minusRect, radius, radius);

    if (Collapsed())
    {
        QRectF plusRect(rect);
        plusRect.setX((rect.width() - plusRect.width() / 4) / 2);
        plusRect.setY(margins);
        plusRect.setWidth(rect.width() / 4);
        plusRect.setHeight(rect.height() - margins * 2);
        painter->drawRoundedRect(plusRect, radius, radius);
    }
}



void Gex::Ui::MultiAttributeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    hover = true;

    QGraphicsItem::hoverEnterEvent(event);
}


void Gex::Ui::MultiAttributeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hover = false;

    QGraphicsItem::hoverLeaveEvent(event);
}


void Gex::Ui::MultiAttributeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (hover)
        hover = false;

    pressed = true;

//    QGraphicsItem::mousePressEvent(event);
    event->accept();

    update();
}

void Gex::Ui::MultiAttributeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = false;

    if (attributeItem->Collapsed())
        attributeItem->Expand();
    else
        attributeItem->Collapse();

    event->accept();
//    QGraphicsItem::mouseReleaseEvent(event);
    update();

    attributeItem->ParentNode()->PlaceAttributes();
    attributeItem->ParentNode()->UpdateLinks();
}


Gex::Ui::AttributeItem::AttributeItem(Gex::Attribute* attr,
                                                AttributeItem* parent,
                                                NodeItem* node):
        QGraphicsRectItem(parent)
{
    Initialize(attr, parent, node);
}


Gex::Ui::AttributeItem::~AttributeItem()
{
    for (int i = links.length() - 1; i >= 0; i--)
    {
        if (links[i].isNull())
            continue;

//        auto* scene_ = scene();
//        scene()->removeItem(links[i].get());
        links[i]->scene()->removeItem(links[i].get());
        delete links[i];
    }
}


void Gex::Ui::AttributeItem::RemoveLink(LinkItem* link)
{
    links.removeAll(link);
}


void Gex::Ui::AttributeItem::SetTextIndent(qreal indent)
{
    if (attribute->IsOutput() && !attribute->IsStatic())
    {
        indent = -1 * indent;
    }

    text->setPos(text->pos().x() + indent, text->pos().y());

    if (multiButton)
    {
        multiButton->setPos(multiButton->pos().x() + indent,
                            multiButton->pos().y());
    }
}


Gex::Ui::AttributeItem::AttributeItem(Gex::Attribute* attr,
                                                NodeItem* node):
        QGraphicsRectItem(node)
{
    Initialize(attr, nullptr, node);
}


void Gex::Ui::AttributeItem::Initialize(Gex::Attribute* attr,
                                                  AttributeItem* parent,
                                                  NodeItem* node)
{
    setRect(QRect(0, 0, NODE_DEFAULT_WIDTH, ATTRIBUTE_HEIGHT));
    setPen(Qt::NoPen);

    collapsed = true;
    attribute = attr;
    parentNode = node;
    parentAttribute = parent;
    input = new PlugItem(this, true);
    input->setBrush(QBrush("#E58C02"));
    input->setPen(Qt::NoPen);
    input->setRect(PLUG_RECT);
    input->setVisible(attribute->IsInput());

    output = new PlugItem(this, false);
    output->setBrush(QBrush("#E58C02"));
    output->setPen(Qt::NoPen);
    output->setRect(PLUG_RECT);
    output->setVisible(attribute->IsOutput());

    text = new QGraphicsTextItem(this);
    text->setFont(QFont("sans", 8));
    text->setPlainText(attr->Name().c_str());

    QFontMetrics metrics(text->font());

    qreal width = metrics.boundingRect(attribute->Name().c_str()).width();
    if (width > NODE_DEFAULT_WIDTH)
    {
        // TODO : ellide text ?
    }

    text->setDefaultTextColor(QColor("white"));
    qreal height = metrics.boundingRect(attribute->Name().c_str()).height();
    text->update();

    qreal textY = (ATTRIBUTE_HEIGHT - text->boundingRect().height()) / 2;
    width = text->boundingRect().width();
//    qreal textY = 3;
//    if (attribute->IsStatic())
//    {
//        text->setPos((NODE_DEFAULT_WIDTH - width) / 2, textY);
//    }
//    else
    if (attribute->IsInput() || attribute->IsStatic())
    {
        text->setPos(0, textY);
        output->setVisible(attribute->IsStatic());
    }
    else
    {
        text->setPos((NODE_DEFAULT_WIDTH - width), textY);
        input->setVisible(false);
    }

    qreal plugY = (ATTRIBUTE_HEIGHT - PLUG_RECT.height())/2;
    input->setPos(-PLUG_RECT.width(), plugY);
    output->setPos(rect().width(), plugY);

    if (attribute->IsMulti() || attribute->IsHolder())
    {
        multiButton = new MultiAttributeItem(this);
        if (attribute->IsInput() || attribute->IsStatic())
        {
            text->setPos(text->pos().x() + 14,
                         text->pos().y());

            multiButton->setPos(4, (ATTRIBUTE_HEIGHT - multiButton->boundingRect().height())/2);
        }
        else
        {
            text->setPos(text->pos().x() - 14,
                         text->pos().y());

            multiButton->setPos(NODE_DEFAULT_WIDTH - 4 - multiButton->boundingRect().width(),
                                (ATTRIBUTE_HEIGHT - multiButton->boundingRect().height())/2);
        }
    }

    CreateAttributes();
}


void Gex::Ui::AttributeItem::ShowAsInternal(bool internal)
{
    qreal plugY = (ATTRIBUTE_HEIGHT - PLUG_RECT.height())/2;
    if (internal)
    {
        output->setPos(-PLUG_RECT.width(), plugY);
        input->setPos(rect().width(), plugY);
        if (attribute->IsStatic())
        {
            output->setVisible(false);
        }
    }
    else
    {
        input->setPos(-PLUG_RECT.width(), plugY);
        output->setPos(rect().width(), plugY);
        if (attribute->IsStatic())
        {
            output->setVisible(true);
        }
    }

    asInternal = internal;

    for (auto* attr : indexedAttributes)
    {
        attr->ShowAsInternal(internal);
    }

    for (auto* attr : childAttributes)
    {
        attr->ShowAsInternal(internal);
    }
}


bool Gex::Ui::AttributeItem::AsInternal() const
{
    return asInternal;
}


void Gex::Ui::AttributeItem::SetTextVisibility(bool visibility)
{
    text->setVisible(visibility);
    if (multiButton)
    {
        multiButton->setVisible(visibility);
    }
}


bool Gex::Ui::AttributeItem::TextVisibility() const
{
    return text->isVisible();
}



Gex::Attribute* Gex::Ui::AttributeItem::Attribute() const
{
    return attribute;
}


Gex::Ui::AttributeItem* Gex::Ui::AttributeItem::GetSubAttribute(Gex::Attribute* attr) const
{
    for (auto ca : childAttributes)
    {
        if (ca->Attribute() == attr)
        {
            return ca;
        }
    }

    for (auto ia : indexedAttributes)
    {
        if (ia->Attribute() == attr)
        {
            return ia;
        }
    }

    return nullptr;
}


void Gex::Ui::AttributeItem::CreateAttributes()
{
    indexedAttributes.clear();

    for (unsigned int index : attribute->ValidIndices())
    {
        auto* attr = attribute->GetIndexAttribute(index);
        AttributeItem* indexAttr = new AttributeItem(attr, this, parentNode);
        indexAttr->SetTextIndent(textIndent + 14);
        indexedAttributes.push_back(indexAttr);
    }

    childAttributes.clear();

    for (auto* attr : attribute->GetChildAttributes())
    {

        AttributeItem* childAttr = new AttributeItem(attr, this, parentNode);
        childAttr->SetTextIndent(textIndent + 10);
        childAttributes.push_back(childAttr);
    }

    Collapse();
}


void Gex::Ui::AttributeItem::RebuildAttributes()
{
    bool collapsedBeforeRebuild = collapsed;
    for (auto* indexAttr : indexedAttributes)
    {
        delete indexAttr;
    }
    indexedAttributes.clear();

    for (auto* childAttr : childAttributes)
    {
        delete childAttr;
    }
    childAttributes.clear();

    CreateAttributes();

    InitializeLinks();

    if (!collapsedBeforeRebuild)
    {
        Expand();
    }

    ParentNode()->PlaceAttributes();
}


void Gex::Ui::AttributeItem::InitializeLinks()
{
    for (auto* attrItem : indexedAttributes)
    {
        attrItem->InitializeLinks();
    }

    for (auto* attrItem : childAttributes)
    {
        attrItem->InitializeLinks();
    }

    QList<QGraphicsItem*> items = scene()->items();
    std::unordered_map<Gex::Node*, NodeItem*> nodesItems;
    for (auto* item : items)
    {
        NodeItem* nodeitem = dynamic_cast<NodeItem*>(item);
        if (!nodeitem)
        {
            continue;
        }

        nodesItems[nodeitem->Node()] = nodeitem;
    }

    auto* attr = attribute->Source();
    if (attr)
    {
        auto* node = attr->Node();
        if (nodesItems.find(node) != nodesItems.end())
        {
            NodeItem* nodeItem = nodesItems.at(node);
            AttributeItem* attrItem = nodeItem->FindAttribute(attr->Longname());

            LinkItem* link = new LinkItem(attrItem, this);
            if (links.contains(link))
            {
                delete link;
                return;
            }

            attrItem->links.append(link);
            links.append(link);

            scene()->addItem(link);
            link->Update();
        }
    }
}


QPointF Gex::Ui::AttributeItem::SceneInputPosition() const
{
    return input->sceneBoundingRect().center();
}


QPointF Gex::Ui::AttributeItem::SceneOutputPosition() const
{
    return output->sceneBoundingRect().center();
}


Gex::Ui::PlugItem* Gex::Ui::AttributeItem::InputAnchor() const
{
    return input;
}


Gex::Ui::PlugItem* Gex::Ui::AttributeItem::OutputAnchor() const
{
    return output;
}


Gex::Ui::NodeItem* Gex::Ui::AttributeItem::ParentNode() const
{
    return parentNode;
}


bool Gex::Ui::AttributeItem::IsMulti() const
{
    return attribute->IsMulti();
}


void Gex::Ui::AttributeItem::Collapse()
{
    for (AttributeItem* item : indexedAttributes)
    {
        item->setVisible(false);
        item->setPos(0, 0);
        item->Collapse();
    }

    for (AttributeItem* item : childAttributes)
    {
        item->setVisible(false);
        item->setPos(0, 0);
        item->Collapse();
    }

    collapsed = true;

    UpdateLinks();
}


void Gex::Ui::AttributeItem::Expand()
{
    qreal pos = ATTRIBUTE_HEIGHT;
    for (AttributeItem* item : indexedAttributes)
    {
        item->setVisible(true);
        item->setPos(0, pos);
        pos += item->TotalHeight();
    }

    for (AttributeItem* item : childAttributes)
    {
        item->setVisible(true);
        item->setPos(0, pos);
        pos += item->TotalHeight();
    }

    collapsed = false;

    UpdateLinks();

    if (multiButton)
    {
        multiButton->update();
    }
}


bool Gex::Ui::AttributeItem::Collapsed() const
{
    return collapsed;
}


qreal Gex::Ui::AttributeItem::TotalHeight() const
{
    qreal height = ATTRIBUTE_HEIGHT;

    if (!Collapsed())
    {
        // Child attributes can be holder
        for (auto* indexedItem : indexedAttributes)
            height += indexedItem->TotalHeight();

        // Child attributes can be holder
        for (auto* childItem : childAttributes)
            height += childItem->TotalHeight();
    }

    return height;
}


QVariant Gex::Ui::AttributeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged)
    {
        for (auto link : links)
        {
            link->Update();
        }
    }

    return QGraphicsRectItem::itemChange(change, value);
}


void Gex::Ui::AttributeItem::UpdateLinks()
{
    for (auto* attr : indexedAttributes)
    {
        attr->UpdateLinks();
    }

    for (auto* attr : childAttributes)
    {
        attr->UpdateLinks();
    }

    for (auto link : links)
    {
        link->Update();
    }
}


QList<QString> Gex::Ui::AttributeItem::SubNames() const
{
    QList<QString> names;
    for (auto* attr : indexedAttributes)
    {
        names.append(attr->Attribute()->Longname().c_str());
        for (auto name : attr->SubNames())
        {
            names.append(name);
        }
    }

    for (auto* attr : childAttributes)
    {
        names.append(attr->Attribute()->Longname().c_str());
        for (auto name : attr->SubNames())
        {
            names.append(name);
        }
    }

    return names;


}


QList<Gex::Ui::LinkItem*> Gex::Ui::AttributeItem::SourceLinks() const
{
    QList<Gex::Ui::LinkItem*> items;

    for (auto link : links)
    {
        if (link->Dest() == this)
            items.append(link);
    }

    return items;
}


Gex::Ui::NodePlugItem::NodePlugItem(NodeItem* node_,
                                              bool input):
        QGraphicsEllipseItem(node_)
{
    node = node_;
    isInput = input;
}


Gex::Ui::NodeItem* Gex::Ui::NodePlugItem::Node() const
{
    return node;
}


bool Gex::Ui::NodePlugItem::IsInputAnchor() const
{
    return isInput;
}

bool Gex::Ui::NodePlugItem::IsOutputAnchor() const
{
    return (!isInput);
}



Gex::Ui::NodeItem::NodeItem(Gex::Node* node_,
                                      QGraphicsItem* parent):
        QObject(), QGraphicsItem(parent)
{
    node = node_;
    sourcePlug = new NodePlugItem(this, true);
    sourcePlug->setRect(PLUG_RECT);
    sourcePlug->setPos(-PLUG_RECT.width(), (ATTRIBUTE_HEIGHT - PLUG_RECT.height()) / 2);
    sourcePlug->setBrush(QBrush(QColor("#408CCB")));
    sourcePlug->setPen(Qt::NoPen);
    sourcePlug->setZValue(10);
//    sourcePlug->setBrush(QBrush(QColor("#E58C02")));

    destPlug = new NodePlugItem(this, false);
    destPlug->setRect(PLUG_RECT);
    destPlug->setPos(NODE_DEFAULT_WIDTH, (ATTRIBUTE_HEIGHT - PLUG_RECT.height()) / 2);
    destPlug->setBrush(QBrush(QColor("#408CCB")));
    destPlug->setPen(Qt::NoPen);
    destPlug->setZValue(10);
//    destPlug->setBrush(QBrush(QColor("#E58C02")));

    attrVisMode = AttributeVisibilityMode::All;

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemSendsGeometryChanges);

    title = new QGraphicsTextItem(this);
    title->setFont(QFont("sans", 13));
    title->setDefaultTextColor(QColor("white"));
    title->setPlainText(node_->Name().c_str());
    title->setTextInteractionFlags(Qt::TextEditable);
//    title->update();

    QTextDocument* doc = title->document();

    QObject::connect(doc, &QTextDocument::contentsChanged,
                     this, &NodeItem::TitleChanged,
                     Qt::QueuedConnection);

    QFont typeFont("sans", 8);
    typeFont.setItalic(true);

    QGraphicsTextItem* type = new QGraphicsTextItem(this);
    type->setFont(typeFont);
    type->setDefaultTextColor(QColor("white"));
    type->setPlainText(node->Type().c_str());
    type->setPos(0, title->boundingRect().height());

    CreateAttributes();

    PlaceAttributes();
}


void Gex::Ui::NodeItem::TitleChanged()
{
    node->SetName(title->toPlainText().toStdString());
}


Gex::Node* Gex::Ui::NodeItem::Node() const
{
    return node;
}


void Gex::Ui::NodeItem::SetAttributeVisibility(AttributeVisibilityMode mode)
{
    attrVisMode = mode;

    PlaceAttributes();

    UpdateLinks();
}


Gex::Ui::NodeItem::AttributeVisibilityMode Gex::Ui::NodeItem::AttributeVisibility() const
{
    return attrVisMode;
}

void Gex::Ui::NodeItem::CreateAttributes()
{
    for (auto* attr : node->GetAttributes())
    {
        AttributeItem* item = new AttributeItem(attr, this);
        attributes.push_back(item);
    }
}


void Gex::Ui::NodeItem::RebuildAttributes()
{
    for (auto* attr : attributes)
    {
        delete attr;
    }

    attributes.clear();

    CreateAttributes();

    PlaceAttributes();

    InitializeLinks();
}


void Gex::Ui::NodeItem::PlaceAttributes()
{
    qreal posY = NODE_HEADER_HEIGHT;

    bool skippedInput = false;
    bool skippedOutput = false;

    switch (AttributeVisibility())
    {
        case AttributeVisibilityMode::All:
            for (auto* attr : attributes)
            {
                attr->setVisible(true);
                attr->SetTextVisibility(true);
                attr->setPos(0, posY);
                posY += attr->TotalHeight();
            }
            sourcePlug->setVisible(false);
            destPlug->setVisible(false);
            break;
        case AttributeVisibilityMode::Settable:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->HasSource() ||
                    attr->Attribute()->IsDefault() ||
                    (attr->Attribute()->IsOutput() && !attr->Attribute()->IsStatic()))
                {
                    attr->Collapse();
                    attr->SetTextVisibility(false);
                    attr->setPos(0, 0);
                    if (attr->Attribute()->IsInput())
                    {
                        skippedInput = true;
                    }
                    if (attr->Attribute()->IsOutput())
                    {
                        skippedOutput = true;
                    }
                }
                else
                {
                    attr->setVisible(true);
                    attr->SetTextVisibility(true);
                    attr->setPos(0, posY);
                    posY += attr->TotalHeight();
                }
            }

            break;
        case AttributeVisibilityMode::Connected:

            for (auto* attr : attributes)
            {
                if (attr->Attribute()->HasSource() ||
                    attr->Attribute()->HasDests())
                {
                    attr->setVisible(true);
                    attr->SetTextVisibility(true);
                    attr->setPos(0, posY);
                    posY += attr->TotalHeight();
                }
                else
                {
                    attr->Collapse();
                    attr->SetTextVisibility(false);
                    attr->setPos(0, 0);

                    if (attr->Attribute()->IsInput())
                    {
                        skippedInput = true;
                    }
                    if (attr->Attribute()->IsOutput())
                    {
                        skippedOutput = true;
                    }
                }
            }
            break;

        case AttributeVisibilityMode::OnlyInputs:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->IsInput() ||
                    attr->Attribute()->IsStatic())
                {
                    attr->setVisible(true);
                    attr->SetTextVisibility(true);
                    attr->setPos(0, posY);
                    posY += attr->TotalHeight();
                }
                else
                {
                    attr->Collapse();
                    attr->SetTextVisibility(false);
                    attr->setPos(0, 0);

                    if (attr->Attribute()->IsInput())
                    {
                        skippedInput = true;
                    }
                    if (attr->Attribute()->IsOutput())
                    {
                        skippedOutput = true;
                    }
                }
            }
            break;

        case AttributeVisibilityMode::OnlyOutputs:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->IsOutput() ||
                    attr->Attribute()->IsStatic())
                {
                    attr->setVisible(true);
                    attr->SetTextVisibility(true);
                    attr->setPos(0, posY);
                    posY += attr->TotalHeight();
                }
                else
                {
                    attr->Collapse();
                    attr->SetTextVisibility(false);
                    attr->setPos(0, 0);

                    if (attr->Attribute()->IsInput())
                    {
                        skippedInput = true;
                    }
                    if (attr->Attribute()->IsOutput())
                    {
                        skippedOutput = true;
                    }
                }
            }
            break;

        case AttributeVisibilityMode::OnlyInternalInputs:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->IsInternal() &&
                    (attr->Attribute()->IsInput() ||
                     attr->Attribute()->IsStatic()))
                {
                    attr->setVisible(true);
                    attr->SetTextVisibility(true);
                    attr->setPos(0, posY);
                    posY += attr->TotalHeight();
                }
                else
                {
                    attr->Collapse();
                    attr->setVisible(false);
                    attr->setPos(0, 0);
                }
            }
            break;

        case AttributeVisibilityMode::OnlyInternalOutputs:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->IsInternal() &&
                    (attr->Attribute()->IsOutput() &&
                     !attr->Attribute()->IsStatic()))
                {
                    attr->setVisible(true);
                    attr->SetTextVisibility(true);
                    attr->setPos(0, posY);
                    posY += attr->TotalHeight();
                }
                else
                {
                    attr->Collapse();
                    attr->setVisible(false);
                    attr->setPos(0, 0);
                }
            }
            break;

        default:
            break;
    }

    sourcePlug->setVisible(skippedInput);
    destPlug->setVisible(skippedOutput);
}


std::vector<Gex::Attribute*> AttributeHierarchy(Gex::Attribute* at)
{
    std::vector<Gex::Attribute*> hierarchy = {at};

    Gex::Attribute* attr = at;
    while(attr->ParentAttribute())
    {
        hierarchy.insert(hierarchy.begin(), attr->ParentAttribute());
        attr = attr->ParentAttribute();
    }

    return hierarchy;
}


// TODO : Optimize this.
Gex::Ui::AttributeItem* Gex::Ui::NodeItem::FindAttribute(std::string longname) const
{
    Gex::Attribute* coreAttribute = node->GetAttribute(longname);
    auto hierarchy = AttributeHierarchy(coreAttribute);

    AttributeItem* current = nullptr;
    for (Gex::Attribute* attr : hierarchy)
    {
        AttributeItem* item = nullptr;
        if (current)
            item = current->GetSubAttribute(attr);
        else
            item = GetAttribute(attr);

        if (!item)
        {
            return nullptr;
        }

        current = item;
    }

    return current;
}


Gex::Ui::AttributeItem* Gex::Ui::NodeItem::GetAttribute(Gex::Attribute* searched) const
{
    for (auto* at : attributes)
    {
        if (at->Attribute() == searched)
        {
            return at;
        }
    }

    return nullptr;
}


void Gex::Ui::NodeItem::InitializeLinks()
{
    for (auto* attr : attributes)
    {
        // TODO: Not the best way of doing this...
        if (showInternal && !attr->isVisible())
        {
            continue;
        }

        attr->InitializeLinks();
    }
}


QSize Gex::Ui::NodeItem::AdjustedSize() const
{
    qreal height = NODE_HEADER_HEIGHT;

    switch (AttributeVisibility())
    {
        case AttributeVisibilityMode::All:
            for (auto* attr : attributes)
            {
                height += attr->TotalHeight();
            }
            break;
        case AttributeVisibilityMode::Settable:
            for (auto* attr : attributes)
            {
                if (!attr->Attribute()->HasSource() &&
                    !attr->Attribute()->IsDefault() &&
                    attr->Attribute()->IsInput())
                {
                    height += attr->TotalHeight();
                }
            }
            break;
        case AttributeVisibilityMode::Connected:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->HasSource() ||
                    attr->Attribute()->HasDests())
                {
                    height += attr->TotalHeight();
                }
            }
            break;
        case AttributeVisibilityMode::OnlyInputs:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->IsInput() ||
                    attr->Attribute()->IsStatic())
                {
                    height += attr->TotalHeight();
                }
            }
            break;
        case AttributeVisibilityMode::OnlyOutputs:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->IsOutput() ||
                    !attr->Attribute()->IsStatic())
                {
                    height += attr->TotalHeight();
                }
            }
            break;
        case AttributeVisibilityMode::OnlyInternalInputs:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->IsInternal() &&
                    (attr->Attribute()->IsInput() ||
                     attr->Attribute()->IsStatic()))
                {
                    height += attr->TotalHeight();
                }
            }
            break;
        case AttributeVisibilityMode::OnlyInternalOutputs:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->IsInternal() &&
                    (attr->Attribute()->IsOutput() &&
                     !attr->Attribute()->IsStatic()))
                {
                    height += attr->TotalHeight();
                }
            }
            break;
    }


    return QSize(NODE_DEFAULT_WIDTH, height);
}


void Gex::Ui::NodeItem::AdjustSize()
{
    update();
}


QRectF Gex::Ui::NodeItem::boundingRect() const
{
    QSize size = AdjustedSize();
    return QRectF(0, 0, size.width(), size.height());
}


QVariant Gex::Ui::NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged)
    {
        for (auto* attribute : attributes)
        {
            attribute->UpdateLinks();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}


QColor Gex::Ui::NodeItem::DefaultBorderColor() const
{
    return QColor("#E58C02");
}

QColor Gex::Ui::NodeItem::DefaultSelectedBorderColor() const
{
    return QColor("#E58C02");
}

void Gex::Ui::NodeItem::SetCustomBorderColor(QColor color)
{
    customBorderColor = color;
    userCustomBorderColor = true;

    update();

    QApplication::processEvents();
}

void Gex::Ui::NodeItem::ClearCustomBorderColor()
{
    userCustomBorderColor = false;

    update();
}

QColor Gex::Ui::NodeItem::CustomBorderColor() const
{
    return customBorderColor;
}

bool Gex::Ui::NodeItem::HasCustomBorderColor() const
{
    return userCustomBorderColor;
}


void Gex::Ui::NodeItem::paint(QPainter *painter,
                                        const QStyleOptionGraphicsItem *option,
                                        QWidget *widget)
{
    QRectF rect = boundingRect();

    painter->save();

    painter->setPen(Qt::NoPen);

    if (!isSelected() && !HasCustomBorderColor())
    {
        painter->setBrush(QBrush(QColor("#383838")));
        painter->drawRoundedRect(rect, 5, 5);
    }
    else
    {
        QColor border = DefaultBorderColor();
        if (HasCustomBorderColor())
            border = CustomBorderColor();

        if (isSelected())
            border = border.lighter(150);

        painter->setBrush(QBrush(border));
        painter->drawRoundedRect(rect, 5, 5);

        QRectF inSet(rect);
        inSet.setX(2);
        inSet.setY(2);
        inSet.setWidth(rect.width() - 4);
        inSet.setHeight(rect.height() - 4);

        painter->setBrush(QBrush(QColor("#383838")));
        painter->drawRoundedRect(inSet, 5, 5);
    }

    painter->restore();
}


QList<QString> Gex::Ui::NodeItem::HiddenAttributes() const
{
    QList<QString> hidden;
    for (auto* attr : attributes)
    {
        if (attr->TextVisibility())
        {
            continue;
        }

        hidden.append(attr->Attribute()->Longname().c_str());
        for (QString name : attr->SubNames())
        {
            hidden.append(name);
        }
    }

    return hidden;
}


void Gex::Ui::NodeItem::UpdateLinks()
{
    for (auto* attr : attributes)
    {
        attr->UpdateLinks();
    }
}


void Gex::Ui::NodeItem::ShowAsInternal(bool internal)
{
    for (auto* attribute : attributes)
    {
        attribute->ShowAsInternal(internal);
    }

    showInternal = internal;
}


bool Gex::Ui::NodeItem::IsShowingInternal() const
{
    return showInternal;
}


void Gex::Ui::NodeItem::ShowAsInternalInput()
{
    ShowAsInternal(true);

    SetAttributeVisibility(AttributeVisibilityMode::OnlyInternalInputs);
}


void Gex::Ui::NodeItem::ShowAsInternalOutput()
{
    ShowAsInternal(true);

    SetAttributeVisibility(AttributeVisibilityMode::OnlyInternalOutputs);
}


Gex::Ui::LinkItem::LinkItem(AttributeItem *source, AttributeItem *dest): QGraphicsPathItem(), QObject()
{
    setFlag(QGraphicsItem::ItemIsSelectable);

    src = source;
    dst = dest;

    if (auto* cmp = Gex::CompoundNode::FromNode(src->Attribute()->Node()))
    {
        if (cmp->IsInternalNode(dst->Attribute()->Node()))
        {
            sourceInternal = true;
        }
    }

    if (auto* cmp = Gex::CompoundNode::FromNode(dst->Attribute()->Node()))
    {
        if (cmp->IsInternalNode(src->Attribute()->Node()))
        {
            destInternal = true;
        }
    }

    QPen pen(QColor("#E58C02"));
    pen.setWidth(2);
    setPen(pen);

    setZValue(-10);
}


Gex::Ui::LinkItem::~LinkItem() {
    src->RemoveLink(this);
    dst->RemoveLink(this);
}


Gex::Ui::AttributeItem* Gex::Ui::LinkItem::Source() const
{
    return src;
}


Gex::Ui::AttributeItem* Gex::Ui::LinkItem::Dest() const
{
    return dst;
}


void Gex::Ui::LinkItem::Update()
{
    QPointF srcPos = src->SceneOutputPosition();
    if (sourceInternal)
        srcPos = src->SceneInputPosition();

    QPointF dstPos = dst->SceneInputPosition();
    if (destInternal)
        dstPos = dst->SceneOutputPosition();

    QPainterPath path;
    path.moveTo(srcPos);

    qreal middle = std::abs(dstPos.x() - srcPos.x());
    QPoint cp1(srcPos.x() + middle / 2, srcPos.y());
    QPoint cp2(dstPos.x() - middle / 2, dstPos.y());
    path.cubicTo(cp1, cp2, dstPos);

    setPath(path);
}


void Gex::Ui::LinkItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                        QWidget *widget)
{
    QStyleOptionGraphicsItem copiedOption(*option);
    copiedOption.state = QStyle::State_Enabled;

    QGraphicsPathItem::paint(painter, &copiedOption, widget);
}


QVariant Gex::Ui::LinkItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    QPen pen;
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        if (isSelected())
        {
            pen = QPen(QColor("#E58C02"));
            pen.setWidth(2);
        }
        else
        {
            pen = QPen(QColor("#F5DB1E"));
            pen.setWidth(2);
        }

        setPen(pen);
    }

    return value;
}


Gex::Ui::PreviewLinkItem::PreviewLinkItem(PlugItem* plug): QGraphicsPathItem()
{
    anchorPlug = plug;

    SetState(State::Default);

    setZValue(-100);
}


Gex::Ui::PlugItem* Gex::Ui::PreviewLinkItem::SourcePlug() const
{
    return anchorPlug;
}


Gex::Attribute* Gex::Ui::PreviewLinkItem::Attribute() const
{
    return anchorPlug->Attribute()->Attribute();
}


void Gex::Ui::PreviewLinkItem::SetState(PreviewLinkItem::State state)
{
    QColor color;
    switch (state)
    {
        case State::Default:
            color = QColor("#E58C02");
            break;
        case State::Valid:
            color = QColor("#21F192");
            break;
        case State::Invalid:
            color = QColor("#F13121");
            break;
    }

    QPen pen(color);
    pen.setColor(color);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    setPen(pen);
}


void Gex::Ui::PreviewLinkItem::Draw(QPointF dest)
{
    QPainterPath path;
    path.moveTo(anchorPlug->sceneBoundingRect().center());
    path.lineTo(dest);

    setPath(path);
}





Gex::Ui::NodeGraphScene::NodeGraphScene(QObject* parent): QGraphicsScene(parent)
{
    previewLink = nullptr;

    setSceneRect(QRect(-100000, -100000, 200000, 200000));
}


void Gex::Ui::NodeGraphScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsScene::mousePressEvent(mouseEvent);

    QGraphicsItem* item = itemAt(mouseEvent->scenePos(), QTransform());
    if (item)
    {
        PlugItem *plugItem = dynamic_cast<PlugItem *>(item);
        if (!previewLink && plugItem)
        {
            previewLink = new PreviewLinkItem(plugItem);
            previewLink->SetState(PreviewLinkItem::State::Default);
            addItem(previewLink);
            previewLink->Draw(mouseEvent->scenePos());

            for (auto *view: views())
            {
                view->setDragMode(QGraphicsView::NoDrag);
            }
        }
    }

    if (mouseZooming && mouseEvent->button() == Qt::RightButton)
    {
        mouseZoomingClicked = true;
        mouseZoomingPos = mouseEvent->screenPos();
    }
}


void Gex::Ui::NodeGraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);

    if (previewLink)
    {
        QGraphicsItem* item = itemAt(mouseEvent->scenePos(), QTransform());
        if (item)
        {
            PlugItem* plugItem = dynamic_cast<PlugItem*>(item);
            if (plugItem && (plugItem != previewLink->SourcePlug()))
            {
                AttributeItem* destItem = plugItem->Attribute();
                AttributeItem* sourceItem = previewLink->SourcePlug()->Attribute();
                if (destItem->Attribute()->IsMulti() == sourceItem->Attribute()->IsMulti())
                {
                    // If source plug is an input, then check if the dest attribute can
                    // be connected to destination.
                    if (previewLink->SourcePlug()->IsInputAnchor() &&
                        sourceItem->Attribute()->CanConnectSource(destItem->Attribute()))
                    {
                        previewLink->SetState(PreviewLinkItem::State::Valid);
                    }
                        // If source plug is an output, check if the destination attribute can
                        // be connected as a source to the source plug.
                    else if (previewLink->SourcePlug()->IsOutputAnchor() &&
                             destItem->Attribute()->CanConnectSource(sourceItem->Attribute()))
                    {
                        previewLink->SetState(PreviewLinkItem::State::Valid);
                    }
                        // If source plug is an input, is internal and belongs to the current
                        // compound input, check if the dest attribute can receive it as a
                        // source attribute.
                    else if (previewLink->SourcePlug()->IsInputAnchor() &&
                             sourceItem->Attribute()->IsInternal() &&
                             sourceItem->ParentNode() == input &&
                             destItem->Attribute()->CanConnectSource(sourceItem->Attribute()))
                    {
                        previewLink->SetState(PreviewLinkItem::State::Valid);
                    }
                        // If source plug is an internal output of current compound, check if source
                        // plug can receive dest as a connection.
                    else if (previewLink->SourcePlug()->IsOutputAnchor() &&
                             sourceItem->Attribute()->IsInternal() &&
                             sourceItem->ParentNode() == output &&
                             sourceItem->Attribute()->CanConnectSource(destItem->Attribute()))
                    {
                        previewLink->SetState(PreviewLinkItem::State::Valid);
                    }
                    else
                    {
                        previewLink->SetState(PreviewLinkItem::State::Invalid);
                    }
                }
                else if (!sourceItem->IsMulti() && destItem->IsMulti())
                {
                    if (!destItem->Attribute()->ValidIndices().empty())
                    {
                        destItem->Expand();
                        destItem->ParentNode()->PlaceAttributes();
                        previewLink->SetState(PreviewLinkItem::State::Valid);
                    }
                }
                else
                {
                    previewLink->SetState(PreviewLinkItem::State::Invalid);
                }
            }
            else
            {
                previewLink->SetState(PreviewLinkItem::State::Default);
            }
        }

        previewLink->Draw(mouseEvent->scenePos());
    }

// TODO : Generates a Stack Overflow since ZoomIn / ZoomOut launches another
//  mouseMoveEvent on scene.
//    if (mouseZooming && mouseZoomingClicked)
//    {
//        qreal delta = mouseEvent->screenPos().x() - mouseZoomingPos.x();
//        NodeGraphView* view = dynamic_cast<NodeGraphView*>(views()[0]);
//        if (view)
//        {
//            if (delta > 0)
//                view->ZoomOut();
//            else
//                view->ZoomIn();
//        }
//        mouseZoomingPos = mouseEvent->screenPos();
//    }
}


void Connect(Gex::Ui::AttributeItem* source,
             Gex::Ui::AttributeItem* dest)
{
    for (auto* l : dest->SourceLinks())
    {
        delete l;
    }

    if (dest->Attribute()->ConnectSource(source->Attribute()))
    {
        dest->InitializeLinks();
    }
}


void Gex::Ui::NodeGraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    QGraphicsItem *item = itemAt(mouseEvent->scenePos(), QTransform());
    if (previewLink) {
        if (item)
        {
            PlugItem *plugItem = dynamic_cast<PlugItem *>(item);

            if (plugItem && (plugItem != previewLink->SourcePlug()))
            {
                AttributeItem* destItem = plugItem->Attribute();
                AttributeItem* sourceItem = previewLink->SourcePlug()->Attribute();
                if (destItem->IsMulti() == sourceItem->IsMulti())
                {
                    // If source plug is an input, then check if the dest attribute can
                    // be connected to destination.
                    if (previewLink->SourcePlug()->IsInputAnchor() &&
                        sourceItem->Attribute()->CanConnectSource(destItem->Attribute()))
                    {
//                        sourceItem->Attribute()->ConnectSource(destItem->Attribute());
//                        sourceItem->InitializeLinks();
                        Connect(destItem, sourceItem);
                    }
                        // If source plug is an output, check if the destination attribute can
                        // be connected as a source to the source plug.
                    else if (previewLink->SourcePlug()->IsOutputAnchor() &&
                             destItem->Attribute()->CanConnectSource(sourceItem->Attribute()))
                    {
//                        destItem->Attribute()->ConnectSource(sourceItem->Attribute());
//                        destItem->InitializeLinks();
                        Connect(sourceItem, destItem);
                    }
                        // If source plug is an input, is internal and belongs to the current
                        // compound input, check if the dest attribute can receive it as a
                        // source attribute.
                    else if (previewLink->SourcePlug()->IsInputAnchor() &&
                             sourceItem->Attribute()->IsInternal() &&
                             sourceItem->ParentNode() == input &&
                             destItem->Attribute()->CanConnectSource(sourceItem->Attribute()))
                    {
//                        destItem->Attribute()->ConnectSource(sourceItem->Attribute());
//                        destItem->InitializeLinks();
                        Connect(sourceItem, destItem);
                    }
                        // If source plug is an internal output of current compound, check if source
                        // plug can receive dest as a connection.
                    else if (previewLink->SourcePlug()->IsOutputAnchor() &&
                             sourceItem->Attribute()->IsInternal() &&
                             sourceItem->ParentNode() == output &&
                             sourceItem->Attribute()->CanConnectSource(destItem->Attribute()))
                    {
//                        sourceItem->Attribute()->ConnectSource(destItem->Attribute());
//                        sourceItem->InitializeLinks();
                        Connect(destItem, sourceItem);
                    }
                }
                else if (!sourceItem->IsMulti() && destItem->IsMulti())
                {
                    // Connect to next available index.
                    bool connected = false;
                    unsigned int maxIndex = 0;
                    for (unsigned int index : destItem->Attribute()->ValidIndices())
                    {
                        Gex::Attribute* indexAttr = destItem->Attribute()->GetIndexAttribute(index);
                        if (!indexAttr)
                        {
                            continue;
                        }

                        if (!indexAttr->HasSource())
                        {
                            indexAttr->ConnectSource(sourceItem->Attribute());
                            destItem->InitializeLinks();
                            connected = true;
                            break;
                        }
                    }
                    if (!connected)
                    {
                        unsigned int newIndex = NextMultiAttributeIndex(destItem->Attribute());
                        if (destItem->Attribute()->CreateIndex(newIndex))
                        {
                            auto *indexAttr = destItem->Attribute()->GetIndexAttribute(newIndex);
                            indexAttr->ConnectSource(sourceItem->Attribute());

                            // TODO : Rebuilding attributes should be available from attribute !
                            auto* nodeItem = destItem->ParentNode();
                            destItem->RebuildAttributes();
                        }
                    }
                }
            }
        }

        removeItem(previewLink);
        delete previewLink;
        previewLink = nullptr;

        for (auto* view : views())
        {
            view->setDragMode(QGraphicsView::RubberBandDrag);
        }
    }

    else
    {
        if (NodePlugItem* nodePlugItem = dynamic_cast<NodePlugItem*>(item))
        {
            Q_EMIT NodePlugClicked(nodePlugItem);
        }
    }

    if (mouseZoomingClicked)
    {
        mouseZoomingClicked = false;
    }
}


void Gex::Ui::NodeGraphScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);

    QGraphicsItem *item = itemAt(event->scenePos(), QTransform());

    if (item)
    {
        OnItemDoubleClicked(item);
    }
}


void Gex::Ui::NodeGraphScene::StartPlugLinking(PlugItem* source)
{
    if (previewLink)
    {
        removeItem(previewLink);
        delete previewLink;
    }

    previewLink = new PreviewLinkItem(source);
    addItem(previewLink);
    previewLink->SetState(PreviewLinkItem::State::Default);
}


void Gex::Ui::NodeGraphScene::OnItemDoubleClicked(QGraphicsItem* item)
{
    NodeItem* nodeItem = dynamic_cast<NodeItem*>(item);
    if (!nodeItem)
    {
        auto* parent = item->parentItem();
        if (parent)
        {
            nodeItem = dynamic_cast<NodeItem*>(parent);
            if (!nodeItem)
            {
                return;
            }
        }
    }

    Gex::CompoundNode* node = dynamic_cast<Gex::CompoundNode*>(nodeItem->Node());
    if (!node || nodeItem->IsShowingInternal())
    {
        return;
    }


    Q_EMIT CompoundNodeDoubleClicked(node);
}


void Gex::Ui::NodeGraphScene::UpdateNode(Gex::Node* node)
{
    NodeItem* nodeItem = nodeItems.value(node, nullptr);
    if (!nodeItem)
        return;

    nodeItem->RebuildAttributes();
}


void Gex::Ui::NodeGraphScene::UpdateNodeAttribute(Gex::Node* node,
                                                            QString attribute)
{
    NodeItem* nodeItem = nodeItems.value(node, nullptr);
    if (!nodeItem)
        return;

    AttributeItem* attributeItem = nodeItem->FindAttribute(attribute.toStdString());
    if (!attributeItem)
        return;

    attributeItem->RebuildAttributes();
}


Gex::Ui::NodeGraphView::NodeGraphView(QGraphicsScene* scene, QWidget* parent):
    BaseGraphView(scene, parent, true)
{

    QShortcut* shortcut_1 = new QShortcut(QKeySequence("1"), this);
    QObject::connect(shortcut_1, &QShortcut::activated, this,
                     &NodeGraphView::SetSelectedNodesVisAll);

    QShortcut* shortcut_2 = new QShortcut(QKeySequence("2"), this);
    QObject::connect(shortcut_2, &QShortcut::activated, this,
                     &NodeGraphView::SetSelectedNodesVisSettable);

    QShortcut* shortcut_3 = new QShortcut(QKeySequence("3"), this);
    QObject::connect(shortcut_3, &QShortcut::activated, this,
                     &NodeGraphView::SetSelectedNodesVisConnected);
}


void Gex::Ui::NodeGraphView::SetSelectedNodesVis(NodeItem::AttributeVisibilityMode mode)
{
    for (QGraphicsItem* item : scene()->selectedItems())
    {
        NodeItem* node = dynamic_cast<NodeItem*>(item);
        if (!node)
        {
            continue;
        }

        node->SetAttributeVisibility(mode);
    }
}


void Gex::Ui::NodeGraphView::SetSelectedNodesVisAll()
{
    SetSelectedNodesVis(NodeItem::AttributeVisibilityMode::All);
}


void Gex::Ui::NodeGraphView::SetSelectedNodesVisSettable()
{
    SetSelectedNodesVis(NodeItem::AttributeVisibilityMode::Settable);
}


void Gex::Ui::NodeGraphView::SetSelectedNodesVisConnected()
{
    SetSelectedNodesVis(NodeItem::AttributeVisibilityMode::Connected);
}


QList<QGraphicsItem*> Gex::Ui::NodeGraphView::FilterSelectedItems(
        const QList<QGraphicsItem*> items) const
{
    QList<QGraphicsItem*> nodes;
    for (auto item : items)
    {
        if (dynamic_cast<Gex::Ui::NodeItem*>(item))
        {
            nodes.append(item);
        }
    }

    return nodes;
}


struct NodeCreator: public QObject
{
    Gex::Ui::NodeGraphScene* scene;
    QString type;

    NodeCreator(): QObject() {}

    void CreateNode(bool)
    {
        scene->CreateNode(type, "");
    }
};


QMenu* Gex::Ui::NodeGraphView::GetMenu()
{
    QMenu* createMenu = new QMenu();

    QList<NodeCreator*> creators;
    QMap<QString, QMenu*> menus;
    for (std::string type : Gex::NodeFactory::GetFactory()->NodeTypes())
    {
        QString nodeType = type.c_str();
        QStringList tokens = nodeType.split("/");

        QMenu* parent = createMenu;
        if (tokens.length() > 1)
        {
            for (int i = 0; i < tokens.length() - 1; i++)
            {
                QString token = tokens[i];

                QMenu* curMenu = menus.value(token, nullptr);
                if (!curMenu)
                {
                    curMenu = parent->addMenu(token);
//                    curMenu->setTitle(token);
                    menus[token] = curMenu;
                }

                parent = curMenu;
            }
        }

        QAction* action = new QAction(parent);
        action->setText(tokens[tokens.length() - 1]);
        parent->addAction(action);

        NodeCreator* creator = new NodeCreator();
        creator->scene = dynamic_cast<NodeGraphScene*>(scene());
        creator->type = nodeType;

        QObject::connect(action, &QAction::triggered,
                         creator, &NodeCreator::CreateNode);
    }

    return createMenu;
}


void Gex::Ui::NodeGraphView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete)
    {
        auto* scene_ = dynamic_cast<NodeGraphScene*>(scene());
        if (scene_)
        {
            scene_->DeleteSelection();
        }
    }
    else
    {
        BaseGraphView::keyPressEvent(event);
    }
}


void Gex::Ui::NodeGraphView::ConnectCallback::DoIt()
{
    scene->StartPlugLinking(plug);

    if (mainMenu)
    {
        mainMenu->close();
    }
}


void Gex::Ui::ClickableMenu::mouseReleaseEvent(QMouseEvent* event)
{
    Q_EMIT clicked();

    QMenu::mouseReleaseEvent(event);
}


void Gex::Ui::NodeGraphView::OnNodePlugClicked(NodePlugItem* plug)
{
    QMenu* plugMenu = new QMenu(this);

    QList<ConnectCallback*> callbacks;

    bool searchInput = (plug->IsInputAnchor());

    std::unordered_map<QString, ClickableMenu*> generatedActions;
    for (QString name : plug->Node()->HiddenAttributes())
    {
        QStringList tokens = name.split(Config::GetConfig().attributeSeparator.c_str());

        ClickableMenu* action = new ClickableMenu(plugMenu);
        action->setTitle(tokens[tokens.length() - 1]);

        AttributeItem* attribute = plug->Node()->FindAttribute(name.toStdString());

        if (searchInput && !attribute->Attribute()->IsInput())
            continue;

        else if (!searchInput && !attribute->Attribute()->IsOutput())
            continue;

        ConnectCallback* cb = new ConnectCallback();
        if (searchInput)
            cb->plug = attribute->InputAnchor();
        else
            cb->plug = attribute->OutputAnchor();

        cb->mainMenu = plugMenu;
        cb->scene = dynamic_cast<NodeGraphScene*>(scene());

        QObject::connect(action, &ClickableMenu::clicked,
                         cb, &ConnectCallback::DoIt);

        callbacks.append(cb);

        if (tokens.length() > 1)
        {
            ClickableMenu* parentAction = generatedActions.at(tokens[tokens.length() - 2]);
            if (parentAction)
            {
                parentAction->addMenu(action);
            }
        }
        else
        {
            plugMenu->addMenu(action);
        }
    }

    plugMenu->exec(QCursor::pos());

    for (ConnectCallback* callback : callbacks)
    {
        delete callback;
    }

    plugMenu->deleteLater();
}


void Gex::Ui::NodeGraphScene::Clear()
{
    clear();
    nodeItems.clear();
    input = nullptr;
    output = nullptr;
}


void Gex::Ui::NodeGraphScene::SwitchGraphContext(NodeGraphContext* context)
{
    if (!nodeItems.empty())
    {
        for (auto* node : nodeItems.keys())
        {
            graphContext->nodePositions[node] = nodeItems.value(node)->scenePos();
        }
    }

    Clear();  // Clear all content.

    graphContext = context;
    for (Gex::Node* node : graphContext->GetNodes())
    {
        NodeItem* item = new NodeItem(node);
        addItem(item);

        nodeItems[node] = item;

        if (graphContext->nodePositions.contains(node))
        {
            item->setPos(graphContext->nodePositions.value(node));
        }
    }

    if (context->compound)
    {
        input = new NodeItem(context->compound);
        addItem(input);
        input->ShowAsInternalInput();

        output = new NodeItem(context->compound);
        addItem(output);
        output->ShowAsInternalOutput();
    }

    for (auto* item : nodeItems)
    {
        item->InitializeLinks();
    }

    if (input)
    {
        input->InitializeLinks();
    }
    if (output)
    {
        output->InitializeLinks();
    }
}


void Gex::Ui::NodeGraphScene::DeleteNode(NodeItem* item)
{
    Gex::Node* node = item->Node();
    nodeItems.remove(item->Node());

    removeItem(item);
    delete item;

    graphContext->DeleteNode(node);
}


void Gex::Ui::NodeGraphScene::DeleteLink(LinkItem* item)
{
    AttributeItem* source = item->Source();
    AttributeItem* dest = item->Dest();

    dest->Attribute()->DisconnectSource(source->Attribute());
    removeItem(item);
    delete item;
}


void Gex::Ui::NodeGraphScene::DeleteSelection()
{
    QList<QPointer<NodeItem>> deleteNodes;
    QList<QPointer<LinkItem>> deleteLinks;
    for (QGraphicsItem* item : selectedItems())
    {
        if (NodeItem* nodeItem = dynamic_cast<NodeItem*>(item))
        {
            deleteNodes.append(nodeItem);
        }
        else if (LinkItem* linkItem = dynamic_cast<LinkItem*>(item))
        {
            deleteLinks.append(linkItem);
        }
    }

    for (auto linkItem : deleteLinks)
    {
        if (linkItem.isNull())
            continue;

        DeleteLink(linkItem);
    }

    for (auto nodeItem : deleteNodes)
    {
        if (nodeItem.isNull())
            continue;

        DeleteNode(nodeItem);
    }
}


void Gex::Ui::NodeGraphScene::CreateNode(QString type, QString name)
{
    Gex::Node* node = graphContext->CreateNode(type.toStdString(),
                                                          name.toStdString());

    auto* item = new NodeItem(node);
    addItem(item);
    nodeItems[node] = item;

    item->InitializeLinks();
}


void Gex::Ui::NodeGraphScene::NodeEvaluationStarted(Gex::Node* node)
{
    auto* nodeItem = nodeItems.value(node, nullptr);
    if (!nodeItem)
        return;

    nodeItem->SetCustomBorderColor(QColor("#5B6FD3"));

}

void Gex::Ui::NodeGraphScene::NodeEvaluationDone(Gex::Node* node, bool success)
{
    auto* nodeItem = nodeItems.value(node, nullptr);
    if (!nodeItem)
        return;
    QColor color("#85E837");
    if (!success)
        color = "#E83737";

    nodeItem->SetCustomBorderColor(color);
}

void Gex::Ui::NodeGraphScene::ClearNodeEvaluation()
{
    for (auto p : nodeItems)
    {
        p->ClearCustomBorderColor();
    }
}


void Gex::Ui::NodeGraphScene::DisableInteraction()
{
    for (auto* view : views())
        view->setInteractive(false);
}


void Gex::Ui::NodeGraphScene::EnableInteraction()
{
    for (auto* view : views())
        view->setInteractive(true);
}


struct LayoutColumn
{
    QList<Gex::Ui::NodeItem*> nodes;
    qreal spacing;

    qreal Height() const
    {
        qreal height = 0;
        for (auto node : nodes)
        {
            height += node->sceneBoundingRect().height();
        }

        height += (nodes.size() - 1) * spacing;
        return height;
    }

    qreal Width() const
    {
        qreal width = 0;
        for (auto node : nodes)
        {
            qreal itemWidth = node->sceneBoundingRect().width();
            if (itemWidth > width)
                width = itemWidth;
        }

        return width;
    }
};


struct Layout
{
    std::vector<LayoutColumn> columns;
    qreal hspacing;

    QRect Rect(const QPointF& center) const
    {
        qreal width = 0;
        qreal height = 0;
        for (auto col : columns)
        {
            width += col.Width();
            if (col.Height() > height)
                height = col.Height();
        }

        width += (columns.size() - 1) * hspacing;

        return QRect(center.x() - (width / 2),
                     center.y() - (height / 2),
                     width, height);
    }
};



void Gex::Ui::NodeGraphScene::AutoLayoutNodes(const QPointF& destination,
                                                        qreal hspacing, qreal vspacing)
{
    auto nodes = nodeItems.keys();

    std::vector<Gex::Node*> nodeVec;
    for (auto* node : nodes)
        nodeVec.push_back(node);

    auto schelNodes = Gex::ScheduleNodes(nodeVec, false);

    std::map<int, std::vector<Gex::Node*>> nodeLevels;
    for (auto schel : schelNodes)
    {
        int i = 0;
        for (auto* node : schel->previousNodes)
        {
            for (auto nl : nodeLevels)
            {
                if (std::find(nl.second.begin(), nl.second.end(), node->node) != nl.second.end())
                {
                    if (nl.first > i)
                        i = nl.first;
                }
            }
        }

        if (nodeLevels.find(i + 1) == nodeLevels.end())
        {
            nodeLevels[i + 1] = {schel->node};
        }
        else
        {
            nodeLevels[i + 1].push_back(schel->node);
        }
    }

    // Clean Scheduled nodes.
    for (auto* schelnode : schelNodes)
        delete schelnode;

    schelNodes.clear();

    qreal len = nodeLevels.size();

    qreal rectHeight = 0;
    qreal rectWidth = 0;

    std::vector<LayoutColumn> columns;
    for (const auto& levelNodes : nodeLevels)
    {
        size_t columnWidth = 0;
        size_t columnHeight = 0;

        QList<Gex::Ui::NodeItem*> items;
        for (auto node : levelNodes.second)
        {
            if (!nodeItems.contains(node))
                continue;

            items.push_back(nodeItems.value(node));
        }

        LayoutColumn column = {items, vspacing};
        columns.push_back(column);
    }

    Layout layout = {columns, hspacing};

    QPointF center = destination;
    QRect destinationRect = layout.Rect(center);

    // For level, node in nodeLevels
    //     for node in level
    //          calculer la hauter vide
    //     se deplacer horizontalement
    qreal px = destinationRect.x();
    for (const auto& col : layout.columns)
    {
        qreal leftSpace = (destinationRect.height() - col.Height()) / 2.0;
        qreal py = destinationRect.y() + leftSpace;

        for (auto* node : col.nodes)
        {
            node->setPos(px, py);
            py += vspacing + node->sceneBoundingRect().height();
        }

        px += hspacing + col.Width();
    }
}


Gex::Ui::ContextButton::ContextButton(std::string name_, unsigned int index_,
                                                QWidget* parent): QPushButton(parent)
{
    index = index_;
    name = name_;

    QObject::connect(this, &ContextButton::clicked,
                     this, &ContextButton::SignalClicked);

    setContentsMargins(5, 5, 5, 5);
    setText(name.c_str());
}


void Gex::Ui::ContextButton::SignalClicked(bool)
{
    Q_EMIT ContextClicked(index);
}


Gex::Ui::ContextsWidget::ContextsWidget(QWidget* parent): QWidget(parent)
{
    buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(0);
    buttonsLayout->setAlignment(Qt::AlignLeft);
    setLayout(buttonsLayout);
}


void Gex::Ui::ContextsWidget::ContextClicked(unsigned int index)
{
    Q_EMIT ContextRequested(index);
}


void Gex::Ui::ContextsWidget::AddContext(std::string name)
{
    auto* button = new ContextButton("/ " + name, contexts.length(), this);
    button->setObjectName("NodeGraphContextButton");
    buttonsLayout->addWidget(button);
    contexts.append(button);

    QObject::connect(button, &ContextButton::ContextClicked,
                     this, &ContextsWidget::ContextClicked);
}


void Gex::Ui::ContextsWidget::RemoveContexts(unsigned int number)
{
    for (unsigned int i = 0; i < number; i++)
    {
        contexts.removeLast();
        QWidget* del = buttonsLayout->itemAt(contexts.length())->widget();
        del->deleteLater();
    }
}


void Gex::Ui::ContextsWidget::ClearContexts()
{
    RemoveContexts(contexts.size());
}


Gex::Ui::GraphWidget::GraphWidget(Gex::Graph* graph_,
                                  QWidget* parent): QWidget(parent)
{
    graph = graph_;
    profiler = MakeProfiler();

    setWindowFlag(Qt::Window, true);
    setWindowFlag(Qt::WindowMaximizeButtonHint, true);
    setWindowFlag(Qt::WindowMinimizeButtonHint, true);

    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    QSplitter* splitter = new QSplitter(this);
    splitter->setContentsMargins(0, 0, 0, 0);
    splitter->setOrientation(Qt::Horizontal);
    mainLayout->addWidget(splitter);

    QVBoxLayout* viewLayout = new QVBoxLayout();
    viewLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* viewWidget = new QWidget(this);
    viewWidget->setLayout(viewLayout);
    splitter->addWidget(viewWidget);

    contextsWidget = new ContextsWidget(viewWidget);
    viewLayout->addWidget(contextsWidget);

    scene = new NodeGraphScene(this);
    view = new NodeGraphView(scene, viewWidget);
    viewLayout->addWidget(view);

    QObject::connect(scene, &NodeGraphScene::NodePlugClicked,
                     view, &NodeGraphView::OnNodePlugClicked);

    QObject::connect(scene, &NodeGraphScene::CompoundNodeDoubleClicked,
                     this, &GraphWidget::RegisterContext);

    QObject::connect(contextsWidget, &ContextsWidget::ContextRequested,
                     this, &GraphWidget::SwitchContext);

    editor = new AttributeEditor(this);
    splitter->addWidget(editor);

    QObject::connect(scene, &NodeGraphScene::selectionChanged,
                     this, &GraphWidget::OnNodeSelected);

    auto* toolbar = view->GetToolbar();

    QPushButton* frameSelButton = toolbar->NewButton();
    frameSelButton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_arrows_to_dot));

    QObject::connect(frameSelButton, &QPushButton::clicked,
                     view, &NodeGraphView::FrameSelection);

    QPushButton* frameAllButton = toolbar->NewButton();
    frameAllButton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_arrows_to_eye));

    QObject::connect(frameAllButton, &QPushButton::clicked,
                     view, &NodeGraphView::FrameAll);


    QPushButton* layoutButton = toolbar->NewButton();
    layoutButton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_circle_nodes));

    QObject::connect(layoutButton, &QPushButton::clicked,
                     this, &GraphWidget::AutoLayoutNodes);

    QPushButton* runButton = toolbar->NewButton();
    runButton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_play));

    QObject::connect(runButton, &QPushButton::clicked,
                     this, &GraphWidget::InteractiveRun);

    QPushButton* settingsButton = toolbar->NewButton();
    settingsButton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_cogs));

    auto* optionMenu = new QMenu(settingsButton);

    auto* threadsAction = new QWidgetAction(optionMenu);

    auto* threadsWidget = new QWidget(optionMenu);
    auto* threadsLayout = new QHBoxLayout();
    threadsLayout->setContentsMargins(2, 2, 2, 2);
    threadsWidget->setLayout(threadsLayout);

    auto* text = new QLabel(threadsWidget);
    text->setText("Threads number");
    threadsLayout->addWidget(text);

    threadsSpinBox = new QSpinBox(threadsWidget);
    threadsSpinBox->setMinimum(1);
    threadsSpinBox->setMaximum(16);
    threadsLayout->addWidget(threadsSpinBox);

    threadsAction->setDefaultWidget(threadsWidget);

    optionMenu->addAction(threadsAction);

    settingsButton->setMenu(optionMenu);

    Initialize();
}


Gex::Ui::GraphWidget::~GraphWidget()
{
    ClearContexts();
}


void Gex::Ui::GraphWidget::Initialize()
{
    auto* context = new NodeGraphContext();
    context->name = "Root";
    context->GetNodes = [this](){return this->graph->Nodes();};
    context->CreateNode = [this](std::string t, std::string n){return this->graph->CreateNode(t, n);};
    context->DeleteNode = [this](Gex::Node* node){this->graph->RemoveNode(node);};
    contexts.append(context);

    contextsWidget->AddContext("Root");

    scene->SwitchGraphContext(context);
}



void Gex::Ui::GraphWidget::OnNodeSelected()
{
    QList<QGraphicsItem*> items = scene->selectedItems();

    std::vector<Gex::Node*> nodes;
    for (auto* item : items)
    {
        auto* nodeItem = dynamic_cast<NodeItem*>(item);
        if (!nodeItem)
            continue;

        nodes.push_back(nodeItem->Node());
    }

    editor->SetNodes(nodes);
}


void Gex::Ui::GraphWidget::RegisterContext(Gex::CompoundNode* compound)
{
    auto *context = new NodeGraphContext();

    context->name = compound->Name().c_str();
    context->compound = compound;
    context->GetNodes = [compound](){return compound->InternalNodes();};
    context->CreateNode = [compound](std::string t, std::string n){return compound->CreateInternalNode(t, n);},
    context->DeleteNode = [compound](Gex::Node* node){compound->RemoveInternalNode(node);};

    contextsWidget->AddContext(compound->Name());

    scene->SwitchGraphContext(context);

    contexts.append(context);
}


void Gex::Ui::GraphWidget::SwitchContext(unsigned int index)
{
    auto context = contexts[index];
    for (unsigned int idx = contexts.length() - 1; idx > index; idx--)
    {
        contexts.remove(idx);
        contextsWidget->RemoveContexts(1);
    }

    scene->SwitchGraphContext(context);
}


void Gex::Ui::GraphWidget::ClearContexts()
{
    contextsWidget->ClearContexts();
    for (auto* ctx : contexts)
        delete ctx;
    contexts.clear();
}


void Gex::Ui::GraphWidget::SwitchGraph(Gex::Graph* graph_)
{
    graph = graph_;

    ClearContexts();

    scene->Clear();

    Initialize();
}



void Gex::Ui::GraphWidget::UpdateNode(Gex::Node* node)
{
    scene->UpdateNode(node);
}


void Gex::Ui::GraphWidget::UpdateNodeAttribute(Gex::Node* node,
                                                         QString attribute)
{
    scene->UpdateNodeAttribute(node, attribute);
}


void Gex::Ui::GraphWidget::DisableInteraction()
{
    scene->DisableInteraction();
    editor->setEnabled(false);
}


void Gex::Ui::GraphWidget::EnableInteraction()
{
    scene->EnableInteraction();
    editor->setEnabled(true);
}


Gex::Profiler Gex::Ui::GraphWidget::GetProfiler() const
{
    return profiler;
}


void LocallyThreadedRunGraph(Gex::Ui::GraphWidget* widget)
{
    widget->RunGraph();
}


void EmitProfiler(Gex::Ui::GraphWidget* this_, const Gex::GraphContext& ctx)
{
    Q_EMIT this_->GraphEvaluated(this_->GetProfiler());
}


void Gex::Ui::GraphWidget::RunGraph()
{
    profiler->Reset();

    scene->ClearNodeEvaluation();

    Gex::GraphContext evaluationContext;

    graph->Compute(evaluationContext, profiler, threadsSpinBox->value(),
                   [this](Gex::Node* node)
                   {this->scene->NodeEvaluationStarted(node);},
                   [this](Gex::Node* node, bool success)
                   {if (success) this->scene->NodeEvaluationDone(node, success);},
                   [this](const Gex::GraphContext& ctx)
                   {EmitProfiler(this, ctx);}
                   );
}


void Gex::Ui::GraphWidget::InteractiveRun()
{
    DisableInteraction();

    QThread* thread = QThread::create(LocallyThreadedRunGraph, this);
    thread->setParent(this);

    QObject::connect(thread, &QThread::finished, this, &GraphWidget::EnableInteraction);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}


void Gex::Ui::GraphWidget::AutoLayoutNodes()
{
    QPoint center = view->viewport()->rect().center();
    scene->AutoLayoutNodes(view->mapToScene(center));
}


void Gex::Ui::GraphWidget::ShowMessage(Gex::Ui::UiFeedback feedback)
{
    view->ShowMessage(feedback);
}
