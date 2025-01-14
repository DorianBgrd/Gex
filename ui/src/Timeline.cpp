#include "ui/include/Timeline.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QTimerEvent>
#include <QApplication>

#include "UiRes/include/uires.h"


Gex::Ui::Timeline::Timeline(QWidget* parent):
        QWidget(parent)
{
    setObjectName("TimelineWidget");

    interval = 1000.0 / 24.0;
    profiler = Gex::MakeProfiler();

    timer = new QTimer(this);
    timer->setInterval(interval);

    auto *res = Res::UiRes::GetRes();

    auto* layout = new QHBoxLayout();
    setLayout(layout);

    minBox = new QDoubleSpinBox(this);
    minBox->setObjectName("TimelineSpinBox");
    minBox->setMinimum(INT_MIN);
    minBox->setMaximum(INT_MAX);
    minBox->setButtonSymbols(QSpinBox::NoButtons);
    layout->addWidget(minBox);

    slider = new QSlider(this);
    slider->setObjectName("TimelineSlider");
    slider->setOrientation(Qt::Horizontal);
    slider->setSingleStep(1);
    layout->addWidget(slider);

    timeBox = new QDoubleSpinBox(this);
    timeBox->setObjectName("TimelineSpinBox");
    timeBox->setMinimum(INT_MIN);
    timeBox->setMaximum(INT_MAX);
    timeBox->setButtonSymbols(QSpinBox::NoButtons);
    layout->addWidget(timeBox);

    maxBox = new QDoubleSpinBox(this);
    maxBox->setObjectName("TimelineSpinBox");
    maxBox->setMinimum(INT_MIN);
    maxBox->setMaximum(INT_MAX);
    maxBox->setButtonSymbols(QSpinBox::NoButtons);
    layout->addWidget(maxBox);

    previousButton = new QPushButton(this);
    previousButton->setIcon(res->GetQtAwesome()->icon(
            fa::fa_solid, fa::fa_backward_fast));
    layout->addWidget(previousButton);

    playButton = new QPushButton(this);
    playButton->setIcon(res->GetQtAwesome()->icon(
            fa::fa_solid, fa::fa_play));
    layout->addWidget(playButton);

    stopButton = new QPushButton(this);
    stopButton->setIcon(res->GetQtAwesome()->icon(
            fa::fa_solid, fa::fa_stop));
    layout->addWidget(stopButton);

    nextButton = new QPushButton(this);
    nextButton->setIcon(res->GetQtAwesome()->icon(
            fa::fa_solid, fa::fa_forward_fast));
    layout->addWidget(nextButton);

    timeAttribute = new QComboBox(this);
    layout->addWidget(timeAttribute);

    layout->setStretchFactor(minBox, 0);
    layout->setStretchFactor(slider, 1);
    layout->setStretchFactor(timeBox, 0);
    layout->setStretchFactor(maxBox, 0);
    layout->setStretchFactor(previousButton, 0);
    layout->setStretchFactor(playButton, 0);
    layout->setStretchFactor(stopButton, 0);
    layout->setStretchFactor(nextButton, 0);
    layout->setStretchFactor(timeAttribute, 0);

    minBox->setValue(minTime);
    maxBox->setValue(maxTime);
    slider->setMinimum(minTime * std::pow(10, decimals));
    slider->setMaximum(maxTime * std::pow(10, decimals));

    QObject::connect(timeBox, &QDoubleSpinBox::valueChanged,
                     this, &Timeline::SpinboxToSlider);
    QObject::connect(slider, &QSlider::valueChanged,
                     this, &Timeline::SliderToSpinbox);
    QObject::connect(playButton, &QPushButton::clicked,
                     this, &Timeline::Play);
    QObject::connect(stopButton, &QPushButton::clicked,
                     this, &Timeline::Stop);
    QObject::connect(timeAttribute, &QComboBox::currentTextChanged,
                     this, &Timeline::SetCurrentAttr);
}


void Gex::Ui::Timeline::RefreshAvailableAttributes()
{
    QString currentAttribute = timeAttribute->currentText();

    attributes.clear();
    for (const Gex::AttributeWkPtr& attr : currentNode->GetAttributes())
    {
        if (attr->IsMulti() || attr->IsHolder())
        {
            continue;
        }

        if (attr->TypeHash() == typeid(double).hash_code())
        {
            attributes.push_back(attr->Name());
        }
    }

    timeAttribute->clear();
    timeAttribute->addItem("None");
    for (const std::string& at : attributes)
    {
        timeAttribute->addItem(at.c_str());
    }

    timeAttribute->setCurrentText(currentAttribute);
}


void Gex::Ui::Timeline::OnAttributeSelected(const QString& at)
{
    SetCurrentAttr(at);
}


void Gex::Ui::Timeline::SliderToSpinbox(int value)
{
    double time = value / std::pow(10, decimals);
    _SetCurrentTime(time);

    timeBox->blockSignals(true);
    timeBox->setValue(time);
    timeBox->blockSignals(false);
}


void Gex::Ui::Timeline::SpinboxToSlider(double value)
{
    _SetCurrentTime(value);

    slider->blockSignals(true);
    slider->setValue(value * std::pow(10, decimals));
    slider->blockSignals(false);
}


Gex::NodeWkPtr Gex::Ui::Timeline::CurrentNode() const
{
    return currentNode;
}


void Gex::Ui::Timeline::SetCurrentNode(
        const Gex::NodeWkPtr& node)
{
    if (currentNode)
    {
        DisconnectFromNode(currentNode);
    }

    currentNode = node;

    RefreshAvailableAttributes();

    ConnectToNode(node);
}


void Gex::Ui::Timeline::ConnectToNode(
        const Gex::NodeWkPtr& node)
{
    nodeCbId = node->RegisterNodeChangedCallback(
            [this](const Gex::NodeChange& change,
                    const Gex::NodeWkPtr& node)
            {
                if (change == Gex::NodeChange::AttributeAdded ||
                    change == Gex::NodeChange::AttributeRemoved)
                {
                    RefreshAvailableAttributes();
                }
            }
    );
}


void Gex::Ui::Timeline::DisconnectFromNode(
        const Gex::NodeWkPtr& node)
{
    node->DeregisterNodeChangedCallback(nodeCbId);
}


void Gex::Ui::Timeline::SetCurrentAttr(
        const QString& attribute)
{
    if (!currentNode)
        return;

    auto attr = currentNode->GetAttribute(
            attribute.toStdString());
    if (!attr)
        return;

    currentAttr = attr;
    currentAttr->Set(currentTime);
}


double Gex::Ui::Timeline::CurrentTime() const
{
    return currentTime;
}


void Gex::Ui::Timeline::_SetCurrentTime(double time)
{
    currentTime = time;

    if (currentAttr)
    {
        currentAttr->Set(time);

        if (currentNode)
        {
//            currentNode->Run();
        }
    }
}


void Gex::Ui::Timeline::SetCurrentTime(double time)
{
    _SetCurrentTime(time);

    timeBox->blockSignals(true);
    timeBox->setValue(time);
    timeBox->blockSignals(false);

    slider->blockSignals(true);
    slider->setValue(time * std::pow(10, decimals));
    slider->blockSignals(false);
}


double Gex::Ui::Timeline::MaxTime() const
{
    return maxTime;
}


void Gex::Ui::Timeline::SetMaxTime(double max)
{
    maxTime = max;
}


double Gex::Ui::Timeline::MinTime() const
{
    return minTime;
}


void Gex::Ui::Timeline::SetMinTime(double min)
{
    minTime = min;
}


void Gex::Ui::Timeline::Play()
{
    currentTimer = startTimer(interval);
}


void Gex::Ui::Timeline::Stop()
{
    killTimer(currentTimer);
}


void Gex::Ui::Timeline::Loop()
{
    double newTime = CurrentTime() + 1.0;
    if (newTime > maxTime)
    {
        newTime = minTime;
    }

    SetCurrentTime(newTime);

    if (currentNode && currentAttr)
    {
        currentNode->Run(profiler, 1);
    }

    QApplication::processEvents();
}


void Gex::Ui::Timeline::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == currentTimer)
    {
        Loop();
    }
}


Gex::Ui::TimelineDock::TimelineDock(
        QWidget* parent):
        QDockWidget(parent)
{
    setWindowTitle("Timeline");
    setObjectName("TimelineDock");

    timeline = new Timeline(this);
    setWidget(timeline);
}


Gex::NodeWkPtr Gex::Ui::TimelineDock::CurrentNode() const
{
    return timeline->CurrentNode();
}


void Gex::Ui::TimelineDock::SetCurrentNode(
        const Gex::NodeWkPtr& node)
{
    timeline->SetCurrentNode(node);
}
