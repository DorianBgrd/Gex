#ifndef GEX_UI_TYPEENGINE
#define GEX_UI_TYPEENGINE

#include <QOBject>
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QHBoxLayout>

#include <any>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <unordered_map>

#include "api.h"



#define MakeUiPtr(type) std::make_shared<type>()


namespace UiTSys
{
    class UI_TSYS_API TypedWidget: public QWidget
    {
    public:
        Q_OBJECT

    private:
        QLabel* title;

    public:
        Q_SIGNAL void valueChanged(std::any);

    public:
        TypedWidget(QWidget* parent=nullptr);

        virtual void Setup(std::string name);

        virtual QWidget* CreateTypedWidget() = 0;

        virtual void SetValue(std::any value) = 0;

        virtual std::any GetValue() const = 0;

        virtual void ValueChanged(std::any value);

        virtual void ShowConnected(bool connected) = 0;

        virtual void ShowDisabled(bool disabled) = 0;

        void OnValueChanged();

        template <typename Sender, typename Member>
        bool Connect(Sender src, Member signal)
        {
            auto connection = QObject::connect(src, signal, this, &TypedWidget::OnValueChanged);
            return bool(connection);
        }
    };


    struct UI_TSYS_API TypedWidgetCreator
    {
    public:
        TypedWidgetCreator() = default;

        virtual ~TypedWidgetCreator() = default;

        virtual TypedWidget* CreateWidget() const = 0;

        TypedWidget* NewTypedWidget(std::string name) const;
    };


    class UI_TSYS_API TypedInitWidget: public QWidget
    {
        Q_OBJECT

        QHBoxLayout* mainLayout;
    public:
        TypedInitWidget(QWidget* parent=nullptr);

        void Initialize();

        virtual QWidget* CreateInitWidget() = 0;

        virtual std::any CreateValue() = 0;
    };


    struct UI_TSYS_API TypedInitWidgetCreator
    {
    public:
        TypedInitWidgetCreator() = default;

        virtual ~TypedInitWidgetCreator() = default;

        virtual TypedInitWidget* CreateWidget() const = 0;

        TypedInitWidget* NewWidget() const;
    };


    typedef std::shared_ptr<TypedInitWidgetCreator> TypedInitCreatorPtr;
    typedef std::shared_ptr<TypedWidgetCreator> TypedCreatorPtr;


    struct UI_TSYS_API UiType
    {
        std::string uiName;
        TypedInitCreatorPtr initCreator;
        TypedCreatorPtr widgetCreator;

        bool IsValid() const;

        bool HasInitCreator() const;

        bool HasWidgetCreator() const;

        bool operator==(const UiType& other);
    };


    typedef std::shared_ptr<UiType> UiTypePtr;


    class UI_TSYS_API UiTypeEngine
    {
        std::unordered_map<std::string, UiTypePtr> uiTypes;
        UiTypePtr defaultType;

        static UiTypeEngine* instance;

    protected:
        UiTypeEngine();

    public:
        UiTypeEngine(UiTypeEngine& other) = delete;

    public:
        static UiTypeEngine* GetEngine();

    public:
        void RegisterDefaultType(const TypedInitCreatorPtr& initCreator,
                                 const TypedCreatorPtr& widgetCreator);

        bool RegisterType(const std::string& typeName,
                          const TypedInitCreatorPtr& initCreator,
                          const TypedCreatorPtr& widgetCreator,
                          bool force=false);

        bool RegisterType(const std::string& t,
                          const UiTypePtr& uiType,
                          bool force=false);

        template<class InitCreatorType, class CreatorType>
        bool RegisterType(const std::string& typeName, bool force=false)
        {
            return RegisterType(
                    typeName,
                MakeUiPtr(InitCreatorType),
                MakeUiPtr(CreatorType),
                force
            );
        }

        template<class CreatorType>
        bool RegisterType(const std::string& typeName, bool force=false)
        {
            return RegisterType(
                    typeName,
                    nullptr,
                    MakeUiPtr(CreatorType),
                    force
            );
        }

        TypedWidget* CreateWidget(const std::string& typeName,
                                  const std::string& title);

        TypedInitWidget* CreateInitWidget(const std::string& t);

        std::vector<std::string> UiTypeNames() const;
    };
}


#define GENERATE_DEFAULT_WIDGET_CREATOR(uiclass, cls)  \
    struct uiclass: public UiTSys::TypedWidgetCreator \
    {  \
        UiTSys::TypedWidget* CreateWidget() const override  \
        {  \
            return new cls();  \
        }  \
    };


#define GENERATE_DEFAULT_INIT_WIDGET_CREATOR(uiclass, cls)  \
    struct uiclass: public UiTSys::TypedInitWidgetCreator \
    {  \
        UiTSys::TypedInitWidget* CreateWidget() const override \
        {  \
            return new cls();  \
        }  \
    };


#endif // GEX_UI_TYPEENGINE