#ifndef PIPELINELIBRARY2COMPLETE_EXPORT_H
#define PIPELINELIBRARY2COMPLETE_EXPORT_H

#ifndef Plugin_EXPORT
#define Plugin_API __declspec(dllexport)
#else
#define Plugin_API __declspec(dllimport)
#endif


#define EXPORT "C" void __declspec(dllexport) __cdecl

#endif //PIPELINELIBRARY2COMPLETE_EXPORT_H
