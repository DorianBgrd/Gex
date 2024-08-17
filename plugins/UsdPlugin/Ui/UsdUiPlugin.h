#ifndef PIPELINELIBRARY2COMPLETE_USDUIPLUGIN_H
#define PIPELINELIBRARY2COMPLETE_USDUIPLUGIN_H

#include "UiTsys/include/uitsys.h"

#include <QSpinBox>

#include "plugins/export.h"
#include "ui/include/PluginLoader.h"

#include "pxr/base/gf/half.h"
#include "pxr/usd/usd/prim.h"


namespace Gex::Usd
{
    class DoubleSpinBox: public QDoubleSpinBox
    {
        Q_OBJECT
    public:
        DoubleSpinBox(QWidget* parent=nullptr): QDoubleSpinBox(parent)
        {
            setStyleSheet("QDoubleSpinBox{background-color: black; color: white;} "
                          "QDoubleSpinBox::up-button{width: 0;} "
                          "QDoubleSpinBox::down-button{width: 0;} "
                          "QDoubleSpinBox[connected=true]{background-color: #65AAC4; color: white;} "
                          "QDoubleSpinBox[connected=true]{background-color: black; color: #818181;}");
            setMinimum(-9999999);
            setMaximum(9999999);
        }
    };


    class IntSpinBox: public QSpinBox
    {
        Q_OBJECT
    public:
        IntSpinBox(QWidget* parent=nullptr): QSpinBox(parent)
        {
            setStyleSheet("QSpinBox{background-color: black; color: white;} "
                          "QSpinBox::up-button{width: 0;} "
                          "QSpinBox::down-button{width: 0;} "
                          "QSpinBox[connected=true]{background-color: #65AAC4; color: white;} "
                          "QSpinBox[connected=true]{background-color: black; color: #818181;}");
            setMinimum(-9999999);
            setMaximum(9999999);
        }
    };


    class Plugin_API GfHalfWidget: public UiTSys::TypedWidget
    {
        Q_OBJECT

        DoubleSpinBox* widget = nullptr;
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };


    GENERATE_DEFAULT_WIDGET_CREATOR(GfHalfWidgetBuilder, GfHalfWidget)


    class Plugin_API GfHalfInitWidget: public UiTSys::TypedInitWidget
    {
        DoubleSpinBox* widget = nullptr;
    public:
        virtual QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(GfHalfInitWidgetBuilder, GfHalfInitWidget)




    class Plugin_API SdfTimeCodeWidget: public UiTSys::TypedWidget
    {
        Q_OBJECT

        DoubleSpinBox* widget = nullptr;
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    GENERATE_DEFAULT_WIDGET_CREATOR(SdfTimeCodeWidgetBuilder, SdfTimeCodeWidget)

    class Plugin_API SdfTimeCodeInitWidget: public UiTSys::TypedInitWidget
    {
        DoubleSpinBox* widget = nullptr;
    public:
        virtual QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };

    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(SdfTimeCodeInitWidgetBuilder, SdfTimeCodeInitWidget)



    class Plugin_API TfTokenWidget: public UiTSys::TypedWidget
    {
        Q_OBJECT

        QLineEdit* widget = nullptr;
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    GENERATE_DEFAULT_WIDGET_CREATOR(TfTokenWidgetBuilder, TfTokenWidget)

    class Plugin_API TfTokenInitWidget: public UiTSys::TypedInitWidget
    {
        QLineEdit* widget = nullptr;
    public:
        virtual QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };

    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(TfTokenInitWidgetBuilder, TfTokenInitWidget)


    class Plugin_API Matrix2dWidget: public UiTSys::TypedWidget
    {
        Q_OBJECT

        DoubleSpinBox* widget1 = nullptr;
        DoubleSpinBox* widget2 = nullptr;
        DoubleSpinBox* widget3 = nullptr;
        DoubleSpinBox* widget4 = nullptr;

    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    GENERATE_DEFAULT_WIDGET_CREATOR(Matrix2dWidgetBuilder, Matrix2dWidget)

    class Plugin_API Matrix2dInitWidget: public UiTSys::TypedInitWidget
    {
        DoubleSpinBox* widget1 = nullptr;
        DoubleSpinBox* widget2 = nullptr;
        DoubleSpinBox* widget3 = nullptr;
        DoubleSpinBox* widget4 = nullptr;

    public:
        virtual QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };

    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Matrix2dInitWidgetBuilder, Matrix2dInitWidget)


    class Plugin_API Matrix3dWidget: public UiTSys::TypedWidget
    {
        Q_OBJECT

        DoubleSpinBox* widget1 = nullptr;
        DoubleSpinBox* widget2 = nullptr;
        DoubleSpinBox* widget3 = nullptr;
        DoubleSpinBox* widget4 = nullptr;
        DoubleSpinBox* widget5 = nullptr;
        DoubleSpinBox* widget6 = nullptr;
        DoubleSpinBox* widget7 = nullptr;
        DoubleSpinBox* widget8 = nullptr;
        DoubleSpinBox* widget9 = nullptr;

    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    GENERATE_DEFAULT_WIDGET_CREATOR(Matrix3dWidgetBuilder, Matrix3dWidget)

    class Plugin_API Matrix3dInitWidget: public UiTSys::TypedInitWidget
    {
        DoubleSpinBox* widget1 = nullptr;
        DoubleSpinBox* widget2 = nullptr;
        DoubleSpinBox* widget3 = nullptr;
        DoubleSpinBox* widget4 = nullptr;
        DoubleSpinBox* widget5 = nullptr;
        DoubleSpinBox* widget6 = nullptr;
        DoubleSpinBox* widget7 = nullptr;
        DoubleSpinBox* widget8 = nullptr;
        DoubleSpinBox* widget9 = nullptr;

    public:
        virtual QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };

    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Matrix3dInitWidgetBuilder, Matrix3dInitWidget)


    class Plugin_API Matrix4dWidget: public UiTSys::TypedWidget
    {
        Q_OBJECT

        DoubleSpinBox* widget1 = nullptr;
        DoubleSpinBox* widget2 = nullptr;
        DoubleSpinBox* widget3 = nullptr;
        DoubleSpinBox* widget4 = nullptr;
        DoubleSpinBox* widget5 = nullptr;
        DoubleSpinBox* widget6 = nullptr;
        DoubleSpinBox* widget7 = nullptr;
        DoubleSpinBox* widget8 = nullptr;
        DoubleSpinBox* widget9 = nullptr;
        DoubleSpinBox* widget10 = nullptr;
        DoubleSpinBox* widget11 = nullptr;
        DoubleSpinBox* widget12 = nullptr;
        DoubleSpinBox* widget13 = nullptr;
        DoubleSpinBox* widget14 = nullptr;
        DoubleSpinBox* widget15 = nullptr;
        DoubleSpinBox* widget16 = nullptr;

    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    GENERATE_DEFAULT_WIDGET_CREATOR(Matrix4dWidgetBuilder, Matrix4dWidget)

    class Plugin_API Matrix4dInitWidget: public UiTSys::TypedInitWidget
    {
        DoubleSpinBox* widget1 = nullptr;
        DoubleSpinBox* widget2 = nullptr;
        DoubleSpinBox* widget3 = nullptr;
        DoubleSpinBox* widget4 = nullptr;
        DoubleSpinBox* widget5 = nullptr;
        DoubleSpinBox* widget6 = nullptr;
        DoubleSpinBox* widget7 = nullptr;
        DoubleSpinBox* widget8 = nullptr;
        DoubleSpinBox* widget9 = nullptr;
        DoubleSpinBox* widget10 = nullptr;
        DoubleSpinBox* widget11 = nullptr;
        DoubleSpinBox* widget12 = nullptr;
        DoubleSpinBox* widget13 = nullptr;
        DoubleSpinBox* widget14 = nullptr;
        DoubleSpinBox* widget15 = nullptr;
        DoubleSpinBox* widget16 = nullptr;

    public:
        virtual QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };

    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Matrix4dInitWidgetBuilder, Matrix4dInitWidget)


    template<typename Quat, typename Widget>
    class Plugin_API QuatWidget: public UiTSys::TypedWidget
    {
        Widget* widget1 = nullptr;
        Widget* widget2 = nullptr;
        Widget* widget3 = nullptr;
        Widget* widget4 = nullptr;

    public:
        QWidget* CreateTypedWidget() override
        {
            QWidget* widget = new QWidget();
            QGridLayout* layout = new QGridLayout();
            widget->setLayout(layout);

            widget1 = new DoubleSpinBox();
            layout->addWidget(widget1, 0, 0, 1, 1);
            widget2 = new DoubleSpinBox();
            layout->addWidget(widget2, 0, 1, 1, 1);
            widget3 = new DoubleSpinBox();
            layout->addWidget(widget3, 0, 2, 1, 1);
            widget4 = new DoubleSpinBox();
            layout->addWidget(widget4, 0, 3, 1, 1);

            Connect(widget1, &DoubleSpinBox::valueChanged);
            Connect(widget2, &DoubleSpinBox::valueChanged);
            Connect(widget3, &DoubleSpinBox::valueChanged);
            Connect(widget4, &DoubleSpinBox::valueChanged);

            return widget;
        }

        void SetValue(std::any value) override
        {
            Quat mat = std::any_cast<Quat>(value);

            auto imag = mat.GetImaginary();
            widget1->setValue(mat.GetReal());
            widget2->setValue(imag[0]);
            widget3->setValue(imag[1]);
            widget4->setValue(imag[2]);
        }

        std::any GetValue() const override
        {
            Quat mat(widget1->value(), widget2->value(),
                     widget3->value(), widget4->value());
            return std::any_cast<Quat>(mat);
        }

        void ShowConnected(bool connected) override
        {
            widget1->setReadOnly(connected);
            widget1->setProperty("connected", connected);
            widget2->setReadOnly(connected);
            widget2->setProperty("connected", connected);
            widget3->setReadOnly(connected);
            widget3->setProperty("connected", connected);
            widget4->setReadOnly(connected);
            widget4->setProperty("connected", connected);
        }

        void ShowDisabled(bool disabled) override
        {
            widget1->setDisabled(disabled);
            widget2->setDisabled(disabled);
            widget3->setDisabled(disabled);
            widget4->setDisabled(disabled);
        }
    };


    template<typename Quat, typename Widget>
    class Plugin_API QuatInitWidget: public UiTSys::TypedInitWidget
    {
        Widget* widget1 = nullptr;
        Widget* widget2 = nullptr;
        Widget* widget3 = nullptr;
        Widget* widget4 = nullptr;

    public:
        virtual QWidget* CreateInitWidget() override
        {
            QWidget* widget = new QWidget();
            QGridLayout* layout = new QGridLayout();
            widget->setLayout(layout);

            widget1 = new Widget();
            layout->addWidget(widget1, 0, 0, 1, 1);
            widget2 = new Widget();
            layout->addWidget(widget2, 0, 1, 1, 1);
            widget3 = new Widget();
            layout->addWidget(widget3, 1, 0, 1, 1);
            widget4 = new Widget();
            layout->addWidget(widget4, 0, 1, 1, 1);

            return widget;
        }

        std::any CreateValue() override
        {
            Quat mat(widget1->value(), widget2->value(),
                     widget3->value(), widget4->value());

            return std::make_any<Quat>(mat);
        }
    };


    class QuatdWidget: public QuatWidget<pxr::GfQuatd, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(QuatdWidgetBuilder, QuatdWidget)


    class QuatdInitWidget: public QuatInitWidget<pxr::GfQuatd, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(QuatdInitWidgetBuilder, QuatdInitWidget)


    class QuatfWidget: public QuatWidget<pxr::GfQuatd, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(QuatfWidgetBuilder, QuatfWidget)


    class QuatfInitWidget: public QuatInitWidget<pxr::GfQuatd, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(QuatfInitWidgetBuilder, QuatfInitWidget)


    class QuathWidget: public QuatWidget<pxr::GfQuatd, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(QuathWidgetBuilder, QuathWidget)


    class QuathInitWidget: public QuatInitWidget<pxr::GfQuatd, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(QuathInitWidgetBuilder, QuathInitWidget)





    template<typename Vec, typename Widget>
    class Plugin_API Vec2Widget: public UiTSys::TypedWidget
    {

        Widget* widget1 = nullptr;
        Widget* widget2 = nullptr;

    public:
        QWidget* CreateTypedWidget() override
        {
            QWidget* widget = new QWidget();
            QGridLayout* layout = new QGridLayout();
            widget->setLayout(layout);

            widget1 = new Widget();
            layout->addWidget(widget1, 0, 0, 1, 1);
            widget2 = new Widget();
            layout->addWidget(widget2, 0, 1, 1, 1);

            Connect(widget1, &Widget::valueChanged);
            Connect(widget2, &Widget::valueChanged);

            return widget;
        }

        void SetValue(std::any value) override
        {
            Vec vec = std::any_cast<Vec>(value);

            widget1->setValue(vec[0]);
            widget2->setValue(vec[1]);
        }

        std::any GetValue() const override
        {
            Vec vec(widget1->value(), widget2->value());
            return std::any_cast<Vec>(vec);
        }

        void ShowConnected(bool connected) override
        {
            widget1->setReadOnly(connected);
            widget1->setProperty("connected", connected);
            widget2->setReadOnly(connected);
            widget2->setProperty("connected", connected);
        }

        void ShowDisabled(bool disabled) override
        {
            widget1->setDisabled(disabled);
            widget2->setDisabled(disabled);
        }
    };


    template<typename Vec, typename Widget>
    class Plugin_API Vec2InitWidget: public UiTSys::TypedInitWidget
    {
        Widget* widget1 = nullptr;
        Widget* widget2 = nullptr;

    public:
        virtual QWidget* CreateInitWidget() override
        {
            QWidget* widget = new QWidget();
            QGridLayout* layout = new QGridLayout();
            widget->setLayout(layout);

            widget1 = new Widget();
            layout->addWidget(widget1, 0, 0, 1, 1);
            widget2 = new Widget();
            layout->addWidget(widget2, 0, 1, 1, 1);

            return widget;
        }

        std::any CreateValue() override
        {
            Vec mat(widget1->value(), widget2->value());

            return std::make_any<Vec>(mat);
        }
    };


    class Vec2fWidget: public Vec2Widget<pxr::GfVec2f, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec2fWidgetBuilder, Vec2fWidget)


    class Vec2fInitWidget: public Vec2InitWidget<pxr::GfVec2f, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec2fInitWidgetBuilder, Vec2fInitWidget)


    class Vec2dWidget: public Vec2Widget<pxr::GfVec2d, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec2dWidgetBuilder, Vec2dWidget)


    class Vec2dInitWidget: public Vec2InitWidget<pxr::GfVec2d, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec2dInitWidgetBuilder, Vec2dInitWidget)


    class Vec2hWidget: public Vec2Widget<pxr::GfVec2h, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec2hWidgetBuilder, Vec2hWidget)


    class Vec2hInitWidget: public Vec2InitWidget<pxr::GfVec2h, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec2hInitWidgetBuilder, Vec2hInitWidget)


    class Vec2iWidget: public Vec2Widget<pxr::GfVec2i, IntSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec2iWidgetBuilder, Vec2iWidget)


    class Vec2iInitWidget: public Vec2InitWidget<pxr::GfVec2i, IntSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec2iInitWidgetBuilder, Vec2iInitWidget)






    template<typename Vec, typename Widget>
    class Plugin_API Vec3Widget: public UiTSys::TypedWidget
    {

        Widget* widget1 = nullptr;
        Widget* widget2 = nullptr;
        Widget* widget3 = nullptr;

    public:
        QWidget* CreateTypedWidget() override
        {
            QWidget* widget = new QWidget();
            QGridLayout* layout = new QGridLayout();
            widget->setLayout(layout);

            widget1 = new Widget();
            layout->addWidget(widget1, 0, 0, 1, 1);
            widget2 = new Widget();
            layout->addWidget(widget2, 0, 1, 1, 1);
            widget3 = new Widget();
            layout->addWidget(widget3, 0, 2, 1, 1);

            Connect(widget1, &Widget::valueChanged);
            Connect(widget2, &Widget::valueChanged);
            Connect(widget3, &Widget::valueChanged);

            return widget;
        }

        void SetValue(std::any value) override
        {
            Vec vec = std::any_cast<Vec>(value);

            widget1->setValue(vec[0]);
            widget2->setValue(vec[1]);
            widget3->setValue(vec[2]);
        }

        std::any GetValue() const override
        {
            Vec vec(widget1->value(), widget2->value(), widget3->value());
            return std::any_cast<Vec>(vec);
        }

        void ShowConnected(bool connected) override
        {
            widget1->setReadOnly(connected);
            widget1->setProperty("connected", connected);
            widget2->setReadOnly(connected);
            widget2->setProperty("connected", connected);
            widget3->setReadOnly(connected);
            widget3->setProperty("connected", connected);
        }

        void ShowDisabled(bool disabled) override
        {
            widget1->setDisabled(disabled);
            widget2->setDisabled(disabled);
            widget3->setDisabled(disabled);
        }
    };


    template<typename Vec, typename Widget>
    class Plugin_API Vec3InitWidget: public UiTSys::TypedInitWidget
    {
        Widget* widget1 = nullptr;
        Widget* widget2 = nullptr;
        Widget* widget3 = nullptr;

    public:
        virtual QWidget* CreateInitWidget() override
        {
            QWidget* widget = new QWidget();
            QGridLayout* layout = new QGridLayout();
            widget->setLayout(layout);

            widget1 = new Widget();
            layout->addWidget(widget1, 0, 0, 1, 1);
            widget2 = new Widget();
            layout->addWidget(widget2, 0, 1, 1, 1);
            widget3 = new Widget();
            layout->addWidget(widget3, 0, 2, 1, 1);

            return widget;
        }

        std::any CreateValue() override
        {
            Vec mat(widget1->value(), widget2->value(), widget3->value());

            return std::make_any<Vec>(mat);
        }
    };


    class Vec3fWidget: public Vec3Widget<pxr::GfVec3f, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec3fWidgetBuilder, Vec3fWidget)


    class Vec3fInitWidget: public Vec3InitWidget<pxr::GfVec3f, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec3fInitWidgetBuilder, Vec3fInitWidget)


    class Vec3dWidget: public Vec3Widget<pxr::GfVec3d, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec3dWidgetBuilder, Vec3dWidget)


    class Vec3dInitWidget: public Vec3InitWidget<pxr::GfVec3d, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec3dInitWidgetBuilder, Vec3dInitWidget)


    class Vec3hWidget: public Vec3Widget<pxr::GfVec3h, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec3hWidgetBuilder, Vec3hWidget)


    class Vec3hInitWidget: public Vec3InitWidget<pxr::GfVec3h, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec3hInitWidgetBuilder, Vec3hInitWidget)


    class Vec3iWidget: public Vec3Widget<pxr::GfVec3i, IntSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec3iWidgetBuilder, Vec3iWidget)


    class Vec3iInitWidget: public Vec3InitWidget<pxr::GfVec3i, IntSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec3iInitWidgetBuilder, Vec3iInitWidget)







    template<typename Vec, typename Widget>
    class Plugin_API Vec4Widget: public UiTSys::TypedWidget
    {
        Widget* widget1 = nullptr;
        Widget* widget2 = nullptr;
        Widget* widget3 = nullptr;
        Widget* widget4 = nullptr;

    public:
        QWidget* CreateTypedWidget() override
        {
            QWidget* widget = new QWidget();
            QGridLayout* layout = new QGridLayout();
            widget->setLayout(layout);

            widget1 = new Widget();
            layout->addWidget(widget1, 0, 0, 1, 1);
            widget2 = new Widget();
            layout->addWidget(widget2, 0, 1, 1, 1);
            widget3 = new Widget();
            layout->addWidget(widget3, 0, 2, 1, 1);
            widget4 = new Widget();
            layout->addWidget(widget4, 0, 3, 1, 1);

            Connect(widget1, &Widget::valueChanged);
            Connect(widget2, &Widget::valueChanged);
            Connect(widget3, &Widget::valueChanged);
            Connect(widget4, &Widget::valueChanged);

            return widget;
        }

        void SetValue(std::any value) override
        {
            Vec vec = std::any_cast<Vec>(value);

            widget1->setValue(vec[0]);
            widget2->setValue(vec[1]);
            widget3->setValue(vec[2]);
            widget4->setValue(vec[4]);
        }

        std::any GetValue() const override
        {
            Vec vec(widget1->value(), widget2->value(),
                    widget3->value(), widget4->value());
            return std::any_cast<Vec>(vec);
        }

        void ShowConnected(bool connected) override
        {
            widget1->setReadOnly(connected);
            widget1->setProperty("connected", connected);
            widget2->setReadOnly(connected);
            widget2->setProperty("connected", connected);
            widget3->setReadOnly(connected);
            widget3->setProperty("connected", connected);
            widget4->setReadOnly(connected);
            widget4->setProperty("connected", connected);
        }

        void ShowDisabled(bool disabled) override
        {
            widget1->setDisabled(disabled);
            widget2->setDisabled(disabled);
            widget3->setDisabled(disabled);
            widget4->setDisabled(disabled);
        }
    };


    template<typename Vec, typename Widget>
    class Plugin_API Vec4InitWidget: public UiTSys::TypedInitWidget
    {
        Widget* widget1 = nullptr;
        Widget* widget2 = nullptr;
        Widget* widget3 = nullptr;
        Widget* widget4 = nullptr;

    public:
        virtual QWidget* CreateInitWidget() override
        {
            QWidget* widget = new QWidget();
            QGridLayout* layout = new QGridLayout();
            widget->setLayout(layout);

            widget1 = new Widget();
            layout->addWidget(widget1, 0, 0, 1, 1);
            widget2 = new Widget();
            layout->addWidget(widget2, 0, 1, 1, 1);
            widget3 = new Widget();
            layout->addWidget(widget3, 0, 2, 1, 1);
            widget4 = new Widget();
            layout->addWidget(widget4, 0, 3, 1, 1);

            return widget;
        }

        std::any CreateValue() override
        {
            Vec mat(widget1->value(), widget2->value(),
                    widget3->value(), widget4->value());

            return std::make_any<Vec>(mat);
        }
    };


    class Vec4fWidget: public Vec4Widget<pxr::GfVec4f, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec4fWidgetBuilder, Vec4fWidget)


    class Vec4fInitWidget: public Vec4InitWidget<pxr::GfVec4f, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec4fInitWidgetBuilder, Vec4fInitWidget)


    class Vec4dWidget: public Vec4Widget<pxr::GfVec4d, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec4dWidgetBuilder, Vec4dWidget)


    class Vec4dInitWidget: public Vec4InitWidget<pxr::GfVec4d, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec4dInitWidgetBuilder, Vec4dInitWidget)


    class Vec4hWidget: public Vec4Widget<pxr::GfVec4h, DoubleSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec4hWidgetBuilder, Vec4hWidget)


    class Vec4hInitWidget: public Vec4InitWidget<pxr::GfVec4h, DoubleSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec4hInitWidgetBuilder, Vec4hInitWidget)


    class Vec4iWidget: public Vec4Widget<pxr::GfVec4i, IntSpinBox> {};


    GENERATE_DEFAULT_WIDGET_CREATOR(Vec4iWidgetBuilder, Vec4iWidget)


    class Vec4iInitWidget: public Vec4InitWidget<pxr::GfVec4i, IntSpinBox> {};


    GENERATE_DEFAULT_INIT_WIDGET_CREATOR(Vec4iInitWidgetBuilder, Vec4iInitWidget)


}


extern "C" Plugin_API
void RegisterUiPlugin(Gex::Ui::UiPluginLoader* loader);


#endif //PIPELINELIBRARY2COMPLETE_USDUIPLUGIN_H
