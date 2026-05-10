#ifndef GEX_MATRIX_H
#define GEX_MATRIX_H

#include <array>

namespace Gex::Particles
{
    template<typename T, size_t L>
    struct Matrix4
    {
        std::array<std::array<T, L>, L> matrix;

        std::array<T, L>& operator[](size_t row)
        {
            return matrix[row];
        }
    };



}

#endif //GEX_MATRIX_H
