#ifndef GEX_UI_API_H
#define GEX_UI_API_H

#include <QtCore/QtGlobal>

#ifdef GEX_UI_API_EXPORT
#define GEX_UI_API Q_DECL_EXPORT
#else
#define GEX_UI_API Q_DECL_IMPORT
#endif

#ifdef GEX_UI_API_IMPORT
#undef GEX_UI_API
#define GEX_UI_API Q_DECL_IMPORT
#endif

#endif //GEX_UI_API_H
