#ifndef GEX_IMP_COLOR_H
#define GEX_IMP_COLOR_H

#include "Tsys/tsys.h"

#include "../export.h"

#include <QColor>

namespace ImageManip::Types
{

    void SerializeColor(const TSys::TypedValuePtr& v,
                        rapidjson::Value& value,
                        rapidjson::Document& document);

    void DeserializeColor(const TSys::TypedValuePtr& v,
                          rapidjson::Value& value,
                          rapidjson::Document&);

}

#endif //GEX_IMP_COLOR_H
