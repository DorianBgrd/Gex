#pragma once
#include "typeSystem.h"


#include <any>
#include <variant>
#include <string>
#include <map>
#include <vector>
#include "rapidjson/reader.h"
#include "rapidjson/document.h"

#include "api.h"


namespace TSys
{
    void RegisterDefaultTypes(TSys::TypeValueRegistry* registry);
}

