#include "UsdUiPlugin.h"


QWidget* Gex::Usd::GfHalfWidget::CreateTypedWidget()
{
    widget = new DoubleSpinBox();
    Connect(widget, &DoubleSpinBox::valueChanged);
    return widget;
}

void Gex::Usd::GfHalfWidget::SetValue(std::any value)
{
    pxr::GfHalf half = std::any_cast<pxr::GfHalf>(value);

    widget->setValue(half);
}

std::any Gex::Usd::GfHalfWidget::GetValue() const
{
    pxr::GfHalf half(widget->value());
    return std::any_cast<pxr::GfHalf>(half);
}

void Gex::Usd::GfHalfWidget::ShowConnected(bool connected)
{
    widget->setReadOnly(connected);
    widget->setProperty("connected", connected);
}

void Gex::Usd::GfHalfWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}


QWidget* Gex::Usd::GfHalfInitWidget::CreateInitWidget()
{
    widget = new DoubleSpinBox(this);
    return widget;
}


std::any Gex::Usd::GfHalfInitWidget::CreateValue()
{
    return std::make_any<pxr::GfHalf>(pxr::GfHalf(widget->value()));
}



QWidget* Gex::Usd::SdfTimeCodeWidget::CreateTypedWidget()
{
    widget = new DoubleSpinBox();
    Connect(widget, &DoubleSpinBox::valueChanged);
    return widget;
}


void Gex::Usd::SdfTimeCodeWidget::SetValue(std::any value)
{
    pxr::SdfTimeCode half = std::any_cast<pxr::SdfTimeCode>(value);

    widget->setValue((double)half);
}

std::any Gex::Usd::SdfTimeCodeWidget::GetValue() const
{
    pxr::GfHalf half(widget->value());
    return std::any_cast<pxr::SdfTimeCode>(half);
}

void Gex::Usd::SdfTimeCodeWidget::ShowConnected(bool connected)
{
    widget->setReadOnly(connected);
    widget->setProperty("connected", connected);
}

void Gex::Usd::SdfTimeCodeWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}


QWidget* Gex::Usd::SdfTimeCodeInitWidget::CreateInitWidget()
{
    widget = new DoubleSpinBox(this);
    return widget;
}


std::any Gex::Usd::SdfTimeCodeInitWidget::CreateValue()
{
    return std::make_any<pxr::SdfTimeCode>(pxr::SdfTimeCode(widget->value()));
}





QWidget* Gex::Usd::TfTokenWidget::CreateTypedWidget()
{
    widget = new QLineEdit();
    Connect(widget, &QLineEdit::editingFinished);
    return widget;
}

void Gex::Usd::TfTokenWidget::SetValue(std::any value)
{
    pxr::TfToken str = std::any_cast<pxr::TfToken>(value);

    widget->setText(str.GetString().c_str());
}

std::any Gex::Usd::TfTokenWidget::GetValue() const
{
    pxr::TfToken str(widget->text().toStdString());
    return std::any_cast<pxr::TfToken>(str);
}

void Gex::Usd::TfTokenWidget::ShowConnected(bool connected)
{
    widget->setReadOnly(connected);
    widget->setProperty("connected", connected);
}

void Gex::Usd::TfTokenWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}



QWidget* Gex::Usd::TfTokenInitWidget::CreateInitWidget()
{
    widget = new QLineEdit(this);
    return widget;
}


std::any Gex::Usd::TfTokenInitWidget::CreateValue()
{
    return std::make_any<pxr::TfToken>(pxr::TfToken(widget->text().toStdString()));
}






QWidget* Gex::Usd::Matrix2dWidget::CreateTypedWidget()
{
    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout();
    widget->setLayout(layout);

    widget1 = new DoubleSpinBox();
    layout->addWidget(widget1, 0, 0, 1, 1);
    widget2 = new DoubleSpinBox();
    layout->addWidget(widget2, 0, 1, 1, 1);
    widget3 = new DoubleSpinBox();
    layout->addWidget(widget3, 1, 0, 1, 1);
    widget4 = new DoubleSpinBox();
    layout->addWidget(widget4, 0, 1, 1, 1);

    Connect(widget1, &DoubleSpinBox::valueChanged);
    Connect(widget2, &DoubleSpinBox::valueChanged);
    Connect(widget3, &DoubleSpinBox::valueChanged);
    Connect(widget4, &DoubleSpinBox::valueChanged);

    return widget;
}

void Gex::Usd::Matrix2dWidget::SetValue(std::any value)
{
    pxr::GfMatrix2d mat = std::any_cast<pxr::GfMatrix2d>(value);

    widget1->setValue(mat[0][0]);
    widget2->setValue(mat[0][1]);
    widget3->setValue(mat[1][0]);
    widget4->setValue(mat[1][1]);
}

std::any Gex::Usd::Matrix2dWidget::GetValue() const
{
    pxr::GfMatrix2d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value());
    return std::any_cast<pxr::GfMatrix2d>(mat);
}

void Gex::Usd::Matrix2dWidget::ShowConnected(bool connected)
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

void Gex::Usd::Matrix2dWidget::ShowDisabled(bool disabled)
{
    widget1->setDisabled(disabled);
    widget2->setDisabled(disabled);
    widget3->setDisabled(disabled);
    widget4->setDisabled(disabled);
}




QWidget* Gex::Usd::Matrix2dInitWidget::CreateInitWidget()
{
    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout();
    widget->setLayout(layout);

    widget1 = new DoubleSpinBox();
    layout->addWidget(widget1, 0, 0, 1, 1);
    widget2 = new DoubleSpinBox();
    layout->addWidget(widget2, 0, 1, 1, 1);
    widget3 = new DoubleSpinBox();
    layout->addWidget(widget3, 1, 0, 1, 1);
    widget4 = new DoubleSpinBox();
    layout->addWidget(widget4, 0, 1, 1, 1);

    return widget;
}


std::any Gex::Usd::Matrix2dInitWidget::CreateValue()
{
    pxr::GfMatrix2d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value());

    return std::make_any<pxr::GfMatrix2d>(mat);
}






QWidget* Gex::Usd::Matrix3dWidget::CreateTypedWidget()
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
    layout->addWidget(widget4, 1, 0, 1, 1);
    widget5 = new DoubleSpinBox();
    layout->addWidget(widget5, 1, 1, 1, 1);
    widget6 = new DoubleSpinBox();
    layout->addWidget(widget6, 1, 2, 1, 1);
    widget7 = new DoubleSpinBox();
    layout->addWidget(widget7, 2, 0, 1, 1);
    widget8 = new DoubleSpinBox();
    layout->addWidget(widget8, 2, 1, 1, 1);
    widget9 = new DoubleSpinBox();
    layout->addWidget(widget9, 2, 2, 1, 1);

    Connect(widget1, &DoubleSpinBox::valueChanged);
    Connect(widget2, &DoubleSpinBox::valueChanged);
    Connect(widget3, &DoubleSpinBox::valueChanged);
    Connect(widget4, &DoubleSpinBox::valueChanged);
    Connect(widget5, &DoubleSpinBox::valueChanged);
    Connect(widget6, &DoubleSpinBox::valueChanged);
    Connect(widget7, &DoubleSpinBox::valueChanged);
    Connect(widget8, &DoubleSpinBox::valueChanged);
    Connect(widget9, &DoubleSpinBox::valueChanged);

    return widget;
}

void Gex::Usd::Matrix3dWidget::SetValue(std::any value)
{
    pxr::GfMatrix3d mat = std::any_cast<pxr::GfMatrix3d>(value);

    widget1->setValue(mat[0][0]);
    widget2->setValue(mat[0][1]);
    widget3->setValue(mat[0][2]);
    widget4->setValue(mat[1][0]);
    widget5->setValue(mat[1][1]);
    widget6->setValue(mat[1][2]);
    widget7->setValue(mat[2][0]);
    widget8->setValue(mat[2][1]);
    widget9->setValue(mat[2][2]);
}

std::any Gex::Usd::Matrix3dWidget::GetValue() const
{
    pxr::GfMatrix3d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value(),
                        widget5->value(), widget6->value(),
                        widget7->value(), widget8->value(),
                        widget9->value());
    return std::any_cast<pxr::GfMatrix3d>(mat);
}

void Gex::Usd::Matrix3dWidget::ShowConnected(bool connected)
{
    widget1->setReadOnly(connected);
    widget1->setProperty("connected", connected);
    widget2->setReadOnly(connected);
    widget2->setProperty("connected", connected);
    widget3->setReadOnly(connected);
    widget3->setProperty("connected", connected);
    widget4->setReadOnly(connected);
    widget4->setProperty("connected", connected);
    widget5->setReadOnly(connected);
    widget5->setProperty("connected", connected);
    widget6->setReadOnly(connected);
    widget6->setProperty("connected", connected);
    widget7->setReadOnly(connected);
    widget7->setProperty("connected", connected);
    widget8->setReadOnly(connected);
    widget8->setProperty("connected", connected);
    widget9->setReadOnly(connected);
    widget9->setProperty("connected", connected);
}

void Gex::Usd::Matrix3dWidget::ShowDisabled(bool disabled)
{
    widget1->setDisabled(disabled);
    widget2->setDisabled(disabled);
    widget3->setDisabled(disabled);
    widget4->setDisabled(disabled);
    widget5->setDisabled(disabled);
    widget6->setDisabled(disabled);
    widget7->setDisabled(disabled);
    widget8->setDisabled(disabled);
    widget9->setDisabled(disabled);
}



QWidget* Gex::Usd::Matrix3dInitWidget::CreateInitWidget()
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
    layout->addWidget(widget4, 1, 0, 1, 1);
    widget5 = new DoubleSpinBox();
    layout->addWidget(widget5, 1, 1, 1, 1);
    widget6 = new DoubleSpinBox();
    layout->addWidget(widget6, 1, 2, 1, 1);
    widget7 = new DoubleSpinBox();
    layout->addWidget(widget7, 2, 0, 1, 1);
    widget8 = new DoubleSpinBox();
    layout->addWidget(widget8, 2, 1, 1, 1);
    widget9 = new DoubleSpinBox();
    layout->addWidget(widget9, 2, 2, 1, 1);

    return widget;
}


std::any Gex::Usd::Matrix3dInitWidget::CreateValue()
{
    pxr::GfMatrix3d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value(),
                        widget5->value(), widget6->value(),
                        widget7->value(), widget8->value(),
                        widget9->value());

    return std::make_any<pxr::GfMatrix3d>(mat);
}






QWidget* Gex::Usd::Matrix4dWidget::CreateTypedWidget()
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
    widget5 = new DoubleSpinBox();
    layout->addWidget(widget5, 1, 0, 1, 1);
    widget6 = new DoubleSpinBox();
    layout->addWidget(widget6, 1, 1, 1, 1);
    widget7 = new DoubleSpinBox();
    layout->addWidget(widget7, 1, 2, 1, 1);
    widget8 = new DoubleSpinBox();
    layout->addWidget(widget8, 1, 3, 1, 1);
    widget9 = new DoubleSpinBox();
    layout->addWidget(widget9, 2, 0, 1, 1);
    widget10 = new DoubleSpinBox();
    layout->addWidget(widget10, 2, 1, 1, 1);
    widget11 = new DoubleSpinBox();
    layout->addWidget(widget11, 2, 2, 1, 1);
    widget12 = new DoubleSpinBox();
    layout->addWidget(widget12, 2, 3, 1, 1);
    widget13 = new DoubleSpinBox();
    layout->addWidget(widget13, 3, 0, 1, 1);
    widget14 = new DoubleSpinBox();
    layout->addWidget(widget14, 3, 1, 1, 1);
    widget15 = new DoubleSpinBox();
    layout->addWidget(widget15, 3, 2, 1, 1);
    widget16 = new DoubleSpinBox();
    layout->addWidget(widget16, 3, 3, 1, 1);


    Connect(widget1, &DoubleSpinBox::valueChanged);
    Connect(widget2, &DoubleSpinBox::valueChanged);
    Connect(widget3, &DoubleSpinBox::valueChanged);
    Connect(widget4, &DoubleSpinBox::valueChanged);
    Connect(widget5, &DoubleSpinBox::valueChanged);
    Connect(widget6, &DoubleSpinBox::valueChanged);
    Connect(widget7, &DoubleSpinBox::valueChanged);
    Connect(widget8, &DoubleSpinBox::valueChanged);
    Connect(widget9, &DoubleSpinBox::valueChanged);
    Connect(widget10, &DoubleSpinBox::valueChanged);
    Connect(widget11, &DoubleSpinBox::valueChanged);
    Connect(widget12, &DoubleSpinBox::valueChanged);
    Connect(widget13, &DoubleSpinBox::valueChanged);
    Connect(widget14, &DoubleSpinBox::valueChanged);
    Connect(widget15, &DoubleSpinBox::valueChanged);
    Connect(widget16, &DoubleSpinBox::valueChanged);

    return widget;
}

void Gex::Usd::Matrix4dWidget::SetValue(std::any value)
{
    pxr::GfMatrix4d mat = std::any_cast<pxr::GfMatrix4d>(value);

    widget1->setValue(mat[0][0]);
    widget2->setValue(mat[0][1]);
    widget3->setValue(mat[0][2]);
    widget4->setValue(mat[0][3]);
    widget5->setValue(mat[1][0]);
    widget6->setValue(mat[1][1]);
    widget7->setValue(mat[1][2]);
    widget8->setValue(mat[1][3]);
    widget9->setValue(mat[2][0]);
    widget10->setValue(mat[2][1]);
    widget11->setValue(mat[2][2]);
    widget12->setValue(mat[2][3]);
    widget13->setValue(mat[3][0]);
    widget14->setValue(mat[3][1]);
    widget15->setValue(mat[3][2]);
    widget16->setValue(mat[3][3]);
}

std::any Gex::Usd::Matrix4dWidget::GetValue() const
{
    pxr::GfMatrix4d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value(),
                        widget5->value(), widget6->value(),
                        widget7->value(), widget8->value(),
                        widget9->value(), widget10->value(),
                        widget11->value(), widget12->value(),
                        widget13->value(), widget14->value(),
                        widget15->value(), widget16->value());
    return std::any_cast<pxr::GfMatrix4d>(mat);
}

void Gex::Usd::Matrix4dWidget::ShowConnected(bool connected)
{
    widget1->setReadOnly(connected);
    widget1->setProperty("connected", connected);
    widget2->setReadOnly(connected);
    widget2->setProperty("connected", connected);
    widget3->setReadOnly(connected);
    widget3->setProperty("connected", connected);
    widget4->setReadOnly(connected);
    widget4->setProperty("connected", connected);
    widget5->setReadOnly(connected);
    widget5->setProperty("connected", connected);
    widget6->setReadOnly(connected);
    widget6->setProperty("connected", connected);
    widget7->setReadOnly(connected);
    widget7->setProperty("connected", connected);
    widget8->setReadOnly(connected);
    widget8->setProperty("connected", connected);
    widget9->setReadOnly(connected);
    widget9->setProperty("connected", connected);
    widget10->setReadOnly(connected);
    widget10->setProperty("connected", connected);
    widget11->setReadOnly(connected);
    widget11->setProperty("connected", connected);
    widget12->setReadOnly(connected);
    widget12->setProperty("connected", connected);
    widget13->setReadOnly(connected);
    widget13->setProperty("connected", connected);
    widget14->setReadOnly(connected);
    widget14->setProperty("connected", connected);
    widget15->setReadOnly(connected);
    widget15->setProperty("connected", connected);
    widget16->setReadOnly(connected);
    widget16->setProperty("connected", connected);
}

void Gex::Usd::Matrix4dWidget::ShowDisabled(bool disabled)
{
    widget1->setDisabled(disabled);
    widget2->setDisabled(disabled);
    widget3->setDisabled(disabled);
    widget4->setDisabled(disabled);
    widget5->setDisabled(disabled);
    widget6->setDisabled(disabled);
    widget7->setDisabled(disabled);
    widget8->setDisabled(disabled);
    widget9->setDisabled(disabled);
    widget10->setDisabled(disabled);
    widget11->setDisabled(disabled);
    widget12->setDisabled(disabled);
    widget13->setDisabled(disabled);
    widget14->setDisabled(disabled);
    widget15->setDisabled(disabled);
    widget16->setDisabled(disabled);
}


QWidget* Gex::Usd::Matrix4dInitWidget::CreateInitWidget()
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
    widget5 = new DoubleSpinBox();
    layout->addWidget(widget5, 1, 0, 1, 1);
    widget6 = new DoubleSpinBox();
    layout->addWidget(widget6, 1, 1, 1, 1);
    widget7 = new DoubleSpinBox();
    layout->addWidget(widget7, 1, 2, 1, 1);
    widget8 = new DoubleSpinBox();
    layout->addWidget(widget8, 1, 3, 1, 1);
    widget9 = new DoubleSpinBox();
    layout->addWidget(widget9, 2, 0, 1, 1);
    widget10 = new DoubleSpinBox();
    layout->addWidget(widget10, 2, 1, 1, 1);
    widget11 = new DoubleSpinBox();
    layout->addWidget(widget11, 2, 2, 1, 1);
    widget12 = new DoubleSpinBox();
    layout->addWidget(widget12, 2, 3, 1, 1);
    widget13 = new DoubleSpinBox();
    layout->addWidget(widget13, 3, 0, 1, 1);
    widget14 = new DoubleSpinBox();
    layout->addWidget(widget14, 3, 1, 1, 1);
    widget15 = new DoubleSpinBox();
    layout->addWidget(widget15, 3, 2, 1, 1);
    widget16 = new DoubleSpinBox();
    layout->addWidget(widget16, 3, 3, 1, 1);

    return widget;
}


std::any Gex::Usd::Matrix4dInitWidget::CreateValue()
{
    pxr::GfMatrix4d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value(),
                        widget5->value(), widget6->value(),
                        widget7->value(), widget8->value(),
                        widget9->value(), widget10->value(),
                        widget11->value(), widget12->value(),
                        widget13->value(), widget14->value(),
                        widget15->value(), widget16->value());

    return std::make_any<pxr::GfMatrix4d>(mat);
}


void RegisterUiPlugin(Gex::Ui::UiPluginLoader* loader)
{
    loader->RegisterTypeWidget<pxr::GfHalf,
            Gex::Usd::GfHalfInitWidgetBuilder,
            Gex::Usd::GfHalfWidgetBuilder>("Half");

    loader->RegisterTypeWidget<pxr::SdfTimeCode,
            Gex::Usd::SdfTimeCodeInitWidgetBuilder,
            Gex::Usd::SdfTimeCodeWidgetBuilder>("TimeCode");

    loader->RegisterTypeWidget<pxr::TfToken,
            Gex::Usd::TfTokenInitWidgetBuilder,
            Gex::Usd::TfTokenWidgetBuilder>("Token");

    loader->RegisterTypeWidget<pxr::GfMatrix2d,
            Gex::Usd::Matrix2dInitWidgetBuilder,
            Gex::Usd::Matrix2dWidgetBuilder>("Matrix2d");

    loader->RegisterTypeWidget<pxr::GfMatrix3d,
            Gex::Usd::Matrix3dInitWidgetBuilder,
            Gex::Usd::Matrix3dWidgetBuilder>("Matrix3d");

    loader->RegisterTypeWidget<pxr::GfMatrix4d,
            Gex::Usd::Matrix4dInitWidgetBuilder,
            Gex::Usd::Matrix4dWidgetBuilder>("Matrix4d");

    loader->RegisterTypeWidget<pxr::GfQuatd,
            Gex::Usd::QuatdInitWidgetBuilder,
            Gex::Usd::QuatdWidgetBuilder>("Quatd");

    loader->RegisterTypeWidget<pxr::GfQuatf,
            Gex::Usd::QuatfInitWidgetBuilder,
            Gex::Usd::QuatfWidgetBuilder>("Quatf");

    loader->RegisterTypeWidget<pxr::GfQuath,
            Gex::Usd::QuathInitWidgetBuilder,
            Gex::Usd::QuathWidgetBuilder>("Quath");

    loader->RegisterTypeWidget<pxr::GfVec2d,
            Gex::Usd::Vec2dInitWidgetBuilder,
            Gex::Usd::Vec2dWidgetBuilder>("Vec2d");

    loader->RegisterTypeWidget<pxr::GfVec2f,
            Gex::Usd::QuatfInitWidgetBuilder,
            Gex::Usd::QuatfWidgetBuilder>("Vec2f");

    loader->RegisterTypeWidget<pxr::GfVec2h,
            Gex::Usd::Vec2hInitWidgetBuilder,
            Gex::Usd::Vec2hWidgetBuilder>("Vec2h");

    loader->RegisterTypeWidget<pxr::GfVec2i,
            Gex::Usd::Vec2iInitWidgetBuilder,
            Gex::Usd::Vec2iWidgetBuilder>("Vec2i");

    loader->RegisterTypeWidget<pxr::GfVec3d,
            Gex::Usd::Vec3dInitWidgetBuilder,
            Gex::Usd::Vec3dWidgetBuilder>("Vec3d");

    loader->RegisterTypeWidget<pxr::GfVec3f,
            Gex::Usd::Vec3fInitWidgetBuilder,
            Gex::Usd::Vec3fWidgetBuilder>("Vec3f");

    loader->RegisterTypeWidget<pxr::GfVec3h,
            Gex::Usd::Vec3hInitWidgetBuilder,
            Gex::Usd::Vec3hWidgetBuilder>("Vec3h");

    loader->RegisterTypeWidget<pxr::GfVec3i,
            Gex::Usd::Vec3iInitWidgetBuilder,
            Gex::Usd::Vec3iWidgetBuilder>("Vec3i");

    loader->RegisterTypeWidget<pxr::GfVec4d,
            Gex::Usd::Vec4dInitWidgetBuilder,
            Gex::Usd::Vec4dWidgetBuilder>("Vec4d");

    loader->RegisterTypeWidget<pxr::GfVec4f,
            Gex::Usd::Vec4fInitWidgetBuilder,
            Gex::Usd::Vec4fWidgetBuilder>("Vec4f");

    loader->RegisterTypeWidget<pxr::GfVec4h,
            Gex::Usd::Vec4hInitWidgetBuilder,
            Gex::Usd::Vec4hWidgetBuilder>("Vec4h");

    loader->RegisterTypeWidget<pxr::GfVec4i,
            Gex::Usd::Vec4iInitWidgetBuilder,
            Gex::Usd::Vec4iWidgetBuilder>("Vec4i");
}