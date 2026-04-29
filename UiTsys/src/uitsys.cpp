#include "include/UiTsys/uitsys.h"
#include <QHBoxLayout>

#include "include/UiTsys//defaultTypes.h"
#include "Tsys/defaultTypes.h"


UiTSys::TypedWidget::TypedWidget(QWidget* parent): QWidget(parent)
{
    setObjectName("TypedWidget");
}


void UiTSys::TypedWidget::Setup(std::string name)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    title = new QLabel(this);
    title->setObjectName("TypedWidgetTitle");
    title->setText(name.c_str());
    layout->addWidget(title);

    QWidget* typeWidget = CreateTypedWidget();
    layout->addWidget(typeWidget);

    layout->setStretchFactor(title, 0);
    layout->setStretchFactor(typeWidget, 1);
}


void UiTSys::TypedWidget::ValueChanged(std::any value)
{
    Q_EMIT valueChanged(value);
}


void UiTSys::TypedWidget::OnValueChanged()
{
    ValueChanged(GetValue());
}


//bool UiTSys::TypedWidget::Connect(QObject* src, const char* signal)
//{
//    auto connection = QObject::connect(src, signal,this,
//                                       "OnValueChanged");
//    return bool(connection);
//}


UiTSys::TypedWidget* UiTSys::TypedWidgetCreator::NewTypedWidget(std::string name) const
{
    TypedWidget* widget = CreateWidget();
    widget->Setup(name);

    return widget;
}


UiTSys::TypedInitWidget::TypedInitWidget(QWidget* parent): QWidget(parent)
{
    mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    QLabel* defaultvalue = new QLabel(this);
    defaultvalue->setObjectName("TypedWidgetTitle");
    defaultvalue->setText("Default value :");
    mainLayout->addWidget(defaultvalue);
}


void UiTSys::TypedInitWidget::Initialize()
{
    mainLayout->insertWidget(1, CreateInitWidget());
}




UiTSys::TypedInitWidget* UiTSys::TypedInitWidgetCreator::NewWidget() const
{
    TypedInitWidget* widget = CreateWidget();

    widget->Initialize();

    return widget;
}


bool UiTSys::UiType::IsValid() const
{
    return (initCreator || widgetCreator);
}


bool UiTSys::UiType::HasInitCreator() const
{
    return bool(initCreator);
}


bool UiTSys::UiType::HasWidgetCreator() const
{
    return bool(widgetCreator);
}


bool UiTSys::UiType::operator==(const UiType& other)
{
    return (uiName == other.uiName);
}




UiTSys::UiTypeEngine* UiTSys::UiTypeEngine::instance = nullptr;


UiTSys::UiTypeEngine::UiTypeEngine()
{
    RegisterDefaultType(nullptr, MakeUiPtr(DefaultWidgetCreator));


    RegisterType(
        typeid(bool), "Bool",
        MakeUiPtr(BoolInitWidgetCreator),
        MakeUiPtr(BoolWidgetCreator)
    );


    RegisterType(
        typeid(int), "Int",
        MakeUiPtr(IntInitWidgetCreator),
        MakeUiPtr(IntWidgetCreator)
    );

    RegisterType(
        typeid(float), "Float",
        MakeUiPtr(FloatInitWidgetCreator),
        MakeUiPtr(FloatWidgetCreator)
    );

    RegisterType(
        typeid(double), "Double",
        MakeUiPtr(DoubleInitWidgetCreator),
        MakeUiPtr(DoubleWidgetCreator)
    );

    RegisterType(
        typeid(std::string), "String",
        MakeUiPtr(StringInitWidgetCreator),
        MakeUiPtr(StringWidgetCreator)
    );

    RegisterType(
        typeid(TSys::Enum), "Enum",
        MakeUiPtr(EnumInitWidgetCreator),
        MakeUiPtr(EnumWidgetCreator)
    );
}



UiTSys::UiTypeEngine*  UiTSys::UiTypeEngine::GetEngine()
{
    if (!instance)
        instance = new UiTypeEngine();

    return instance;
}


bool UiTSys::UiTypeEngine::RegisterType(
        const std::type_index& t, std::string uiName,
        const TypedInitCreatorPtr& initCreator,
        const TypedCreatorPtr& widgetCreator,
        bool force
)
{
    auto type = MakeUiPtr(UiType);
    type->uiName = uiName;
    type->initCreator = initCreator;
    type->widgetCreator = widgetCreator;

    return RegisterType(t, type, force);
}


bool UiTSys::UiTypeEngine::RegisterType(
        const std::type_index& t,
        const UiTypePtr& uiType,
        bool force
)
{
    if (!force && uiTypes.find(t) != uiTypes.end())
    {
        return false;
    }

    uiTypes[t] = uiType;
    return true;
}


void UiTSys::UiTypeEngine::RegisterDefaultType(
        const TypedInitCreatorPtr& initCreator,
        const TypedCreatorPtr& widgetCreator
)
{
    auto type = MakeUiPtr(UiType);
    type->uiName = "";
    type->initCreator = initCreator;
    type->widgetCreator = widgetCreator;

    defaultType = type;
}


UiTSys::TypedWidget* UiTSys::UiTypeEngine::CreateWidget(
        const std::type_index& t,
        const std::string& name
)
{
    auto iter = uiTypes.find(t);
    if (iter == uiTypes.end() || !iter->second->HasWidgetCreator())
    {
        if (defaultType->IsValid())
        {
            if (!defaultType->widgetCreator)
                return nullptr;

            return defaultType->widgetCreator->NewTypedWidget(name);
        }

        return nullptr;
    }

    auto type = uiTypes.at(t);
    return type->widgetCreator->NewTypedWidget(name);
}


UiTSys::TypedInitWidget* UiTSys::UiTypeEngine::CreateInitWidget(
        const std::type_index& t
)
{
    auto iter = uiTypes.find(t);
    if (iter == uiTypes.end() || !iter->second->HasInitCreator())
    {
        if (defaultType->IsValid())
        {
            if (!defaultType->initCreator)
                return nullptr;

            return defaultType->initCreator->NewWidget();
        }

        return nullptr;
    }

    auto type = uiTypes.at(t);
    return type->initCreator->NewWidget();
}


std::string UiTSys::UiTypeEngine::UiName(
        const std::type_index& t
) const
{
    if (uiTypes.find(t) == uiTypes.end())
    {
        return {};
    }

    auto type = uiTypes.at(t);
    return type->uiName;
}


std::type_index UiTSys::UiTypeEngine::UiTypeIndex(
        const std::string& name
) const
{
    auto cmp = [name](const std::pair<std::type_index, UiTypePtr> p) -> bool
    {
        return p.second->uiName == name;
    };

    auto iter = std::find_if(uiTypes.begin(), uiTypes.end(), cmp);
    if (iter == uiTypes.end())
        return typeid(TSys::None);

    return iter->first;
}


std::vector<std::type_index> UiTSys::UiTypeEngine::UiTypes() const
{
    std::vector<std::type_index> types;
    for (const auto& pair : uiTypes)
    {
        types.push_back(pair.first);
    }

    return types;
}


std::vector<std::string> UiTSys::UiTypeEngine::UiTypeNames() const
{
    std::vector<std::string> names;
    for (auto pair : uiTypes)
    {
        names.push_back(pair.second->uiName);
    }

    return names;
}

