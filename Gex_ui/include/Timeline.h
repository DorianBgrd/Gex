#ifndef GEX_TIMELINE_H
#define GEX_TIMELINE_H

#include "Gex/include/Node.h"

#include <QWidget>
#include <QTimeLine>
#include <QDockWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>

#include "api.h"


namespace Gex::Ui
{
    class GEX_UI_API Timeline: public QWidget
    {
        double minTime = 0;
        double maxTime = 100;
        double currentTime = 0;

        int decimals = 3;

        QSlider* slider;
        QDoubleSpinBox* minBox;
        QDoubleSpinBox* timeBox;
        QDoubleSpinBox* maxBox;
        QComboBox* timeAttribute;
        QPushButton* playButton;
        QPushButton* stopButton;
        QPushButton* previousButton;
        QPushButton* nextButton;

        Gex::NodeWkPtr currentNode;
        Gex::AttributeWkPtr currentAttr;
        std::vector<std::string> attributes;

        double interval;
        QTimer* timer;
        int currentTimer;

        Gex::CallbackId nodeCbId;

        Gex::Profiler profiler;

    public:
        Timeline(QWidget* parent=nullptr);

        void RefreshAvailableAttributes();

        void OnAttributeSelected(const QString& at);

    protected:
        void SliderToSpinbox(int value);

        void SpinboxToSlider(double value);

    public:
        Gex::NodeWkPtr CurrentNode() const;

        void SetCurrentNode(const Gex::NodeWkPtr& node);

    protected:
        void ConnectToNode(const Gex::NodeWkPtr& node);

        void DisconnectFromNode(const Gex::NodeWkPtr& node);

    public:
        void SetCurrentAttr(const QString& attribute);

        double CurrentTime() const;

    protected:
        void _SetCurrentTime(double time);

    public:
        void SetCurrentTime(double time);

        double MaxTime() const;

        void SetMaxTime(double max);

        double MinTime() const;

        void SetMinTime(double min);

        void Play();

        void Stop();

        void Loop();

        void timerEvent(QTimerEvent *event) override;
    };


    class GEX_UI_API TimelineDock: public QDockWidget
    {
        Timeline* timeline;
    public:
        TimelineDock(QWidget* parent=nullptr);

        Gex::NodeWkPtr CurrentNode() const;

        void SetCurrentNode(const Gex::NodeWkPtr& node);
    };
}

#endif //GEX_TIMELINE_H
