#ifndef GEX_UISTATUS_H
#define GEX_UISTATUS_H

#include <QColor>
#include "api.h"
#include "Gex/include/Status.h"

namespace Gex::Ui
{
    struct UiFeedback: public Gex::Feedback
    {
        QColor color;
        QColor textColor;

        void Set(Status st, const std::string& msg,
                 QColor textColor_, QColor color_);
    };


    GEX_UI_API
    UiFeedback MakeFeedback(Gex::Status status, const std::string&  msg);


    GEX_UI_API
    UiFeedback MakeFeedback(Gex::Feedback f);
}


#endif //GEX_UISTATUS_H
