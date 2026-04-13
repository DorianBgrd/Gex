#ifndef GEX_MATH_LOCALAPI_H
#define GEX_MATH_LOCALAPI_H

#ifndef MathPlugin_API_IMPORT
#define MathPlugin_API __declspec(dllexport)
#else
#define MathPlugin_API __declspec(dllimport)
#endif


#endif //GEX_MATH_LOCALAPI_H
