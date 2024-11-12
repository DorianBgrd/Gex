#ifndef GEX_CNTRS_H
#define GEX_CNTRS_H
#include <vector>


#define VectorIterator(T) std::_Vector_iterator<T>


namespace Gex::Cntrs
{
//    template<typename T>
//    class Vector: public std::vector<T>
//    {
//        using std::vector<T>::vector;
//
//        std::vector<T>::iterator find(const T& other) const
//        {
//            return std::find(this->begin(), this->end(), other);
//        }
//    };

    template<typename T>
    VectorIterator(T) Find(std::vector<T> vec, const T& other)
    {
        return std::find(vec.begin(), vec.end(), other);
    }

    template<typename T>
    bool Contains(std::vector<T> vec, const T& other)
    {
        return (Find(vec, other) != vec.end());
    }
}

#endif //GEX_CNTRS_H
