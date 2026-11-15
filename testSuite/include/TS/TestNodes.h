#ifndef TS_TESTNODES_H
#define TS_TESTNODES_H

#include <iostream>
#include <vector>

#include "Core.h"

#include "Tsys/typeSystem.h"
#include "Gex/NodeFactory.h"
#include "Gex/PluginLoader.h"
#include "Gex/Profiler.h"

namespace TS
{
    struct AttrField
    {
    public:
        std::string name;
        TSys::TypedValuePtr value;
        int index = -1;

        AttrField(const std::string& n,
                  const TSys::TypedValuePtr& v,
                  int idx=-1)
        {
            name = n;
            value = v;
            index = idx;
        }

        template<class T>
        AttrField(const std::string& n,
                  T v, int idx=-1)
        {
            name = n;
            value = TSys::GetRegistry()->WrapValue(v);
            index = idx;
        }

        Gex::AttributeWkPtr GetAttribute(const Gex::NodePtr& node) const
        {
            auto attr = node->GetAttribute(name);

            if (index >= 0)
            {
                if (!attr->CreateIndex(index))
                {
                    return {};
                }

                return attr->GetIndexAttribute(index);
            }

            return attr;
        }
    };


    class NodeTest: public TestCase
    {
        std::string nodeType;

        std::vector<AttrField> expectedInputs;
        std::vector<AttrField> expectedOutputs;

    public:
        NodeTest(const std::string& type,
                 const std::vector<AttrField>& expectedIn,
                 const std::vector<AttrField>& expectedOut)
        {
            nodeType = type;
            expectedInputs = expectedIn;
            expectedOutputs = expectedOut;
        }

        std::string Name() const override
        {
            return nodeType;
        }

        template<class ValueType>
        void AddExpectedInput(const std::string& name,
                              ValueType value)
        {
            expectedInputs.emplace_back(
                    name,
                    TSys::GetRegistry()->WrapValue(value)
            );
        }

        template<class ValueType>
        void AddExpectedOutput(const std::string& name,
                               ValueType value)
        {
            expectedOutputs.emplace_back(
                    name,
                    TSys::GetRegistry()->WrapValue(value)
            );
        }

        TestResult Test() override
        {
            auto node = Gex::NodeFactory::GetFactory()->CreateNode(
                    nodeType, nodeType
            );

            if (!node)
            {
                return {false, "Type " + nodeType + " does not exist."};
            }

            for (const auto& input : expectedInputs)
            {
                auto inputAttr = input.GetAttribute(node);
                if (!inputAttr)
                {
                    return {false, "Input attr " + input.name + " not found."};
                }

                inputAttr->SetValue(input.value);

                if (inputAttr->GetValue() != input.value)
                {
                    return {false,
                            "Input attr " + input.name
                            + " could not be set with a value of type "
                            + input.value->GetType()->GetName() + " ."};
                }
            }

            Gex::GraphContext context;
            auto profilter = Gex::MakeProfiler();

            auto nodeProfiler = Gex::EvaluationNodeProfiler(
                    profilter, node, "No Thread"
            );

            if (!node->Compute(context, nodeProfiler))
            {
                return {false, "Node evaluation Failed."};
            }

            for (const auto& output : expectedOutputs)
            {
                auto outputAttr = output.GetAttribute(node);
                if (!outputAttr)
                {
                    return {false, "Output attr " + output.name + " not found."};
                }

                outputAttr->SetValue(output.value);

                if (outputAttr->GetValue() != output.value)
                {
                    return {false,
                            "Output attr " + output.name
                            + " does not contain expected value of type "
                            + output.value->GetType()->GetName() + " ."};
                }
            }

            return {true, "Test Passed"};
        }
    };


    class NodesTest: public TestGroup
    {
        std::vector<std::string> _plugins;

    public:
        NodesTest(const std::vector<std::string>& plugins)
        {
            _plugins = plugins;
        }

        void Prepare() {
            for (const auto& plugin : _plugins)
            {
                Gex::PluginLoader::LoadPlugin(plugin);
            }
        }
    };
}

#endif //TS_TESTNODES_H
