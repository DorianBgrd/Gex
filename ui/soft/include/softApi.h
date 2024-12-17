#ifndef SOFT_API_H
#define SOFT_API_H

#ifdef SOFT_API_EXPORT
#define SOFT_API __declspec(dllexport)
#else
#define SOFT_API __declspec(dllimport)
#endif

#endif //SOFT_API_H