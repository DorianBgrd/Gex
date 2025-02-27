#include "Gex/include/Gex.h"

#include "../export.h"

#include "jsonDefs.h"
#include "types/types.h"
#include "nodes/writers.h"
#include "editor.h"


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterTypeHandler<
            JsonPlugin::JsonValuePtr,
            JsonPlugin::JsonValueHandler>();

    loader->RegisterTypeHandler<
            JsonPlugin::EditorPtr,
            JsonPlugin::JsonEditorHandler>();

    loader->RegisterNode<JsonPlugin::JsonListBuilder>("Json/JsonList");

    loader->RegisterNode<JsonPlugin::JsonDictBuilder>("Json/JsonDict");

    loader->RegisterNode<JsonPlugin::JsonWriteToStringBuilder>("Json/WriteToString");
}
