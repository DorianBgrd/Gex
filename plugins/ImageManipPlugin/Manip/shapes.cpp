#include "shapes.h"

#include <QPainter>


QImage ImageManip::Manip::GenerateRectangle(
        int imageWidth,
        int imageHeight,
        int x,
        int y,
        int width,
        int height,
        const QColor& color)
{
    QImage res(imageWidth, imageHeight, QImage::Format_RGBA64);

    res.fill(QColor(0, 0, 0, 0));

    QPainter painter(&res);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRect(x, y, width, height);

    painter.end();
    return res;
}


QImage ImageManip::Manip::GenerateEllipse(
        int imageWidth,
        int imageHeight,
        int x,
        int y,
        int width,
        int height,
        const QColor& color)
{
    QImage res(imageWidth, imageHeight, QImage::Format_RGBA64);

    res.fill(QColor(0, 0, 0, 0));

    QPainter painter(&res);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(x, y, width, height);

    painter.end();
    return res;
}