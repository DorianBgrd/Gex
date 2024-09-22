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
        return ((x + y) < (other.x + other.y));
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


QImage ImageManip::Manip::BoxBlurPixmap(const QImage& image, int steps)
{
    int width = image.width();
    int height = image.height();

    QImage blurred(width, height, image.format());
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            std::set<Point> points = {
                    Point(x, y)
            };

            for (int s = 0; s < steps; s++)
            {
                GrowSelection(points);
            }

            int sumR = 0;
            int sumG = 0;
            int sumB = 0;
            int sumA = 0;
            int n = 0;

            for (const Point& p : points)
            {
                if (p.x < 0 || p.x >= width)
                {
                    continue;
                }
                if (p.y < 0 || p.y >= height)
                {
                    continue;
                }

                QColor c = image.pixelColor(p.x, p.y);

                sumR += c.red();
                sumG += c.green();
                sumB += c.blue();
                sumA += c.alpha();
                n++;
            }

            QColor newColor(
                    sumR / n, sumG / n,
                    sumB / n, sumA / n
                    );

            blurred.setPixelColor(
                    x, y, newColor);
        }
    }

    return blurred;
}