#ifndef GEX_FUNCTION_H
#define GEX_FUNCTION_H

#include <vector>


namespace ImgManip
{
    class Function
    {
        virtual void Apply(int width, int height) = 0;
    };


    class FunctionStack
    {
        std::vector<Function> stack;

        unsigned int AddToStack(Function func);

        void RemFromStack(unsigned int index);

        void Apply(int width, int height);
    };
}


#endif //GEX_FUNCTION_H
