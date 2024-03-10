#include "Gex/include/io.h"

#include <fstream>
#include <filesystem>

#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"


void Gex::ExportToFile(const std::string& filepath,
                       rapidjson::Document& json,
                       Feedback* result)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer write(buffer);
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    json.Accept(writer);

    std::filesystem::path file(filepath);
    std::filesystem::path directory = file.parent_path();
    if (!std::filesystem::exists(directory))
    {
        if (!std::filesystem::create_directories(file.parent_path()))
        {
            if (result)
                result->Set(Status::Failed, "Failed creating directory " + directory.string());
        }

        return;
    }

    std::ofstream stream;
    stream.open(filepath);

    stream << buffer.GetString();
    stream.close();

    result->Set(Gex::Status::Success, "");
}


Gex::Feedback Gex::SaveGraph(Gex::Graph *graph, const std::string& filepath)
{
    Feedback result;

    rapidjson::Document json;
    rapidjson::Value& dict = json.SetObject();

    graph->Serialize(dict, json);

    ExportToFile(filepath, json, &result);
    if (!result)
    {
        return result;
    }

    result.Set(Status::Success, "Successfully saved graph to " + filepath);
    return result;
}


Gex::Graph* Gex::LoadGraph(const std::string& filepath, Feedback* success)
{
    std::filesystem::path p(filepath);
    if (!std::filesystem::exists(p))
    {
        if (success)
            success->Set(Status::Failed, "File does not exist.");
        return nullptr;
    }

    std::ifstream filestream(p.string());

    std::string content((std::istreambuf_iterator<char>(filestream)),
                        (std::istreambuf_iterator<char>()));

    filestream.close();

    rapidjson::Document doc;
    doc.Parse(content.c_str());

    if (!doc.IsObject())
    {
        if (success)
            success->Set(Status::Failed, "Failed deserializing graph.");
        return nullptr;
    }

    auto* graph = new Graph();
    if (!graph->Deserialize(doc.GetObject()))
    {
        delete graph;
        if (success)
            success->Set(Status::Failed, "Failed deserializing graph.");
        return nullptr;
    }

    if (success)
        success->Set(Status::Success, "Successfully loaded graph.");
    return graph;
}


Gex::Feedback Gex::ExportNodes(NodeList nodes, Graph* graph, const std::string& directory,
                               const std::string& name, bool force)
{
    Node* compound = graph->ToCompound(nodes, true, false);

    rapidjson::Document json;
    rapidjson::Value& dict = json.SetObject();

    compound->Serialize(dict, json);

    Feedback success;

    std::filesystem::path rpath = directory;
    rpath.append(name + ".json");
    if (!force && std::filesystem::exists(rpath))
    {
        success.Set(Gex::Status::Failed,
                    "File already exist.");
        return success;
    }

    Gex::ExportToFile(rpath.string(), json, &success);
    return success;
}