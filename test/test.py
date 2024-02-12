import sys
sys.path.insert(0, r"D:\WORK\GEX\Gex\cmake-build-release\python")

import Gex

loaded = Gex.PluginLoader.LoadPlugin(r"D:\WORK\GEX\Gex\cmake-build-release\plugins\ArraysPlugin\ArraysPlugin.dll")
print("Loaded :", loaded)

graph = Gex.Graph()
print("Graph :", graph)

n1 = graph.CreateNode("Arrays/Create", "Node1")
print(n1)

n2 = graph.CreateNode("Arrays/FirstItem", "Node2")
print(n2)

n3 = graph.CreateNode("Arrays/Create", "Node1")
print(n3)

at2 = n2.GetAttribute("InputArray")
print(at2)

at1 = n1.GetAttribute("Array")
print(at1)

print(at2.ConnectSource(at1))
