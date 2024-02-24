#include "Gex/include/Gex.h"
#include <filesystem>


#define PLUGIN_PATH "D:\\WORK\\GEX\\Gex\\cmake-build-debug\\plugins\\StringPlugin\\StringPlugin.dll"
#define PLUGIN_DIR "D:\\WORK\\GEX\\Gex\\cmake-build-debug\\plugins"




int main(int argc, char** argv)
{
    auto* graph = new Gex::Graph();

    bool success = Gex::PluginLoader::LoadPlugin(PLUGIN_PATH);

    Gex::Node* n1 = graph->CreateNode("String/Constant", "Node1");
    Gex::Node* n2 = graph->CreateNode("String/Constant", "Node2");

    Gex::Node* n3 = graph->CreateNode("String/Constant", "Node3");

    auto* at1 = n1->GetAttribute("String");
    auto* at3 = n3->GetAttribute("String");
    bool connected = at3->ConnectSource(at1);

    at1->Set(std::string("Value"));

    Gex::SaveGraph(graph, "D:\\graph.json");

    Gex::Graph* lg = Gex::LoadGraph("D:\\graph.json");

    Gex::Node* n_1 = lg->FindNode("Node1");
    auto* at_1 = n_1->GetAttribute("String");
    auto v = at_1->Get<std::string>();

    return 0;
}
