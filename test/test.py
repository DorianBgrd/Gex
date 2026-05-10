import sys
import os

for path in sys.path:
    print(path)


import Gex

Gex.PluginLoader.LoadPlugin(r"D:\WORK\GEX\Gex\test\TestWrapPlugin")

print("Node types :")
for type in Gex.NodeTypes():
    print("\t", type)