//
// Created by bouga on 23/10/2023.
//
#include "UsdUiPlugin.h"


QWidget* PipelineSoftware2::GfHalfWidget::CreateTypedWidget()
{
    widget = new DoubleSpinBox();
    Connect(widget, &DoubleSpinBox::valueChanged);
    return widget;
}

void PipelineSoftware2::GfHalfWidget::SetValue(std::any value)
{
    pxr::GfHalf half = std::any_cast<pxr::GfHalf>(value);

    widget->setValue(half);
}

std::any PipelineSoftware2::GfHalfWidget::GetValue() const
{
    pxr::GfHalf half(widget->value());
    return std::any_cast<pxr::GfHalf>(half);
}

void PipelineSoftware2::GfHalfWidget::ShowConnected(bool connected)
{
    widget->setReadOnly(connected);
    widget->setProperty("connected", connected);
}

void PipelineSoftware2::GfHalfWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}


QWidget* PipelineSoftware2::GfHalfInitWidget::CreateInitWidget()
{
    widget = new DoubleSpinBox(this);
    return widget;
}


std::any PipelineSoftware2::GfHalfInitWidget::CreateValue()
{
    return std::make_any<pxr::GfHalf>(pxr::GfHalf(widget->value()));
}


PipelineLibrary::Property* PipelineSoftware2::GfHalfInitWidget::CreateProperty(std::string name,
                                                                               bool array, bool upstream
                                                                               ) const
{
    return new PipelineLibrary::Property(name, pxr::GfHalf(widget->value()),
                                         nullptr, upstream);
}




QWidget* PipelineSoftware2::SdfTimeCodeWidget::CreateTypedWidget()
{
    widget = new DoubleSpinBox();
    Connect(widget, &DoubleSpinBox::valueChanged);
    return widget;
}


void PipelineSoftware2::SdfTimeCodeWidget::SetValue(std::any value)
{
    pxr::SdfTimeCode half = std::any_cast<pxr::SdfTimeCode>(value);

    widget->setValue((double)half);
}

std::any PipelineSoftware2::SdfTimeCodeWidget::GetValue() const
{
    pxr::GfHalf half(widget->value());
    return std::any_cast<pxr::SdfTimeCode>(half);
}

void PipelineSoftware2::SdfTimeCodeWidget::ShowConnected(bool connected)
{
    widget->setReadOnly(connected);
    widget->setProperty("connected", connected);
}

void PipelineSoftware2::SdfTimeCodeWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}


QWidget* PipelineSoftware2::SdfTimeCodeInitWidget::CreateInitWidget()
{
    widget = new DoubleSpinBox(this);
    return widget;
}


std::any PipelineSoftware2::SdfTimeCodeInitWidget::CreateValue()
{
    return std::make_any<pxr::SdfTimeCode>(pxr::SdfTimeCode(widget->value()));
}


PipelineLibrary::Property* PipelineSoftware2::SdfTimeCodeInitWidget::CreateProperty(std::string name,
                                                                               bool array, bool upstream
                                                                                ) const
{
    return new PipelineLibrary::Property(name, pxr::SdfTimeCode(widget->value()),
                                         nullptr, upstream);
}





QWidget* PipelineSoftware2::TfTokenWidget::CreateTypedWidget()
{
    widget = new QLineEdit();
    Connect(widget, &QLineEdit::editingFinished);
    return widget;
}

void PipelineSoftware2::TfTokenWidget::SetValue(std::any value)
{
    pxr::TfToken str = std::any_cast<pxr::TfToken>(value);

    widget->setText(str.GetString().c_str());
}

std::any PipelineSoftware2::TfTokenWidget::GetValue() const
{
    pxr::TfToken str(widget->text().toStdString());
    return std::any_cast<pxr::TfToken>(str);
}

void PipelineSoftware2::TfTokenWidget::ShowConnected(bool connected)
{
    widget->setReadOnly(connected);
    widget->setProperty("connected", connected);
}

void PipelineSoftware2::TfTokenWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}



QWidget* PipelineSoftware2::TfTokenInitWidget::CreateInitWidget()
{
    widget = new QLineEdit(this);
    return widget;
}


std::any PipelineSoftware2::TfTokenInitWidget::CreateValue()
{
    return std::make_any<pxr::TfToken>(pxr::TfToken(widget->text().toStdString()));
}


PipelineLibrary::Property* PipelineSoftware2::TfTokenInitWidget::CreateProperty(std::string name,
                                                                                bool array, bool upstream
                                                                                ) const
{
    return new PipelineLibrary::Property(name, pxr::TfToken(widget->text().toStdString()),
                                         nullptr, upstream);
}






QWidget* PipelineSoftware2::Matrix2dWidget::CreateTypedWidget()
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

void PipelineSoftware2::Matrix2dWidget::SetValue(std::any value)
{
    pxr::GfMatrix2d mat = std::any_cast<pxr::GfMatrix2d>(value);

    widget1->setValue(mat[0][0]);
    widget2->setValue(mat[0][1]);
    widget3->setValue(mat[1][0]);
    widget4->setValue(mat[1][1]);
}

std::any PipelineSoftware2::Matrix2dWidget::GetValue() const
{
    pxr::GfMatrix2d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value());
    return std::any_cast<pxr::GfMatrix2d>(mat);
}

void PipelineSoftware2::Matrix2dWidget::ShowConnected(bool connected)
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

void PipelineSoftware2::Matrix2dWidget::ShowDisabled(bool disabled)
{
    widget1->setDisabled(disabled);
    widget2->setDisabled(disabled);
    widget3->setDisabled(disabled);
    widget4->setDisabled(disabled);
}




QWidget* PipelineSoftware2::Matrix2dInitWidget::CreateInitWidget()
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


std::any PipelineSoftware2::Matrix2dInitWidget::CreateValue()
{
    pxr::GfMatrix2d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value());

    return std::make_any<pxr::GfMatrix2d>(mat);
}


PipelineLibrary::Property* PipelineSoftware2::Matrix2dInitWidget::CreateProperty(std::string name,
                                                                                 bool array, bool upstream
) const
{
    pxr::GfMatrix2d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value());

    return new PipelineLibrary::Property(name, mat, nullptr, upstream);
}






QWidget* PipelineSoftware2::Matrix3dWidget::CreateTypedWidget()
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

void PipelineSoftware2::Matrix3dWidget::SetValue(std::any value)
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

std::any PipelineSoftware2::Matrix3dWidget::GetValue() const
{
    pxr::GfMatrix3d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value(),
                        widget5->value(), widget6->value(),
                        widget7->value(), widget8->value(),
                        widget9->value());
    return std::any_cast<pxr::GfMatrix3d>(mat);
}

void PipelineSoftware2::Matrix3dWidget::ShowConnected(bool connected)
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

void PipelineSoftware2::Matrix3dWidget::ShowDisabled(bool disabled)
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



QWidget* PipelineSoftware2::Matrix3dInitWidget::CreateInitWidget()
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


std::any PipelineSoftware2::Matrix3dInitWidget::CreateValue()
{
    pxr::GfMatrix3d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value(),
                        widget5->value(), widget6->value(),
                        widget7->value(), widget8->value(),
                        widget9->value());

    return std::make_any<pxr::GfMatrix3d>(mat);
}


PipelineLibrary::Property* PipelineSoftware2::Matrix3dInitWidget::CreateProperty(std::string name,
                                                                                 bool array, bool upstream
) const
{
    pxr::GfMatrix3d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value(),
                        widget5->value(), widget6->value(),
                        widget7->value(), widget8->value(),
                        widget9->value());

    return new PipelineLibrary::Property(name, mat, nullptr, upstream);
}






QWidget* PipelineSoftware2::Matrix4dWidget::CreateTypedWidget()
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

void PipelineSoftware2::Matrix4dWidget::SetValue(std::any value)
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

std::any PipelineSoftware2::Matrix4dWidget::GetValue() const
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

void PipelineSoftware2::Matrix4dWidget::ShowConnected(bool connected)
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

void PipelineSoftware2::Matrix4dWidget::ShowDisabled(bool disabled)
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


QWidget* PipelineSoftware2::Matrix4dInitWidget::CreateInitWidget()
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


std::any PipelineSoftware2::Matrix4dInitWidget::CreateValue()
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


PipelineLibrary::Property* PipelineSoftware2::Matrix4dInitWidget::CreateProperty(std::string name,
                                                                                 bool array, bool upstream
) const
{
    pxr::GfMatrix4d mat(widget1->value(), widget2->value(),
                        widget3->value(), widget4->value(),
                        widget5->value(), widget6->value(),
                        widget7->value(), widget8->value(),
                        widget9->value(), widget10->value(),
                        widget11->value(), widget12->value(),
                        widget13->value(), widget14->value(),
                        widget15->value(), widget16->value());

    return new PipelineLibrary::Property(name, mat, nullptr, upstream);
}


REGISTER_UI_PLUGIN(PipelineSoftware2::UiPlugin* loader)
{
    loader->RegisterUiType<pxr::GfHalf,
            PipelineSoftware2::GfHalfWidgetBuilder,
            PipelineSoftware2::GfHalfInitWidgetBuilder>("Half");

    loader->RegisterUiType<pxr::SdfTimeCode,
            PipelineSoftware2::SdfTimeCodeWidgetBuilder,
            PipelineSoftware2::SdfTimeCodeInitWidgetBuilder>("TimeCode");

    loader->RegisterUiType<pxr::TfToken,
            PipelineSoftware2::TfTokenWidgetBuilder,
            PipelineSoftware2::TfTokenInitWidgetBuilder>("Token");

    loader->RegisterUiType<pxr::GfMatrix2d,
            PipelineSoftware2::Matrix2dWidgetBuilder,
            PipelineSoftware2::Matrix2dInitWidgetBuilder>("Matrix2d");

    loader->RegisterUiType<pxr::GfMatrix3d,
            PipelineSoftware2::Matrix3dWidgetBuilder,
            PipelineSoftware2::Matrix3dInitWidgetBuilder>("Matrix3d");

    loader->RegisterUiType<pxr::GfMatrix4d,
            PipelineSoftware2::Matrix4dWidgetBuilder,
            PipelineSoftware2::Matrix4dInitWidgetBuilder>("Matrix4d");

    loader->RegisterUiType<pxr::GfQuatd,
            PipelineSoftware2::QuatdWidgetBuilder,
            PipelineSoftware2::QuatdInitWidgetBuilder>("Quatd");

    loader->RegisterUiType<pxr::GfQuatf,
            PipelineSoftware2::QuatfWidgetBuilder,
            PipelineSoftware2::QuatfInitWidgetBuilder>("Quatf");

    loader->RegisterUiType<pxr::GfQuath,
            PipelineSoftware2::QuathWidgetBuilder,
            PipelineSoftware2::QuathInitWidgetBuilder>("Quath");

    loader->RegisterUiType<pxr::GfVec2d,
            PipelineSoftware2::Vec2dWidgetBuilder,
            PipelineSoftware2::Vec2dInitWidgetBuilder>("Vec2d");

    loader->RegisterUiType<pxr::GfVec2f,
            PipelineSoftware2::QuatfWidgetBuilder,
            PipelineSoftware2::QuatfInitWidgetBuilder>("Vec2f");

    loader->RegisterUiType<pxr::GfVec2h,
            PipelineSoftware2::Vec2hWidgetBuilder,
            PipelineSoftware2::Vec2hInitWidgetBuilder>("Vec2h");

    loader->RegisterUiType<pxr::GfVec2i,
            PipelineSoftware2::Vec2iWidgetBuilder,
            PipelineSoftware2::Vec2iInitWidgetBuilder>("Vec2i");

    loader->RegisterUiType<pxr::GfVec3d,
            PipelineSoftware2::Vec3dWidgetBuilder,
            PipelineSoftware2::Vec3dInitWidgetBuilder>("Vec3d");

    loader->RegisterUiType<pxr::GfVec3f,
            PipelineSoftware2::Vec3fWidgetBuilder,
            PipelineSoftware2::Vec3fInitWidgetBuilder>("Vec3f");

    loader->RegisterUiType<pxr::GfVec3h,
            PipelineSoftware2::Vec3hWidgetBuilder,
            PipelineSoftware2::Vec3hInitWidgetBuilder>("Vec3h");

    loader->RegisterUiType<pxr::GfVec3i,
            PipelineSoftware2::Vec3iWidgetBuilder,
            PipelineSoftware2::Vec3iInitWidgetBuilder>("Vec3i");

    loader->RegisterUiType<pxr::GfVec4d,
            PipelineSoftware2::Vec4dWidgetBuilder,
            PipelineSoftware2::Vec4dInitWidgetBuilder>("Vec4d");

    loader->RegisterUiType<pxr::GfVec4f,
            PipelineSoftware2::Vec4fWidgetBuilder,
            PipelineSoftware2::Vec4fInitWidgetBuilder>("Vec4f");

    loader->RegisterUiType<pxr::GfVec4h,
            PipelineSoftware2::Vec4hWidgetBuilder,
            PipelineSoftware2::Vec4hInitWidgetBuilder>("Vec4h");

    loader->RegisterUiType<pxr::GfVec4i,
            PipelineSoftware2::Vec4iWidgetBuilder,
            PipelineSoftware2::Vec4iInitWidgetBuilder>("Vec4i");
}