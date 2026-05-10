#include "pixelEngine.h"

#include <mutex>


ImageManip::Manip::PixelEngine::PixelEngine(
        const Processor& proc,
        const QImage& image,
        int numberOfThread
)
{
    function = proc;
    original = image;
    threads = numberOfThread;
}


struct SharedData
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    int Next()
    {
        int nx = x + 1;
        if (nx > width)
        {
            x = 0;
            y++;
        }

        return (y * width + x);
    }

    bool IsDone() const
    {
        return (x == width && y == height);
    }

};


QImage ImageManip::Manip::PixelEngine::Process()
{
    QImage modified(original.width(),
                    original.height(),
                    original.format());

    const uchar* originalBits = original.constBits();
    uchar* imageBits = modified.bits();

    SharedData data = {
        0, 0, original.width(),
        original.height()
    };

    std::mutex mutex;

    auto pixelProcessor = [this, &mutex, &data, originalBits, imageBits]()
    {
        while (!data.IsDone())
        {
            mutex.lock();

            int n = data.Next();

            imageBits[n] = function(n, originalBits);

            mutex.unlock();
        }
    };


    for (unsigned int i = 0; i < threads; i++)
    {
        pool.emplace_back(pixelProcessor);
    }

    for (auto& thread : pool)
    {
        thread.join();
    }

    return modified;
}