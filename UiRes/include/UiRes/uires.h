#ifndef UIRES_UIRES_H
#define UIRES_UIRES_H

#include "QtAwesome/QtAwesome.h"
#include "api.h"

namespace Res
{
    class UIRES_API UiRes
    {
    private:
        static UiRes* res;
        fa::QtAwesome* qtaw;

        UiRes();
    public:

        static UiRes* GetRes();

        fa::QtAwesome* GetQtAwesome();
    };
}



#endif //UIRES_UIRES_H
