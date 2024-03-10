#include <ui/include/UiStatus.h>


void Gex::Ui::UiFeedback::Set(Status st, const std::string& msg,
                              QColor textColor_, QColor color_)
{
    Gex::Feedback::Set(st, msg);

    color = color_;
    textColor = textColor_;
}


void ResolveColor(const Gex::Status status, QColor& back, QColor& text)
{
    switch (status)
    {
        case Gex::Status::None:
            back = "#474747";
            text = "#FFFFFF";
            break;
        case Gex::Status::Success:
            back = "#459362";
            text = "#FFFFFF";
            break;
        case Gex::Status::Failed:
            back = "#C14936";
            text = "#FFFFFF";
            break;
        case Gex::Status::Warning:
            back = "#CAA343";
            text = "#FFFFFF";
            break;
        case Gex::Status::Error:
            back = "#474747";
            text = "#FFFFFF";
            break;
        default:
            back = "#CA4366";
            text = "#FFFFFF";

    }
}


Gex::Ui::UiFeedback Gex::Ui::MakeFeedback(Gex::Status status, const std::string&  msg)
{
    QColor back;
    QColor text;

    ResolveColor(status, back, text);

    UiFeedback f;
    f.Set(status, msg, back, text);
    return f;
}


Gex::Ui::UiFeedback Gex::Ui::MakeFeedback(Gex::Feedback f)
{
    QColor back;
    QColor text;

    ResolveColor(f.status, back, text);

    UiFeedback uif;
    uif.Set(f.status, f.message, text, back);
    return uif;
}