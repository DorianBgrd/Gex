#ifndef GEX_TIMELINE_H
#define GEX_TIMELINE_H

#include <QWidget>
#include <QTimeLine>
#include <QDockWidget>


namespace Gex::Ui
{
    class Timeline: public QWidget
    {
        double minTime = 0;
        double maxTime = 100;
        double currentTime = 0;

    public:
        Timeline(QWidget* parent=nullptr);

        double CurrentTime() const;

        void SetCurrentTime(double time);

        double MaxTime() const;

        void SetMaxTime(double max);

        double MinTime() const;

        void SetMinTime(double min);

        void paintEvent(QPaintEvent *event) override;

        void mousePressEvent(QMouseEvent *event) override;

        void mouseMoveEvent(QMouseEvent *event) override;

        void mouseReleaseEvent(QMouseEvent *event) override;
    };


    class TimelineDock: public QDockWidget
    {
    public:
        TimelineDock(QWidget* parent=nullptr);
    };
}

#endif //GEX_TIMELINE_H
