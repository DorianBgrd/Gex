#ifndef NODELIBRARY2COMPLETE_DEBUGLIBRARY_H
#define NODELIBRARY2COMPLETE_DEBUGLIBRARY_H

#include <iostream>

#include "Gex/include/Gex.h"
#include "Tsys/include/defaultTypes.h"

#include "../export.h"

#include <chrono>
#include <thread>
#include "windows.h"

namespace Debug
{
    class Plugin_API Print: public Gex::Node
    {
    public:
        using Gex::Node::Node;

        void InitAttributes() override
        {
            std::cout << "[DEBUG][Print] : InitAttributes" << std::endl;

            CreateAttribute<TSys::AnyValue>("Attribute1");
        }

        bool Evaluate(Gex::NodeAttributeData &data,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            std::cout << "[DEBUG][Print] : Evaluate" << std::endl;
            return true;
        }

        void AttributeChanged(const Gex::AttributePtr& attribute,
                              const Gex::AttributeChange& change) override
        {
            std::cout << "[DEBUG][Print] : AttributeChanged " << attribute->Name() << " " << (int)change << std::endl;
        }
    };


    class Plugin_API PrintBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new Print();
        }
    };


    class Plugin_API Wait: public Gex::Node
    {
    public:
        using Gex::Node::Node;

        void InitAttributes() override
        {
            CreateAttribute<int>("WaitingTime", Gex::AttrValueType::Single,
                                 Gex::AttrType::Input);
        }

        bool Evaluate(Gex::NodeAttributeData &data,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            int w = data.GetAttribute("WaitingTime").GetValue<int>();
            std::cout << "Starting waiting for " << std::to_string(w) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(w));
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(WaitBuilder, Wait)


    class Plugin_API Blocker: public Gex::Node
    {
    public:
        using Gex::Node::Node;

        void InitAttributes() override
        {
            CreateAttribute<TSys::AnyValue>(
                    "Input", Gex::AttrValueType::Single,
                    Gex::AttrType::Input);
            CreateAttribute<TSys::AnyValue>(
                    "Output", Gex::AttrValueType::Single,
                    Gex::AttrType::Output);
            CreateAttribute<bool>(
                    "Block", Gex::AttrValueType::Single,
                    Gex::AttrType::Input);
        }

        bool Evaluate(Gex::NodeAttributeData &data,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            bool block = data.GetAttribute("Block").GetValue<bool>();
            if (block)
                return false;

            return data.GetAttribute("Output").SetValue(
                    data.GetAttribute("Input").GetValue<TSys::AnyValue>());
        }
    };


    GENERATE_DEFAULT_BUILDER(BlockerBuilder, Blocker)
}


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterNode<Debug::PrintBuilder>("Debug/Print");

    loader->RegisterNode<Debug::WaitBuilder>("Debug/Wait");

    loader->RegisterNode<Debug::BlockerBuilder>("Debug/Block");
}


#endif //NODELIBRARY2COMPLETE_DEBUGLIBRARY_H
