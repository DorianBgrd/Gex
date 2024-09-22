#ifndef GEX_UI_TYPES_H
#define GEX_UI_TYPES_H

#include "UiTsys/include/uitsys.h"

#include "../export.h"

#include <QSpinBox>
#include <QColorDialog>
#include <QPushButton>
#include <QEnterEvent>
#include <map>

#include "Types/gradient.h"

namespace ImageManip::Ui::Types
{
    class Plugin_API ImageViewer: public QWidget
    {
        QImage img;
        float ratio = 1.f;
    public:
        ImageViewer(const QImage& img=QImage(),
                    QWidget* parent=nullptr);

        void SetImage(const QImage& pixmap);

        QImage Image() const;

        void paintEvent(QPaintEvent *event) override;

        QSize minimumSizeHint() const override;
    };


    class Plugin_API ImageWidget: public UiTSys::TypedWidget
    {
        ImageViewer* viewer = nullptr;

    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    GENERATE_DEFAULT_WIDGET_CREATOR(
            ImageWidgetBuilder,
            ImageWidget)


    class ColorButton: public QPushButton
    {
        Q_OBJECT

        QColor c;
        QColorDialog* dialog = nullptr;
    public:
        ColorButton(QColor color=QColor(),
                    QWidget* parent=nullptr);

        void ChooseColor();

        QColor Color() const;

        void SetColor(QColor color);

        Q_SIGNAL void ColorChanged(QColor);

        void paintEvent(QPaintEvent *) override;
    };


    class Plugin_API ColorWidget: public UiTSys::TypedWidget
    {
        ColorButton* button = nullptr;

    public:
        void OnColorChanged(QColor color);

        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    GENERATE_DEFAULT_WIDGET_CREATOR(
            ColorWidgetBuilder,
            ColorWidget)


    class Plugin_API ResolutionWidget: public UiTSys::TypedWidget
    {
        QSpinBox* xsb = nullptr;
        QSpinBox* ysb = nullptr;
        QPushButton* lbtn = nullptr;
    public:
        void SpinBoxValueChangedX(int v);

        void SpinBoxValueChangedY(int v);

        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    GENERATE_DEFAULT_WIDGET_CREATOR(
            ResolutionWidgetBuilder,
            ResolutionWidget)


    class Plugin_API LinearGradientEditor: public QWidget
    {
        Q_OBJECT

        ImageManip::Types::LinearGradient g;
        int handleHeight = 15;
        int handleWidth = 10;

        QRect gradientRect;
        QPointF pos;
        bool contains = false;

        bool clicked = false;
        double clickedGradientStep = -1;

        struct StepHandle
        {

            QRect rect;
            double gradient;

            StepHandle() = default;

            StepHandle(QRect r, double g)
            {
                rect = r;
                gradient = g;
            }
        };

        QVector<StepHandle> stepHandles;
    public:
        LinearGradientEditor(QWidget* parent=nullptr);

        void SetGradient(const ImageManip::Types::LinearGradient& gradient);

        ImageManip::Types::LinearGradient Gradient() const { return g;}

        void paintEvent(QPaintEvent *event) override;

        QSize minimumSizeHint() const override;

        void enterEvent(QEnterEvent *event) override;

        void leaveEvent(QEvent *event) override;

        void moveEvent(QMoveEvent *event) override;

        void mousePressEvent(QMouseEvent *event) override;

        void mouseMoveEvent(QMouseEvent *event) override;

        void mouseReleaseEvent(QMouseEvent *event) override;

        double SelectedStep() const;

        Q_SIGNAL void StepSelected(const double& step, const QColor& color);

        Q_SIGNAL void StepChanged(const double& source, const double& dest);

        Q_SIGNAL void GradientChanged();

        void OnStepModified(double src, double dst);

        void OnColorModified(double step, QColor color);
    };


    class Plugin_API GradientStepEditor: public QWidget
    {
        Q_OBJECT

        double step;
        QColor color;

        ColorButton* button;
        QDoubleSpinBox* spinbox;
    public:
        GradientStepEditor(QWidget* parent=nullptr);

        void SetCurrentStep(const double& step, const QColor& color);

        double CurrentStep() const { return step; }

        Q_SIGNAL void StepModified(double src, double dst);

        Q_SIGNAL void ColorModified(double step, QColor color);

        void OnGradientStepMoved(const double& src,
                                 const double& dst);

    private:
        void OnSpinboxValueChanged(double value);

        void OnColorChanged(QColor color);
    };


    class Plugin_API LinearGradient: public QWidget
    {
        Q_OBJECT

        LinearGradientEditor* editor;
        GradientStepEditor* stepEditor;
    public:
        LinearGradient(QWidget* parent=nullptr);

        ImageManip::Types::LinearGradient Gradient() const
        {
            return editor->Gradient();
        }

        void SetGradient(ImageManip::Types::LinearGradient g)
        {
            editor->SetGradient(g);
        }

        Q_SIGNAL void GradientChanged();

        void OnStepChanged();
    };


    class Plugin_API LinearGradientWidget: public UiTSys::TypedWidget
    {
        LinearGradient* w;
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    GENERATE_DEFAULT_WIDGET_CREATOR(
            LinearGradientWidgetBuilder,
            LinearGradientWidget)
}

#endif //GEX_UI_TYPES_H
