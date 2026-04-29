import os.path
import subprocess
from PipelineLibrary import proxy


class BlenderPy(proxy.Node):
    def InitAttributes(self):
        self.CreateAttribute("ScriptPath", str, proxy.AttrType.Static,
                             proxy.AttrValueType.Single)

    def Evaluate(self, ctx, gctx):
        fp = ctx.GetAttribute("ScriptPath").GetValue()

        ret = subprocess.call(["blender", "--background", "--python",  fp])
        return not ret


class BlenderPyBuilder(proxy.NodeBuilder):
    def CreateNode(self):
        return BlenderPy()


class SaveBlenderFile(proxy.Node):
    def InitAttributes(self):
        self.CreateAttribute("FilePath", str, proxy.AttrType.Static,
                             proxy.AttrValueType.Single)
        self.CreateAttribute("Command", str, proxy.AttrType.Output,
                             proxy.AttrValueType.Single)

    def Evaluate(self, ctx, gctx):
        fp = ctx.GetAttribute("FilePath").GetValue()

        directory = os.path.dirname(fp)
        if not os.path.exists(directory):
            os.makedirs(directory)

        code = "import bpy\nbpy.ops.wm.save_as_mainfile(filepath=r\"{}\")".format(fp)

        ctx.GetAttribute("Command").SetValue(code)
        return True


class SaveBlenderFileBuilder(proxy.NodeBuilder):
    def CreateNode(self):
        return SaveBlenderFile()


def RegisterPlugin(loader):
    loader.InitializeContext()

    loader.RegisterNode("Blender/BlenderPy", BlenderPyBuilder())

    loader.RegisterNode("Blender/SaveBlenderFile", SaveBlenderFileBuilder())

    return True
