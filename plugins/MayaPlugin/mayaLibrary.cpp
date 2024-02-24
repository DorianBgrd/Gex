#ifndef LIBRARY_MAYALIBRARY_H
#define LIBRARY_MAYALIBRARY_H


#include "Gex/include/Gex.h"

#include "../export.h"

#include <string>


namespace MayaNodes
{

    class Plugin_API MayaAction : public Gex::CompoundNode
    {
        void InitAttributes() override {
            CreateAttribute<std::string>("MayaFilePath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Static);
        }

    };


    class Plugin_API MayaOpenSceneCommand : public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Generates maya open scene command for specified file path.";
        }

        using Node::Node;

        void InitAttributes() override {
            CreateAttribute<std::string>("MayaFilePath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Command", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override {
            auto mayapath = ctx.GetAttribute("MayaFilePath").GetValue<std::string>();
            ctx.GetAttribute("Command").SetValue<std::string>("cmds.file(r\"" + mayapath + "\", open=True)");
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(MayaOpenSceneCommandBuilder, MayaOpenSceneCommand)


    class Plugin_API MayaRenameSceneCommand: public Gex::Node
    {
    public:

        std::string Description() const override
        {
            return "Renames current maya scene to specified filepath.";
        }

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Filepath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Command", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            std::string path = ctx.GetAttribute("Filepath").GetValue<std::string>();

            std::string cmd = "cmds.file(rename=r\"" + path + "\")";
            ctx.GetAttribute("Command").SetValue(cmd);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(MayaRenameSceneCommandBuilder, MayaRenameSceneCommand)


    class Plugin_API MayaSaveSceneCommand : public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Saves current scene to specified file path.";
        }
        void InitAttributes() override {
            CreateAttribute<std::string>("MayaFilePath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Command", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override {
            auto mayapath = ctx.GetAttribute("MayaFilePath").GetValue<std::string>();
            std::string cmd = ("import os\n"
                               "path = r\"" + mayapath + "\"\n" +
                               "if not os.path.exists(os.path.dirname(path)):\n"
                               "    os.makedirs(os.path.dirname(path))\n"
                               "if not path.endswith(\".ma\") and not path.endswith(\".mb\"):\n"
                               "    path += \".ma\"\n"
                               "cmds.file(save=True, type=\"mayaAscii\")");
            ctx.GetAttribute("Command").SetValue<std::string>(cmd);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(MayaSaveSceneCommandBuilder, MayaSaveSceneCommand)


    class Plugin_API MayaCreateNodeCommand : public Gex::Node
    {
    public:

        using Node::Node;

        void InitAttributes() override {
            CreateAttribute<std::string>("NodeType", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("NodeName", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Command", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override {
            auto nodetype = ctx.GetAttribute("NodeType").GetValue<std::string>();
            auto nodename = ctx.GetAttribute("NodeName").GetValue<std::string>();
            std::string cmd = "cmds.createNode(\"" + nodetype + "\"";
            if (!nodename.empty()) {
                cmd += ", name=\"" + nodename + "\"";
            }
            cmd += ")";
            ctx.GetAttribute("Command").SetValue<std::string>(cmd);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(MayaCreateNodeCommandBuilder, MayaCreateNodeCommand)


    class Plugin_API MayaCreateReferenceCommand: public Gex::Node
    {std::string Description() const override
        {
            return "References specified filepath to current scene.";
        }

        void InitAttributes() override {
            CreateAttribute<std::string>("Filepath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Namespace", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Command", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override {
            auto filepath = ctx.GetAttribute("Filepath").GetValue<std::string>();
            auto namespace_ = ctx.GetAttribute("Namespace").GetValue<std::string>();
            std::string cmd = "cmds.file(r\"" + filepath + "\", reference=True, namespace=\"" + namespace_ + "\")";
            ctx.GetAttribute("Command").SetValue<std::string>(cmd);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(MayaCreateReferenceCommandBuilder, MayaCreateReferenceCommand)


    class Plugin_API MayaImportFileCommand: public Gex::Node
    {
        std::string Description() const override
        {
            return "Imports specified file path to current scene.";
        }

        void InitAttributes() override {
            CreateAttribute<std::string>("Filepath", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Namespace", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("Command", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override {
            auto filepath = ctx.GetAttribute("Filepath").GetValue<std::string>();
            auto namespace_ = ctx.GetAttribute("Namespace").GetValue<std::string>();
            std::string cmd = "cmds.file(\"" + filepath + "\", i=True, namespace=\"" + namespace_ + "\")";
            ctx.GetAttribute("Command").SetValue<std::string>(cmd);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(MayaImportFileCommandBuilder, MayaImportFileCommand)


    class Plugin_API MayapyComposeCommand : public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Composes a mayapy compatible command.";
        }

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Commands", Gex::AttrValueType::Multi,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("ResultCommand", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            std::string command = "import maya.standalone\nmaya.standalone.initialize(\"python\")\nfrom maya import cmds\n";

            Gex::Attribute *commandsAttr = GetAttribute("Commands");
            std::vector<std::string> subCommands = commandsAttr->GetArrayValues<std::string>();
            std::cout << "Commands size : " + std::to_string(subCommands.size());
            if (!subCommands.empty())
            {
                for (std::string &subCommand: subCommands)
                {
                    command += "\n" + subCommand;
                }
            }

            ctx.GetAttribute("ResultCommand").SetValue(command);
            return true;
        }
    };


    GENERATE_DEFAULT_BUILDER(MayapyComposeCommandBuilder, MayapyComposeCommand)


    class Plugin_API MayapyLaunchCommand:public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Launches specified command using mayapy.";
        }

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Command", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            std::string command = ctx.GetAttribute("Command").GetValue<std::string>();

            std::string mayapyCmd = "mayapy.exe -c \"" + command + "\"";
            std::system(mayapyCmd.c_str());
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(MayapyLaunchCommandBuilder, MayapyLaunchCommand)


    class Plugin_API MayapyExecuteFile: public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Execute specified python file using mayapy.";
        }

        void InitAttributes() override
        {
            CreateAttribute<std::string>("PythonFile", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            std::string module = ctx.GetAttribute("PythonFile").GetValue<std::string>();
            std::string mayapyCmd = "mayapy.exe \"" + module + "\"";

            std::system(mayapyCmd.c_str());
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(MayapyExecuteFileBuilder, MayapyExecuteFile)


    class Plugin_API CreateMayaURI: public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Creates a maya URI from specified filepath.";
        }

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Identifier", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("File", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<std::string>("URI", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto identifier = ctx.GetAttribute("Identifier").GetValue<std::string>();
            auto file = ctx.GetAttribute("File").GetValue<std::string>();

            std::string uri = "pli:" + identifier + "?file=" + file;
            ctx.GetAttribute("URI").SetValue(uri);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(CreateMayaURIBuilder, CreateMayaURI)


    class Plugin_API LoadPlugin: public Gex::Node
    {
    public:
        std::string Description() const override
        {
            return "Loads maya plugin.";
        }

        void InitAttributes() override
        {
            CreateAttribute<std::string>("Plugin", Gex::AttrValueType::Single,
                                         Gex::AttrType::Input);
            CreateAttribute<bool>("Quiet", Gex::AttrValueType::Single,
                                  Gex::AttrType::Input);
            CreateAttribute<std::string>("Command", Gex::AttrValueType::Single,
                                         Gex::AttrType::Output);
        }

        bool Evaluate(Gex::NodeAttributeData &ctx,
                      Gex::GraphContext &,
                      Gex::NodeProfiler& profiler) override
        {
            auto plugin = ctx.GetAttribute("Plugin").GetValue<std::string>();
            auto quiet = ctx.GetAttribute("Quiet").GetValue<bool>();

            std::string strquiet = "False";
            if (quiet)
                strquiet = "True";

            std::string command = "cmds.loadPlugin(r\"" + plugin + "\", quiet=" + strquiet + ")";
            ctx.GetAttribute("Command").SetValue(command);
            return true;
        }
    };

    GENERATE_DEFAULT_BUILDER(LoadPluginBuilder, LoadPlugin)
}


extern EXPORT RegisterPlugin(Gex::PluginLoader* loader)
{
    loader->RegisterNode<MayaNodes::MayaOpenSceneCommandBuilder>("Maya/OpenScene");

    loader->RegisterNode<MayaNodes::MayaRenameSceneCommandBuilder>("Maya/RenameScene");

    loader->RegisterNode<MayaNodes::MayaCreateNodeCommandBuilder>("Maya/CreateNode");

    loader->RegisterNode<MayaNodes::MayaSaveSceneCommandBuilder>("Maya/SaveScene");

    loader->RegisterNode<MayaNodes::LoadPluginBuilder>("Maya/LoadPlugin");

    loader->RegisterNode<MayaNodes::MayaCreateReferenceCommandBuilder>("Maya/CreateReference");

    loader->RegisterNode<MayaNodes::MayaImportFileCommandBuilder>("Maya/ImportReference");

    loader->RegisterNode<MayaNodes::MayaImportFileCommandBuilder>("Maya/ImportFile");

    loader->RegisterNode<MayaNodes::MayapyComposeCommandBuilder>("Maya/ComposeCommand");

    loader->RegisterNode<MayaNodes::MayapyLaunchCommandBuilder>("Maya/LaunchMayapy");

    loader->RegisterNode<MayaNodes::MayapyExecuteFileBuilder>("Maya/Mayapy");

    loader->RegisterNode<MayaNodes::CreateMayaURIBuilder>("Maya/MakeURI");
}


#endif //LIBRARY_MAYALIBRARY_H