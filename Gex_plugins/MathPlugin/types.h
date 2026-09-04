#ifndef GEX_TYPES_H
#define GEX_TYPES_H

#include "../export.h"
#include "LocalApi.h"
#include "Tsys/tsys.h"


namespace Math::Types
{

    void SerializePoint2(const TSys::TypedValuePtr& v,
                         rapidjson::Value& value,
                         rapidjson::Document& document);

    void DeserializePoint2(const TSys::TypedValuePtr& v,
                           rapidjson::Value& value,
                           rapidjson::Document&);

    void SerializeVector2(const TSys::TypedValuePtr& v,
                          rapidjson::Value& value,
                          rapidjson::Document& document);

    void DeserializeVector2(const TSys::TypedValuePtr& v,
                            rapidjson::Value& value,
                            rapidjson::Document&);

    void SerializePoint3(const TSys::TypedValuePtr& v,
                         rapidjson::Value& value,
                         rapidjson::Document& document);

    void DeserializePoint3(const TSys::TypedValuePtr& v,
                           rapidjson::Value& value,
                           rapidjson::Document&);

    void SerializeVector3(const TSys::TypedValuePtr& v,
                          rapidjson::Value& value,
                          rapidjson::Document& document);

    void DeserializeVector3(const TSys::TypedValuePtr& v,
                            rapidjson::Value& value,
                            rapidjson::Document&);

}

#endif //GEX_TYPES_H
