import sys
import os

moduledir = sys.argv[1]
dfp = sys.argv[2]

sys.path.insert(0, moduledir)

import Gex

plugins_dir = moduledir
for element in os.listdir(plugins_dir):
    pdir = os.path.join(plugins_dir, element)
    if not os.path.isdir(pdir):
        continue

    for subelement in os.listdir(pdir):
        if not subelement.endswith(".dll"):
            continue

        plugin = os.path.join(pdir, subelement)
        loaded = Gex.PluginLoader.LoadPlugin(plugin)
        if loaded:
            print("Loaded plugin :", plugin)


graph = Gex.Graph()
print("Graph :", graph)

n1 = graph.CreateNode("String/Constant", "Node1")
print(n1)

n2 = graph.CreateNode("String/Constant", "Node2")
print(n2)

n3 = graph.CreateNode("String/Constant", "Node3")
print(n3)

at2 = n2.GetAttribute("String")
print(at2)

at1 = n1.GetAttribute("String")
print(at1)

at1.Set("Value")

print(at2.ConnectSource(at1))

feedback = Gex.Feedback()
print("Feedback :", feedback)

print(feedback.status)

feedback.status = Gex.Status.Success
print(feedback.status)

feedback = Gex.SaveGraph(graph, dfp)
print(feedback.status, feedback.message)

graph = Gex.LoadGraph(dfp, feedback=feedback)
print(graph)
print(feedback.status, feedback.message)