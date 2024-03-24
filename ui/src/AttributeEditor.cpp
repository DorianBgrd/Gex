#include "ui/include/AttributeEditor.h"
#include "UiRes/include/uires.h"
#include "ui/include/NodeGraph.h"
#include "ui/include/commands.h"

#include <QWidget>



Gex::Ui::MultiAttributeWidget::MultiAttributeWidget(
        Gex::Attribute* attr,
        GraphWidget* graphWidget,
        QWidget* parent): QFrame(parent)
{
    graph = graphWidget;
    attribute = attr;
    setObjectName("MultiAttributeFrame");

    mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);

    title = new QPushButton(this);
    title->setObjectName("MultiTypedWidget");
    title->setText(attr->Name().c_str());
    title->setCheckable(true);
    title->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_caret_right));
    mainLayout->addWidget(title);

    QHBoxLayout* subLayout = new QHBoxLayout();
    subLayout->setContentsMargins(0, 0, 0, 0);
    subLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    title->setLayout(subLayout);

    newIndexButton = new QPushButton(this);
    newIndexButton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_plus));
//    newIndexButton->setFixedSize(QSize(25, 25));
    subLayout->addWidget(newIndexButton);

    newIndexButton->setVisible(attribute->IsMulti());

    subAttributeWidget = new QWidget(this);
    mainLayout->addWidget(subAttributeWidget);

    subAttributeLayout = new QVBoxLayout();
    subAttributeWidget->setLayout(subAttributeLayout);
    subAttributeLayout->setContentsMargins(15, 0, 0, 0);

    QObject::connect(newIndexButton, &QPushButton::clicked, this,
                     &MultiAttributeWidget::AddMultiIndex);

    QObject::connect(title, &QPushButton::clicked, this,
                     &MultiAttributeWidget::SetExpanded);

    CreateAttributes();

    SetExpanded(false);
}





void Gex::Ui::MultiAttributeWidget::AddMultiIndex(bool)
{
    attribute->CreateIndex(NextMultiAttributeIndex(attribute));

    graph->UpdateNodeAttribute(attribute->Node(),
                               attribute->Longname().c_str()
    );

    RebuildAttributes();
}


void Gex::Ui::MultiAttributeWidget::SetExpanded(bool expanded)
{
    subAttributeWidget->setVisible(expanded);

    QIcon icon;
    if (expanded)
        icon = Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_caret_down);
    else
        icon = Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_caret_right);
    title->setIcon(icon);
}


void Gex::Ui::MultiAttributeWidget::FlushAttributes()
{
    for (int i = 0; i < subAttributeLayout->count(); i++)
    {
        QWidget* widget = subAttributeLayout->itemAt(i)->widget();
        if (!widget)
            continue;

        widget->deleteLater();
    }
}


void Gex::Ui::MultiAttributeWidget::CreateAttributes()
{
    for (auto index : attribute->ValidIndices())
    {
        auto* indexAttr = attribute->GetIndexAttribute(index);
        if (indexAttr->IsMulti() || indexAttr->HasChildAttributes())
        {
            auto* multiWidget = new MultiAttributeWidget(indexAttr, graph, this);
            subAttributeLayout->addWidget(multiWidget);
        }

        else
        {
            auto* widget = new AttributeWidget(indexAttr, graph, this);
            subAttributeLayout->addWidget(widget);
        }
    }
}


void Gex::Ui::MultiAttributeWidget::RebuildAttributes()
{
    FlushAttributes();

    CreateAttributes();
}


Gex::Ui::AttributeWidget::AttributeWidget(Gex::Attribute* attr,
                                                    GraphWidget* graphWidget,
                                                    QWidget* parent): QWidget(parent)
{
    graph = graphWidget;
    attribute = attr;
    mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    UiTSys::TypedWidget* widget = UiTSys::UiTypeEngine::GetEngine()->CreateWidget(attr->TypeHash(),
                                                                            attr->Name());
    mainLayout->addWidget(widget);

    QPushButton* followCnx = new QPushButton(this);
    followCnx->setFixedSize(QSize(25, 25));
    followCnx->setObjectName("FollowCnxButton");
    mainLayout->addWidget(followCnx);

    widget->SetValue(attribute->GetAnyValue());
    widget->ShowConnected(attribute->HasSource());
    widget->ShowDisabled(attribute->IsOutput() && !attribute->IsStatic());

    QObject::connect(widget, &UiTSys::TypedWidget::valueChanged,
                     this, &AttributeWidget::OnValueChanged);
}


void Gex::Ui::AttributeWidget::OnValueChanged(std::any value)
{
    attribute->SetAnyValue(value);
}




Gex::Ui::ExtraAttributeDialog::ExtraAttributeDialog(Gex::Node* node,
                                                              GraphWidget* _graphWidget,
                                                              QWidget* parent, QWidget* _updateWidget):
        QDialog(parent)
{
    graphWidget = _graphWidget;
    updateWidget = nullptr;
    if (_updateWidget)
        updateWidget = _updateWidget;
    else if (parent)
        updateWidget = parent;

    targetNode = node;

    Setup();
}


void Gex::Ui::ExtraAttributeDialog::Setup()
{
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(nameLayout);

    QLabel* nameLabel = new QLabel();
    nameLabel->setObjectName("defaultLabel");
    nameLabel->setText("Attribute name :");
    nameLayout->addWidget(nameLabel);

    attributeName = new QLineEdit(this);
    nameLayout->addWidget(attributeName);

    QHBoxLayout* vtypeLayout = new QHBoxLayout();
    vtypeLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(vtypeLayout);

    input = new QCheckBox(this);
    input->setText("Input");
    vtypeLayout->addWidget(input);

    output = new QCheckBox(this);
    output->setText("Output");
    vtypeLayout->addWidget(output);

    // TODO : Holder attributes...
    multi = new QCheckBox(this);
    multi->setText("Multi");
    vtypeLayout->addWidget(multi);

    internal = new QCheckBox(this);
    internal->setText("Internal");
    internal->setVisible(targetNode->IsCompound());
    vtypeLayout->addWidget(internal);

    QHBoxLayout* typeLayout = new QHBoxLayout();
    typeLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(typeLayout);

    QLabel* typeLabel = new QLabel();
    typeLabel->setObjectName("defaultLabel");
    typeLabel->setText("Type :");
    typeLayout->addWidget(typeLabel);

    QComboBox* typecb = new QComboBox(this);
    for (auto t : UiTSys::UiTypeEngine::GetEngine()->UiTypes())
    {
        typecb->addItem(UiTSys::UiTypeEngine::GetEngine()->UiName(t).c_str());
    }
    typeLayout->addWidget(typecb);

    initWidgetLayout = new QVBoxLayout();
    initWidgetLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(initWidgetLayout);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(buttonsLayout);

    QPushButton* createButton = new QPushButton(this);
    createButton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_add));
    createButton->setText("Create");
    buttonsLayout->addWidget(createButton);

    QPushButton* cancelButton = new QPushButton(this);
    cancelButton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_times));
    cancelButton->setText("Cancel");
    buttonsLayout->addWidget(cancelButton);

    QObject::connect(typecb, &QComboBox::currentTextChanged,
                     this, &ExtraAttributeDialog::SetType);

    QObject::connect(createButton, &QPushButton::clicked,
                     this, &ExtraAttributeDialog::CreateAttribute);

    QObject::connect(cancelButton, &QPushButton::clicked,
                     this, &ExtraAttributeDialog::reject);
}


void Gex::Ui::ExtraAttributeDialog::SetType(const QString& type)
{
    if (initWidget)
        initWidget->deleteLater();

    size_t hash = UiTSys::UiTypeEngine::GetEngine()->UiHash(type.toStdString());

    initWidget = UiTSys::UiTypeEngine::GetEngine()->CreateInitWidget(hash);

    initWidgetLayout->addWidget(initWidget);
}


void Gex::Ui::ExtraAttributeDialog::CreateAttribute()
{
    if (!initWidget)
        return;

    std::any value = initWidget->CreateValue();

    Gex::AttrType attrType;
    if (input->isChecked() && output->isChecked())
        attrType = Gex::AttrType::Static;
    else if (input->isChecked())
        attrType = Gex::AttrType::Input;
    else if (output->isChecked())
        attrType = Gex::AttrType::Output;
    else
        attrType = Gex::AttrType::Static;

    Gex::AttrValueType valueType = Gex::AttrValueType::Single;
    if (multi->isChecked())
        valueType = Gex::AttrValueType::Multi;

    if (targetNode->IsCompound() && internal->isChecked())
    {
        auto* compound = Gex::Node::ConvertTo<Gex::CompoundNode>(targetNode);
        auto* at = compound->CreateAttributeFromValue(attributeName->text().toStdString(),
                                                      value, valueType, attrType);
        at->SetInternal(true);

    }
    else
    {
        targetNode->CreateAttributeFromValue(attributeName->text().toStdString(),
                                             value, valueType, attrType);
    }

    if (updateWidget)
    {
        auto u = dynamic_cast<AttributeTab*>(updateWidget);
        if (u)
        {
            u->RebuildAttributes();
        }
    }

    graphWidget->UpdateNode(targetNode);

    accept();
}



Gex::Ui::AttributeTab::AttributeTab(
        Gex::Node* node_,
        GraphWidget* graphWidget,
        QWidget* parent): QWidget(parent)
{
    node = node_;
    graph = graphWidget;

    Setup();
    UpdateAttributes();
}


void Gex::Ui::AttributeTab::Setup()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    setLayout(mainLayout);

    QLabel* title = new QLabel(this);
    title->setObjectName("AttributeTitle");
    title->setText(node->Name().c_str());
    mainLayout->addWidget(title);

    QLabel* type = new QLabel(this);
    type->setObjectName("AttributeType");
    type->setText(node->Type().c_str());
    mainLayout->addWidget(type);

    QLabel* description = new QLabel(this);
    description->setWordWrap(true);
    description->setObjectName("AttributeDescription");
    description->setText(node->Description().c_str());
    mainLayout->addWidget(description);

    QFrame* separator = new QFrame(this);
    separator->setObjectName("AttributeSeparator");
    separator->setLineWidth(1);
    separator->setFixedHeight(1);
    separator->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(separator);

    QScrollArea* attributesArea = new QScrollArea(this);
    attributesArea->setWidgetResizable(true);
    attributesArea->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(attributesArea);

    QWidget* attributesWidget = new QWidget(attributesArea);
    attributesWidget->setContentsMargins(0, 0, 0, 0);
    attributesArea->setWidget(attributesWidget);

    widgetsLayout = new QVBoxLayout();
    widgetsLayout->setContentsMargins(0, 0, 0, 0);
    widgetsLayout->setAlignment(Qt::AlignTop);
    attributesWidget->setLayout(widgetsLayout);

    QPushButton* extraAttr = new QPushButton(this);
    extraAttr->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_add));
    extraAttr->setText("Add attribute");
    mainLayout->addWidget(extraAttr);

    QObject::connect(extraAttr, &QPushButton::clicked, this,
                     &AttributeTab::CreateExtraAttribute);
}


void Gex::Ui::AttributeTab::Clear()
{
    for (auto* widget : widgets)
    {
        widget->deleteLater();
    }

    widgets.clear();
}


void Gex::Ui::AttributeTab::UpdateAttributes()
{
    Clear();

    for (auto* attr : node->GetAttributes())
    {
        QWidget* widget = nullptr;
        if (attr->IsMulti() || attr->HasChildAttributes())
        {
            widget = new MultiAttributeWidget(attr, graph, this);
        }

        else
        {
            widget = new AttributeWidget(attr, graph, this);
        }

        widgetsLayout->addWidget(widget);
        widgets.push_back(widget);
    }
}


void Gex::Ui::AttributeTab::RebuildAttributes()
{
    UpdateAttributes();
}


void Gex::Ui::AttributeTab::CreateExtraAttribute()
{
    auto* window = new Gex::Ui::ExtraAttributeDialog(node, graph, this);
    window->exec();
    window->deleteLater();
}


Gex::Ui::AttributeEditor::AttributeEditor(GraphWidget* parent): QWidget(parent)
{
    graph = parent;
    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);

    stacked = new QStackedWidget(this);
    layout->addWidget(stacked);

    emptyWidget = new QWidget(stacked);
    QVBoxLayout* emptyLayout = new QVBoxLayout();
    emptyLayout->setAlignment(Qt::AlignCenter);
    emptyWidget->setLayout(emptyLayout);

    QLabel* emptyLabel = new  QLabel(emptyWidget);
    emptyLabel->setFont(QFont("sans", 9));
    emptyLabel->setObjectName("DefaultLabel");
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setText("Select some nodes\nto edit their attributes.");
    emptyLayout->addWidget(emptyLabel);

    stacked->addWidget(emptyWidget);

    tab = new QTabWidget(stacked);
    tab->setObjectName("TabWidget");
    stacked->addWidget(tab);
}


void Gex::Ui::AttributeEditor::SetNodes(std::vector<Gex::Node*> nodes)
{
    tab->clear();

    for (auto* node : nodes)
    {
        AttributeTab* widget = new AttributeTab(node, graph, this);
        tab->addTab(widget, node->Name().c_str());
    }

    stacked->setCurrentIndex((int)(!nodes.empty()));
}
