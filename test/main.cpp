#include "Gex/include/Gex.h"
#include <filesystem>





int main(int argc, char** argv)
{
    char* pluginPath = argv[0];
    char* graphF = argv[1];

    auto* graph = new Gex::CompoundNode();

    bool success = Gex::PluginLoader::LoadPlugin(pluginPath);

    Gex::Node* n1 = graph->CreateNode("String/Constant", "Node1");
    Gex::Node* n2 = graph->CreateNode("String/Constant", "Node2");

    Gex::Node* n3 = graph->CreateNode("String/Constant", "Node3");

    auto* at1 = n1->GetAttribute("String");
    auto* at3 = n3->GetAttribute("String");
    bool connected = at3->ConnectSource(at1);

    at1->Set(std::string("Value"));

    Gex::SaveGraph(graph, graphF);

    Gex::CompoundNode* lg = Gex::CompoundNode::FromNode(
            Gex::LoadGraph(graphF));

    Gex::Node* n_1 = lg->GetNode("Node1");
    auto* at_1 = n_1->GetAttribute("String");
    auto v = at_1->Get<std::string>();

    return 0;
}
