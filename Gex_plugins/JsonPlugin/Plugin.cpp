#include "Gex/include/Gex/Gex.h"

#include "../export.h"

#include "jsonDefs.h"
#include "types/types.h"
#include "nodes/writers.h"
#include "nodes/readers.h"
#include "editor.h"


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->GetRegistry()->RegisterType<JsonPlugin::JsonValuePtr>(
        "Json::Value", TSys::DefaultCreator,
        JsonPlugin::SerializeJsonValue,
        JsonPlugin::DeserializeJsonValue,
        TSys::UnuseFromPython,
        TSys::UnuseToPython,
        nullptr, nullptr,
        nullptr, nullptr,
        nullptr, nullptr
    );

    loader->GetRegistry()->RegisterType<JsonPlugin::EditorPtr>(
            "Json::Editor", TSys::DefaultCreator,
            JsonPlugin::SerializeJsonValueEditor,
            JsonPlugin::DeserializeJsonValueEditor,
            TSys::UnuseFromPython,
            TSys::UnuseToPython,
            nullptr, nullptr,
            nullptr, nullptr,
            nullptr, nullptr
    );

    loader->RegisterNode<JsonPlugin::Writers::JsonListBuilder>("Json/Write/JsonList");

    loader->RegisterNode<JsonPlugin::Writers::JsonDictBuilder>("Json/Write/JsonDict");

    loader->RegisterNode<JsonPlugin::Writers::JsonWriteToStringBuilder>("Json/Write/WriteToString");

    loader->RegisterNode<JsonPlugin::Writers::JsonListBuilder>("Json/Read/JsonList");

    loader->RegisterNode<JsonPlugin::Readers::ReadDictKeyBuilder>("Json/Read/JsonDict");

    loader->RegisterNode<JsonPlugin::Readers::ReadJsonBuilder>("Json/Read/ReadFromString");
}
