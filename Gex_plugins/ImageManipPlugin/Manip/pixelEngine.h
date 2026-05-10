#ifndef GEX_PIXELENGINE_H
#define GEX_PIXELENGINE_H

#include <thread>
#include <functional>
#include <QColor>
#include <QImage>


namespace ImageManip::Manip
{
    typedef std::function<QRgba64(int p, const uchar*)> Processor;


    class PixelEngine
    {
        Processor function;
        QImage original;
        int threads;

        std::vector<std::thread> pool;

    public:
        PixelEngine(const Processor& proc,
                    const QImage& image,
                    int numberOfThread=8);

        QImage Process();
    };
}

#endif //GEX_PIXELENGINE_H
