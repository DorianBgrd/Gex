#ifndef GEX_PYTHON_API_H
#define GEX_PYTHON_API_H

#ifdef GEX_PYTHON_API_EXPORT
#define GEX_PYTHON_API __declspec(dllexport)
#else
#define GEX_PYTHON_API __declspec(dllimport)
#endif

#endif //GEX_PYTHON_API_H
