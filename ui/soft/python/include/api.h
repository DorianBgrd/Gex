#ifndef SOFT_PYTHON_API_H
#define SOFT_PYTHON_API_H

#ifdef SOFT_PYTHON_API_EXPORT
#define SOFT_PYTHON_API __declspec(dllexport)
#else
#define SOFT_PYTHON_API __declspec(dllimport)
#endif

#endif //SOFT_PYTHON_API_H
