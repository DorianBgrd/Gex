#ifndef TS_TESTNODES_H
#define TS_TESTNODES_H

#include <iostream>
#include <vector>

#include "Tsys/typeSystem.h"
#include "Gex/NodeFactory.h"
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


    struct TestResult
    {
        bool success;
        std::string message;
    };


    class NodeTest
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

        std::string Type() const
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

        TestResult Test()
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


    auto& Stream(bool err)
    {
        return err ? std::cout : std::cerr;
    }

    void PrintLine(char c, int n, bool err=false)
    {
        std::string s;
        for (int i =0 ; i < n; i++)
        {
            s += c;
        }

        Stream(err) << s << std::endl;
        Stream(err).flush();
    }

    void Print(const std::string& str, bool err=false)
    {
        Stream(err) << str << std::endl;
        Stream(err).flush();
    }

    void PrintRet(bool err=false)
    {
        Stream(err) << std::endl;
    }

    void TestNodes(const std::vector<NodeTest>& testNodes)
    {
        for (auto testNode : testNodes)
        {

            auto result = testNode.Test();

            std::string fmsg = result.success ? "Succeeded" : "FAILED";

//            PrintLine('-', 100);
//            Print("Testing node " + testNode.Type());
//            PrintLine('-', 100);

            PrintLine('-', 100, result.success);
            Print("Testing node " + testNode.Type(), result.success);
            Print("Test " + fmsg + " !", result.success);
            Print("Feedback : " + result.message, result.success);
            PrintLine('-', 100, result.success);
            PrintRet(result.success);
        }
    }
}

#endif //TS_TESTNODES_H
