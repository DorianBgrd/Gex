#ifndef GEX_STATUS_H
#define GEX_STATUS_H

#include <string>
#include "api.h"


namespace Gex
{
    enum class GEX_API Status
    {
        None,
        Success,
        Warning,
        Failed,
        Error
    };


    struct GEX_API Feedback
    {

        Status status = Status::None;
        std::string message;

        operator bool() const
        {
            return (status == Status::Success);
        }
    };
}


#endif //GEX_STATUS_H
