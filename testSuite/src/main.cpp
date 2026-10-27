#include "Tests/ArrayNodes.h"
#include "Tests/ConditionNodes.h"

#include "Gex/PluginLoader.h"

#include <filesystem>


int main(int argc, char** argv)
{
    std::filesystem::path pluginDirName = "plugins";
    auto pluginDirPath = std::filesystem::absolute(pluginDirName);

    Gex::PluginLoader::AddSearchPath(pluginDirPath.string());

    TestSuite::Array::TestArrayNodes();
}