#include "shapes.h"

#include <QPainter>


QImage ImageManip::Manip::GenerateRectangle(
        int imageWidth,
        int imageHeight,
        int x, int y,
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
        int x, int y,
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


QImage ImageManip::Manip::GeneratePolygon(
        int imageWidth, int imageHeight,
        const std::vector<Types::Point>& points,
        const QColor& color, const QImage& source)
{
    QImage result(imageWidth, imageHeight, QImage::Format_RGBA64);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!source.isNull())
    {
        painter.drawImage(QPoint(0, 0), source);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(color);

    QPolygon poly;
    for (const auto& point : points)
    {
        poly.push_back(point.ToQPoint());
    }

    painter.drawPolygon(poly);
    painter.end();

    return result;
}
