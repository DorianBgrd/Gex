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
                             rapidjson::Document& document)
    {
        auto res = v->Get<Resolution>();

        auto& arrayValue = value.SetArray();

        arrayValue.PushBack(rapidjson::Value().SetInt(res.at(0)),
                         document.GetAllocator());
        arrayValue.PushBack(rapidjson::Value().SetInt(res.at(1)),
                         document.GetAllocator());
    }

    void DeserializeResolution(const TSys::TypedValuePtr& v,
                               rapidjson::Value& value,
                               rapidjson::Document& document)
    {
        Resolution res = {
                value[0].GetInt(),
                value[1].GetInt()
        };

        v->Set(res);
    }

}

#endif //GEX_IMP_RESOLUTION_H
