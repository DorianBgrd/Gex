//
// Created by Dorian on 6/7/2022.
//

#ifndef LIBRARY_FILES_H
#define LIBRARY_FILES_H

#endif //LIBRARY_FILES_H


#include "include/Gex.h"

#include <filesystem>
#include <fstream>

#include <string>
#include <cstdio>

#include "../export.h"


namespace FileNodes
{
//    class PipelineLibrary2_API TemporaryFile : public Gex::PythonCommandNode
//    {
//    public:
//        void InitAttributes() override
//        {
//            CreateAttribute<std::string>("Extension", Gex::AttrValueType::Single,
//                                         Gex::AttrType::Input);
//            CreateAttribute<std::string>("TemporaryFilePath", Gex::AttrValueType::Single,
//                                         Gex::AttrType::Output);
//        }
//
//        bool Evaluate(Gex::NodeAttributeData &ctx) override
//        {
//            std::string ext = ctx.GetAttribute("Extension").GetValue<std::string>();
//
//            return true;
//        }
//    };


    class Plugin_API SaveToPythonFile : public Gex::Node
    {
    public:
        using Gex::Node::Node;

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Directory", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Filename", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<bool>("UseTemporaryFilename", Gex::AttrValueType::Single,
                                  Gex::AttrType::Input);
            CreateAttribute<std::string>("Content", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("OutputFilepath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &) override
        {
            auto directory = ctx.GetAttribute("Directory").GetValue<std::string>();
            auto filename = ctx.GetAttribute("Filename").GetValue<std::string>();
            auto content = ctx.GetAttribute("Content").GetValue<std::string>();
            bool temp = ctx.GetAttribute("UseTemporaryFilename").GetValue<bool>();

            if (temp)
            {
                std::string tmpfilename = std::tmpnam(nullptr);

                const auto removeChar = '.';

                std::size_t found = tmpfilename.find(removeChar);
                while (found != std::string::npos)
                {
                    tmpfilename.replace(found, 1, "");
                    found = tmpfilename.find(removeChar);
                }
                tmpfilename += ".py";

                filename = std::filesystem::path(tmpfilename).filename().string();
            }

            if (directory.empty())
            {
                directory = std::filesystem::temp_directory_path().string();
            }

            std::filesystem::path result;
            result.append(directory);
            if (!std::filesystem::exists(result))
                std::filesystem::create_directories(result);

            result.append(filename);

            std::ofstream file;
            file.open(result.string());
            if (!content.empty())
                file << content;

            file.close();

            return ctx.GetAttribute("OutputFilepath").SetValue(result.string());
        }
    };


    class Plugin_API SaveToPyhonFileBuilder: public Gex::DefaultNodeBuilder
    {
        Gex::Node* CreateNode() const override
        {
            return new SaveToPythonFile();
        }
    };


    class Plugin_API CreateFile: public Gex::Node
    {
    public:
        using Gex::Node::Node;

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Path", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);
            CreateAttribute<bool>("Force", Gex::AttrValueType::Single,
                                  Gex::AttrType::Input);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &) override
        {
            auto cpath = ctx.GetAttribute("Path").GetValue<std::string>();
            auto force = ctx.GetAttribute("Force").GetValue<bool>();

            if (std::filesystem::exists(cpath) && !force)
            {
                return true;
            }

            auto directory = std::filesystem::path(cpath).parent_path();
            if (!std::filesystem::exists(directory))
            {
                std::filesystem::create_directories(directory);
            }

            std::ofstream file(cpath);
            file.close();

            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(CreateFileBuilder, CreateFile)
}


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    // File nodes :
    loader->RegisterNode<FileNodes::SaveToPyhonFileBuilder>("Files/SaveToPython");

    loader->RegisterNode<FileNodes::CreateFileBuilder>("Files/CreateFile");
}