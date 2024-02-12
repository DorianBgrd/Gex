#ifndef LIBRARY_STRINGLIBRARY_H
#define LIBRARY_STRINGLIBRARY_H

#endif //LIBRARY_STRINGLIBRARY_H



#include "include/Gex.h"


#include <string>

namespace StringNodes
{

    class ConstantString : public Gex::Node
    {
    public:
        using Node::Node;

        void InitAttributes() override
        {
            CreateAttribute<std::string>("String", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);
        }
    };


    class ConstantStringBuilder: public Gex::DefaultNodeBuilder
    {
    public:
        Gex::Node* CreateNode() const override
        {
            return new ConstantString();
        }
    };

    class StringsJoin: public Gex::Node
    {
    public:
        using Node::Node;

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Strings", Gex::AttrValueType::Multi,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("delimiter", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("ResultString", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &) override
        {
            std::string delimiter = ctx.GetAttribute("delimiter").GetValue<std::string>();
            std::vector<std::string> strings = ctx.GetAttribute("Strings").GetArrayValues<std::string>();

            std::string result;

            if (strings.empty())
            {
                return ctx.GetAttribute("ResultString").SetValue<std::string>("");
            }

            unsigned int lastIndex = strings.size() - 1;
            for (unsigned int i = 0; i < strings.size(); i++)
            {
                result += strings.at(i);
                if (i != lastIndex)
                    result += delimiter;
            }

            return ctx.GetAttribute("ResultString").SetValue<std::string>(result);
        }
    };

    class StringsJoinBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new StringsJoin();
        }
    };


    class StringSplit: public Gex::Node
    {
    public:
        using Node::Node;

        void InitAttributes() override
        {
            CreateAttribute<std::string>("String", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Delimiter", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("ResultStrings", Gex::AttrValueType::Multi,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &) override
        {
            std::string str = ctx.GetAttribute("String").GetValue<std::string>();
            std::string delimiter = ctx.GetAttribute("Delimiter").GetValue<std::string>();

            int index = 0;
            std::vector<std::string> substrings;

            std::string substring;
            unsigned int loop = 0;
            for (char c : str)
            {
                if (c == delimiter.at(index))
                {
                    index += 1;
                }
                else
                {
                    index = 0;
                }

                if (index >= delimiter.size())
                {
                    if (!substring.empty())
                    {
                        substrings.push_back(substring);
                    }
                    substring.clear();
                    index = 0;
                }
                else if (loop == (str.size() - 1))
                {
                    substring += c;
                    if (!substring.empty())
                    {
                        substrings.push_back(substring);
                    }
                }
                else
                {
                    substring += c;
                }

                loop += 1;
            }

            unsigned int multiIndex = 0;

            ctx.GetAttribute("ResultStrings").ClearMultiIndices();
            for (const std::string& substr : substrings)
            {
                ctx.GetAttribute("ResultStrings").CreateIndex(multiIndex, substr);
                multiIndex += 1;
            }

            return true;
        }
    };


    class StringSplitBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new StringSplit();
        }
    };


    class IsEmpty: public Gex::Node
    {
    public:
        using Node::Node;

        void InitAttributes() override
        {
            CreateAttribute<std::string>("String", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<bool>("IsEmpty", Gex::AttrValueType::Single,
                                  Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &) override
        {
            std::string str = ctx.GetAttribute("String").GetValue<std::string>();

            ctx.GetAttribute("IsEmpty").SetValue(str.empty());
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(IsEmptyBuilder, IsEmpty)
}


#define EXPORT "C" void __declspec(dllexport) __cdecl


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterNode<StringNodes::ConstantStringBuilder>("String/Constant");

    loader->RegisterNode<StringNodes::StringsJoinBuilder>("String/Join");

    loader->RegisterNode<StringNodes::StringSplitBuilder>("String/Split");

    loader->RegisterNode<StringNodes::IsEmptyBuilder>("String/IsEmpty");
}