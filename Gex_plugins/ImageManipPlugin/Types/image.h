#ifndef GEX_IMAGE_H
#define GEX_IMAGE_H

#include "Tsys/tsys.h"
#include <QPixmap>

#include "../export.h"


namespace ImageManip::Types
{

    void SerializeImage(const TSys::TypedValuePtr& v,
                        rapidjson::Value& value,
                        rapidjson::Document& document);


    void DeserializeImage(const TSys::TypedValuePtr& v,
                          rapidjson::Value& value,
                          rapidjson::Document&);

}


#endif //GEX_IMAGE_H
