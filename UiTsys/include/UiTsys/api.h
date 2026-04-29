#ifndef UI_TSYS_API_H
#define UI_TSYS_API_H

#include <QtCore/QtGlobal>

#ifdef UI_TSYS_EXPORT
#define UI_TSYS_API Q_DECL_EXPORT
#else
#define UI_TSYS_API Q_DECL_IMPORT
#endif

#endif //UI_TSYS_API_H
