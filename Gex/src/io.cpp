#include "Gex/include/io.h"

#include <fstream>
#include <filesystem>

#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

Gex::Feedback Gex::SaveGraph(Gex::Graph *graph, const std::string& filepath)
{
    Feedback result;

    rapidjson::Document json;
    rapidjson::Value& dict = json.SetObject();

    graph->Serialize(dict, json);

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
            result.Set(Status::Failed, "Failed creating directory " + directory.string());
        }
        return result;
    }

    std::ofstream stream;
    stream.open(filepath);

    stream << buffer.GetString();
    stream.close();

    result.Set(Status::Success, "Successfully saved graph to " + filepath);
    return result;
}


Gex::Graph* Gex::LoadGraph(std::string filepath, Feedback* success)
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