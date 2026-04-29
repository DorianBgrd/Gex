#ifndef GEX_JSONDEFS_H
#define GEX_JSONDEFS_H

#include <memory>
#include "rapidjson/document.h"

namespace JsonPlugin
{
    typedef rapidjson::Document JsonDoc;
    typedef rapidjson::Value JsonValue;

    typedef std::shared_ptr<rapidjson::Document> JsonDocPtr;
    typedef std::shared_ptr<JsonValue> JsonValuePtr;

}

#endif //GEX_JSONDEFS_H
