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


Gex::Feedback Gex::SaveGraph(Gex::Node *graph, const std::string& filepath)
{
    Feedback result;

    rapidjson::Document json;
    rapidjson::Value& dict = json.SetObject();

    NodeBuilder* builder = NodeFactory::GetFactory()->GetBuilder(graph->Type());
    if (!builder)
    {
        result.Set(Status::Failed, "Specified node type "
            + graph->Type() +  " is not registered.");
        return result;
    }

    builder->SaveNode(graph, dict, json);

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

Gex::Node* Gex::LoadGraph(const std::string& filepath, Feedback* success)
{
    rapidjson::Document doc;
    if (!LoadJsonFile(filepath, doc, success))
    {
        return nullptr;
    }

    auto* graph = NodeFactory::GetFactory()->LoadNode(doc.GetObject());
    if (!graph)
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


Gex::Feedback Gex::ExportAsCompound(NodeList nodes, Node* graph, const std::string& directory,
                                    const std::string& name, bool force)
{
    CompoundNode* graphCmp = CompoundNode::FromNode(graph);

    Feedback success;
    if (!graphCmp)
    {
        success.Set(Gex::Status::Failed,
                    "Provided graph node is "
                    "not a compound node.");
        return success;
    }

    Node* compound = graphCmp->ToCompound(nodes, true, false);

    rapidjson::Document json;
    rapidjson::Value& dict = json.SetObject();

    compound->Serialize(dict, json);

    std::filesystem::path rpath = directory;
    rpath.append(name + ".json");
    if (!force && std::filesystem::exists(rpath))
    {
        success.Set(Gex::Status::Failed,
                    "File already exist.");
        return success;
    }

    graphCmp->RemoveNode(compound);

    Gex::ExportToFile(rpath.string(), json, &success);
    return success;
}


//Gex::Feedback Gex::ExportNodes(Node* graph, NodeList nodes,
//                               const std::string& filepath)
//{
//    auto* exportGraph = new CompoundNode();
//
//    CompoundNode* cmpGraph = CompoundNode::FromNode(graph);
//    cmpGraph->DuplicateNodes(nodes, true, nullptr, exportGraph);
//
//    auto result = SaveGraph(exportGraph, filepath);
//
//    delete exportGraph;
//
//    return result;
//}


Gex::NodeList Gex::ImportNodes(const std::string& filepath,
                               CompoundNode* destinationNode,
                               Feedback* success)
{
    rapidjson::Document json;

    auto* importGraph = LoadGraph(filepath);

    auto* importCompound = CompoundNode::FromNode(importGraph);

    for (auto* node : importCompound->GetNodes())
    {
        destinationNode->AddNode(node);
    }

    auto res = importCompound->GetNodes();

    delete importGraph;

    return res;
}