import sys
sys.path.insert(0, r"D:\WORK\GEX\Gex\cmake-build-debug\Gex\python")
sys.path.insert(0, r"D:\WORK\GEX\Gex\test")

import Gex

print()
print("Loading Test python plugin.")
feedback = Gex.Feedback()
result = Gex.PluginLoader.LoadPlugin(r"D:\WORK\GEX\Gex\test\TestPlugin\TestPlugin.json", feedback)
print("Success :", result)
print(feedback.status, ":", feedback.message)
print()


print("Loading Math python plugin.")
feedback = Gex.Feedback()
result = Gex.PluginLoader.LoadPlugin(r"D:\WORK\GEX\Gex\cmake-build-debug\plugins\MathPlugin\MathPlugin", feedback)
print("Success :", result)
print(feedback.status, ":", feedback.message)

print()
print("Node types :")
for n in Gex.NodeTypes():
    print(n)
print()

print("Available references graphs :")
refs = Gex.References.GetLoader()

for r in refs.GetDirectories():
    print(r)

for r in refs.GetAvailableReferences():
    print(r)


print(refs.ResolvePath("usd/UsdGraph1"))

node1 = Gex.CreateNode("CompoundNode", "CompoundNode")
print(node1, bool(node1))
node1weak = node1.GetAttribute("Next").Node()
print(node1, bool(node1))
del node1
print(node1weak, bool(node1weak))
print("------>", node1weak.Name())

pnode = Gex.CreateNode("TestNode", "TestNode")
print(pnode)

print("Attributes :")
nextAttr = pnode.GetAttribute("Next")
for at in pnode.GetAttributes():
    print("\t", at.Name())
print(nextAttr, bool(nextAttr))

nodeWk = nextAttr.Node()
print("Node Weak :", nodeWk)
print("Node Weak -> ToNode() :", nodeWk)
# print("Node To Weak ref() :", node1)

print("__call__() -> ", bool(nodeWk))
print("__call__() (expired) -> ", bool(nodeWk))


stack_node = Gex.CreateNode("Math/Add2Doubles", "BatchNode")

Gex.Undo.OpenGroup("Batch")

stack_node.GetAttribute("InputA").Set(5)
stack_node.GetAttribute("InputB").Set(10)

Gex.Undo.CloseGroup()

Gex.Undo.Undo()

print("InputA :", stack_node.GetAttribute("InputA").Get())
print("InputB :", stack_node.GetAttribute("InputB").Get())
