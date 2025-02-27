#ifndef GEX_JSON_NODES_H
#define GEX_JSON_NODES_H

#include <string>

#include "Gex/include/Gex.h"
#include "jsonDefs.h"

#include "Tsys/include/defaultTypes.h"

#include "editor.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"


namespace JsonPlugin
{

//    typedef std::variant<std::string, int, float, double> JsonVariant;

    class JsonDict: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
                      override;
    };


    GENERATE_DEFAULT_BUILDER(JsonDictBuilder, JsonDict)


    class JsonList: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;


        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };


    GENERATE_DEFAULT_BUILDER(JsonListBuilder, JsonList)


    class JsonWriteToString: public Gex::Node
    {
        std::string Description() const override;

        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
                      override;
    };


    GENERATE_DEFAULT_BUILDER(JsonWriteToStringBuilder, JsonWriteToString)

}

#endif //GEX_JSON_NODES_H
