#ifndef GEX_API_H
#define GEX_API_H

#ifdef GEX_API_EXPORT
#define GEX_API __declspec(dllexport)
#else
#define GEX_API __declspec(dllimport)
#endif

#endif //GEX_API_H
