#include "gradient.h"


ImageManip::Types::LinearGradient::LinearGradient()
{
    steps = {
            {0., QColor(255, 255, 255)},
            {1., QColor(0, 0, 0)}
    };
}


ImageManip::Types::LinearGradient::LinearGradient(
        std::map<double, QColor> stps)
{
    steps = stps;
}


void ImageManip::Types::LinearGradient::AddStep(
        double x, const QColor& color)
{
    steps[x] = color;
}


struct Step
{
    double x;
    QColor color;
};


Step PreviousStep(double x, const ImageManip::Types::LinearGradient* gradient)
{
    Step res = {-1, QColor(0, 0, 0)};
    for (auto p : gradient->StepColors())
    {
        if (p.first >= x)
            break;

        if (p.first > res.x)
        {
            res.x = p.first;
            res.color = p.second;
        }
    }

    return res;
}


Step NextStep(double x, const ImageManip::Types::LinearGradient* gradient)
{
    Step res = {-1, QColor(0, 0, 0)};

    auto colors = gradient->StepColors();

    for (auto iter = colors.rbegin(); iter != colors.rend(); iter++)
    {
        if (iter->first <= x)
            break;

        if (res.x == -1)
        {
            res.x = iter->first;
            res.color = iter->second;
            continue;
        }

        if (iter->first < res.x)
        {
            res.x = iter->first;
            res.color = iter->second;
        }
    }

    return res;
}


void ImageManip::Types::LinearGradient::InsertStep(double x)
{
    AddStep(x, ColorAtStep(x));
}


bool ImageManip::Types::LinearGradient::RemoveStep(double x)
{
    if (steps.find(x) == steps.end())
        return true;

    steps.erase(x);
    return false;
}


bool ImageManip::Types::LinearGradient::MoveStep(double x, double dx)
{
    auto iter = steps.find(x);
    if (iter == steps.end())
        return false;

    QColor c = iter->second;
    steps.erase(iter);
    steps[dx] = c;
    return true;
}


bool ImageManip::Types::LinearGradient::EditStep(double x, QColor color)
{
    auto iter = steps.find(x);
    if (iter == steps.end())
        return false;

    iter->second = color;
    return true;
}


QColor ImageManip::Types::LinearGradient::ColorAtStep(double x) const
{
    auto iter = steps.find(x);
    if (iter != steps.end())
    {
        return iter->second;
    }

    Step previous = PreviousStep(x, this);
    Step next = NextStep(x, this);

    // Only one step on the gradient.
    if (previous.x == -1 && next.x == -1)
    {
        return QColor(0, 0, 0);
    }

    // Searching before the first step.
    if (previous.x == -1)
    {
        return next.color;
    }

    // Searching after the last step.
    if (next.x == -1)
    {
        return previous.color;
    }

    // 0.5-----0.75------1.0

    double diff = next.x - previous.x;
    double ratio = (next.x - x) / diff;

    QColor result(
            ratio * previous.color.red() +
            (1 - ratio) * next.color.red(),
            ratio * previous.color.green() +
            (1 - ratio) * next.color.green(),
            ratio * previous.color.blue() +
            (1 - ratio) * next.color.blue(),
            ratio * previous.color.alpha() +
            (1 - ratio) * next.color.alpha()
    );

    return result;
}


std::vector<double> ImageManip::Types::LinearGradient::Steps() const
{
    std::vector<double> rsteps;
    for (auto p : steps)
        rsteps.push_back(p.first);

    return rsteps;
}

std::map<double, QColor> ImageManip::Types::LinearGradient::StepColors() const
{
    return steps;
}