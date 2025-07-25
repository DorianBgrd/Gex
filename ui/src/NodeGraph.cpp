#include "Gex/include/Config.h"
#include "ui/include/NodeGraph.h"

#include "UiTsys/include/uitsys.h"
#include "UiRes/include/uires.h"

#include "ui/include/AttributeEditor.h"
#include "ui/include/Commands.h"

#include <array>

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

#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

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
        QGraphicsRectItem(parent), QObject(node)
{
    Initialize(attr, parent, node);
}


Gex::Ui::AttributeItem::~AttributeItem()
{
    for (qsizetype i = links.length() - 1; i >= 0; i--)
    {
        QPointer<LinkItem> linkItem = links[i];
        if (linkItem.isNull())
            continue;

        links[i]->scene()->removeItem(links[i].get());
        delete links[i];
    }

    parentNode->DeregisterAttribute(this);
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
    setFlag(ItemSendsScenePositionChanges, true);

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

    qreal indent = 14;
    if (attribute->IsMulti() || attribute->IsHolder())
    {
        multiButton = new MultiAttributeItem(this);
        if (attribute->IsInput() || attribute->IsStatic())
        {
            text->setPos(text->pos().x() + indent,
                         text->pos().y());

            multiButton->setPos(4, (defaultHeight - multiButton->boundingRect().height())/2);
        }
        else
        {
            text->setPos(text->pos().x() - indent,
                         text->pos().y());

            multiButton->setPos(nodeWidth - 4 - multiButton->boundingRect().width(),
                                (defaultHeight - multiButton->boundingRect().height())/2);
        }

        text->setPlainText(metrics.elidedText(attribute->Name().c_str(),
                                              Qt::ElideRight, nodeWidth - (indent / 2)));
    }

    parentNode->RegisterAttribute(this);

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


Gex::Ui::NodeGraphScene* Gex::Ui::AttributeItem::GraphScene() const
{
    if  (!parentNode)
        return nullptr;

    return parentNode->GraphScene();
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
        auto* indexAttr = new AttributeItem(attr, this, parentNode);
        indexAttr->SetTextIndent(textIndent + 14);
        indexedAttributes.push_back(indexAttr);
    }

    childAttributes.clear();

    if (!(attribute->IsHolder() && attribute->IsMulti()))
    {
        for (const auto& attr : attribute->GetChildAttributes())
        {
            auto* childAttr = new AttributeItem(attr, this, parentNode);

            qreal indent = 14;
            if (!attribute->IsMulti())
            {
                indent += 14;
            }

            childAttr->SetTextIndent(textIndent + indent);
            childAttributes.push_back(childAttr);
        }
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


void Gex::Ui::AttributeItem::ClearLinks(bool recursive)
{
    for (const QPointer<LinkItem>& link : links)
    {
        delete link;
    }

    links.clear();

    if (recursive)
    {
        for (AttributeItem* attrItem : indexedAttributes)
        {
            attrItem->ClearLinks(recursive);
        }

        for (AttributeItem* attrItem : childAttributes)
        {
            attrItem->ClearLinks(recursive);
        }
    }
}


void Gex::Ui::AttributeItem::InitializeLinks()
{
    ClearLinks(false);

    for (auto* attrItem : indexedAttributes)
    {
        attrItem->InitializeLinks();
    }

    for (auto* attrItem : childAttributes)
    {
        attrItem->InitializeLinks();
    }

    NodeGraphScene* graphScene = GraphScene();
    if (!graphScene)
    {
        return;
    }

    if (auto attr = attribute->Source())
    {
        if (!attr)
        {
            return;
        }

        if (NodeItem* srcNodeItem = graphScene->ItemFromNode(attr->Node()))
        {
            AttributeItem* attrItem = srcNodeItem->FindAttribute(attr->Longname());

            auto* link = new LinkItem(attrItem, this);
            if (!links.contains(link))
            {
                attrItem->links.append(link);
                links.append(link);

                scene()->addItem(link);
                link->Update();
            }
            else
            {
                delete link;
            }
        }
    }

    for (const Gex::AttributeWkPtr& dest : attribute->Dests())
    {
        if (!dest)
        {
            continue;
        }

        if (Gex::Ui::NodeItem* destNodeItem = graphScene->ItemFromNode(dest->Node()))
        {
            AttributeItem* attrItem = destNodeItem->FindAttribute(dest->Longname());

            auto* link = new LinkItem(this, attrItem);
            if (links.contains(link))
            {
                delete link;
                continue;
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
//        item->setPos(0, 0);
        item->Collapse();
    }

    for (AttributeItem* item : childAttributes)
    {
        item->setVisible(false);
//        item->setPos(0, 0);
        item->Collapse();
    }

    collapsed = true;

    AdjustPlacement();

    if (multiButton)
    {
        multiButton->update();
    }

    UpdateLinks();
}


void Gex::Ui::AttributeItem::Expand()
{
    qreal pos = boundingRect().height();
    for (AttributeItem* item : indexedAttributes)
    {
        item->setVisible(true);
//        item->setPos(0, pos);
//        pos += item->TotalHeight();
    }

    for (AttributeItem* item : childAttributes)
    {
        item->setVisible(true);
//        item->setPos(0, pos);
//        pos += item->TotalHeight();
    }

    collapsed = false;

//    UpdateLinks();

    AdjustPlacement();

    if (multiButton)
    {
        multiButton->update();
    }
}


void Gex::Ui::AttributeItem::AdjustPlacement()
{
    qreal pos = boundingRect().height();
    for (AttributeItem* item : indexedAttributes)
    {
        if (!Collapsed())
            item->setPos(0, pos);
        else
            item->setPos(0, 0);
        pos += item->TotalHeight();
    }

    for (AttributeItem* item : childAttributes)
    {
        if (!Collapsed())
            item->setPos(0, pos);
        else
            item->setPos(0, 0);
        pos += item->TotalHeight();
    }

    if (parentAttribute && parentAttribute->IsMulti())
    {
        parentAttribute->AdjustPlacement();
    }

    UpdateLinks();
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
    if (change == QGraphicsItem::ItemScenePositionHasChanged)
    {
        for (const auto& link : links)
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

    for (const auto& link : links)
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
        for (const QString& name : attr->SubNames())
        {
            names.append(name);
        }
    }

    for (auto* attr : childAttributes)
    {
        names.append(attr->Attribute()->Longname().c_str());
        for (const auto& name : attr->SubNames())
        {
            names.append(name);
        }
    }

    return names;


}


QList<Gex::Ui::LinkItem*> Gex::Ui::AttributeItem::SourceLinks() const
{
    QList<Gex::Ui::LinkItem*> items;

    for (const auto& link : links)
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


void Gex::Ui::NodeItem::RegisterAttribute(AttributeItem* item)
{
    allAttributes.append(item);
}


void Gex::Ui::NodeItem::DeregisterAttribute(AttributeItem* item)
{
    allAttributes.removeAll(item);
}


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
        QGraphicsObject(parent)
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

    auto* type = new QGraphicsTextItem(this);
    type->setFont(typeFont);
    type->setDefaultTextColor(QColor("white"));
    type->setPlainText(node->Type().c_str());
    type->setPos(0, title->boundingRect().bottom());

    QFontMetrics typeMetrics(typeFont);
    type->setPlainText(typeMetrics.elidedText(
            node->Type().c_str(), Qt::ElideLeft,
            DefaultWidth() - 4
        )
    );

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
            if (AttributeItem* attr = GetAttribute(attribute))
                attr->InitializeLinks();

        Gex::AttributeWkPtr attrWeak = attribute;
        this->graphScene->OnNodeModified(this->node, attrWeak, change);
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
    for (AttributeItem* attrItem : attributes)
    {
        attrItem->ClearLinks(true);
    }

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


Gex::Ui::NodeGraphScene* Gex::Ui::NodeItem::GraphScene() const
{
    return graphScene;
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
    for (const Gex::AttributeWkPtr& attr : node->GetAttributes())
    {
        auto* item = new AttributeItem(attr, this);
        attributes.append(item);
    }
}


void Gex::Ui::NodeItem::RebuildAttributes()
{
    for (auto* attr : attributes)
    {
        delete attr;
    }

    attributes.clear();
    allAttributes.clear();

    CreateAttributes();

    PlaceAttributes();

    InitializeLinks();
}


void Gex::Ui::NodeItem::PlaceAttributes()
{
    prepareGeometryChange();

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
    for (auto* at : allAttributes)
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


Gex::Ui::LinkItem::~LinkItem()
{
    if (!src.isNull())
    {
        src->RemoveLink(this);
    }

    if (!dst.isNull())
    {
        dst->RemoveLink(this);
    }
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


Gex::Ui::FrameItem::FrameItem(
        const QPoint& topLeft,
        qreal width, qreal height,
        NodeGraphScene* graphSc,
        QGraphicsItem* parent
): QGraphicsObject(parent)
{
    textItem = new QGraphicsTextItem(this);

    resizeHandle = new QGraphicsRectItem(this);
    resizeHandle->setBrush(Qt::transparent);

    QPen pen(Qt::white);
    pen.setStyle(Qt::DashLine);
    resizeHandle->setPen(pen);
    resizeHandle->setVisible(false);

    textItem->setDefaultTextColor(Qt::white);
    textItem->setTextInteractionFlags(
            Qt::TextSelectableByMouse|
            Qt::TextSelectableByKeyboard|
            Qt::TextEditable);
    textItem->setPlainText("Node group");
    graphScene = graphSc;

    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemIsMovable|
             QGraphicsItem::ItemSendsGeometryChanges
    );

    SetRect(QRect(topLeft.x(), topLeft.y(),
                 width, height));

    setZValue(-20);
    setAcceptHoverEvents(true);
}


QRect Gex::Ui::FrameItem::Rect() const
{
    return rect;
}


void Gex::Ui::FrameItem::SetRect(const QRect& r)
{
    prepareGeometryChange();

    rect = r;

    update();
}


bool Gex::Ui::FrameItem::Active() const
{
    return moveItems;
}


void Gex::Ui::FrameItem::SetActive(bool active)
{
    moveItems = active;
}


void Gex::Ui::FrameItem::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget
)
{
    QPen pen = Qt::NoPen;
    if (isSelected())
    {
        pen = QPen(QColor("#E58C02"));
    }
    painter->setPen(pen);

    painter->setBrush(background);
    painter->drawRoundedRect(rect, 5, 5);
}


QRectF Gex::Ui::FrameItem::boundingRect() const
{
    return rect;
}


QColor Gex::Ui::FrameItem::BackgroundColor() const
{
    return background;
}


void Gex::Ui::FrameItem::SetBackgroundColor(const QColor& color)
{
    background = color;
}


QList<QPointer<QGraphicsObject>> Gex::Ui::FrameItem::ContainedItems() const
{
    QList<QPointer<QGraphicsObject>> contained;

    auto scRect = sceneBoundingRect();

    for (auto* nodeItem : graphScene->NodeItems())
    {
        if (scRect.intersects(nodeItem->sceneBoundingRect()))
        {
            contained.emplace_back(nodeItem);
        }
    }

    auto sortedFrames = graphScene->SortedFrameItems(scRect);
    int index = sortedFrames.indexOf(this);

    for (auto* frameItem : sortedFrames.sliced(index))
    {
        if (frameItem == this)
            continue;

        if (scRect.intersects(frameItem->sceneBoundingRect()))
        {
            contained.emplace_back(frameItem);

            auto frameContained = frameItem->ContainedItems();
            for (const auto& subitem : frameContained)
            {
                if (subitem == this)
                    continue;

                if (contained.contains(subitem))
                {
                    continue;
                }

                contained.append(subitem);
            }
        }
    }

    return contained;
}


void Gex::Ui::FrameItem::mousePressEvent(
        QGraphicsSceneMouseEvent *event)
{
    moveItems = true;
    items.clear();
    movedItemsPositions.clear();

    Grip cur = CurrentGrip(event->pos());
    if (cur == Grip::None)
    {
        items = ContainedItems();
    }
    else
    {
        moveItems = false;
        heldGrip = cur;

        resizeHandle->setVisible(true);
        resizeHandle->setRect(rect);
    }

    QGraphicsObject::mousePressEvent(event);
}


void Gex::Ui::FrameItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (heldGrip != Grip::None)
    {
        if (heldGrip == Grip::TopLeft)
        {
            // event->pos() becomes the top left corner.
            resizeHandle->setRect(
                    QRectF(
                            event->pos(),
                            rect.bottomRight()
                    )
            );
        }
        else if (heldGrip == Grip::TopRight)
        {
            resizeHandle->setRect(
                    QRectF(
                        QPointF(rect.topLeft().x(), event->pos().y()),
                        QPointF(event->pos().x(), rect.bottomRight().y())
                    )
            );
        }
        else if (heldGrip == Grip::BottomLeft)
        {
            resizeHandle->setRect(
                    QRectF(
                            QPointF(event->pos().x(), rect.y()),
                            QPointF(rect.bottomRight().x(), event->pos().y())
                    )
            );
        }
        else if (heldGrip == Grip::BottomRight)
        {
            resizeHandle->setRect(
                    QRectF(
                            rect.topLeft(),
                            event->pos()
                    )
            );
        }
    }
    else
    {
        QGraphicsObject::mouseMoveEvent(event);
    }
}


void Gex::Ui::FrameItem::mouseReleaseEvent(
        QGraphicsSceneMouseEvent *event)
{
    items.clear();
    movedItemsPositions.clear();

    moveItems = true;

    if (heldGrip != Grip::None)
    {
        Resize(resizeHandle->rect());
    }

    heldGrip = Grip::None;
    resizeHandle->setVisible(false);

    QGraphicsObject::mouseReleaseEvent(event);
}



Gex::Ui::FrameItem::Grip Gex::Ui::FrameItem::CurrentGrip(QPointF pos) const
{
    if (pos.x() <= gripWidth && pos.y() <= gripHeight)
        return Grip::TopLeft;

    if (pos.x() >= rect.width() - gripWidth &&
        pos.y() >= rect.height() - gripHeight)
        return Grip::BottomRight;

    if (pos.x() >= rect.width() - gripWidth && pos.y() < gripHeight)
        return Grip::TopRight;

    if (pos.x() <= gripWidth && pos.y() >= rect.height() - gripHeight)
        return Grip::BottomLeft;

    return Grip::None;
}


void Gex::Ui::FrameItem::Resize(QRectF resize)
{
    moveItems = false;
    QPointF offset = resize.topLeft();
    setPos(pos() + offset);
    SetRect(QRect(0, 0, resize.width(), resize.height()));
    moveItems = true;
}



void Gex::Ui::FrameItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Grip cur = CurrentGrip(event->pos());
    if (cur == Grip::TopLeft || cur == Grip::BottomRight)
    {
        setCursor(Qt::SizeFDiagCursor);
    }
    else if (cur == Grip::TopRight || cur == Grip::BottomLeft)
    {
        setCursor(Qt::SizeBDiagCursor);
    }
    else
    {
        unsetCursor();
    }

    QGraphicsObject::hoverEnterEvent(event);
}


void Gex::Ui::FrameItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    unsetCursor();

    QGraphicsObject::hoverLeaveEvent(event);
}


void Gex::Ui::FrameItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    Grip cur = CurrentGrip(event->pos());
    if (cur == Grip::TopLeft || cur == Grip::BottomRight)
    {
        setCursor(Qt::SizeFDiagCursor);
    }
    else if (cur == Grip::TopRight || cur == Grip::BottomLeft)
    {
        setCursor(Qt::SizeBDiagCursor);
    }
    else
    {
        unsetCursor();
    }

    QGraphicsObject::hoverMoveEvent(event);
}


QVariant Gex::Ui::FrameItem::itemChange(
        QGraphicsItem::GraphicsItemChange change,
        const QVariant &value)
{
    if (moveItems)
    {
        if (change == QGraphicsItem::ItemPositionChange)
        {
            sceneRect = sceneBoundingRect();

            movedItemsPositions.clear();

            for (const auto& item : items)
            {
                if (item.isNull())
                    continue;

                if (auto* frameItem = qgraphicsitem_cast<FrameItem*>(item.get()))
                {
                    frameItem->SetActive(false);
                }

                movedItemsPositions[item] = item->pos();
            }

            originPos = pos();
        }

        else if (change == QGraphicsItem::ItemPositionHasChanged)
        {
            auto offset = pos() - originPos;

            for (auto iter = movedItemsPositions.keyValueBegin();
                 iter != movedItemsPositions.keyValueEnd(); iter++)
            {
                iter->first->setPos(iter->first->pos() + offset);

                if (auto* frameItem = qgraphicsitem_cast<FrameItem*>(iter->first.get()))
                {
                    frameItem->SetActive(true);
                }
            }

            graphScene->SaveFrames();
        }
    }

    return value;
}


std::string Gex::Ui::FrameItem::Save() const
{
    rapidjson::Document doc;

    rapidjson::Value& data = doc.SetArray();

    rapidjson::Value textValue(rapidjson::kStringType);
    textValue.SetString(textItem->toPlainText().toStdString().c_str(),
                        textItem->toPlainText().toStdString().size(),
                        doc.GetAllocator());
    data.PushBack(textValue, doc.GetAllocator());

    rapidjson::Value originXValue(rapidjson::kNumberType);
    originXValue.SetDouble(originPos.x());
    data.PushBack(originXValue, doc.GetAllocator());

    rapidjson::Value originYValue(rapidjson::kNumberType);
    originYValue.SetDouble(originPos.y());
    data.PushBack(originYValue, doc.GetAllocator());

    rapidjson::Value posXValue(rapidjson::kNumberType);
    posXValue.SetDouble(pos().x());
    data.PushBack(posXValue, doc.GetAllocator());

    rapidjson::Value posYValue(rapidjson::kNumberType);
    posYValue.SetDouble(pos().y());
    data.PushBack(posYValue, doc.GetAllocator());

    rapidjson::Value xValue(rapidjson::kNumberType);
    xValue.SetDouble(rect.x());
    data.PushBack(xValue, doc.GetAllocator());

    rapidjson::Value yValue(rapidjson::kNumberType);
    yValue.SetDouble(rect.y());
    data.PushBack(yValue, doc.GetAllocator());

    rapidjson::Value widthValue(rapidjson::kNumberType);
    widthValue.SetDouble(rect.width());
    data.PushBack(widthValue, doc.GetAllocator());

    rapidjson::Value heightValue(rapidjson::kNumberType);
    heightValue.SetDouble(rect.height());
    data.PushBack(heightValue, doc.GetAllocator());

    rapidjson::Value colorValue(rapidjson::kStringType);
    colorValue.SetString(background.name().toStdString().c_str(),
                         background.name().toStdString().size(),
                         doc.GetAllocator());
    data.PushBack(colorValue, doc.GetAllocator());

    rapidjson::Value textColorValue(rapidjson::kStringType);
    textColorValue.SetString(textColor.name().toStdString().c_str(),
                             textColor.name().toStdString().size(),
                             doc.GetAllocator());
    data.PushBack(textColorValue, doc.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    doc.Accept(writer);

    return buffer.GetString();
}

void  Gex::Ui::FrameItem::Restore(const std::string& d)
{
    rapidjson::Document json;
    json.Parse(d.c_str());

    rapidjson::Value& data = json.GetArray();

    std::string text = data[0].GetString();
    textItem->setPlainText(text.c_str());

    originPos.setX(data[1].GetDouble());
    originPos.setY(data[2].GetDouble());

    setPos(data[3].GetDouble(),
           data[4].GetDouble());

    rect.setX(data[5].GetDouble());
    rect.setY(data[6].GetDouble());

    rect.setWidth(data[7].GetDouble());
    rect.setHeight(data[8].GetDouble());

    background = QColor(data[9].GetString());
    textColor = QColor(data[10].GetString());

    update();
}



Gex::Ui::FrameEditDialog::FrameEditDialog(
        FrameItem* item_, QWidget* widget):
        QDialog(widget)
{
    item = item_;

    auto* layout = new QGridLayout();
    setLayout(layout);

    auto* textLabel = new QLabel(this);
    textLabel->setText("Text color");
    layout->addWidget(textLabel, 1, 0, 1, 1);

    auto* textButton = new QPushButton(this);
    layout->addWidget(textButton, 1, 0, 1, 1);

    auto* backgroundLabel = new QLabel(this);
    backgroundLabel->setText("Background color");
    layout->addWidget(backgroundLabel, 0, 0, 1, 1);

    auto* backgroundButton = new QPushButton(this);
    layout->addWidget(backgroundButton, 0, 1, 1 ,1);
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

    previewFrameBrush = Qt::NoBrush;
    previewFramePen = QPen(Qt::white, 1, Qt::DashLine);
    setSceneRect(QRect(-100000, -100000, 200000, 200000));
}


void Gex::Ui::NodeGraphScene::mousePressEvent(
        QGraphicsSceneMouseEvent* mouseEvent
)
{

    QGraphicsScene::mousePressEvent(mouseEvent);
}


void Gex::Ui::NodeGraphScene::mouseMoveEvent(
        QGraphicsSceneMouseEvent* mouseEvent
)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
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


void Gex::Ui::NodeGraphScene::mouseReleaseEvent(
        QGraphicsSceneMouseEvent* mouseEvent
)
{
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
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


void Gex::Ui::NodeGraphScene::StartFrameMode()
{
    creatingFrame = true;
}


void Gex::Ui::NodeGraphScene::StopFrameMode()
{
    creatingFrame = false;
}


void Gex::Ui::NodeGraphScene::SaveFrames()
{
    if (!CurrentContext())
        return;

    CurrentContext()->Compound()->SetMetadata(
            "FrameNumber", (int)frames.size()
    );

    for (unsigned int i = 0; i < frames.size(); i++)
    {
        CurrentContext()->Compound()->SetMetadata(
                "Frame:" + std::to_string(i),
                frames[i]->Save()
        );
    }
}


void Gex::Ui::NodeGraphScene::RestoreFrames()
{
    auto compound = CurrentContext()->Compound();

    Gex::Feedback result;

    int frameNumber = compound->GetMetadata<int>(
            "FrameNumber", &result
    );

    if (!result)
        return;

    for (int i = 0; i < frameNumber; i++)
    {
        auto* frame = CreateFrame(QPointF(), QPointF());

        frame->Restore(
            compound->GetMetadata<std::string>(
                "Frame:" + std::to_string(i)
            )
        );
    }
}


Gex::Ui::FrameItem* Gex::Ui::NodeGraphScene::CreateFrame(
        QPointF topLeft, QPointF bottomRight)
{
    qreal width = bottomRight.x() - topLeft.x();
    qreal height = bottomRight.y() - topLeft.y();

    QPoint rectTopLeft(0, 0);

    auto* frame = new FrameItem(rectTopLeft, width, height, this);
    frame->SetBackgroundColor(QColor(255, 0, 0, 50));

    addItem(frame);

    frame->setPos(topLeft);

    frames.push_back(frame);

    return frame;
}


QList<Gex::Ui::FrameItem*> Gex::Ui::NodeGraphScene::FrameItems() const
{
    return frames;
}


QList<Gex::Ui::FrameItem*> Gex::Ui::NodeGraphScene::SortedFrameItems(const QRectF& rect) const
{
    QList<Gex::Ui::FrameItem*> frames;

    for (auto* item : items(rect, Qt::IntersectsItemShape, Qt::AscendingOrder))
    {
        if (auto* frame = qgraphicsitem_cast<FrameItem*>(item))
        {
            frames.append(frame);
        }
    }

    return frames;
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


void Gex::Ui::NodeGraphScene::OnNodeModified(const Gex::NodeWkPtr& node,
                                             const Gex::AttributeWkPtr& attr,
                                             const Gex::AttributeChange& change)
{
    Q_EMIT NodeModified(node, attr, change);
}


void Gex::Ui::NodeGraphScene::UpdateNodeAttribute(
        const Gex::NodePtr& node,
        const QString& attribute
)
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


bool Gex::Ui::ConnectionContext::AcceptBaseEvents() const
{
    return false;
}

void Gex::Ui::ConnectionContext::Setup(BaseGraphView* view)
{
    if (auto* nodeView = dynamic_cast<NodeGraphView*>(view))
    {
        input = nodeView->GetInput();
        output = nodeView->GetOutput();
    }
}


QPointF Gex::Ui::ConnectionContext::ScenePos(QPoint viewPos)
{
    return CurrentView()->mapToScene(viewPos);
}


QGraphicsItem* Gex::Ui::ConnectionContext::SceneItem(QPoint viewPos)
{
    return CurrentView()->scene()->itemAt(
            ScenePos(viewPos), QTransform()
    );
}


void Gex::Ui::ConnectionContext::OnPressEvent(QMouseEvent* event)
{
    if (QGraphicsItem* item = SceneItem(event->pos()))
    {
        auto* plugItem = qgraphicsitem_cast<PlugItem *>(item);
        if (!previewLink && plugItem) {
            previewLink = new PreviewLinkItem(plugItem);
            previewLink->SetState(PreviewLinkItem::State::Default);
            CurrentView()->scene()->addItem(previewLink);
            previewLink->Draw(ScenePos(event->pos()));
        }
    }
}

void Gex::Ui::ConnectionContext::OnMoveEvent(QMouseEvent* event)
{
    if (previewLink)
    {
        QGraphicsItem* item = SceneItem(event->pos());
        if (item)
        {
            auto* plugItem = qgraphicsitem_cast<PlugItem*>(item);
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

        previewLink->Draw(ScenePos(event->pos()));
    }
}

void Gex::Ui::ConnectionContext::OnReleaseEvent(QMouseEvent* event)
{
    QGraphicsItem *item = SceneItem(event->pos());
    if (previewLink)
    {
        if (item)
        {
            auto* plugItem = qgraphicsitem_cast<PlugItem *>(item);

            if (plugItem && (plugItem != previewLink->SourcePlug()))
            {
                AttributeItem *destItem = plugItem->Attribute();
                AttributeItem *sourceItem = previewLink->SourcePlug()->Attribute();
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
                    for (unsigned int index: destItem->Attribute()->ValidIndices())
                    {
                        Gex::AttributeWkPtr indexAttr = destItem->Attribute()->GetIndexAttribute(index);
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
                            auto *nodeItem = destItem->ParentNode();
                            destItem->RebuildAttributes();
                        }
                    }
                }
            }
        }

        CurrentView()->scene()->removeItem(previewLink);
        delete previewLink;
        previewLink = nullptr;
    }
}



Gex::Ui::NodeGraphView::NodeGraphView(NodeGraphScene* scene, QWidget* parent):
    BaseGraphView(scene, parent, true)
{
    graphScene = scene;

    GetEditors()->RegisterContext<ConnectionContext>("Connection");

    auto* shortcut_1 = new QShortcut(QKeySequence("1"), this);
    QObject::connect(shortcut_1, &QShortcut::activated, this,
                     &NodeGraphView::SetSelectedNodesVisAll);

    auto* shortcut_2 = new QShortcut(QKeySequence("2"), this);
    QObject::connect(shortcut_2, &QShortcut::activated, this,
                     &NodeGraphView::SetSelectedNodesVisSettable);

    auto* shortcut_3 = new QShortcut(QKeySequence("3"), this);
    QObject::connect(shortcut_3, &QShortcut::activated, this,
                     &NodeGraphView::SetSelectedNodesVisConnected);

    auto* shortcut_4 = new QShortcut(QKeySequence("Ctrl+D"), this);
    QObject::connect(shortcut_4, &QShortcut::activated, scene,
                     &NodeGraphScene::DuplicateSelectedNodesNoLinks);

    auto* shortcut_5 = new QShortcut(QKeySequence("Shift+D"), this);
    QObject::connect(shortcut_5, &QShortcut::activated, scene,
                     &NodeGraphScene::DuplicateSelectedNodesAndLinks);

    auto* shortcut_6 = new QShortcut(QKeySequence("Ctrl+Shift+X"), this);
    QObject::connect(shortcut_6, &QShortcut::activated, scene,
                     &NodeGraphScene::ConvertSelectedNodesToCompound);

    auto* shortcut_7 = new QShortcut(QKeySequence("Ctrl+E"), this);
    QObject::connect(shortcut_7, &QShortcut::activated, this,
                     &NodeGraphView::ExportSelectedNodes);

    auto* shortcut_8 = new QShortcut(QKeySequence("Ctrl+Shift+E"), this);
    QObject::connect(shortcut_8, &QShortcut::activated, this,
                     &NodeGraphView::ExportSelectedNodesAsCompound);
}


void Gex::Ui::NodeGraphView::SetSelectedNodesVis(NodeItem::AttributeVisibilityMode mode)
{
    for (QGraphicsItem* item : scene()->selectedItems())
    {
        auto* node = qgraphicsitem_cast<NodeItem*>(item);
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
    auto* createMenu = new QMenu();

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


void Gex::Ui::NodeGraphView::mousePressEvent(QMouseEvent *event)
{
    if (auto* pressedItem = scene()->itemAt(
            mapToScene(event->pos()), QTransform())
    )
    {
        if (auto* plugItem = dynamic_cast<PlugItem*>(pressedItem))
        {
            GetEditors()->SetCurrentContext("Connection");
        }
    }

    BaseGraphView::mousePressEvent(event);
}


void Gex::Ui::NodeGraphView::mouseReleaseEvent(QMouseEvent *event)
{
    BaseGraphView::mouseReleaseEvent(event);

    if (GetEditors()->CurrentContext() == "Connection")
    {
        GetEditors()->UnsetCurrentContext();
    }
}


Gex::Ui::NodeItem* Gex::Ui::NodeGraphView::GetInput() const
{
    return graphScene->GetInput();
}

Gex::Ui::NodeItem* Gex::Ui::NodeGraphView::GetOutput() const
{
    return graphScene->GetOutput();
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

Gex::Ui::NodeItem* Gex::Ui::NodeGraphScene::GetInput() const
{
    return input;
}

Gex::Ui::NodeItem* Gex::Ui::NodeGraphScene::GetOutput() const
{
    return output;
}


void Gex::Ui::NodeGraphScene::Clear()
{
    frames.clear();
    nodeItems.clear();
    input = nullptr;
    output = nullptr;
    clear();
}


void Gex::Ui::NodeGraphScene::SwitchGraphContext(NodeGraphContext* context)
{
    Clear();  // Clear all content.

    graphContext = context;
    for (const Gex::NodePtr& node : graphContext->GetNodes())
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

    RestoreFrames();
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


QList<Gex::Ui::NodeItem*> Gex::Ui::NodeGraphScene::NodeItems() const
{
    return nodeItems.values();
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


void Gex::Ui::NodeGraphScene::SelectNodes(const Gex::NodeWkList& sel,
                                          bool notify)
{
    blockSignals(true);

    clearSelection();

    QList<NodeItem*> items;
    for (const auto& node : sel)
    {
        auto* item = ItemFromNode(node);
        if (!item)
            continue;

        item->setSelected(true);
    }

    if (notify)
        blockSignals(false);

    Q_EMIT selectionChanged();

    if (!notify)
        blockSignals(false);
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


void Gex::Ui::NodeGraphScene::DeleteFrame(FrameItem* item)
{
    frames.removeAll(item);
    delete item;
}


void Gex::Ui::NodeGraphScene::DeleteSelection()
{
    QList<QPointer<NodeItem>> deleteNodes;
    QList<QPointer<LinkItem>> deleteLinks;
    QList<QPointer<FrameItem>> deleteFrames;
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

        else if (auto* frameItem = qgraphicsitem_cast<FrameItem*>(item))
        {
            deleteFrames.append(frameItem);
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

    for (const auto& frameItem : deleteFrames)
    {
        if (frameItem.isNull())
            continue;

        DeleteFrame(frameItem);
    }
}


void Gex::Ui::NodeGraphScene::CreateNode(QString type, QString name)
{
    Gex::NodePtr node = graphContext->CreateNode(
            type.toStdString(), name.toStdString());

    auto* item = new NodeItem(node, this);
    addItem(item);
    nodeItems[node] = item;

    auto* view = views().at(0);
    item->setPos(view->mapToScene(view->rect().center()));

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


void Gex::Ui::NodeGraphScene::NodeEvaluationStarted(const Gex::NodePtr& node)
{
    auto* nodeItem = nodeItems.value(node, nullptr);
    if (!nodeItem)
        return;

    nodeItem->SetCustomBorderColor(QColor("#5B6FD3"));

}

void Gex::Ui::NodeGraphScene::NodeEvaluationDone(const Gex::NodePtr& node, bool success)
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


Gex::Ui::ContextButton::ContextButton(const std::string& name_, unsigned int index_,
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


void Gex::Ui::ContextsWidget::AddContext(const std::string& name)
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


Gex::Ui::GraphWidget::GraphWidget(const Gex::CompoundNodePtr& graph_,
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

    QPushButton* createFrameButton = toolbar->NewButton();
    createFrameButton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_square));

    QObject::connect(createFrameButton, &QPushButton::clicked,
                     scene, &NodeGraphScene::StartFrameMode);

    QPushButton* autoRunButton = toolbar->NewButton();
    autoRunButton->setIcon(
            Res::UiRes::GetRes()->GetQtAwesome()->icon(
                fa::fa_solid, fa::fa_forward
            )
    );

    autoRunButton->setCheckable(true);

    autoRunButton->setChecked(interactiveEvalEnabled);

    QObject::connect(autoRunButton, &QPushButton::toggled, this,
                     &GraphWidget::ToggleInteractiveEvaluation);

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

    scene->blockSignals(true);
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


void Gex::Ui::GraphWidget::RegisterContext(const Gex::CompoundNodePtr& compound)
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


bool Gex::Ui::GraphWidget::InteractiveEvaluation() const
{
    return interactiveEvalEnabled;
}


void Gex::Ui::GraphWidget::ToggleInteractiveEvaluation(bool state)
{
    interactiveEvalEnabled = state;
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


void Gex::Ui::GraphWidget::SwitchGraph(const Gex::CompoundNodePtr& graph_)
{
    scene->Clear();

    graph = graph_;

    ClearContexts();

    Initialize();
}



void Gex::Ui::GraphWidget::UpdateNode(const Gex::NodePtr& node)
{
    scene->UpdateNode(node);
}


void Gex::Ui::GraphWidget::UpdateNodeAttribute(
        const Gex::NodePtr& node,
        const QString& attribute)
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
               [this](const Gex::NodePtr& node)
               {this->scene->NodeEvaluationStarted(node);},
               [this](const Gex::NodePtr& node, bool success)
               {this->scene->NodeEvaluationDone(node, success);},
               [this, prevState](const Gex::GraphContext& ctx)
               {EmitProfiler(this, ctx);this->interactiveEvalEnabled = prevState;}
               );
}


void Gex::Ui::GraphWidget::OnNodeChanged(
        const Gex::NodeWkPtr& node,
        const Gex::AttributeWkPtr& attr,
        const Gex::AttributeChange& change)
{
    if (!node)
        return;

    if (!attr->IsInput() || attr->IsUserDefined())
        return;

    if (change == AttributeChange::Connected ||
        change == AttributeChange::Disconnected ||
        change == AttributeChange::ValueChanged)
        RunFromNode(node.ToShared());
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

    auto postEval = [this](const Gex::GraphContext& ctx)
    {
        EmitProfiler(this, ctx);
        this->interactiveEvalEnabled = true;
    };

    GraphContext context;
    interactiveEval = new Gex::NodeEvaluator(
            subGraph, context, profiler,
            false, threadsSpinBox->value(),
            [this](const Gex::NodePtr& node)
            {this->scene->NodeEvaluationStarted(node);},
            [this](const Gex::NodePtr& node, bool success)
            {if (success) this->scene->NodeEvaluationDone(node, success);},
            postEval
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


void Gex::Ui::GraphWidget::Select(const Gex::NodeWkList& nodes,
                                  bool notify)
{
    scene->SelectNodes(nodes, notify);
}
