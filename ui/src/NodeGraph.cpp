#include "Gex/include/Config.h"
#include "ui/include/NodeGraph.h"

#include "UiTsys/include/uitsys.h"
#include "UiRes/include/uires.h"

#include "ui/include/AttributeEditor.h"
#include "ui/include/Commands.h"

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
#include <QFiledialog>
#include <QStyleOptionGraphicsItem>

#include "Gex/include/Evaluation.h"

Gex::Ui::PlugItem::PlugItem(AttributeItem* attribute, bool input):
    QGraphicsEllipseItem(attribute)
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
    return {0, 0, size, size};
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


Gex::Ui::TitleDoc::TitleDoc(QObject* parent): QTextDocument(parent)
{

}


void Gex::Ui::TitleDoc::Validate()
{
    Q_EMIT Validated(toPlainText());
}


Gex::Ui::NodeNameItem::NodeNameItem(QGraphicsItem* parent): QGraphicsTextItem(parent)
{
    doc = new TitleDoc(this);
    setDocument(doc);
}


void Gex::Ui::NodeNameItem::focusInEvent(QFocusEvent *event)
{
    fallbackText = doc->toPlainText();

    QGraphicsTextItem::focusInEvent(event);
}


void Gex::Ui::NodeNameItem::focusOutEvent(QFocusEvent *event)
{
    QGraphicsTextItem::focusOutEvent(event);

    if (doc->toPlainText() != fallbackText)
    {
        doc->Validate();
    }
}


void Gex::Ui::NodeNameItem::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter)
    {
        doc->Validate();
    }

    QGraphicsTextItem::keyReleaseEvent(event);
}


Gex::Ui::TitleDoc* Gex::Ui::NodeNameItem::TitleDocument() const
{
    return doc;
}


Gex::Ui::AttributeItem::AttributeItem(Gex::AttributeWkPtr attr,
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

    qreal x = text->pos().x() + indent;
    text->setPos(x, text->pos().y());

    qreal width = parentNode->boundingRect().width();

    QFontMetrics metrics(text->font());
    text->setPlainText(metrics.elidedText(
            text->toPlainText(), Qt::ElideRight,
            width - x - 7));

    if (multiButton)
    {
        multiButton->setPos(multiButton->pos().x() + indent,
                            multiButton->pos().y());
    }
}


Gex::Ui::AttributeItem::AttributeItem(Gex::AttributeWkPtr attr,
                                      NodeItem* node):
        QGraphicsRectItem(node)
{
    Initialize(attr, nullptr, node);
}


void Gex::Ui::AttributeItem::Initialize(Gex::AttributeWkPtr attr,
                                        AttributeItem* parent,
                                        NodeItem* node)
{
    QFont textFont("sans", 8);

    auto sharedPtr = attr.ToShared();

    defaultHeight = QFontMetrics(textFont).boundingRect(
            attr->Name().c_str()).height();
    plugRect = QRect(0, 0, defaultHeight / 1.5,
                     defaultHeight / 1.5);

    setRect(QRect(0, 0, node->boundingRect().width(),
                  defaultHeight));
    setPen(Qt::NoPen);

    collapsed = true;
    attribute = attr;
    parentNode = node;
    parentAttribute = parent;
    input = new PlugItem(this, true);
    input->setBrush(QBrush("#E58C02"));
    input->setPen(Qt::NoPen);
    input->setRect(plugRect);
    input->setVisible(attribute->IsInput());

    output = new PlugItem(this, false);
    output->setBrush(QBrush("#E58C02"));
    output->setPen(Qt::NoPen);
    output->setRect(plugRect);
    output->setVisible(attribute->IsOutput());

    text = new QGraphicsTextItem(this);
    text->setFont(QFont("sans", 8));
    text->setPlainText(attr->Name().c_str());

    QFontMetrics metrics(text->font());

    qreal nodeWidth = node->boundingRect().width();
    qreal width = metrics.boundingRect(attribute->Name().c_str()).width();

    auto elideMode = Qt::ElideRight;
    if (width > nodeWidth)
    {
        text->setPlainText(metrics.elidedText(attribute->Name().c_str(),
                                              elideMode, nodeWidth - 5));
    }

    text->setDefaultTextColor(QColor("white"));
    qreal height = metrics.boundingRect(attribute->Name().c_str()).height();
    text->update();

    qreal textY = (defaultHeight - text->boundingRect().height()) / 2;
    width = text->boundingRect().width();

    if (attribute->IsInput() || attribute->IsStatic())
    {
        text->setPos(0, textY);
        output->setVisible(attribute->IsStatic());
    }
    else
    {
        text->setPos((nodeWidth - width), textY);
        input->setVisible(false);
    }

    qreal plugY = (defaultHeight - plugRect.height())/2;
    input->setPos(-plugRect.width(), plugY);
    output->setPos(rect().width(), plugY);

    if (attribute->IsMulti() || attribute->IsHolder())
    {
        multiButton = new MultiAttributeItem(this);
        if (attribute->IsInput() || attribute->IsStatic())
        {
            text->setPos(text->pos().x() + 14,
                         text->pos().y());

            multiButton->setPos(4, (defaultHeight - multiButton->boundingRect().height())/2);
        }
        else
        {
            text->setPos(text->pos().x() - 14,
                         text->pos().y());

            multiButton->setPos(nodeWidth - 4 - multiButton->boundingRect().width(),
                                (defaultHeight - multiButton->boundingRect().height())/2);
        }

        text->setPlainText(metrics.elidedText(attribute->Name().c_str(),
                                              Qt::ElideRight, nodeWidth - 14 - 7));
    }

    CreateAttributes();
}


void Gex::Ui::AttributeItem::ShowAsInternal(bool internal)
{
    qreal plugY = (defaultHeight - plugRect.height())/2;
    if (internal)
    {
        output->setPos(-plugRect.width(), plugY);
        input->setPos(rect().width(), plugY);
        if (attribute->IsStatic())
        {
            output->setVisible(false);
        }
    }
    else
    {
        input->setPos(-plugRect.width(), plugY);
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



Gex::AttributeWkPtr Gex::Ui::AttributeItem::Attribute() const
{
    return attribute;
}


Gex::Ui::AttributeItem* Gex::Ui::AttributeItem::GetSubAttribute(Gex::AttributeWkPtr attr) const
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
        auto attr = attribute->GetIndexAttribute(index);
        AttributeItem* indexAttr = new AttributeItem(attr, this, parentNode);
        indexAttr->SetTextIndent(textIndent + 14);
        indexedAttributes.push_back(indexAttr);
    }

    childAttributes.clear();

    for (auto attr : attribute->GetChildAttributes())
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
    for (const auto& link : links)
    {
        if (link)
            delete link;
    }

    links.clear();

    for (auto* attrItem : indexedAttributes)
    {
        attrItem->InitializeLinks();
    }

    for (auto* attrItem : childAttributes)
    {
        attrItem->InitializeLinks();
    }

    QList<QGraphicsItem*> items = scene()->items();
    std::unordered_map<Gex::NodePtr, NodeItem*> nodesItems;
    for (auto* item : items)
    {
        auto* nodeitem = qgraphicsitem_cast<NodeItem*>(item);
        if (!nodeitem)
        {
            continue;
        }

        nodesItems[nodeitem->Node()] = nodeitem;
    }

    auto attr = attribute->Source();
    if (attr)
    {
        auto node = attr.lock()->Node();
        if (!node)
            return;

        auto lockedNode = node.lock();
        auto lockedAttr = attr.lock();
        if (nodesItems.find(lockedNode) != nodesItems.end())
        {
            NodeItem* nodeItem = nodesItems.at(lockedNode);
            AttributeItem* attrItem = nodeItem->FindAttribute(lockedAttr->Longname());

            auto* link = new LinkItem(attrItem, this);
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
    qreal pos = boundingRect().height();
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
    qreal height = boundingRect().height();

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


qreal Gex::Ui::NodeItem::defaultCompTitleOffset = 15;
qreal Gex::Ui::NodeItem::defaultWidth = 100;
qreal Gex::Ui::NodeItem::defaultSpacing = 10;
qreal Gex::Ui::NodeItem::defaultFooter = 10;


void Gex::Ui::NodeItem::SetDefaultWidth(qreal width)
{
    defaultWidth = width;
}


qreal Gex::Ui::NodeItem::DefaultWidth()
{
    return defaultWidth;
}


qreal Gex::Ui::NodeItem::DefaultTitleOffset()
{
    return defaultCompTitleOffset;
}

void Gex::Ui::NodeItem::SetDefaultTitleOffset(qreal offset)
{
    defaultCompTitleOffset = offset;
}

qreal Gex::Ui::NodeItem::DefaultSpacing()
{
    return defaultSpacing;
}

void Gex::Ui::NodeItem::SetDefaultSpacing(qreal spacing)
{
    defaultSpacing = spacing;
}

qreal Gex::Ui::NodeItem::DefaultFooter()
{
    return defaultFooter;
}

void Gex::Ui::NodeItem::SetDefaultFooter(qreal footer)
{
    defaultFooter = footer;
}



Gex::Ui::NodeItem::NodeItem(Gex::NodePtr node_,
                            NodeGraphScene* sc,
                            QGraphicsItem* parent):
        QObject(), QGraphicsItem(parent)
{
    node = node_;
    graphScene = sc;
    sourcePlug = new NodePlugItem(this, true);
    sourcePlug->setRect(PLUG_RECT);
    sourcePlug->setPos(-PLUG_RECT.width(), 0);
    sourcePlug->setBrush(QBrush(QColor("#408CCB")));
    sourcePlug->setPen(Qt::NoPen);
    sourcePlug->setZValue(10);
//    sourcePlug->setBrush(QBrush(QColor("#E58C02")));

    destPlug = new NodePlugItem(this, false);
    destPlug->setRect(PLUG_RECT);
    destPlug->setPos(DefaultWidth(), 0);
    destPlug->setBrush(QBrush(QColor("#408CCB")));
    destPlug->setPen(Qt::NoPen);
    destPlug->setZValue(10);
//    destPlug->setBrush(QBrush(QColor("#E58C02")));

    attrVisMode = AttributeVisibilityMode::All;

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemSendsGeometryChanges);

    title = new NodeNameItem(this);
    title->setFont(QFont("sans", 13));
    title->setDefaultTextColor(QColor("white"));
    title->setPlainText(node_->Name().c_str());
    title->setTextInteractionFlags(Qt::TextEditable);

    internalTitle = new QGraphicsTextItem(this);
    internalTitle->setDefaultTextColor(QColor("white"));
    QFont internalFont("sans", 10);
    internalFont.setItalic(true);
    internalTitle->setFont(internalFont);

    QFontMetrics metrics(internalFont);
    internalTitle->setPos(0,  -metrics.capHeight() - defaultCompTitleOffset);

    TitleDoc* doc = title->TitleDocument();

    QObject::connect(doc, &TitleDoc::Validated,
                     this, &NodeItem::TitleChanged,
                     Qt::QueuedConnection);

    QFont typeFont("sans", 8);
    typeFont.setItalic(true);

    QGraphicsTextItem* type = new QGraphicsTextItem(this);
    type->setFont(typeFont);
    type->setDefaultTextColor(QColor("white"));
    type->setPlainText(node->Type().c_str());
    type->setPos(0, title->boundingRect().bottom());

    type->setPlainText(node->Type().c_str());

    attributesY = (title->boundingRect().height() +
            type->boundingRect().height() + 5);

    CreateAttributes();

    PlaceAttributes();

    ConnectToNode();
}


void Gex::Ui::NodeItem::ConnectToNode()
{
    if (cbConnected)
    {
        return;
    }

    // Add a callback attached to the node so that its
    // graphic item receives attribute changes and make
    // it possible to update both ways.
    auto callback = [this](
            const AttributePtr& attribute,
            const AttributeChange& change)
    {

        if (change == AttributeChange::Connected ||
            change == AttributeChange::Disconnected)
            InitializeLinks();

        this->graphScene->OnNodeModified(this->node, change);
    };
    cbIndex = node->RegisterAttributeCallback(callback);

    // This callback will be launched when the node is
    // deleted before its graphical representation, then
    // the about to be deleted callback will not have to
    // be removed.
    // Setting cbConnected to false makes it possible to
    // avoid disconnection when the graphical item will
    // be deleted.
    auto delCallback = [this](){
        if (this->cbConnected)
        {
            this->node->DeregisterAttributeCallback(this->cbIndex);
            this->cbConnected = false;
        }
    };
    delIndex = node->RegisterAboutToBeDeletedCallback(delCallback);

    cbConnected = true;
}


void Gex::Ui::NodeItem::DisconnectFromNode()
{
    if (cbConnected)
    {
        node->DeregisterAttributeCallback(cbIndex);
        node->DeregisterAboutToBeDeletedCallback(delIndex);
        cbConnected = false;
    }
}


Gex::Ui::NodeItem::~NodeItem()
{
    DisconnectFromNode();
}


void Gex::Ui::NodeItem::TitleChanged(const QString& text)
{
    title->document()->blockSignals(true);

    title->setPlainText(node->SetName(text.toStdString()).c_str());

    title->document()->blockSignals(false);
}


Gex::NodePtr Gex::Ui::NodeItem::Node() const
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
    for (auto attr : node->GetAttributes())
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
    qreal posY = attributesY;

    bool skippedInput = false;
    bool skippedOutput = false;

    switch (AttributeVisibility())
    {
        case AttributeVisibilityMode::All:
            for (auto* attr : attributes)
            {
                if (!attr->Attribute()->IsExternal())
                {
                    attr->Collapse();
                    attr->setVisible(false);
                    attr->SetTextVisibility(false);
                    attr->setPos(0, 0);
                }
                else
                {
                    attr->setVisible(true);
                    attr->SetTextVisibility(true);
                    attr->setPos(0, posY);
                    posY += attr->TotalHeight() + DefaultSpacing();
                }
            }

            sourcePlug->setVisible(false);
            destPlug->setVisible(false);
            break;
        case AttributeVisibilityMode::Settable:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->HasSource() ||
                    attr->Attribute()->IsDefault() ||
                    (attr->Attribute()->IsOutput() &&
                    !attr->Attribute()->IsStatic()))
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
                    posY += attr->TotalHeight() + DefaultSpacing();
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
                    posY += attr->TotalHeight() + DefaultSpacing();
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
                    attr->Attribute()->IsStatic() ||
                    !attr->Attribute()->IsExternal())
                {
                    attr->setVisible(true);
                    attr->SetTextVisibility(true);
                    attr->setPos(0, posY);
                    posY += attr->TotalHeight() + DefaultSpacing();
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
                    attr->Attribute()->IsStatic() ||
                    !attr->Attribute()->IsExternal())
                {
                    attr->setVisible(true);
                    attr->SetTextVisibility(true);
                    attr->setPos(0, posY);
                    posY += attr->TotalHeight() + DefaultSpacing();
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
                    posY += attr->TotalHeight() + DefaultSpacing();
                }
                else
                {
                    attr->Collapse();
                    attr->setVisible(false);
                    attr->SetTextVisibility(false);
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
                    posY += attr->TotalHeight() + DefaultSpacing();
                }
                else
                {
                    attr->Collapse();
                    attr->setVisible(false);
                    attr->SetTextVisibility(false);
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


std::vector<Gex::AttributeWkPtr> AttributeHierarchy(const Gex::AttributeWkPtr& at)
{
    std::vector<Gex::AttributeWkPtr> hierarchy = {at};

    Gex::AttributeWkPtr attr = at;
    while(!attr->ParentAttribute().expired())
    {
        auto lockedAttr = attr->ParentAttribute().lock();
        hierarchy.insert(hierarchy.begin(), lockedAttr);
        attr = lockedAttr;
    }

    return hierarchy;
}


// TODO : Optimize this.
Gex::Ui::AttributeItem* Gex::Ui::NodeItem::FindAttribute(std::string longname) const
{
    Gex::AttributeWkPtr coreAttribute = node->GetAttribute(longname);
    auto hierarchy = AttributeHierarchy(coreAttribute);

    AttributeItem* current = nullptr;
    for (const Gex::AttributeWkPtr& attr : hierarchy)
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


Gex::Ui::AttributeItem* Gex::Ui::NodeItem::GetAttribute(Gex::AttributeWkPtr searched) const
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
    qreal height = attributesY;

    switch (AttributeVisibility())
    {
        case AttributeVisibilityMode::All:
            for (auto* attr : attributes)
            {
                if (!attr->Attribute()->IsExternal())
                    continue;

                height += attr->TotalHeight() + defaultSpacing;
            }
            break;
        case AttributeVisibilityMode::Settable:
            for (auto* attr : attributes)
            {
                if (!attr->Attribute()->HasSource() &&
                    !attr->Attribute()->IsDefault() &&
                    attr->Attribute()->IsInput())
                {
                    height += attr->TotalHeight() + defaultSpacing;
                }
            }
            break;
        case AttributeVisibilityMode::Connected:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->HasSource() ||
                    attr->Attribute()->HasDests())
                {
                    height += attr->TotalHeight() + defaultSpacing;
                }
            }
            break;
        case AttributeVisibilityMode::OnlyInputs:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->IsInput() ||
                    attr->Attribute()->IsStatic() ||
                    !attr->Attribute()->IsExternal())
                {
                    height += attr->TotalHeight() + defaultSpacing;
                }
            }
            break;
        case AttributeVisibilityMode::OnlyOutputs:
            for (auto* attr : attributes)
            {
                if (attr->Attribute()->IsOutput() ||
                    !attr->Attribute()->IsStatic() ||
                    !attr->Attribute()->IsExternal())
                {
                    height += attr->TotalHeight() + defaultSpacing;
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
                    height += attr->TotalHeight() + defaultSpacing;
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
                    height += attr->TotalHeight() + defaultSpacing;
                }
            }
            break;
    }


    return QSize(DefaultWidth(),
                 height + defaultFooter);
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


void Gex::Ui::NodeItem::SavePosition(QPointF pos)
{
    if (IsShowingInternal())
    {
        if (attrVisMode == AttributeVisibilityMode::OnlyInternalInputs)
        {
            node->SetMetadata("inputx", pos.x());
            node->SetMetadata("inputy", pos.y());
        }
        else
        {
            node->SetMetadata("outputx", pos.x());
            node->SetMetadata("outputy", pos.y());
        }
    }
    else
    {
        node->SetMetadata("x", pos.x());
        node->SetMetadata("y", pos.y());
    }
}


void Gex::Ui::NodeItem::RestorePosition()
{
    double x = 0;
    double y = 0;
    Feedback res;
    if (IsShowingInternal())
    {
        if (attrVisMode == AttributeVisibilityMode::OnlyInternalInputs)
        {
            x = node->GetMetadata<double>("inputx", &res);
            y = node->GetMetadata<double>("inputy", &res);
        }
        else
        {
            x = node->GetMetadata<double>("outputx", &res);
            y = node->GetMetadata<double>("outputy", &res);
        }
    }
    else
    {
        x = node->GetMetadata<double>("x", &res);
        y = node->GetMetadata<double>("y", &res);
    }

    if (!res)
        return;

    setPos(x, y);
}


void Gex::Ui::NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    savePosition = false;

    QGraphicsItem::mousePressEvent(event);
}


void Gex::Ui::NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    savePosition = true;
    if (positionChanged)
    {
        SavePosition(pos());
        positionChanged = false;
    }

    QGraphicsItem::mouseReleaseEvent(event);
}


QVariant Gex::Ui::NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged)
    {
        for (auto* attribute : attributes)
        {
            attribute->UpdateLinks();
        }

        if (savePosition)
        {
            SavePosition(value.value<QPointF>());
        }

        positionChanged = true;
    }

    return QGraphicsItem::itemChange(change, value);
}


//void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
//{
//
//}


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

    internalTitle->setPlainText("(Inputs)");

    SetAttributeVisibility(AttributeVisibilityMode::OnlyInternalInputs);
}


void Gex::Ui::NodeItem::ShowAsInternalOutput()
{
    ShowAsInternal(true);

    internalTitle->setPlainText("(Outputs)");

    SetAttributeVisibility(AttributeVisibilityMode::OnlyInternalOutputs);
}


Gex::Ui::LinkItem::LinkItem(AttributeItem *source, AttributeItem *dest): QGraphicsPathItem(), QObject()
{
    setFlag(QGraphicsItem::ItemIsSelectable);

    src = source;
    dst = dest;
    if (src && src->Attribute())
    {
        if (Gex::NodeWkPtr srcNode = src->Attribute()->Node())
        {
            if (auto cmp = Gex::CompoundNode::FromNode(srcNode))
            {
                if (cmp->HasNode(dst->Attribute()->Node()))
                {
                    sourceInternal = true;
                }
            }
        }
    }

    if (auto cmp = Gex::CompoundNode::FromNode(dst->Attribute()->Node()))
    {
        if (cmp->HasNode(src->Attribute()->Node()))
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


Gex::AttributeWkPtr Gex::Ui::PreviewLinkItem::Attribute() const
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


Gex::Ui::NodeGraphContext::NodeGraphContext(const QString& name_,
                                            Gex::CompoundNodePtr node_)
{
    name = name_;
    node = node_;
}


QString Gex::Ui::NodeGraphContext::Name() const
{
    return name;
}


Gex::CompoundNodePtr Gex::Ui::NodeGraphContext::Compound() const
{
    return node;
}

Gex::NodeList Gex::Ui::NodeGraphContext::GetNodes() const
{
    return node->GetNodes();
}


Gex::NodePtr Gex::Ui::NodeGraphContext::CreateNode(std::string type, std::string name)
{
    return node->CreateNode(type, name);
}


Gex::NodePtr Gex::Ui::NodeGraphContext::ReferenceNode(std::string path, std::string name)
{
    return node->ReferenceNode(path, name);
}


bool Gex::Ui::NodeGraphContext::DeleteNode(Gex::NodePtr node_)
{
    return node->RemoveNode(node_);
}


Gex::NodeList Gex::Ui::NodeGraphContext::DuplicateNodes(Gex::NodeList nodes, bool copyLinks)
{
    return node->DuplicateNodes(nodes, copyLinks);
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
        PlugItem *plugItem = qgraphicsitem_cast<PlugItem *>(item);
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
            PlugItem* plugItem = qgraphicsitem_cast<PlugItem*>(item);
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
}


void Connect(Gex::Ui::AttributeItem* source,
             Gex::Ui::AttributeItem* dest)
{
    for (auto* l : dest->SourceLinks())
    {
        delete l;
    }

    dest->Attribute()->ConnectSource(source->Attribute());
}


void Gex::Ui::NodeGraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    QGraphicsItem *item = itemAt(mouseEvent->scenePos(), QTransform());
    if (previewLink) {
        if (item)
        {
            PlugItem *plugItem = qgraphicsitem_cast<PlugItem *>(item);

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
                        Connect(destItem, sourceItem);
                    }
                        // If source plug is an output, check if the destination attribute can
                        // be connected as a source to the source plug.
                    else if (previewLink->SourcePlug()->IsOutputAnchor() &&
                             destItem->Attribute()->CanConnectSource(sourceItem->Attribute()))
                    {
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
                        Connect(sourceItem, destItem);
                    }
                        // If source plug is an internal output of current compound, check if source
                        // plug can receive dest as a connection.
                    else if (previewLink->SourcePlug()->IsOutputAnchor() &&
                             sourceItem->Attribute()->IsInternal() &&
                             sourceItem->ParentNode() == output &&
                             sourceItem->Attribute()->CanConnectSource(destItem->Attribute()))
                    {
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
                        Gex::AttributePtr indexAttr = destItem->Attribute()->GetIndexAttribute(index);
                        if (!indexAttr)
                        {
                            continue;
                        }

                        if (!indexAttr->HasSource())
                        {
                            indexAttr->ConnectSource(sourceItem->Attribute());
                            connected = true;
                            break;
                        }
                    }
                    if (!connected)
                    {
                        unsigned int newIndex = Gex::Ui::NextMultiAttributeIndex(destItem->Attribute());
                        if (destItem->Attribute()->CreateIndex(newIndex))
                        {
                            auto indexAttr = destItem->Attribute()->GetIndexAttribute(newIndex);
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
        if (auto* nodePlugItem = qgraphicsitem_cast<NodePlugItem*>(item))
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
    NodeItem* nodeItem = qgraphicsitem_cast<NodeItem*>(item);
    if (!nodeItem)
    {
        auto* parent = item->parentItem();
        if (parent)
        {
            nodeItem = qgraphicsitem_cast<NodeItem*>(parent);
            if (!nodeItem)
            {
                return;
            }
        }
    }

    if (!nodeItem)
        return;

    auto node = Gex::CompoundNode::FromNode(nodeItem->Node());
    if (!node || nodeItem->IsShowingInternal())
    {
        return;
    }

    Q_EMIT CompoundNodeDoubleClicked(node);
}


void Gex::Ui::NodeGraphScene::UpdateNode(Gex::NodePtr node)
{
    NodeItem* nodeItem = nodeItems.value(node, nullptr);
    if (nodeItem)
    {
        nodeItem->RebuildAttributes();
    }
    else if (node == graphContext->Compound())
    {
        input->RebuildAttributes();
        input->ShowAsInternalInput();

        output->RebuildAttributes();
        output->ShowAsInternalOutput();
    }
}


void Gex::Ui::NodeGraphScene::OnNodeModified(Gex::NodePtr node, const Gex::AttributeChange& change)
{
    Q_EMIT NodeModified(node, change);
}


void Gex::Ui::NodeGraphScene::UpdateNodeAttribute(
        Gex::NodePtr node, QString attribute)
{
    NodeItem* nodeItem = nodeItems.value(node, nullptr);
    if (!nodeItem)
        return;

    AttributeItem* attributeItem = nodeItem->FindAttribute(attribute.toStdString());
    if (!attributeItem)
        return;

    attributeItem->RebuildAttributes();
}


//Gex::Ui::NodeGraphView::NodeGraphView(const NodeGraphView& other):
//        NodeGraphView(other.graphScene, nullptr)
//{
//
//}


Gex::Ui::NodeGraphView::NodeGraphView(NodeGraphScene* scene, QWidget* parent):
    BaseGraphView(scene, parent, true)
{
    graphScene = scene;

    QShortcut* shortcut_1 = new QShortcut(QKeySequence("1"), this);
    QObject::connect(shortcut_1, &QShortcut::activated, this,
                     &NodeGraphView::SetSelectedNodesVisAll);

    QShortcut* shortcut_2 = new QShortcut(QKeySequence("2"), this);
    QObject::connect(shortcut_2, &QShortcut::activated, this,
                     &NodeGraphView::SetSelectedNodesVisSettable);

    QShortcut* shortcut_3 = new QShortcut(QKeySequence("3"), this);
    QObject::connect(shortcut_3, &QShortcut::activated, this,
                     &NodeGraphView::SetSelectedNodesVisConnected);

    QShortcut* shortcut_4 = new QShortcut(QKeySequence("Ctrl+D"), this);
    QObject::connect(shortcut_4, &QShortcut::activated, scene,
                     &NodeGraphScene::DuplicateSelectedNodesNoLinks);

    QShortcut* shortcut_5 = new QShortcut(QKeySequence("Shift+D"), this);
    QObject::connect(shortcut_5, &QShortcut::activated, scene,
                     &NodeGraphScene::DuplicateSelectedNodesAndLinks);

    QShortcut* shortcut_6 = new QShortcut(QKeySequence("Ctrl+Shift+X"), this);
    QObject::connect(shortcut_6, &QShortcut::activated, scene,
                     &NodeGraphScene::ConvertSelectedNodesToCompound);

    QShortcut* shortcut_7 = new QShortcut(QKeySequence("Ctrl+E"), this);
    QObject::connect(shortcut_7, &QShortcut::activated, this,
                     &NodeGraphView::ExportSelectedNodes);

    QShortcut* shortcut_8 = new QShortcut(QKeySequence("Ctrl+Shift+E"), this);
    QObject::connect(shortcut_8, &QShortcut::activated, this,
                     &NodeGraphView::ExportSelectedNodesAsCompound);
}


void Gex::Ui::NodeGraphView::SetSelectedNodesVis(NodeItem::AttributeVisibilityMode mode)
{
    for (QGraphicsItem* item : scene()->selectedItems())
    {
        NodeItem* node = qgraphicsitem_cast<NodeItem*>(item);
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
        if (qgraphicsitem_cast<Gex::Ui::NodeItem*>(item))
        {
            nodes.append(item);
        }
    }

    return nodes;
}


void Gex::Ui::NodeGraphView::ExportSelectedNodes()
{
    QString file = QFileDialog::getSaveFileName(nullptr, "Save",
                                                QString(), "*.json",
                                                nullptr);
    if (file.isEmpty())
        return;

    std::filesystem::path p(file.toStdString());

    auto success = Gex::ExportAsCompound(graphScene->SelectedNodes(),
                                         graphScene->CurrentContext()->Compound(),
                                         p.parent_path().string(),
                                         p.filename().string());

    ShowMessage(Gex::Ui::MakeFeedback(success));
}


void Gex::Ui::NodeGraphView::ExportSelectedNodesAsCompound()
{
    QString file = QFileDialog::getSaveFileName(nullptr, "Save",
                                                QString(), "*.json",
                                                nullptr);
    if (file.isEmpty())
        return;

    std::filesystem::path exf(file.toStdString());

    auto directory = exf.parent_path().string();
    auto file_ = exf.filename().string();

    auto success = Gex::ExportAsCompound(graphScene->SelectedNodes(),
                                         graphScene->CurrentContext()->Compound(),
                                         directory, file_);

    ShowMessage(Gex::Ui::MakeFeedback(success));
}



struct NodeCreator: public QObject
{
    Gex::Ui::NodeGraphScene* scene;
    QString type;

    NodeCreator(QObject* parent=nullptr): QObject(parent) {}

    void CreateNode(bool)
    {
        scene->CreateNode(type, "");
    }
};


struct NodeLoader: public QObject
{
    Gex::Ui::NodeGraphScene* scene;
    QString type;

    NodeLoader(QObject* parent=nullptr): QObject(parent) {}

    void ReferenceNode(bool)
    {
        scene->ReferenceNode(type, "");
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

        NodeCreator* creator = new NodeCreator(action);
        creator->scene = dynamic_cast<NodeGraphScene*>(scene());
        creator->type = nodeType;

        QObject::connect(action, &QAction::triggered,
                         creator, &NodeCreator::CreateNode);
    }

    auto references = Gex::References::GetLoader()->GetAvailableReferences();
    if (!references.empty())
    {
        QMenu* customMenu = createMenu->addMenu("Compounds");

        for (std::string type : references)
        {
            QString nodeType = type.c_str();
            QStringList tokens = nodeType.split(std::filesystem::path::preferred_separator);

            QMenu* parent = customMenu;
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

            QAction *action = new QAction(parent);
            action->setText(tokens[tokens.length() - 1]);
            parent->addAction(action);

            NodeLoader *creator = new NodeLoader(action);
            creator->scene = dynamic_cast<NodeGraphScene *>(scene());
            creator->type = nodeType;

            QObject::connect(action, &QAction::triggered,
                             creator, &NodeLoader::ReferenceNode);
        }
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
    Clear();  // Clear all content.

    graphContext = context;
    for (Gex::NodePtr node : graphContext->GetNodes())
    {
        auto* item = new NodeItem(node, this);
        addItem(item);

        nodeItems[node] = item;

        item->RestorePosition();
    }

    if (context->Compound())
    {
        auto cmpd = context->Compound();

        input = new NodeItem(cmpd, this);
        addItem(input);
        input->ShowAsInternalInput();
        input->RestorePosition();

        output = new NodeItem(cmpd, this);
        addItem(output);
        output->ShowAsInternalOutput();
        output->RestorePosition();
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
    Gex::NodePtr node = item->Node();

    if (!graphContext->DeleteNode(node))
    {
        return;
    }

    nodeItems.remove(item->Node());

    removeItem(item);
    delete item;
}


QList<Gex::Ui::NodeItem*> Gex::Ui::NodeGraphScene::SelectedNodeItems() const
{
    auto items  = selectedItems();

    QList<NodeItem*> nodes;
    for (auto* item : items)
    {
        if (auto* node = qgraphicsitem_cast<NodeItem*>(item))
        {
            nodes.append(node);
        }
    }

    return nodes;
}


std::vector<Gex::NodePtr>  Gex::Ui::NodeGraphScene::SelectedNodes() const
{
    auto items  = selectedItems();

    std::vector<Gex::NodePtr> core;
    for (auto* item : items)
    {
        if (auto* node = qgraphicsitem_cast<NodeItem*>(item))
        {
            core.push_back(node->Node());
        }
    }

    return core;
}


Gex::Ui::NodeGraphContext* Gex::Ui::NodeGraphScene::CurrentContext() const
{
    return graphContext;
}


Gex::Ui::NodeItem* Gex::Ui::NodeGraphScene::ItemFromNode(
        const Gex::NodeWkPtr& node) const
{
    if (!node)
        return nullptr;

    return ItemFromNode(node.ToShared());
}


Gex::Ui::NodeItem* Gex::Ui::NodeGraphScene::ItemFromNode(
        const Gex::NodePtr& node) const
{
    if (!node)
        return nullptr;

    return nodeItems.value(node, nullptr);
}


void Gex::Ui::NodeGraphScene::DuplicateNodes(std::vector<NodePtr> nodes, bool copyLinks)
{
    std::vector<Gex::NodePtr> duplicateds = graphContext->DuplicateNodes(nodes, copyLinks);

    QList<NodeItem*> newItems;
    for (auto dupnode : duplicateds)
    {
        auto* nodeItem = new NodeItem(dupnode, this);
        addItem(nodeItem);
        nodeItems[dupnode] = nodeItem;

        newItems.append(nodeItem);
    }

    for (auto* item : newItems)
    {
        item->InitializeLinks();
    }

    // Place new nodes.
    for (unsigned int i = 0; i < nodes.size(); i++)
    {
        auto* src = nodeItems[nodes.at(i)];
        auto* dst = nodeItems[duplicateds.at(i)];

        dst->setPos(src->pos() + QPointF(10, 10));
    }
}


void Gex::Ui::NodeGraphScene::DuplicateSelectedNodes(bool copyLinks)
{
    auto duplicate = SelectedNodes();
    if (duplicate.empty())
        return;

    DuplicateNodes(duplicate, copyLinks);
}

void Gex::Ui::NodeGraphScene::DuplicateSelectedNodesNoLinks()
{
    DuplicateSelectedNodes(false);
}


void Gex::Ui::NodeGraphScene::DuplicateSelectedNodesAndLinks()
{
    DuplicateSelectedNodes(true);
}


void Gex::Ui::NodeGraphScene::ConvertSelectedNodesToCompound()
{
    auto extract = SelectedNodeItems();

    std::vector<Gex::NodePtr> nodes;
    for (auto n : extract)
    {
        nodes.push_back(n->Node());
    }

    auto cmp = graphContext->Compound()->ToCompound(nodes);

    for (auto* item : extract)
    {
        DeleteNode(item);
    }

    auto* cmpItem = new NodeItem(cmp, this);
    addItem(cmpItem);
    cmpItem->InitializeLinks();
}


void Gex::Ui::NodeGraphScene::DeleteLink(LinkItem* item)
{
    AttributeItem* source = item->Source();
    AttributeItem* dest = item->Dest();

    if (!dest->Attribute()->DisconnectSource(source->Attribute()))
    {
        return;
    }

    removeItem(item);
    delete item;
}


void Gex::Ui::NodeGraphScene::DeleteSelection()
{
    QList<QPointer<NodeItem>> deleteNodes;
    QList<QPointer<LinkItem>> deleteLinks;
    for (QGraphicsItem* item : selectedItems())
    {
        if (auto* nodeItem = qgraphicsitem_cast<NodeItem*>(item))
        {
            deleteNodes.append(nodeItem);
        }
        else if (auto* linkItem = qgraphicsitem_cast<LinkItem*>(item))
        {
            deleteLinks.append(linkItem);
        }
    }

    for (const auto& linkItem : deleteLinks)
    {
        if (linkItem.isNull())
            continue;

        DeleteLink(linkItem);
    }

    for (const auto& nodeItem : deleteNodes)
    {
        if (nodeItem.isNull())
            continue;

        DeleteNode(nodeItem);
    }
}


void Gex::Ui::NodeGraphScene::CreateNode(QString type, QString name)
{
    Gex::NodePtr node = graphContext->CreateNode(
            type.toStdString(), name.toStdString());

    auto* item = new NodeItem(node, this);
    addItem(item);
    nodeItems[node] = item;

    item->InitializeLinks();
}


void Gex::Ui::NodeGraphScene::ReferenceNode(QString type, QString name)
{
    Gex::NodePtr node = graphContext->ReferenceNode(
            type.toStdString(), name.toStdString());

    if (!node)
        return;

    auto* item = new NodeItem(node, this);
    addItem(item);
    nodeItems[node] = item;

    item->InitializeLinks();
}


void Gex::Ui::NodeGraphScene::NodeEvaluationStarted(Gex::NodePtr node)
{
    auto* nodeItem = nodeItems.value(node, nullptr);
    if (!nodeItem)
        return;

    nodeItem->SetCustomBorderColor(QColor("#5B6FD3"));

}

void Gex::Ui::NodeGraphScene::NodeEvaluationDone(Gex::NodePtr node, bool success)
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

    std::vector<Gex::NodePtr> nodeVec;
    for (const auto& node : nodes)
        nodeVec.push_back(node);

    auto schelNodes = Gex::ScheduleNodes(nodeVec, false);

    std::map<int, std::vector<Gex::NodePtr>> nodeLevels;
    for (const auto& schel : schelNodes)
    {
        int i = 0;
        for (const auto& node : schel->previousNodes)
        {
            for (auto nl : nodeLevels)
            {
                if (std::find(nl.second.begin(), nl.second.end(), node->node.lock()) != nl.second.end())
                {
                    if (nl.first > i)
                        i = nl.first;
                }
            }
        }

        if (nodeLevels.find(i + 1) == nodeLevels.end())
        {
            nodeLevels[i + 1] = {schel->node.lock()};
        }
        else
        {
            nodeLevels[i + 1].push_back(schel->node.lock());
        }
    }

    schelNodes.clear();

    size_t len = nodeLevels.size();

    qreal rectHeight = 0;
    qreal rectWidth = 0;

    std::vector<LayoutColumn> columns;
    for (const auto& levelNodes : nodeLevels)
    {
        size_t columnWidth = 0;
        size_t columnHeight = 0;

        QList<Gex::Ui::NodeItem*> items;
        for (const auto& node : levelNodes.second)
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


QEvent::Type Gex::Ui::GraphWidget::scheduleEventType = QEvent::Type(QEvent::registerEventType());


Gex::Ui::GraphWidget::GraphWidget(Gex::CompoundNodePtr graph_,
                                  QWidget* parent): QWidget(parent)
{
    graph = graph_;

    profiler = MakeProfiler();

    setWindowFlag(Qt::Window, true);
    setWindowFlag(Qt::WindowMaximizeButtonHint, true);
    setWindowFlag(Qt::WindowMinimizeButtonHint, true);

    auto* mainLayout = new QGridLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    auto* splitter = new QSplitter(this);
    splitter->setContentsMargins(0, 0, 0, 0);
    splitter->setOrientation(Qt::Horizontal);
    mainLayout->addWidget(splitter);

    auto* viewLayout = new QVBoxLayout();
    viewLayout->setContentsMargins(0, 0, 0, 0);

    auto* viewWidget = new QWidget(this);
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

    QObject::connect(scene, &NodeGraphScene::NodeModified,
                     this, &GraphWidget::OnNodeChanged);

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

    QObject::connect(this, &GraphWidget::GraphEvaluated,
                     editor, &AttributeEditor::Update);

    Initialize();
}


Gex::Ui::GraphWidget::~GraphWidget()
{
    ClearContexts();
}


void Gex::Ui::GraphWidget::Initialize()
{
    auto* context = new NodeGraphContext("Root", graph);
    contexts.append(context);

    contextsWidget->AddContext("Root");

    scene->SwitchGraphContext(context);
}



void Gex::Ui::GraphWidget::OnNodeSelected()
{
    QList<QGraphicsItem*> items = scene->selectedItems();

    std::vector<Gex::NodeWkPtr> nodes;
    for (auto* item : items)
    {
        auto* nodeItem = qgraphicsitem_cast<NodeItem*>(item);
        if (!nodeItem)
            continue;

        nodes.emplace_back(nodeItem->Node());
    }

    editor->SetNodes(nodes);

    Q_EMIT SelectedNodeChanged(nodes);
}


void Gex::Ui::GraphWidget::RegisterContext(Gex::CompoundNodePtr compound)
{
    auto *context = new NodeGraphContext(compound->Name().c_str(), compound);

    contextsWidget->AddContext(compound->Name());

    scene->SwitchGraphContext(context);

    contexts.append(context);
}


void Gex::Ui::GraphWidget::SwitchContext(unsigned int index)
{
    auto context = contexts[index];
    for (unsigned int idx = contexts.length() - 1; idx > index; idx--)
    {
        auto* ctx = contexts[idx];
        contexts.remove(idx);
        contextsWidget->RemoveContexts(1);

        delete ctx;
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


void Gex::Ui::GraphWidget::InitIdlePrepare()
{
    QCoreApplication::postEvent(this, new QEvent(scheduleEventType),
                                Qt::EventPriority::LowEventPriority - 1000);
}


// TODO : Mettre ca dans un thread en priority low plutot ?
bool Gex::Ui::GraphWidget::event(QEvent *event)
{
    return QWidget::event(event);
}


void Gex::Ui::GraphWidget::SwitchGraph(Gex::CompoundNodePtr graph_)
{
    graph = graph_;

    ClearContexts();

    scene->Clear();

    Initialize();
}



void Gex::Ui::GraphWidget::UpdateNode(Gex::NodePtr node)
{
    scene->UpdateNode(node);
}


void Gex::Ui::GraphWidget::UpdateNodeAttribute(Gex::NodePtr node,
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
    bool prevState = interactiveEvalEnabled;
    interactiveEvalEnabled = false;

    profiler->Reset();

    scene->ClearNodeEvaluation();

    graph->Run(profiler, threadsSpinBox->value(),
               [this](Gex::NodePtr node)
               {this->scene->NodeEvaluationStarted(node);},
               [this](Gex::NodePtr node, bool success)
               {this->scene->NodeEvaluationDone(node, success);},
               [this, prevState](const Gex::GraphContext& ctx)
               {EmitProfiler(this, ctx);this->interactiveEvalEnabled = prevState;}
               );
}


void Gex::Ui::GraphWidget::OnNodeChanged(
        const Gex::NodePtr& node,
        const Gex::AttributeChange& change)
{
    if (change == AttributeChange::Connected ||
        change == AttributeChange::Disconnected ||
        change == AttributeChange::ValueChanged)
        RunFromNode(node);
}


void Gex::Ui::GraphWidget::RunFromNode(const Gex::NodePtr& node)
{
    if (!interactiveEvalEnabled)
        return;

    interactiveEvalEnabled = false;

    if (interactiveEval && interactiveEval->Status() == Gex::NodeEvaluator::EvaluationStatus::Running)
    {
        interactiveEval->Stop();
        delete interactiveEval;
        interactiveEval = nullptr;
    }

    profiler->Reset();

    scene->ClearNodeEvaluation();

    auto subGraph = SubScheduledNodes(
            graph->ToScheduledNodes(),
            node);

    if (subGraph.empty())
        return;

    GraphContext context;
    interactiveEval = new Gex::NodeEvaluator(
            subGraph, context, profiler,
            false, threadsSpinBox->value(),
            [this](Gex::NodePtr node)
            {this->scene->NodeEvaluationStarted(node);},
            [this](Gex::NodePtr node, bool success)
            {if (success) this->scene->NodeEvaluationDone(node, success);},
            [this](const Gex::GraphContext& ctx)
            {EmitProfiler(this, ctx);this->interactiveEvalEnabled = true;}
            );

    interactiveEval->Run();
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


void Gex::Ui::GraphWidget::ShowMessage(const Gex::Ui::UiFeedback& feedback)
{
    view->ShowMessage(feedback);
}


Gex::NodeWkList Gex::Ui::GraphWidget::CurrentSelection() const
{
    auto selection = scene->SelectedNodes();

    Gex::NodeWkList sel;
    for (const auto& node : selection)
    {
        sel.emplace_back(node);
    }

    return sel;
}
