#include "include/Graph.h"
#include "include/Node.h"
#include "include/PluginLoader.h"


#define PLUGIN_PATH "D:\\WORK\\GEX\\Gex\\cmake-build-debug\\plugins\\ArraysPlugin\\ArraysPlugin.dll"


int main(int argc, char** argv)
{
    auto* graph = new Gex::Graph();

    bool success = Gex::PluginLoader::LoadPlugin(PLUGIN_PATH);

    Gex::Node* n1 = graph->CreateNode("Arrays/Create", "Node1");
    Gex::Node* n2 = graph->CreateNode("Arrays/Create", "Node2");

    Gex::Node* n3 = graph->CreateNode("Arrays/FirstItem", "Node3");

    auto* at1 = n1->GetAttribute("Array");
    auto* at3 = n3->GetAttribute("InputArray");
    bool connected = at3->ConnectSource(at1);

    return 0;
}
