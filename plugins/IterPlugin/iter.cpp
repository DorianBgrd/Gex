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

            auto* idxat = CreateAttribute<int>(
                    "index", Gex::AttrValueType::Single,
                    Gex::AttrType::Input);
            idxat->SetInternal(true);

            auto* in = CreateAttribute<TSys::AnyValue>(
                    "in", Gex::AttrValueType::Single,
                    Gex::AttrType::Input);
            in->SetInternal(true);

            auto* out = CreateAttribute<TSys::AnyValue>(
                    "out", Gex::AttrValueType::Single,
                     Gex::AttrType::Output);
            out->SetInternal(true);

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

//        std::vector<Gex::ScheduledNode*> ToScheduledNodes() override
//        {
//            size_t length_ = GetAttribute("input")->ValidIndices().size();
//
//            std::vector<Gex::ScheduledNode*> result;
//
//            Gex::ScheduledNode* last = nullptr;
//            for (unsigned int i= 0; i < length_; i++)
//            {
//                auto schelNodes = Gex::CompoundNode::ToScheduledNodes();
//
//                if (last)
//                    schelNodes.at(0)->previousNodes.push_back(last);
//
//                result.insert(result.end(), schelNodes.begin(), schelNodes.end());
//
//                last = result.at(result.size() - 1);
//            }
//
//            return result;
//        }
    };

    GENERATE_DEFAULT_BUILDER(IteratorBuilder, Iterator)

    class Plugin_API ForRangeLoop: public Gex::CompoundNode
    {
        Gex::ScheduleNodeList compoundScheduledNodes;

    protected:
        std::vector<unsigned int> indices;

        void InitAttributes() override
        {
            Gex::CompoundNode::InitAttributes();

            CreateAttribute<TSys::AnyValue>(
                    "Array", Gex::AttrValueType::Multi);

            CreateAttribute<TSys::AnyValue>(
                    "Value", Gex::AttrValueType::Single,
                    Gex::AttrType::Input)->SetInternal(true);

            CreateAttribute<TSys::AnyValue>(
                    "OutValue", Gex::AttrValueType::Single,
                    Gex::AttrType::Output)->SetInternal(true);

            CreateAttribute<TSys::AnyValue>(
                    "OutArray", Gex::AttrValueType::Multi,
                    Gex::AttrType::Output);
        }

        bool PreEvaluate(Gex::NodeAttributeData &ctx,
                         Gex::GraphContext &graphContext,
                         Gex::NodeProfiler &profiler) override
        {
            indices = ctx.GetAttribute("Array").Indices();
            return true;
        }


        bool IterEvaluate(Gex::NodeEvaluator& evaluator,
                          Gex::NodeAttributeData &ctx,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler)
        {
            auto prof = profiler.GetProfiler();

            if (!IsScheduled())
            {
                Schedule();
            }

            evaluator.Run();

            return (evaluator.Status() == Gex::NodeEvaluator::EvaluationStatus::Done);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override
        {
            Gex::NodeEvaluator evaluator(
                    compoundScheduledNodes, graphContext,
                    profiler.GetProfiler(), false, 1);

            ctx.GetAttribute("OutArray").ClearMultiIndices();

            for (auto index : indices)
            {
                auto iv = ctx.GetAttribute("Array").GetIndex(index).GetAnyValue();

                ctx.GetAttribute("Value").SetAnyValue(iv);

                if (!IterEvaluate(evaluator, ctx, graphContext, profiler))
                {
                    return false;
                }

//                if (!Gex::CompoundNode::Evaluate(ctx, graphContext, profiler))
//                {
//                    return false;
//                }

                PullInternalOutputs();

                auto ov = ctx.GetAttribute("OutValue").GetAnyValue();

                ctx.GetAttribute("OutArray").CreateIndex(index);

                ctx.GetAttribute("OutArray").GetIndex(index).SetAnyValue(ov);
            }

            return true;
        }

        bool PostEvaluate(Gex::NodeAttributeData &ctx,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override
        {
            return true;
        }

        Gex::ScheduleNodeList ToScheduledNodes() override
        {
            compoundScheduledNodes = Gex::ScheduleNodes(GetNodes());

            return {ToScheduledNode()};
        }
    };

    GENERATE_DEFAULT_BUILDER(ForRangeLoopBuilder, ForRangeLoop)

}


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterNode<iter::IteratorBuilder>("Iter/Iterator");

    loader->RegisterNode<iter::ForRangeLoopBuilder>("Iter/ForRangeLoop");

}


#endif //NODELIBRARY2COMPLETE_ITER_H
