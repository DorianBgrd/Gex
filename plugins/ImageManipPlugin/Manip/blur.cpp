#include "blur.h"

#include <QImage>
#include <set>


struct Point
{
    int x;
    int y;

    Point(int x_, int y_): x(x_), y(y_){}

    bool operator==(const Point& other) const
    {
        return (other.x == x && other.y == y);
    }

    bool operator < (const Point& other) const
    {
        if (y == other.y)
            return (x < other.x);

        return (y < other.y);
    }
};



void GrowSelection(std::set<Point> &points)
{
    std::set<Point> merge;
    for (auto p : points)
    {
        std::set<Point> adjacent = {
                Point(p.x - 1, p.y - 1),
                Point(p.x, p.y - 1),
                Point(p.x + 1, p.y - 1),
                Point(p.x - 1, p.y),
                Point(p.x, p.y),
                Point(p.x + 1, p.y),
                Point(p.x - 1, p.y + 1),
                Point(p.x, p.y + 1),
                Point(p.x + 1, p.y + 1)
        };

        merge.merge(adjacent);
    }

    points.merge(merge);
}


QColor Average(std::vector<QColor> colors)
{
    size_t sumR = 0;
    size_t sumG = 0;
    size_t sumB = 0;
    size_t sumA = 0;
    for (auto c : colors)
    {
        sumR += c.red();
        sumG += c.green();
        sumB += c.blue();
        sumA += c.alpha();
    }

    size_t size = colors.size();
    return QColor(sumR / size, sumG / size,
                  sumB / size, sumA / size);
}


QImage ImageManip::Manip::BoxBlurPixmap(const QImage& image, int steps)
{
    int width = image.width();
    int height = image.height();

    QImage blurred(width, height, image.format());
    std::map<int, const QRgba64*> scanLines;

    for (int y = 0; y < height; y++)
    {
        auto* editScanLine = reinterpret_cast<QRgba64*>(blurred.scanLine(y));

        for (int x = 0; x < width; x++)
        {
            std::set<Point> points = {Point(x, y)};
            for (int s = 0; s < steps; s++)
            {
                GrowSelection(points);
            }

            std::vector<QColor> colors = {reinterpret_cast<const QRgba64*>(
                    image.constScanLine(y))[x]};
            for (auto p : points)
            {
                if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height)
                    continue;

                auto msl = scanLines.find(p.y);
                const QRgba64* line;

                if (msl == scanLines.end())
                {
                    const auto* ssl = reinterpret_cast<const QRgba64*>(
                            image.constScanLine(p.y));
                    scanLines[p.y] = ssl;
                    line = ssl;
                }
                else
                {
                    line = msl->second;
                }

                colors.emplace_back(line[p.x]);
            }

            editScanLine[x] = Average(colors).rgba64();
        }
    }

    return blurred;
}