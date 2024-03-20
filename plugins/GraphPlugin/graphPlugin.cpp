#ifndef LIBRARY_STRINGLIBRARY_H
#define LIBRARY_STRINGLIBRARY_H

#endif //LIBRARY_STRINGLIBRARY_H



#include "Gex/include/Gex.h"
#include "Tsys//include/defaultTypes.h"


namespace Graph {

    class ReadProperty : public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Name", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);

            CreateAttribute<TSys::AnyValue>("Value", Gex::AttrValueType::Single,
                                            Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &context, Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override
        {
            auto name = context.GetAttribute("Name").GetValue<std::string>();

            auto p = graphContext.Property(name);
            if (!p.IsValid())
                return false;

            TSys::AnyValue any(p.Get());
            return context.GetAttribute("Value").SetValue(any);
        }
    };

    GENERATE_DEFAULT_BUILDER(ReadPropertyBuilder, ReadProperty)


    class SetProperty : public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<std::string>("Name", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);

            CreateAttribute<TSys::AnyValue>("Value", Gex::AttrValueType::Single,
                                            Gex::AttrType::Input);
        }

        bool Evaluate(Gex::NodeAttributeData &context, Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override
        {
            auto name = context.GetAttribute("Name").GetValue<std::string>();

            Gex::Feedback res;
            auto v = context.GetAttribute("Value").GetAnyValue(&res);
            if (!res)
                return false;

            auto p = graphContext.Property(name);
            if (!p.IsValid())
                return false;

            return p.SetValue(v);
        }
    };

    GENERATE_DEFAULT_BUILDER(SetPropertyBuilder, SetProperty)
}

REGISTER_PLUGIN(Gex::PluginLoader* loader)
{
    loader->RegisterNode<Graph::ReadPropertyBuilder>("Graph/ReadProperty");

    loader->RegisterNode<Graph::SetPropertyBuilder>("Graph/SetProperty");
}
