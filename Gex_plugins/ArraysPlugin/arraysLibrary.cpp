#ifndef NODELIBRARY2COMPLETE_ARRAYS_H
#define NODELIBRARY2COMPLETE_ARRAYS_H

#include "../export.h"
#include "Gex/include/Gex/Gex.h"
#include "Tsys/tsys.h"
#include "Tsys/defaultTypes.h"

namespace Arrays
{
    class Plugin_API Create: public Gex::GenericNode
    {
    public:
        void InitAttributes() override
        {
            auto valueAttribute = CreateGenericAttribute(
                    "Value", Gex::AttrType::Input,
                    Gex::AttrValueType::Array,
                    {Gex::HasArrayTypeGuard}
            );

            auto arrayAttribute = CreateGenericAttribute(
                    "OutputArray", Gex::AttrType::Output,
                    Gex::AttrValueType::Single
            );

            LinkAttributes(valueAttribute, arrayAttribute, Gex::ToArrayType);
//            LinkAttributes(arrayAttribute, valueAttribute, Gex::ToSingleType);
        }

        bool Evaluate(Gex::NodeAttributeData& ctx,
                      Gex::GraphContext&,
                      Gex::NodeProfiler& profiler) override
        {
            auto values = ctx.GetAttribute("Value").GetAttributeArrayValues();

            auto array = ctx.GetAttribute("OutputArray");

            TSys::TypedValuePtr arrayValue;

            if (values.empty())
                return false;

            auto arrayType = array.GetAttributeType();

            if (arrayType)
            {
                arrayValue = TSys::GetRegistry()->CreateValue(arrayType);

                for (const auto& value : values)
                {
                    arrayValue->Append(value);
                }
            }

            array.SetAttributeValue(arrayValue);

            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(CreateBuilder, Create)

    class Plugin_API Append: public Gex::GenericNode
    {
    public:
        void InitAttributes() override
        {
            auto inputArray = CreateGenericAttribute(
                    "InputArray", Gex::AttrType::Input,
                    Gex::AttrValueType::Array,
                    {Gex::IsArrayTypeGuard, Gex::HasSingleTypeGuard}
            );

            auto inputValue = CreateGenericAttribute(
                    "InputValue", Gex::AttrType::Input,
                    Gex::AttrValueType::Single,
                    {Gex::IsSingleTypeGuard, Gex::HasArrayTypeGuard}
            );

            auto outputArray = CreateGenericAttribute(
                    "OutputArray", Gex::AttrType::Output,
                    Gex::AttrValueType::Array
            );

            LinkAttributes(inputValue, inputArray, Gex::ToArrayType);
            LinkAttributes(inputValue, outputArray, Gex::ToArrayType);

            LinkAttributes(inputArray, inputValue, Gex::ToSingleType);
            LinkAttributes(inputArray, outputArray, Gex::ToSameType);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto inputArray = ctx.GetAttribute("InputArray").GetAttributeValue();
            auto inputValue = ctx.GetAttribute("InputValue").GetAttributeValue();

            inputArray->Append(inputValue);

            ctx.GetAttribute("OutputArray").SetAttributeValue(inputArray);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(AppendBuilder, Append)


    class Plugin_API Remove: public Gex::GenericNode
    {
    public:

        void InitAttributes() override
        {
            auto inputArray = CreateGenericAttribute("InputArray", Gex::AttrType::Input);

            CreateAttribute<int>("Index", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);

            auto outputArray = CreateGenericAttribute("OutputArray", Gex::AttrType::Output);

            LinkAttributes(inputArray, outputArray);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto inputArray = ctx.GetAttribute("InputArray").GetAttributeValue();

            int index = ctx.GetAttribute("Index").Get<int>();

            inputArray->Remove(index);


            ctx.GetAttribute("OutputArray").SetAttributeValue(inputArray);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(RemoveBuilder, Remove)

    class Plugin_API Insert: public Gex::GenericNode
    {
    public:
        void InitAttributes() override
        {
            auto inputArray = CreateGenericAttribute(
                    "InputArray", Gex::AttrType::Input
            );

            CreateAttribute<int>(
                    "Index", Gex::AttrValueType::Single,
                    Gex::AttrType::Input
            );

            auto inputValue = CreateGenericAttribute(
                    "Value", Gex::AttrType::Input
            );

            auto outputArray = CreateGenericAttribute(
                    "OutputArray", Gex::AttrType::Output
            );

            LinkAttributes(inputArray, outputArray);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto inputArray = ctx.GetAttribute("InputArray").GetAttributeValue();
            auto outputArray = ctx.GetAttribute("OutputArray");

            auto inputValue = ctx.GetAttribute("Value").GetAttributeValue();

            int index = ctx.GetAttribute("Index").Get<int>();

            inputValue->Insert(index, inputValue);

            return outputArray.SetAttributeValue(inputValue);
        }
    };

    GENERATE_DEFAULT_BUILDER(InsertBuilder, Insert)

    class Plugin_API Length: public Gex::GenericNode
    {
    public:
        void InitAttributes() override
        {
            CreateGenericAttribute(
                    "InputArray", Gex::AttrType::Input
            );

            CreateAttribute<int>(
                    "Length", Gex::AttrValueType::Single,
                    Gex::AttrType::Output
            );
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            ctx.GetAttribute("Length").Set(
                    (int)ctx.GetAttribute("InputArray")
                    .GetAttributeValue()->Count()
            );
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(LengthBuilder, Length)

    class Plugin_API IndexValue: public Gex::GenericNode
    {
    public:
        void InitAttributes() override
        {
            auto inputArray = CreateGenericAttribute(
                    "InputArray", Gex::AttrType::Input
            );

            CreateAttribute<int>(
                    "Index", Gex::AttrValueType::Single,
                    Gex::AttrType::Input
            );

            auto outputValue = CreateGenericAttribute(
                    "Value", Gex::AttrType::Output
            );

            LinkAttributes(inputArray, outputValue);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto index = ctx.GetAttribute("Index").Get<int>();

            Gex::Feedback status;
            Gex::NodeAttributeData inputArray = ctx.GetAttribute("InputArray");

            auto result = inputArray.GetAttributeValue()->At(index);

            ctx.GetAttribute("Value").SetAttributeValue(result);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(IndexValueBuilder, IndexValue)


    class Plugin_API FirstItem: public Gex::GenericNode
    {
    public:
        void InitAttributes() override
        {
            auto inputArray = CreateGenericAttribute(
                    "InputArray", Gex::AttrType::Input
            );

            auto outputValue = CreateGenericAttribute(
                    "Value", Gex::AttrType::Output
            );

            LinkAttributes(inputArray, outputValue, Gex::ToSingleType);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {

            Gex::Feedback status;
            Gex::NodeAttributeData inarray = ctx.GetAttribute("InputArray");

            auto value = inarray.GetAttributeValue();

            if (!value->Count())
                return false;

            auto first = value->At(0);

            ctx.GetAttribute("Value").SetAttributeValue(first);

            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(FirstItemBuilder, FirstItem)


    class Plugin_API LastItem: public Gex::GenericNode
    {
    public:
        void InitAttributes() override
        {
            auto inputArray = CreateGenericAttribute("InputArray", Gex::AttrType::Input);

            CreateAttribute<int>("Index", Gex::AttrValueType::Single,
                                  Gex::AttrType::Output);

            auto outputValue = CreateGenericAttribute("Value", Gex::AttrType::Output);

            LinkAttributes(inputArray, outputValue, Gex::ToSingleType);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {

            Gex::Feedback status;
            Gex::NodeAttributeData inarray = ctx.GetAttribute("InputArray");

            auto last = inarray.GetAttributeValue()->Count() - 1;

            ctx.GetAttribute("Value").SetAttributeValue(
                    inarray.GetAttributeValue()->At(last)
            );

            ctx.GetAttribute("Index").Set((int)last);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(LastItemBuilder, LastItem)


    class Plugin_API SliceArray: public Gex::GenericNode
    {
    public:
        std::string Description() const override
        {
            return "Slices an array.";
        }

        void InitAttributes() override
        {
            CreateGenericAttribute("InputArray", Gex::AttrType::Input);

            CreateAttribute<int>("Start", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);

            CreateAttribute<int>("End", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);

            CreateGenericAttribute("OutputArray", Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto inputArray = ctx.GetAttribute("InputArray").GetAttributeValue();
//            auto outputArray = ctx.GetAttribute("InputArray").GetAttributeValue();

            auto count = (int)inputArray->Count();

//            outputArray->Clear();

            int start = ctx.GetAttribute("Start").Get<int>();
            int end = ctx.GetAttribute("End").Get<int>();

            int last = count - 1;

            if (start > last || start > end)
            {
                return true;
            }

            auto newArray = TSys::GetRegistry()->CreateValue(
                    ctx.GetAttribute("InputArray").GetAttributeType()
            );

            for (int iter = start; iter <= end; iter++)
            {
                newArray->Append(inputArray->At(iter));
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
