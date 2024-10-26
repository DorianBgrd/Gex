#include "Gex/include/References.h"
#include "Gex/include/utils.h"

#include <filesystem>


Gex::References* Gex::References::instance = nullptr;


#define GEX_REFERENCES_PATH_ENV "GEX_REFERENCES_PATH"
#define ENV_DELIMITER ';'


Gex::References::References()
{
    size_t resSize = 0;
    char* envResult = nullptr;
    std::string envStr = Utils::GetEnv(GEX_REFERENCES_PATH_ENV);

    if (!envStr.empty())
    {
        std::vector<std::string> envDirs;
        while(!envStr.empty())
        {
            size_t index = envStr.find_first_of(ENV_DELIMITER);

            directories.push_back(envStr.substr(0, index));

            if (index == std::string::npos)
                break;

            envStr = envStr.substr(index, std::string::npos);
        }
    }
}


Gex::References* Gex::References::GetLoader()
{
    if (!instance)
        instance = new References();

    return instance;
}


void Gex::References::AddDirectory(const std::string& dir)
{
    directories.push_back(dir);
}


bool Gex::References::RemDirectory(const std::string& dir)
{
    auto iter = std::find(directories.begin(), directories.end(), dir);
    if (iter == directories.end())
        return false;

    directories.erase(iter);
    return true;
}


std::string Gex::References::ResolvePath(const std::string& name) const
{
    std::filesystem::path namePath(name);
    if (namePath.extension() != ".json")
        namePath += ".json";

    for (const auto& d : directories)
    {
        std::filesystem::path dir(d);
        std::filesystem::path nodePath = dir/namePath;

        if (std::filesystem::exists(nodePath))
        {
            return nodePath.string();
        }
    }

    return "";
}


void IterPaths(const std::filesystem::path& path,
               const std::filesystem::path& root,
               std::vector<std::string>& entries)
{
    for (auto p : std::filesystem::directory_iterator(path))
    {
        if (p.is_regular_file() && p.path().extension() == ".json")
        {
            entries.push_back(std::filesystem::relative(p.path(), root)
                .replace_extension().string());
        }
        else if (p.is_directory())
        {
            IterPaths(p.path(), root, entries);
        }
    }
}


std::vector<std::string> Gex::References::GetAvailableReferences() const
{
    std::vector<std::string> references;
    for (auto dir : directories)
    {
        IterPaths(dir, dir, references);
    }

    return references;
}


std::vector<std::string> Gex::References::GetDirectories() const
{
    return directories;
}
