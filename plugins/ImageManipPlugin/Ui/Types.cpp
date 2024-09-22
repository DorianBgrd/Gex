#include "Types.h"
#include "Types/resolution.h"

#include <QPainter>
#include <QPainterPath>

#include "UiRes/include/uires.h"


ImageManip::Ui::Types::ImageViewer::ImageViewer(
        const QImage& im, QWidget* parent):
        QWidget(parent), img(im)
{
    ratio = 1.f;
    if (!img.isNull())
    {
        ratio = (float)img.width() / (float)img.height();
    }
}


void ImageManip::Ui::Types::ImageViewer::SetImage(
        const QImage& image)
{
    img = image;
    update();
}


QImage ImageManip::Ui::Types::ImageViewer::Image() const
{
    return img;
}


QSize ImageManip::Ui::Types::ImageViewer::minimumSizeHint() const
{
    return QSize(100, ratio * 100);
}


void ImageManip::Ui::Types::ImageViewer::paintEvent(
        QPaintEvent *event)
{
    QRect r = rect();

    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    if (img.isNull())
    {
        painter.setBrush(QColor(0, 0, 0));
        painter.drawRect(rect());
    }

    QPixmap pixmap = QPixmap::fromImage(img).scaled(
            r.width(), (int)(ratio * (float)r.height()),
            Qt::KeepAspectRatio);

    int x = (r.width() - pixmap.width()) / 2;
    int y = (r.height() - pixmap.height()) / 2;

    painter.drawPixmap(x, y, pixmap);
}



QWidget* ImageManip::Ui::Types::ImageWidget::CreateTypedWidget()
{
    viewer = new ImageViewer(QImage(), this);
    return viewer;
}


void ImageManip::Ui::Types::ImageWidget::SetValue(std::any value)
{
    if (!viewer)
    {
        return;
    }

    viewer->SetImage(std::any_cast<QImage>(value));
}


std::any ImageManip::Ui::Types::ImageWidget::GetValue() const
{
    if (!viewer)
        return {};

    return std::make_any<QImage>(viewer->Image());
}


void ImageManip::Ui::Types::ImageWidget::ShowConnected(bool connected)
{

}


void ImageManip::Ui::Types::ImageWidget::ShowDisabled(bool disabled)
{

}


ImageManip::Ui::Types::ColorButton::ColorButton(
        QColor color, QWidget *parent):
        c(color), QPushButton(parent)
{
    dialog = new QColorDialog(this);
    dialog->hide();

    QObject::connect(this, &QPushButton::clicked,
                     this, &ColorButton::ChooseColor);
}


void ImageManip::Ui::Types::ColorButton::ChooseColor()
{
    if (!dialog->exec())
    {
        return;
    }

    SetColor(dialog->currentColor());
}


QColor ImageManip::Ui::Types::ColorButton::Color() const
{
    return c;
}


void ImageManip::Ui::Types::ColorButton::SetColor(QColor color)
{
    c = color;
    Q_EMIT ColorChanged(c);

    update();
}


void ImageManip::Ui::Types::ColorButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setBrush(c);

    painter.setPen(Qt::NoPen);

    painter.drawRoundedRect(rect(), 5, 5);
}


void ImageManip::Ui::Types::ColorWidget::OnColorChanged(QColor color)
{
    SetValue(std::make_any<QColor>(color));
}


QWidget* ImageManip::Ui::Types::ColorWidget::CreateTypedWidget()
{
    button = new ColorButton(QColor(), this);
    QObject::connect(button, &ColorButton::ColorChanged,
                     this, &ColorWidget::OnColorChanged);
    Connect(button, &ColorButton::ColorChanged);
    return button;
}


void ImageManip::Ui::Types::ColorWidget::SetValue(std::any value)
{
    button->blockSignals(true);
    button->SetColor(std::any_cast<QColor>(value));
    button->blockSignals(false);
}


std::any ImageManip::Ui::Types::ColorWidget::GetValue() const
{
    return std::make_any<QColor>(button->Color());
}


void ImageManip::Ui::Types::ColorWidget::ShowConnected(bool connected)
{

}


void ImageManip::Ui::Types::ColorWidget::ShowDisabled(bool disabled)
{

}



QWidget* ImageManip::Ui::Types::ResolutionWidget::CreateTypedWidget()
{
    auto* widget = new QWidget(this);
    auto* hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(hlayout);

    xsb = new QSpinBox(this);
    xsb->setMinimum(1);
    xsb->setMaximum(99999);
    xsb->setButtonSymbols(QSpinBox::NoButtons);
    hlayout->addWidget(xsb);

    lbtn = new QPushButton(this);
    lbtn->setFixedSize(20, 20);
    lbtn->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(
            fa::fa_solid, fa::fa_link));
    lbtn->setCheckable(true);
    lbtn->setChecked(true);
    hlayout->addWidget(lbtn);

    ysb = new QSpinBox(this);
    ysb->setMinimum(1);
    ysb->setMaximum(99999);
    ysb->setButtonSymbols(QSpinBox::NoButtons);
    hlayout->addWidget(ysb);

    QObject::connect(xsb, &QSpinBox::valueChanged, this,
                     &ResolutionWidget::SpinBoxValueChangedX);
    QObject::connect(ysb, &QSpinBox::valueChanged, this,
                     &ResolutionWidget::SpinBoxValueChangedY);

    return widget;
}


void ImageManip::Ui::Types::ResolutionWidget::SpinBoxValueChangedX(int v)
{
    if (lbtn->isChecked())
    {
        ysb->blockSignals(true);
        ysb->setValue(v);
        ysb->blockSignals(false);
    }

    OnValueChanged();
}


void ImageManip::Ui::Types::ResolutionWidget::SpinBoxValueChangedY(int v)
{
    if (lbtn->isChecked())
    {
        xsb->blockSignals(true);
        xsb->setValue(v);
        xsb->blockSignals(false);
    }

    OnValueChanged();
}


void ImageManip::Ui::Types::ResolutionWidget::SetValue(std::any value)
{
    auto res = std::any_cast<ImageManip::Types::Resolution>(value);

    xsb->blockSignals(true);
    ysb->blockSignals(true);
    xsb->setValue(res.at(0));
    ysb->setValue(res.at(1));
    xsb->blockSignals(false);
    ysb->blockSignals(false);
}


std::any ImageManip::Ui::Types::ResolutionWidget::GetValue() const
{
    ImageManip::Types::Resolution res = {xsb->value(), ysb->value()};
    return std::make_any<ImageManip::Types::Resolution>(res);
}


void ImageManip::Ui::Types::ResolutionWidget::ShowConnected(bool connected)
{
    xsb->setDisabled(connected);
    ysb->setDisabled(connected);
}


void ImageManip::Ui::Types::ResolutionWidget::ShowDisabled(bool disabled)
{
    xsb->setDisabled(disabled);
    ysb->setDisabled(disabled);
}


ImageManip::Ui::Types::LinearGradientEditor::LinearGradientEditor(
        QWidget* parent): QWidget(parent)
{
    setMouseTracking(true);
}


void ImageManip::Ui::Types::LinearGradientEditor::SetGradient(
        const ImageManip::Types::LinearGradient& gradient)
{
    g = gradient;

    update();
}


QSize ImageManip::Ui::Types::LinearGradientEditor::minimumSizeHint() const
{
    return {50, handleHeight * 2};
}


void ImageManip::Ui::Types::LinearGradientEditor::enterEvent(QEnterEvent *event)
{
    contains = true;
    pos = event->position();

    update();
}


void ImageManip::Ui::Types::LinearGradientEditor::leaveEvent(QEvent *event)
{
    contains = false;

    update();
}


void ImageManip::Ui::Types::LinearGradientEditor::moveEvent(QMoveEvent *event)
{
    pos = event->pos();

    update();
}

#include <iostream>

void ImageManip::Ui::Types::LinearGradientEditor::mouseMoveEvent(QMouseEvent *event)
{
    std::cout << "Clicked " << clicked << std::endl;
    if (clicked)
    {

        float distance = static_cast<double>(pos.x() - event->pos().x()) / static_cast<double>(rect().width());

        std::cout << "Distance : " << distance << std::endl;
        std::cout << "clickedGradientStep : " << clickedGradientStep << std::endl;
        g.MoveStep(clickedGradientStep, clickedGradientStep - distance);

        Q_EMIT StepChanged(clickedGradientStep, clickedGradientStep - distance);

        Q_EMIT GradientChanged();

        clickedGradientStep = clickedGradientStep - distance;
    }

    pos = event->pos();

    update();
}


void ImageManip::Ui::Types::LinearGradientEditor::mousePressEvent(QMouseEvent *event)
{
    for (auto gs : stepHandles)
    {
        if (gs.rect.contains(event->position().toPoint()))
        {
            clicked = true;
            clickedGradientStep = gs.gradient;

            Q_EMIT StepSelected(clickedGradientStep, g.ColorAtStep(clickedGradientStep));
            return;
        }
    }

    clicked = false;
}


void ImageManip::Ui::Types::LinearGradientEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if (!clicked && gradientRect.contains(event->position().toPoint()))
    {
        double gpos = static_cast<double>(event->pos().x() - gradientRect.x()) / static_cast<double>(gradientRect.width());

        g.InsertStep(gpos);

        Q_EMIT GradientChanged();
    }

    clicked = false;

    update();
}


double ImageManip::Ui::Types::LinearGradientEditor::SelectedStep() const
{
    return clickedGradientStep;
}


void ImageManip::Ui::Types::LinearGradientEditor::OnStepModified(
        double src, double dst)
{
    g.MoveStep(src, dst);
    Q_EMIT GradientChanged();
    update();
}


void ImageManip::Ui::Types::LinearGradientEditor::OnColorModified(
        double step, QColor color)
{
    g.EditStep(step, color);
    Q_EMIT GradientChanged();
    update();
}


void ImageManip::Ui::Types::LinearGradientEditor::paintEvent(QPaintEvent *event)
{
    QLinearGradient lgradient(0, 0, 1, 0);
    lgradient.setCoordinateMode(QLinearGradient::ObjectMode);

    auto steps = g.StepColors();

    QGradientStops stops;
    for (auto p : steps)
    {
        stops.append({p.first, p.second});
    }

    lgradient.setStops(stops);

    QPainter painter(this);

    QRect r = rect();

    painter.setBrush(lgradient);
    painter.setPen(Qt::NoPen);

    int gradientWidth = r.width() - handleWidth;
    int gradientHeight = r.height() - handleHeight;
    gradientRect = QRect(handleWidth / 2, 0,
                         gradientWidth,
                         gradientHeight);

    painter.drawRect(gradientRect);

    QColor defaultColor(70, 70, 70);
    QPen defaultPen(defaultColor);
    defaultPen.setWidth(1);

    QPen highlightPen(QColor(255, 255, 255));
    highlightPen.setWidth(1);

    QPen selectedPen(QColor(255, 255, 255));
    selectedPen.setWidth(2);

    stepHandles.clear();

    painter.save();
    painter.setPen(defaultPen);
    for (auto p : steps)
    {
        int x = gradientRect.left() + p.first * gradientRect.width();

        QPainterPath path;
        QPoint p0(x, gradientHeight);
        path.moveTo(p0);

        QPoint p1(x - handleWidth / 2, gradientHeight + 4);
        path.lineTo(p1);

        path.moveTo(p1);
        QPoint p2(x - handleWidth / 2, gradientHeight + handleHeight);
        path.lineTo(p2);

        path.moveTo(p2);
        QPoint p3(x + handleWidth / 2, gradientHeight + handleHeight);
        path.lineTo(p3);

        path.moveTo(p3);
        QPoint p4(x + handleWidth / 2, gradientHeight + 4);
        path.lineTo(p4);

        path.moveTo(p4);
        path.lineTo(p0);

        QRect selRect(p2.x(), p2.y() - (handleHeight / 4),
                      handleWidth, (handleHeight / 4));

        QRect rc(p1, p3);

        QBrush selRectBrush(defaultColor);
        if (p.first == clickedGradientStep)
        {
            painter.setPen(selectedPen);

            selRectBrush.setColor(Qt::white);
        }
        else if (contains && rc.contains(pos.toPoint()))
        {
            painter.setPen(highlightPen);
        }
        else
        {
            painter.setPen(defaultPen);
        }

        painter.setBrush(QBrush(p.second));
        painter.drawPath(path);

        painter.setBrush(selRectBrush);
        painter.drawRect(selRect);

        stepHandles.append({rc, p.first});
    }

    painter.restore();
}


ImageManip::Ui::Types::GradientStepEditor::GradientStepEditor(
        QWidget* parent): QWidget(parent)
{
    auto* hlayout = new QHBoxLayout();
    setLayout(hlayout);

    auto* stepName = new QLabel(this);
    stepName->setText("Step : ");
    hlayout->addWidget(stepName);

    spinbox = new QDoubleSpinBox(this);
    spinbox->setButtonSymbols(QDoubleSpinBox::NoButtons);
    spinbox->setMinimum(0);
    spinbox->setMaximum(1);
    spinbox->setDecimals(6);
    spinbox->setSingleStep(0.001);
    hlayout->addWidget(spinbox);

    button = new ColorButton(Qt::white, this);
    hlayout->addWidget(button);

    QObject::connect(spinbox, &QDoubleSpinBox::valueChanged,
                     this, &GradientStepEditor::OnSpinboxValueChanged);

    QObject::connect(button, &ColorButton::ColorChanged,
                     this, &GradientStepEditor::OnColorChanged);
}


void ImageManip::Ui::Types::GradientStepEditor::SetCurrentStep(
        const double& s, const QColor& c)
{
    step = s;
    color = c;

    spinbox->blockSignals(true);
    spinbox->setValue(step);
    spinbox->blockSignals(false);

    button->blockSignals(true);
    button->SetColor(color);
    button->blockSignals(false);
}


void ImageManip::Ui::Types::GradientStepEditor::OnGradientStepMoved(
        const double& src, const double& dst)
{
    if (src != step)
        return;

    spinbox->blockSignals(true);
    spinbox->setValue(dst);
    spinbox->blockSignals(false);
}


void ImageManip::Ui::Types::GradientStepEditor::OnSpinboxValueChanged(
        double value)
{
    Q_EMIT StepModified(step, value);
}


void ImageManip::Ui::Types::GradientStepEditor::OnColorChanged(
        QColor color)
{
    Q_EMIT ColorModified(step, color);
}


ImageManip::Ui::Types::LinearGradient::LinearGradient(
        QWidget* parent): QWidget(parent)
{
    auto* layout = new QVBoxLayout();
    setLayout(layout);

    editor = new LinearGradientEditor(this);
    layout->addWidget(editor);

    stepEditor = new GradientStepEditor(this);
    layout->addWidget(stepEditor);

    QObject::connect(editor, &LinearGradientEditor::StepSelected,
                     stepEditor, &GradientStepEditor::SetCurrentStep);

    QObject::connect(editor, &LinearGradientEditor::StepChanged,
                     stepEditor, &GradientStepEditor::SetCurrentStep);

    QObject::connect(stepEditor, &GradientStepEditor::StepModified,
                     editor, &LinearGradientEditor::OnStepModified);

    QObject::connect(stepEditor, &GradientStepEditor::ColorModified,
                     editor, &LinearGradientEditor::OnColorModified);

    QObject::connect(editor, &LinearGradientEditor::GradientChanged,
                     this, &LinearGradient::OnStepChanged);
}


void ImageManip::Ui::Types::LinearGradient::OnStepChanged()
{
    Q_EMIT GradientChanged();
}


QWidget* ImageManip::Ui::Types::LinearGradientWidget::CreateTypedWidget()
{
    w = new LinearGradient(this);

    Connect(w, &LinearGradient::GradientChanged);
    return w;
}


void ImageManip::Ui::Types::LinearGradientWidget::SetValue(
        std::any value)
{
    auto gradient = std::any_cast<ImageManip::Types::LinearGradient>(value);

    w->SetGradient(gradient);
}


std::any ImageManip::Ui::Types::LinearGradientWidget::GetValue() const
{
    return std::make_any<ImageManip::Types::LinearGradient>(w->Gradient());
}


void ImageManip::Ui::Types::LinearGradientWidget::ShowConnected(bool connected)
{

}


void ImageManip::Ui::Types::LinearGradientWidget::ShowDisabled(bool disabled)
{

}

