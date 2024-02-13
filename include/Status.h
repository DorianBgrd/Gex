#ifndef GEX_STATUS_H
#define GEX_STATUS_H

#include <string>
#include "api.h"


namespace Gex
{
    enum class Status
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

        void Set(Status st, const std::string& msg)
        {
            status = st;
            message = msg;
        }
    };
}


#endif //GEX_STATUS_H
