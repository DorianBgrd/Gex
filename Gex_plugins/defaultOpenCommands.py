import os

from PipelineLibrary import proxy


# Notepad
class NotepadOpenCommand(proxy.OpenCommand):
    def __init__(self, arg1, arg2):
        proxy.OpenCommand.__init__(self, arg1,  arg2)
    def OpenFilepath(self, filepath):
        return os.system("notepad \"{}\"".format(filepath))
        # return 0

class NotepadOpenCommandBuilder(proxy.CommandBuilder):
    def GenerateCommand(self, identifier, filename):
        return NotepadOpenCommand(identifier, filename)


# Blender
class BlenderOpenCommand(proxy.OpenCommand):
    def __init__(self, arg1, arg2):
        proxy.OpenCommand.__init__(self, arg1,  arg2)
    def OpenFilepath(self, filepath):
        return os.system("blender \"{}\"".format(filepath))
        # return 0

class BlenderOpenCommandBuilder(proxy.CommandBuilder):
    def GenerateCommand(self, identifier, filename):
        return BlenderOpenCommand(identifier, filename)


# Maya
class MayaOpenCommand(proxy.OpenCommand):
    def __init__(self, arg1, arg2):
        proxy.OpenCommand.__init__(self, arg1,  arg2)
    def OpenFilepath(self, filepath):
        if os.path.exists(filepath):
            command = "file -o \"{}\"".format(filepath)
        else:
            command = "file -rename \"{}\"".format(filepath)

        return os.system("maya -command {}".format(filepath, command))
        # return 0

class MayaOpenCommandBuilder(proxy.CommandBuilder):
    def GenerateCommand(self, identifier, filename):
        return MayaOpenCommand(identifier, filename)


class HoudiniApprenticeOpenCommand(proxy.OpenCommand):
    def __init__(self, arg1, arg2):
        proxy.OpenCommand.__init__(self, arg1,  arg2)
    def OpenFilepath(self, filepath):
        return os.system("happrentice -apprentice {}".format(filepath))


class HoudiniApprenticeOpenCommandBuilder(proxy.CommandBuilder):
    def GenerateCommand(self, identifier, filename):
        return HoudiniApprenticeOpenCommand(identifier, filename)



def RegisterPlugin(loader):
    proxy.InitializeContext(loader.GetMainContext())

    loader.RegisterOpenCommandBuilder(
        "Notepad", NotepadOpenCommandBuilder()
    )

    loader.RegisterOpenCommandBuilder(
        "Blender", BlenderOpenCommandBuilder()
    )

    loader.RegisterOpenCommandBuilder(
        "Maya", MayaOpenCommandBuilder()
    )

    loader.RegisterOpenCommandBuilder(
        "HoudiniApprentice",
        HoudiniApprenticeOpenCommandBuilder()
    )


