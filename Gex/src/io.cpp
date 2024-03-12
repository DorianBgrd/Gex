#include "Gex/include/io.h"
#include "Gex/include/NodeFactory.h"

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


bool LoadJsonFile(const std::string& filepath, rapidjson::Document& json,
                  Gex::Feedback* success=nullptr)
{
    std::filesystem::path p(filepath);
    if (!std::filesystem::exists(p))
    {
        if (success)
            success->Set(Gex::Status::Failed, "File does not exist.");
        return false;
    }

    std::ifstream filestream(p.string());

    std::string content((std::istreambuf_iterator<char>(filestream)),
                        (std::istreambuf_iterator<char>()));

    filestream.close();
    json.Parse(content.c_str());

    if (!json.IsObject())
    {
        if (success)
            success->Set(Gex::Status::Failed, "Failed deserializing graph.");
        return false;
    }

    return true;
}

Gex::Graph* Gex::LoadGraph(const std::string& filepath, Feedback* success)
{
    rapidjson::Document doc;
    if (!LoadJsonFile(filepath, doc, success))
    {
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


Gex::Feedback Gex::ExportAsCompound(NodeList nodes, Graph* graph, const std::string& directory,
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

    graph->RemoveNode(compound);

    Gex::ExportToFile(rpath.string(), json, &success);
    return success;
}


Gex::Feedback Gex::ExportNodes(Graph* graph, NodeList nodes,
                               const std::string& filepath)
{
    auto* exportGraph = new Graph();

    graph->DuplicateNodes(nodes, true, nullptr, exportGraph);

    auto result = SaveGraph(exportGraph, filepath);

    delete exportGraph;

    return result;
}


Gex::NodeList Gex::ImportNodes(const std::string& filepath,
                               Graph* destinationGraph,
                               CompoundNode* destinationNode,
                               Feedback* success)
{
    rapidjson::Document json;

    auto* importGraph = LoadGraph(filepath);;

    for (auto* node : importGraph->Nodes())
    {
        if (destinationGraph)
            destinationGraph->AddNode(node);
        else
            destinationNode->AddInternalNode(node);
    }

    auto res = importGraph->Nodes();

    delete importGraph;

    return res;
}