#include "writers.h"


std::string JsonPlugin::JsonDict::Description() const
{
    return ("Generates a Json dict from provided Key/Value pair. "
            "Keys are string value and Values are json data, "
            "converted from scalars if set as input.");
}


void JsonPlugin::JsonDict::InitAttributes()
{
    auto holder = CreateAttribute(
            "Members",
            Gex::AttrType::Input,
            true
    );

    CreateAttribute<std::string>(
            "Key", Gex::AttrValueType::Single,
            Gex::AttrType::Input, holder
    );

    CreateAttribute<EditorPtr>(
            "Value", Gex::AttrValueType::Single,
            Gex::AttrType::Input, holder
    );

    CreateAttribute<EditorPtr>(
            "Dict", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}


bool JsonPlugin::JsonDict::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    auto* outDictEditor = new DictEditor();

    EditorPtr outDict(outDictEditor);

    auto attribute = context.GetAttribute("Members");
    for (unsigned int i : attribute.GetIndices())
    {
        auto indexAttribute = attribute.GetIndex(i);

        auto keyAttribute = indexAttribute.GetAttribute("Key");
        auto valueAttribute = indexAttribute.GetAttribute("Value");

        auto key = keyAttribute.GetValue<std::string>();
        auto value = valueAttribute.GetValue<EditorPtr>();

        outDictEditor->AddEditor(key, value);
    }

    return context.GetAttribute("Dict").SetValue(outDict);
}


std::string JsonPlugin::JsonList::Description() const
{
    return ("Generates a Json list from provided values. "
            "Values are json data, converted from scalars "
            "if set as input.");
}


void JsonPlugin::JsonList::InitAttributes()
{
    {
        CreateAttribute<EditorPtr>(
                "Values", Gex::AttrValueType::Multi,
                Gex::AttrType::Input
        );

        CreateAttribute<EditorPtr>(
                "List", Gex::AttrValueType::Single,
                Gex::AttrType::Output
        );
    }
}


bool JsonPlugin::JsonList::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    auto* outListEditor = new ListEditor();

    EditorPtr outList(outListEditor);

    auto attribute = context.GetAttribute("Values");
    for (unsigned int i : attribute.GetIndices())
    {
        outListEditor->AddEditor(
                attribute.GetIndex(i).GetValue<EditorPtr>()
        );
    }

    return context.GetAttribute("List").SetValue(outList);
}


std::string JsonPlugin::JsonWriteToString::Description() const
{
    return ("Serializes Json data to string.");
}


void JsonPlugin::JsonWriteToString::InitAttributes()
{
    {
        CreateAttribute<EditorPtr>(
                "Value", Gex::AttrValueType::Single,
                Gex::AttrType::Input
        );

        CreateAttribute<bool>(
                "Indent", Gex::AttrValueType::Single,
                Gex::AttrType::Input
        );

        CreateAttribute<std::string>(
                "String", Gex::AttrValueType::Single,
                Gex::AttrType::Output
        );
    }
}


bool JsonPlugin::JsonWriteToString::Evaluate(
        Gex::NodeAttributeData &context,
        Gex::GraphContext &graphContext,
        Gex::NodeProfiler &profiler
)
{
    auto inValue = context.GetAttribute("Value").GetValue<EditorPtr>();
    auto prettyWriter = context.GetAttribute("Indent").GetValue<bool>();

    rapidjson::Document doc;

    JsonValuePtr jsonValue = inValue->EditValue(doc);

    doc.Swap(*jsonValue.get());

    rapidjson::StringBuffer buffer;

    if (prettyWriter)
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
    }
    else
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
    }

    std::string outString = buffer.GetString();

    return context.GetAttribute("String").SetValue(outString);
}