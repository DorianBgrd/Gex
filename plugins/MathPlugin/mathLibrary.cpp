//
// Created by Dorian on 2/22/2022.
//

#include <iostream>

#include "Gex/include/Gex.h"

#include "../export.h"


namespace MathNodes
{
    class Plugin_API ConstantInt: public Gex::Node
    {
    public:
        using Node::Node;

        std::string Description() const override
        {
            return "Node containing constant int value.";
        }

        void InitAttributes() override
        {
            CreateAttribute<int>("Output", Gex::AttrValueType::Single,
                                 Gex::AttrType::Static);
        }
    };


    class Plugin_API ConstantIntBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new ConstantInt();
        }
    };


    class Plugin_API ConstantFloat: public Gex::Node
    {
    public:
        using Node::Node;

        std::string Description() const override
        {
            return "Node containing constant float value.";
        }

        void InitAttributes() override
        {
            CreateAttribute<float>("Output", Gex::AttrValueType::Single,
                                 Gex::AttrType::Static);
        }
    };


    class Plugin_API ConstantFloatBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new ConstantFloat();
        }
    };


    class Plugin_API ConstantDouble: public Gex::Node
    {
    public:
        using Node::Node;

        std::string Description() const override
        {
            return "Node containing constant double value.";
        }

        void InitAttributes() override
        {
            CreateAttribute<float>("Output", Gex::AttrValueType::Single,
                                   Gex::AttrType::Static);
        }
    };


    class Plugin_API ConstantDoubleBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new ConstantDouble();
        }
    };


    class Plugin_API Add2Ints: public Gex::Node
    {
    public:
        using Node::Node;

        std::string Description() const override
        {
            return "Dynamic node adding 2 int values.";
        }

        void InitAttributes() override
        {
            CreateAttribute<int>("Input1", Gex::AttrValueType::Single,
                                   Gex::AttrType::Input);
            CreateAttribute<int>("Input2", Gex::AttrValueType::Single,
                                   Gex::AttrType::Input);
            CreateAttribute<int>("Output", Gex::AttrValueType::Single,
                                   Gex::AttrType::Output);
       }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &graph,
                      Gex::NodeProfiler& profiler) override
        {
            auto input1 = ctx.GetAttribute("Input1").GetValue<int>();
            auto input2 = ctx.GetAttribute("Input2").GetValue<int>();
            return ctx.GetAttribute("Output").SetValue(input1 + input2);
        }
    };


    class Plugin_API Add2IntsBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new Add2Ints();
        }
    };


    class Plugin_API Add2Floats: public Gex::Node
    {
    public:
        using Node::Node;

        std::string Description() const override
        {
            return "Dynamic node adding 2 float values.";
        }

        void InitAttributes() override
        {
            CreateAttribute<float>("Input1", Gex::AttrValueType::Single,
                                   Gex::AttrType::Input);
            CreateAttribute<float>("Input2", Gex::AttrValueType::Single,
                                   Gex::AttrType::Input);
            CreateAttribute<float>("Output", Gex::AttrValueType::Single,
                                   Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto input1 = ctx.GetAttribute("Input1").GetValue<float>();
            auto input2 = ctx.GetAttribute("Input2").GetValue<float>();
            return ctx.GetAttribute("Output").SetValue(input1 + input2);
        }
    };


    class Plugin_API Add2FloatsBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new Add2Floats();
        }
    };


    class Plugin_API Add2Doubles: public Gex::Node
    {
    public:
        using Node::Node;

        std::string Description() const override
        {
            return "Dynamic node adding 2 double values.";
        }

        void InitAttributes() override
        {
            CreateAttribute<double>("Input1", Gex::AttrValueType::Single,
                                   Gex::AttrType::Input);
            CreateAttribute<double>("Input2", Gex::AttrValueType::Single,
                                   Gex::AttrType::Input);
            CreateAttribute<double>("Output", Gex::AttrValueType::Single,
                                   Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto input1 = ctx.GetAttribute("Input1").GetValue<double>();
            auto input2 = ctx.GetAttribute("Input2").GetValue<double>();
            return ctx.GetAttribute("Output").SetValue(input1 + input2);
        }
    };


    class Plugin_API Add2DoublesBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new Add2Doubles();
        }
    };

    class Plugin_API IntDynamicManual: public Gex::Node
    {
    public:
        using Node::Node;

        void InitAttributes() override
        {
            CreateAttribute<int>("Input", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input, nullptr);
            CreateAttribute<int>("Output", Gex::AttrValueType::Single,
                                 Gex::AttrType::Output, nullptr);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            int value = ctx.GetAttribute("Input").GetValue<int>();
            ctx.GetAttribute("Output").SetValue<int>(value);
            return true;
        }
    };


    class Plugin_API IntDynamicManualBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new IntDynamicManual();
        }
    };
}


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterNode<MathNodes::ConstantIntBuilder>("Math/ConstantInt");

    loader->RegisterNode<MathNodes::ConstantFloatBuilder>("Math/ConstantFloat");

    loader->RegisterNode<MathNodes::ConstantDoubleBuilder>("Math/ConstantDouble");

    loader->RegisterNode<MathNodes::Add2IntsBuilder>("Math/Add2Ints");

    loader->RegisterNode<MathNodes::Add2FloatsBuilder>("Math/Add2Floats");

    loader->RegisterNode<MathNodes::Add2DoublesBuilder>("Math/Add2Doubles");

    loader->RegisterNode<MathNodes::IntDynamicManualBuilder>("Math/IntDynamicManual");
}

