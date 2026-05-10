#ifndef GEX_JSON_READERS_NODES_H
#define GEX_JSON_READERS_NODES_H
#include "Gex/include/Gex/Gex.h"
#include "Gex_plugins/export.h"

#include "jsonDefs.h"

#include "reader.h"


namespace JsonPlugin::Readers
{
    class ReadJson: public Gex::Node
    {
        void InitAttributes() override
        {
            CreateAttribute<std::string>(
                    "String", Gex::AttrValueType::Single,
                    Gex::AttrType::Input
            );

            CreateAttribute<JsonPlugin::ReaderPtr>(
                    "Json", Gex::AttrValueType::Single,
                    Gex::AttrType::Output
            );
        }

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
                      override
        {
            auto content = context.GetAttribute("String")
                    .GetValue<std::string>();

            rapidjson::Document doc;
            doc.Parse(content.c_str());

            if (doc.IsNull())
                return false;

            return context.GetAttribute("Json").SetValue(
                    std::make_shared<DocumentReader>(doc)
            );
        }
    };

    GENERATE_DEFAULT_BUILDER(ReadJsonBuilder, ReadJson)

    class ReadDictKey: public Gex::Node
    {
        void InitAttributes() override
        {
            CreateAttribute<JsonPlugin::ReaderPtr>(
                    "Input", Gex::AttrValueType::Single,
                    Gex::AttrType::Input
            );

            CreateAttribute<std::string>(
                    "Key", Gex::AttrValueType::Single,
                    Gex::AttrType::Input
            );

            CreateAttribute<JsonPlugin::ReaderPtr>(
                    "Output", Gex::AttrValueType::Single,
                    Gex::AttrType::Output
            );
        }

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
                      override
        {
            auto inp = context.GetAttribute("Input")
                    .GetValue<JsonPlugin::ReaderPtr>();

            auto key = context.GetAttribute("Key")
                    .GetValue<std::string>();

            return context.GetAttribute("Output").SetValue(
                    std::make_shared<JsonPlugin::DictValue>(key)
            );
        }
    };

    GENERATE_DEFAULT_BUILDER(ReadDictKeyBuilder, ReadDictKey)

    class ReadListIndex: public Gex::Node
    {
        void InitAttributes() override
        {
            CreateAttribute<JsonPlugin::ReaderPtr>(
                    "Input", Gex::AttrValueType::Single,
                    Gex::AttrType::Input
            );

            CreateAttribute<int>(
                    "Index", Gex::AttrValueType::Single,
                    Gex::AttrType::Input
            );

            CreateAttribute<JsonPlugin::ReaderPtr>(
                    "Output", Gex::AttrValueType::Single,
                    Gex::AttrType::Output
            );
        }

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override
        {
            auto inp = context.GetAttribute("Input")
                    .GetValue<JsonPlugin::ReaderPtr>();

            auto index = context.GetAttribute("Index")
                    .GetValue<int>();

            return context.GetAttribute("Output").SetValue(
                    std::make_shared<JsonPlugin::ListValue>(index)
            );
        }
    };

    GENERATE_DEFAULT_BUILDER(ReadListIndexBuilder, ReadListIndex)
}

#endif //GEX_JSON_READERS_NODES_H
