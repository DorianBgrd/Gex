#include <filesystem>
#include "include/Gex.h"

#include "../export.h"


namespace Paths
{
    class JoinPaths: public Gex::Node
    {
    public:

        std::string Description() const override
        {
            return "Joins two paths";
        }

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Path1", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Path2", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("ResultPath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &) override
        {
            std::filesystem::path path1(ctx.GetAttribute("Path1").GetValue<std::string>());
            auto resPath = path1.append(ctx.GetAttribute("Path2").GetValue<std::string>());

            ctx.GetAttribute("ResultPath").SetValue(resPath.string());
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(JoinPathsBuilder, JoinPaths)


    class ParentDirectory: public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Get path parent directory.";
        }

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Path", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Directory", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &) override
        {
            std::filesystem::path path(ctx.GetAttribute("Path").GetValue<std::string>());

            ctx.GetAttribute("Directory").SetValue(path.parent_path().string());
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(ParentDirectoryBuilder, ParentDirectory)


    class SplitExtension: public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Split path extension.";
        }

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Path", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("OutPath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
            CreateAttribute<std::string>("Extension", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &) override
        {
            std::filesystem::path path(ctx.GetAttribute("Path").GetValue<std::string>());

            if (!std::filesystem::is_regular_file(path))
            {
                ctx.GetAttribute("OutPath").SetValue(path.parent_path().string());
                ctx.GetAttribute("Extension").SetValue("");
            }

            std::string ext = path.extension().string();
            std::string out = path.replace_extension("").string();
            ctx.GetAttribute("OutPath").SetValue(ext);
            ctx.GetAttribute("Extension").SetValue(out);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(SplitExtensionBuilder, SplitExtension)


    class MakeDirs: public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Make directories.";
        }

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Path", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &) override
        {
            std::filesystem::path path(ctx.GetAttribute("Path").GetValue<std::string>());

            if (!std::filesystem::exists(path))
            {
                std::filesystem::create_directories(path);
            }

            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(MakeDirsBuilder, MakeDirs)

}


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterNode<Paths::JoinPathsBuilder>("Path/Join");

    loader->RegisterNode<Paths::ParentDirectoryBuilder>("Path/Directory");

    loader->RegisterNode<Paths::SplitExtensionBuilder>("Path/Split");

    loader->RegisterNode<Paths::MakeDirsBuilder>("Path/MakeDirs");
}
