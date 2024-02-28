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
    private:
        int currentIndex = -1;
        unsigned int length = 0;
        std::vector<unsigned int> indices;

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


        void Prepare(Gex::NodeAttributeData &ctx)
        {
            ctx.GetAttribute("output").ClearMultiIndices();
            indices = GetAttribute("input")->ValidIndices();
            length = indices.size();
        }


        bool PreEvaluate(Gex::NodeAttributeData &ctx,
                         Gex::GraphContext &graphContext,
                         Gex::NodeProfiler &profiler)
                         override
        {
            if (currentIndex == -1)
            {
                Prepare(ctx);
            }

            currentIndex++;
            auto idx = indices[currentIndex];

            ctx.GetAttribute("index").SetValue<int>((int)idx);

            auto indexvalue = ctx.GetAttribute("input")
                    .GetIndexValue<TSys::AnyValue>(idx);

            return ctx.GetAttribute("in").SetValue<TSys::AnyValue>(indexvalue);
        }


        void Finalize()
        {
            currentIndex = -1;
            indices.clear();
            length = 0;
        }


        bool PostEvaluate(Gex::NodeAttributeData &ctx,
                         Gex::GraphContext &graphContext,
                         Gex::NodeProfiler &profiler)
                        override
        {
            auto outputs = ctx.GetAttribute("output");

            auto idx = indices.at(currentIndex);
            outputs.CreateIndex(idx);
            outputs.SetIndexValue(idx, ctx.GetAttribute("out").GetValue<TSys::AnyValue>());

            if (currentIndex == (length - 1))
            {
                Finalize();
            }

            return true;
        }

        std::vector<Gex::ScheduledNode*> ToScheduledNodes() override
        {
            size_t length_ = GetAttribute("input")->ValidIndices().size();

            std::vector<Gex::ScheduledNode*> result;

            Gex::ScheduledNode* last = nullptr;
            for (unsigned int i= 0; i < length_; i++)
            {
                auto schelNodes = Gex::CompoundNode::ToScheduledNodes();

                if (last)
                    schelNodes.at(0)->previousNodes.push_back(last);

                result.insert(result.end(), schelNodes.begin(), schelNodes.end());

                last = result.at(result.size() - 1);
            }

            return result;
        }
    };


    GENERATE_DEFAULT_BUILDER(IteratorBuilder, Iterator)
}


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterNode<iter::IteratorBuilder>("Iterator");
}


#endif //NODELIBRARY2COMPLETE_ITER_H
