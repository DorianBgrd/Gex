#ifndef GEX_TEMPLATED_H
#define GEX_TEMPLATED_H

#include "Gex/Gex/include/Node.h"


template<class T>
class TNode: public Gex::Node
{
    void CreateAttributes()
    {
        CreateAttribute<T>("At1");
        CreateAttribute<T>("At2");


    }

};

#endif //GEX_TEMPLATED_H
