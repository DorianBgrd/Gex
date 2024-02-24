#ifndef NODELIBRARY2COMPLETE_ARRAYS_H
#define NODELIBRARY2COMPLETE_ARRAYS_H

#include "../export.h"
#include "Gex/include/Gex.h"
#include "Tsys/include/tsys.h"
#include "Tsys/include/defaultTypes.h"

namespace Arrays
{
    class Plugin_API Create: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<TSys::AnyValue>("Array", Gex::AttrValueType::Multi,
                                                   Gex::AttrType::Output);
        }

        bool Evaluate(Gex::EvaluationContext& ctx,
                      const Gex::GraphContext&,
                      Gex::NodeProfiler& profiler)
        {
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(CreateBuilder, Create)

    class Plugin_API Append: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<TSys::AnyValue>("InputArray", Gex::AttrValueType::Multi,
                                                   Gex::AttrType::Input);
            CreateAttribute<bool>("ConsolidateIndices", Gex::AttrValueType::Single,
                                  Gex::AttrType::Input);
            CreateAttribute<TSys::AnyValue>("Value", Gex::AttrValueType::Single,
                                                   Gex::AttrType::Input);
            CreateAttribute<TSys::AnyValue>("Result", Gex::AttrValueType::Multi,
                                                   Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            ctx.GetAttribute("Result").ClearMultiIndices();

            auto inputCtx = ctx.GetAttribute("InputArray");
            auto resultCtx = ctx.GetAttribute("Result");
            bool consolidate = ctx.GetAttribute("ConsolidateIndices").GetValue<bool>();
            auto value = ctx.GetAttribute("Value").GetValue<TSys::AnyValue>();

            std::vector<unsigned int> indices = inputCtx.GetIndices();

            resultCtx.ClearMultiIndices();
            if (!consolidate)
            {
                if(!indices.empty())
                {
                    unsigned int max = *std::max_element(indices.begin(), indices.end());
                    for (unsigned int idx : indices)
                    {
                        resultCtx.CreateIndex(idx, inputCtx.GetIndexValue<TSys::AnyValue>(idx));
                    }

                    resultCtx.CreateIndex(max + 1, value);
                }
                else
                {
                    resultCtx.CreateIndex(0, value);
                }

            }
            else
            {
                for (unsigned int i = 0; i < indices.size(); i++)
                {
                    resultCtx.CreateIndex(i, inputCtx.GetIndexValue<TSys::AnyValue>(indices.at(i)));
                }
                resultCtx.CreateIndex(indices.size(), value);
            }

            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(AppendBuilder, Append)


    class Plugin_API Remove: public Gex::Node
    {
    public:

        void InitAttributes() override
        {
            CreateAttribute<TSys::AnyValue>("InputArray", Gex::AttrValueType::Multi,
                                                   Gex::AttrType::Input);
            CreateAttribute<int>("Index", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            CreateAttribute<bool>("ConsolidateIndices", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            CreateAttribute<TSys::AnyValue>("Result", Gex::AttrValueType::Multi,
                                                   Gex::AttrType::Output);

        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            ctx.GetAttribute("Result").ClearMultiIndices();

            auto inputCtx = ctx.GetAttribute("InputArray");
            auto resultCtx = ctx.GetAttribute("Result");
            bool consolidate = ctx.GetAttribute("ConsolidateIndices").GetValue<bool>();
            auto index = ctx.GetAttribute("Index").GetValue<int>();

            std::vector<unsigned int> indices = inputCtx.GetIndices();

            if (!consolidate)
            {
                for (unsigned int idx : indices)
                {
                    if (idx == index)
                        continue;

                    resultCtx.SetIndexValue(idx, inputCtx.GetIndexValue<TSys::AnyValue>(idx));
                }
            }
            else
            {
                std::vector<TSys::AnyValue> values;
                for (unsigned int idx : indices)
                {
                    if (idx == index)
                        continue;

                    values.push_back(inputCtx.GetIndexValue<TSys::AnyValue>(idx));
                }

                for (unsigned int i = 0; i < values.size(); i++)
                {
                    resultCtx.SetIndexValue(i, values.at(i));
                }
            }

            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(RemoveBuilder, Remove)

    class Plugin_API Insert: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<TSys::AnyValue>("InputArray", Gex::AttrValueType::Multi,
                                                   Gex::AttrType::Input);
            CreateAttribute<int>("Index", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            CreateAttribute<TSys::AnyValue>("value", Gex::AttrValueType::Single,
                                                   Gex::AttrType::Input);
            CreateAttribute<TSys::AnyValue>("Result", Gex::AttrValueType::Multi,
                                                   Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            ctx.GetAttribute("Result").ClearMultiIndices();

            auto inputCtx = ctx.GetAttribute("InputArray");
            auto resultCtx = ctx.GetAttribute("Result");
            int index = ctx.GetAttribute("Index").GetValue<int>();

            resultCtx.ClearMultiIndices();

            std::vector<unsigned int> indices = inputCtx.GetIndices();
            for (unsigned int idx : indices)
            {
                if (idx == index)
                {
                    auto insertValue = ctx.GetAttribute("value").GetValue<TSys::AnyValue>();
                    resultCtx.CreateIndex(idx, insertValue);
                }

                unsigned int applyIndex = idx;
                if(idx >= index)
                {
                    applyIndex = idx + 1;
                }

                auto value = inputCtx.GetIndexValue<TSys::AnyValue>(idx);
                resultCtx.CreateIndex(applyIndex, value);
            }

            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(InsertBuilder, Insert)

    class Plugin_API Length: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<TSys::AnyValue>("InputArray", Gex::AttrValueType::Multi,
                                                   Gex::AttrType::Input);
            CreateAttribute<TSys::AnyValue>("Length", Gex::AttrValueType::Single,
                                                   Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            ctx.GetAttribute("Length").SetValue<int>((int)ctx.GetAttribute("InputArray").GetIndices().size());
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(LengthBuilder, Length)

    class Plugin_API IndexValue: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<TSys::AnyValue>("InputArray", Gex::AttrValueType::Multi,
                                                   Gex::AttrType::Input);
            CreateAttribute<int>("Index", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            CreateAttribute<TSys::AnyValue>("Value", Gex::AttrValueType::Single,
                                                   Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto index = ctx.GetAttribute("Index").GetValue<int>();

            Gex::Feedback status;
            Gex::NodeAttributeData inarray = ctx.GetAttribute("InputArray");

            TSys::AnyValue result = inarray.GetIndexValue<TSys::AnyValue>(
                    index, &status);

            ctx.GetAttribute("Value").SetValue(result);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(IndexValueBuilder, IndexValue)


    class Plugin_API FirstItem: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<TSys::AnyValue>("InputArray", Gex::AttrValueType::Multi,
                                                       Gex::AttrType::Input);
            CreateAttribute<TSys::AnyValue>("Value", Gex::AttrValueType::Single,
                                                       Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {

            Gex::Feedback status;
            Gex::NodeAttributeData inarray = ctx.GetAttribute("InputArray");

            auto indices = inarray.Indices();
            if (indices.empty())
            {
                return false;
            }

            auto first = std::min(indices.begin(), indices.end());

            ctx.GetAttribute("Value").SetValue(inarray.GetIndexValue<TSys::AnyValue>(*first));
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(FirstItemBuilder, FirstItem)


    class Plugin_API LastItem: public Gex::Node
    {
    public:
        void InitAttributes() override
        {
            CreateAttribute<TSys::AnyValue>("InputArray", Gex::AttrValueType::Multi,
                                                       Gex::AttrType::Input);
            CreateAttribute<int>("Index", Gex::AttrValueType::Single,
                                  Gex::AttrType::Output);
            CreateAttribute<TSys::AnyValue>("Value", Gex::AttrValueType::Single,
                                                       Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {

            Gex::Feedback status;
            Gex::NodeAttributeData inarray = ctx.GetAttribute("InputArray");

            auto indices = inarray.Indices();
            if (indices.empty())
            {
                return false;
            }

            unsigned int last = 0;
            for (auto idx : indices)
            {
                if (idx > last)
                    last = idx;
            }

            ctx.GetAttribute("Value").SetValue(inarray.GetIndexValue<TSys::AnyValue>(last));
            ctx.GetAttribute("Index").SetValue((int)last);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(LastItemBuilder, LastItem)


    class Plugin_API SliceArray: public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Slices an array.";
        }

        void InitAttributes() override
        {
            CreateAttribute<TSys::AnyValue>("InputArray", Gex::AttrValueType::Multi,
                                                       Gex::AttrType::Input);
            CreateAttribute<int>("Start", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            CreateAttribute<int>("End", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
            CreateAttribute<TSys::AnyValue>("OutputArray", Gex::AttrValueType::Multi,
                                                       Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto list = ctx.GetAttribute("InputArray").GetArrayValues<TSys::AnyValue>();

            int start = ctx.GetAttribute("Start").GetValue<int>();
            int end = ctx.GetAttribute("End").GetValue<int>();

            if (start < 0)
                start = list.size() - (start  - 1);

            if (end < 0)
                end = list.size() - (end - 1);

            ctx.GetAttribute("OutputArray").ClearMultiIndices();
            if (start > end)
                return true;

            int index = 0;
            for (int i = start; i < end; i++)
            {
                ctx.GetAttribute("OutputArray").CreateIndex(
                        index, list.at(i));
                index++;
            }

            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(SliceArrayBuilder, SliceArray)
}


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    // Arrays :
    loader->RegisterNode<Arrays::CreateBuilder>("Arrays/Create");

    loader->RegisterNode<Arrays::RemoveBuilder>("Arrays/Remove");

    loader->RegisterNode<Arrays::AppendBuilder>("Arrays/Append");

    loader->RegisterNode<Arrays::InsertBuilder>("Arrays/Insert");

    loader->RegisterNode<Arrays::SliceArrayBuilder>("Arrays/Slice");

    loader->RegisterNode<Arrays::LengthBuilder>("Arrays/Length");

    loader->RegisterNode<Arrays::IndexValueBuilder>("Arrays/IndexValue");

    loader->RegisterNode<Arrays::FirstItemBuilder>("Arrays/FirstItem");

    loader->RegisterNode<Arrays::LastItemBuilder>("Arrays/LastItem");
}


#endif //NODELIBRARY2COMPLETE_ARRAYS_H
