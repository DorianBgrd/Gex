#include "Gex/include/Status.h"


void Gex::LogFeedback(const Feedback& feedback)
{
    std::string statName;
    switch (feedback.status)
    {
        case Status::Success:
            statName = "Success";
            break;
        case Status::Warning:
            statName = "Warning";
            break;
        case Status::Failed:
            statName = "Failed";
            break;
        case Status::Error:
            statName = "Error";
            break;
        default:
            statName = "Undefined";
    }

    std::cout << "[" << statName << "] " << feedback.message << std::endl;
}