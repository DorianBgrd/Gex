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

            auto idxat = CreateAttribute<int>(
                    "index", Gex::AttrValueType::Single,
                    Gex::AttrType::Input);
            idxat->SetInternal(true);

            auto in = CreateAttribute<TSys::AnyValue>(
                    "in", Gex::AttrValueType::Single,
                    Gex::AttrType::Input);
            in->SetInternal(true);
            in->SetExternal(false);

            auto out = CreateAttribute<TSys::AnyValue>(
                    "out", Gex::AttrValueType::Single,
                     Gex::AttrType::Output);
            out->SetInternal(true);
            out->SetExternal(false);

            CreateAttribute<TSys::AnyValue>(
                    "output", Gex::AttrValueType::Multi,
                    Gex::AttrType::Output);
        }


        void Prepare(Gex::NodeAttributeData &ctx)
        {
            ctx.GetAttribute("output").ClearMultiIndices();
            indices = GetAttribute("input").ToShared()->ValidIndices();
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
        Gex::ScheduleNodePtrList compoundScheduledNodes;

    protected:
        std::vector<unsigned int> indices;

        void InitAttributes() override
        {
            Gex::CompoundNode::InitAttributes();

            CreateAttribute<TSys::AnyValue>(
                    "Array", Gex::AttrValueType::Multi);

            auto v = CreateAttribute<TSys::AnyValue>(
                    "Value", Gex::AttrValueType::Single,
                    Gex::AttrType::Input);
            v->SetInternal(true);
            v->SetExternal(false);

            auto ov = CreateAttribute<TSys::AnyValue>(
                    "OutValue", Gex::AttrValueType::Single,
                    Gex::AttrType::Output);
            ov->SetInternal(true);
            ov->SetExternal(false);

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

        Gex::ScheduleNodePtrList ToScheduledNodes() override
        {
            compoundScheduledNodes = Gex::ScheduleNodes(GetNodes());

            return {ToScheduledNode()};
        }
    };

    GENERATE_DEFAULT_BUILDER(ForRangeLoopBuilder, ForRangeLoop)

    class Plugin_API TestIter: public Gex::CompoundNode
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<int>("ItGeom", Gex::AttrValueType::Single,
                                          Gex::AttrType::Input);

            TSys::Enum space;
            space.AddValue(0, "Object");
            space.AddValue(1, "World");

            auto p = CreateAttribute<int>("Position", Gex::AttrValueType::Single,
                                              Gex::AttrType::Input);
            p->SetInternal(true);
            p->SetExternal(false);

            auto s = CreateAttributeFromValue("Space", std::make_any<TSys::Enum>(space),
                                               Gex::AttrValueType::Single, Gex::AttrType::Input);
            s->SetInternal(true);
            s->SetExternal(false);

            auto n = CreateAttribute<int>("Normale", Gex::AttrValueType::Single,
                                              Gex::AttrType::Input);
            n->SetInternal(true);
            n->SetExternal(false);

            auto w = CreateAttribute<float>("Weight",  Gex::AttrValueType::Single,
                                             Gex::AttrType::Input);
            w->SetInternal(true);
            w->SetExternal(false);

            auto m = CreateAttribute<int>("Matrix",  Gex::AttrValueType::Single,
                                               Gex::AttrType::Input);
            m->SetInternal(true);
            m->SetExternal(false);

            auto op = CreateAttribute<int>("OutPosition",  Gex::AttrValueType::Single,
                                               Gex::AttrType::Output);
            op->SetInternal(true);
            m->SetExternal(false);
        }
    };

    GENERATE_DEFAULT_BUILDER(TestIterBuilder, TestIter)

    class Plugin_API CounterIter: public Gex::CompoundNode
    {
        std::string Description() const override
        {
            return "Performs a for loop using a counter.";
        }

        Gex::ScheduleNodePtrList ToScheduledNodes() override
        {
            return {ToScheduledNode()};
        }

        void InitAttributes() override
        {
            CreateAttribute<int>("Start", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            CreateAttribute<int>("End", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            auto c = CreateAttribute<int>("Current", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            c->SetInternal(true);
            c->SetExternal(false);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override
        {
            int start = ctx.GetAttribute("Start").GetValue<int>();
            int end = ctx.GetAttribute("End").GetValue<int>();

            if (start < 0 || end < start)
            {
                return false;
            }

            auto schelNodes = Gex::CompoundNode::ToScheduledNodes();
            auto eval = Gex::NodeEvaluator(schelNodes, graphContext,
                                           profiler.GetProfiler(),
                                           false, 1);

            for (unsigned int i = start; i < end; i++)
            {
                ctx.GetAttribute("Current").SetValue(i);

                eval.Reset();
                eval.Run();
                
                // Pull values.
                PullInternalOutputs();
            }

            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(CounterIterBuilder, CounterIter)
}


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterNode<iter::IteratorBuilder>("Iter/Iterator");

    loader->RegisterNode<iter::ForRangeLoopBuilder>("Iter/ForRangeLoop");

    loader->RegisterNode<iter::TestIterBuilder>("Iter/TestIter");

    loader->RegisterNode<iter::CounterIterBuilder>("Iter/CounterIter");
}


#endif //NODELIBRARY2COMPLETE_ITER_H
