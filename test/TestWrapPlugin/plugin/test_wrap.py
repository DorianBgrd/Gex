import Gex
#
#
# def RegisterPlugin(loader: Gex.PluginLoader):
#     print("Loader : ", loader)

class DummyNode(Gex.Node):
    def __init__(self, *args, **kwargs):
        Gex.Node.__init__(self,  *args, **kwargs)


class DummyNodeBuilder(Gex.NodeBuilder):
    def __init__(self, *args, **kwargs):
        Gex.NodeBuilder.__init__(self,  *args, **kwargs)

    def CreateNode(self):
        return DummyNode()

def RegisterPlugin(loader):
    print("Loader : ", loader)

    loader.RegisterNode("Test/DummyNode", DummyNodeBuilder)
