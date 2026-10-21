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
        "bool",
        MakeUiPtr(BoolInitWidgetCreator),
        MakeUiPtr(BoolWidgetCreator)
    );


    RegisterType(
        "int",
        MakeUiPtr(IntInitWidgetCreator),
        MakeUiPtr(IntWidgetCreator)
    );

    RegisterType(
        "float",
        MakeUiPtr(FloatInitWidgetCreator),
        MakeUiPtr(FloatWidgetCreator)
    );

    RegisterType(
        "double",
        MakeUiPtr(DoubleInitWidgetCreator),
        MakeUiPtr(DoubleWidgetCreator)
    );

    RegisterType(
        "string",
        MakeUiPtr(StringInitWidgetCreator),
        MakeUiPtr(StringWidgetCreator)
    );
}



UiTSys::UiTypeEngine*  UiTSys::UiTypeEngine::GetEngine()
{
    if (!instance)
        instance = new UiTypeEngine();

    return instance;
}


bool UiTSys::UiTypeEngine::RegisterType(
        const std::string& typeName,
        const TypedInitCreatorPtr& initCreator,
        const TypedCreatorPtr& widgetCreator,
        bool force
)
{
    auto type = MakeUiPtr(UiType);
    type->uiName = typeName;
    type->initCreator = initCreator;
    type->widgetCreator = widgetCreator;

    return RegisterType(typeName, type, force);
}


bool UiTSys::UiTypeEngine::RegisterType(
        const std::string& typeName,
        const UiTypePtr& uiType,
        bool force
)
{
    if (!force && uiTypes.find(typeName) != uiTypes.end())
    {
        return false;
    }

    uiTypes[typeName] = uiType;
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
        const std::string& t,
        const std::string& title
)
{
    auto iter = uiTypes.find(t);
    if (iter == uiTypes.end() || !iter->second->HasWidgetCreator())
    {
        if (defaultType->IsValid())
        {
            if (!defaultType->widgetCreator)
                return nullptr;

            return defaultType->widgetCreator->NewTypedWidget(title);
        }

        return nullptr;
    }

    auto type = uiTypes.at(t);
    return type->widgetCreator->NewTypedWidget(title);
}


UiTSys::TypedInitWidget* UiTSys::UiTypeEngine::CreateInitWidget(
        const std::string& t
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


std::vector<std::string> UiTSys::UiTypeEngine::UiTypeNames() const
{
    std::vector<std::string> names;
    for (const auto& pair : uiTypes)
    {
        names.push_back(pair.first);
    }

    return names;
}

