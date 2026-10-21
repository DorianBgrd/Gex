#ifndef GEX_IMP_RESOLUTION_H
#define GEX_IMP_RESOLUTION_H

#include "Tsys/tsys.h"

#include "../export.h"

#include <QColor>
#include <array>

namespace ImageManip::Types
{
    typedef std::array<int, 2> Resolution;


    void SerializeResolution(const TSys::TypedValuePtr & v,
                             rapidjson::Value& value,
                             rapidjson::Document& document);

    void DeserializeResolution(const TSys::TypedValuePtr& v,
                               rapidjson::Value& value,
                               rapidjson::Document& document);

}

#endif //GEX_IMP_RESOLUTION_H
