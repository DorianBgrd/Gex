import Gex


class PNode(Gex.Node):
    def __init__(self):
        Gex.Node.__init__(self)

    def Evaluate(self, *args, **kwargs):
        print("Evaluate")
        return True


class PBuilder(Gex.NodeBuilder):
    def CreateNode(self):
        return PNode()


def RegisterPlugin(loader):
    print(loader)

    loader.RegisterNode("TestNode", PBuilder)


# print(Gex)