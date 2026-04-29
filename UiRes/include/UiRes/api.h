#ifndef UIRES_API_H
#define UIRES_API_H
#ifdef UIRES_API_EXPORT
#define UIRES_API __declspec(dllexport)
#else
#define UIRES_API __declspec(dllimport)
#endif

#endif //UIRES_API
