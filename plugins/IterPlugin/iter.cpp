#ifndef NODELIBRARY2COMPLETE_ITER_H
#define NODELIBRARY2COMPLETE_ITER_H


#include "Gex/include/Gex.h"

#include "../export.h"

#include <string>
#include "Tsys/include/defaultTypes.h"

namespace iter
{
    class Plugin_API Iterator : public Gex::CompoundNode
    {
    protected:

        void InitAttributes() override
        {
            Gex::CompoundNode::InitAttributes();

            CreateAttribute<TSys::AnyValue>(
                    "input", Gex::AttrValueType::Multi);

            CreateInternalAttribute<int>(
                    "index", Gex::AttrValueType::Single,
                    Gex::AttrType::Input);

            CreateInternalAttribute<TSys::AnyValue>(
                    "in", Gex::AttrValueType::Single,
                    Gex::AttrType::Input);

            CreateInternalAttribute<TSys::AnyValue>(
                    "out", Gex::AttrValueType::Single,
                     Gex::AttrType::Output);

            CreateAttribute<TSys::AnyValue>(
                    "output", Gex::AttrValueType::Multi,
                    Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphCtx,
                      Gex::NodeProfiler& profiler) override
        {
            std::vector<unsigned int> indices = GetAttribute("input")->ValidIndices();

            bool success = true;

            auto output = context.GetAttribute("output");
            output.ClearMultiIndices();

            for (unsigned int idx : indices)
            {
                context.GetAttribute("index").SetValue<int>((int)idx);

                TSys::AnyValue indexvalue = context.GetAttribute("input")
                        .GetIndexValue<TSys::AnyValue>(idx);

                context.GetAttribute("in").SetValue<TSys::AnyValue>(indexvalue);

                bool success_ = Gex::CompoundNode::Evaluate(context, graphCtx, profiler);
                if (!success_)
                {
                    continue;
                }

                auto out = context.GetAttribute("out").GetValue<TSys::AnyValue>();

                if (!output.HasIndex(idx))
                    output.CreateIndex(idx);

                output.SetIndexValue(idx, out);
            }

            return success;
        }

    };


    GENERATE_DEFAULT_BUILDER(IteratorBuilder, Iterator)
}


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterNode<iter::IteratorBuilder>("Iterator");
}


#endif //NODELIBRARY2COMPLETE_ITER_H
