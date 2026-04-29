#include "UiRes/uires.h"

Res::UiRes* Res::UiRes::res = nullptr;


Res::UiRes::UiRes()
{
    qtaw = new fa::QtAwesome();
    qtaw->initFontAwesome();
    qtaw->setDefaultOption("color", "#FFFFFF");
    qtaw->setDefaultOption("color-active", "#FFFFFF");
    qtaw->setDefaultOption("color-disabled", "#808080");
}


Res::UiRes* Res::UiRes::GetRes()
{
    if (!res)
        res = new UiRes();

    return res;
}


fa::QtAwesome* Res::UiRes::GetQtAwesome()
{
    return qtaw;
}
