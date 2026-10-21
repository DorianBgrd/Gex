#ifndef NODELIBRARY2COMPLETE_ITER_H
#define NODELIBRARY2COMPLETE_ITER_H


#include "Gex/include/Gex/Gex.h"

#include "../export.h"

#include <string>
#include "Tsys/defaultTypes.h"

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

            auto inputArray = CreateGenericAttribute(
                    "input", Gex::AttrType::Input
            );

            auto idxat = CreateAttribute<int>(
                    "index", Gex::AttrValueType::Single,
                    Gex::AttrType::Input);

            auto in = CreateGenericAttribute(
                    "in", Gex::AttrType::Input
            );

            auto out = CreateGenericAttribute(
                    "out", Gex::AttrType::Output
             );

            auto outputArray = CreateGenericAttribute(
                    "output", Gex::AttrType::Output
            );

            LinkAttributes(inputArray, outputArray);

            LinkAttributes(inputArray, in, Gex::ToSingleType);

            LinkAttributes(inputArray, out, Gex::ToSingleType);
        }


        void Prepare(Gex::NodeAttributeData &ctx)
        {
            ctx.GetAttribute("output").ClearMultiIndices();
            indices = GetAttribute("input").ToShared()->ValidIndices();
            length = indices.size();
        }


        bool PreEvaluate(Gex::NodeAttributeData &ctx,
                         Gex::NodeAttributeData &inputctx,
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

            ctx.GetAttribute("index").Set<int>((int)idx);

            ctx.GetAttribute("in").SetAttributeValue(
                    ctx.GetAttribute("input")
                    .GetAttributeValue()->At(idx)
            );
            return true;
        }


        void Finalize()
        {
            currentIndex = -1;
            indices.clear();
            length = 0;
        }


        bool PostEvaluate(Gex::NodeAttributeData &ctx,
                          Gex::NodeAttributeData &outputctx,
                         Gex::GraphContext &graphContext,
                         Gex::NodeProfiler &profiler)
                        override
        {
            auto outputs = ctx.GetAttribute("output");

            auto idx = indices.at(currentIndex);
            outputs.CreateIndex(idx);
            outputs.GetAttributeValue()->Append(
                    ctx.GetAttribute("out")
                    .GetAttributeValue()
            );

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
        Gex::ScheduledItem compoundScheduledNodes;

    protected:
        std::vector<unsigned int> indices;

        void InitAttributes() override
        {
            Gex::CompoundNode::InitAttributes();

            auto inputArray = CreateGenericAttribute(
                    "InputArray", Gex::AttrType::Input
            );

            auto value = CreateGenericAttribute(
                    "InputValue", Gex::AttrType::Input
            );

            auto outValue = CreateGenericAttribute(
                    "OutputValue", Gex::AttrType::Output
            );

            auto outArray = CreateGenericAttribute(
                    "OutputArray", Gex::AttrType::Output
            );

            LinkAttributes(inputArray, value, Gex::ToSingleType);

            LinkAttributes(inputArray, outValue, Gex::ToSingleType);

            LinkAttributes(inputArray, outArray);
        }

        bool PreEvaluate(Gex::NodeAttributeData &ctx,
                         Gex::NodeAttributeData &inputctx,
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

//            if (!IsScheduled())
//            {
//                Schedule();
//            }

            evaluator.Run();

            return (evaluator.Status() == Gex::NodeEvaluator::EvaluationStatus::Done);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override
        {
            
            Gex::NodeEvaluator evaluator(
                    Schedule(), graphContext,
                    profiler.GetProfiler(), false, 1);

            ctx.GetAttribute("OutArray").ClearMultiIndices();

            auto inputArray = ctx.GetAttribute("InputArray").GetAttributeValue();

            for (unsigned int i = 0; i < inputArray->Count(); i++)
            {
                auto inputValue = inputArray->At(i);

                ctx.GetAttribute("InputValue").SetAttributeValue(inputValue);

                if (!IterEvaluate(evaluator, ctx, graphContext, profiler))
                {
                    return false;
                }

                auto outValue = ctx.GetAttribute("OutputValue").GetAttributeValue();

                ctx.GetAttribute("OutputArray").GetAttributeValue()->Append(outValue);
            }

            return true;
        }

        bool PostEvaluate(Gex::NodeAttributeData &ctx,
                          Gex::NodeAttributeData &outputctx,
                          Gex::GraphContext &graphContext,
                          Gex::NodeProfiler &profiler) override
        {
            return true;
        }

//        Gex::ScheduleNodePtrList ToScheduledNodes() override
//        {
//            compoundScheduledNodes = Gex::ScheduleNodes(GetNodes());
//
//            return {ToScheduledNode()};
//        }
    };

    GENERATE_DEFAULT_BUILDER(ForRangeLoopBuilder, ForRangeLoop)

    class Plugin_API TestIter: public Gex::CompoundNode
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<int>("ItGeom", Gex::AttrValueType::Single,
                                          Gex::AttrType::Input);

            std::vector<std::string> spaces = {"Object", "World"};

            TSys::Enum space(spaces);

            auto p = CreateAttribute<int>("Position", Gex::AttrValueType::Single,
                                              Gex::AttrType::Input);

            auto s = CreateAttributeFromValue("Space", TSys::GetRegistry()->WrapValue(space),
                                               Gex::AttrValueType::Single, Gex::AttrType::Input);

            auto n = CreateAttribute<int>("Normale", Gex::AttrValueType::Single,
                                              Gex::AttrType::Input);

            auto w = CreateAttribute<float>("Weight",  Gex::AttrValueType::Single,
                                             Gex::AttrType::Input);

            auto m = CreateAttribute<int>("Matrix",  Gex::AttrValueType::Single,
                                               Gex::AttrType::Input);

            auto op = CreateAttribute<int>("OutPosition",  Gex::AttrValueType::Single,
                                               Gex::AttrType::Output);
        }
    };

    GENERATE_DEFAULT_BUILDER(TestIterBuilder, TestIter)

    class Plugin_API CounterIter: public Gex::CompoundNode
    {
        std::string Description() const override
        {
            return "Performs a for loop using a counter.";
        }

//        Gex::ScheduleNodePtrList ToScheduledNodes() override
//        {
//            return {ToScheduledNode()};
//        }

        void InitAttributes() override
        {
            CreateAttribute<int>("Start", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            CreateAttribute<int>("End", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            auto c = CreateAttribute<int>("Current", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler) override
        {
            int start = ctx.GetAttribute("Start").Get<int>();
            int end = ctx.GetAttribute("End").Get<int>();

            if (start < 0 || end < start)
            {
                return false;
            }

            auto schelNodes = Gex::CompoundNode::Schedule();
            auto eval = Gex::NodeEvaluator(schelNodes, graphContext,
                                           profiler.GetProfiler(),
                                           false, 1);

            for (unsigned int i = start; i < end; i++)
            {
                ctx.GetAttribute("Current").Set(i);

                eval.Reset();
                eval.Run();
                
                // Pull values.
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
