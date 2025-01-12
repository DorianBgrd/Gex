#include "ui/include/Timeline.h"

#include <QPainter>


Gex::Ui::Timeline::Timeline(QWidget* parent):
        QWidget(parent)
{

}


double Gex::Ui::Timeline::CurrentTime() const
{
    return currentTime;
}


void Gex::Ui::Timeline::SetCurrentTime(double time)
{
    currentTime = time;
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


void Gex::Ui::Timeline::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.fillRect(rect(), Qt::black);

    double interval = rect().width() / (MaxTime() - MinTime());

    int maxDraws = 100;

    double draws = std::ceil(MaxTime() - MinTime());
    while (draws > maxDraws)
    {
        draws /= 10;
        interval *= 2;
    }

    for (unsigned int i = 0; i < draws; i++)
    {

    }

    painter.end();
}


void Gex::Ui::Timeline::mousePressEvent(QMouseEvent *event)
{

}


void Gex::Ui::Timeline::mouseMoveEvent(QMouseEvent *event)
{

}


void Gex::Ui::Timeline::mouseReleaseEvent(QMouseEvent *event)
{

}


Gex::Ui::TimelineDock::TimelineDock(
        QWidget* parent):
        QDockWidget(parent)
{
//    setWidget(new QTimeLine(this));
}
