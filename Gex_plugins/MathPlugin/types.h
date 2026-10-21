#ifndef GEX_TYPES_H
#define GEX_TYPES_H

#include "../export.h"
#include "LocalApi.h"
#include "Tsys/tsys.h"


namespace Math::Types
{

    MathPlugin_API
    void SerializePoint2(const TSys::TypedValuePtr& v,
                         rapidjson::Value& value,
                         rapidjson::Document& document);

    MathPlugin_API
    void DeserializePoint2(const TSys::TypedValuePtr& v,
                           rapidjson::Value& value,
                           rapidjson::Document&);

    MathPlugin_API
    void SerializeVector2(const TSys::TypedValuePtr& v,
                          rapidjson::Value& value,
                          rapidjson::Document& document);

    MathPlugin_API
    void DeserializeVector2(const TSys::TypedValuePtr& v,
                            rapidjson::Value& value,
                            rapidjson::Document&);

    MathPlugin_API
    void SerializePoint3(const TSys::TypedValuePtr& v,
                         rapidjson::Value& value,
                         rapidjson::Document& document);

    MathPlugin_API
    void DeserializePoint3(const TSys::TypedValuePtr& v,
                           rapidjson::Value& value,
                           rapidjson::Document&);

    MathPlugin_API
    void SerializeVector3(const TSys::TypedValuePtr& v,
                          rapidjson::Value& value,
                          rapidjson::Document& document);

    MathPlugin_API
    void DeserializeVector3(const TSys::TypedValuePtr& v,
                            rapidjson::Value& value,
                            rapidjson::Document&);

}

#endif //GEX_TYPES_H
