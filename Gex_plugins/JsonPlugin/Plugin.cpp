#include "Gex/include/Gex/Gex.h"

#include "../export.h"

#include "jsonDefs.h"
#include "types/types.h"
#include "nodes/writers.h"
#include "nodes/readers.h"
#include "editor.h"


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterTypeHandler<
            JsonPlugin::JsonValuePtr,
            JsonPlugin::JsonValueHandler>();

    loader->RegisterTypeHandler<
            JsonPlugin::EditorPtr,
            JsonPlugin::JsonEditorHandler>();

    loader->RegisterNode<JsonPlugin::Writers::JsonListBuilder>("Json/Write/JsonList");

    loader->RegisterNode<JsonPlugin::Writers::JsonDictBuilder>("Json/Write/JsonDict");

    loader->RegisterNode<JsonPlugin::Writers::JsonWriteToStringBuilder>("Json/Write/WriteToString");

    loader->RegisterNode<JsonPlugin::Writers::JsonListBuilder>("Json/Read/JsonList");

    loader->RegisterNode<JsonPlugin::Readers::ReadDictKeyBuilder>("Json/Read/JsonDict");

    loader->RegisterNode<JsonPlugin::Readers::ReadJsonBuilder>("Json/Read/ReadFromString");
}
