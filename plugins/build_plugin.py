import os
import sys
import json
import shutil

import argparse


def build_config(name, plugin, ptype="C++",
                 env=None, python_env=None,
                 python_path=None):
    config = {
        "name": name,
        "plugin": plugin,
        "type": ptype
    }

    if env:
        config["env"] = env

    if python_env:
        config["python_env"] = python_env

    if python_path:
        config["python-path"] = python_path

    return config


def build_plugin_file(directory, name, plugin,
                      ptype="C++", env=None,
                      python_env=None,
                      python_path=None):
    config = build_config(name, plugin, ptype=ptype,
                          env=env, python_env=python_env,
                          python_path=python_path)

    json_file = os.path.join(directory, "{}.json".format(name))

    print(config)
    with open(json_file, "w") as f:
        json.dump(config, f, indent=4)

    return json_file


def search_file(filename, paths):
    if os.path.exists(filename):
        return filename

    for directory in paths:
        search_path = os.path.join(directory, filename)
        if os.path.exists(search_path):
            return search_path

    return None


def build_plugin(destination, name, plugin, plugin_file=None,
                 type="C++", scripts=None, env=None, python_env=None,
                 python_path=None, package=False, search_paths=None,
                 libs=None, verbose=True, strict=False):
    default_paths = os.environ.get("path").split(os.pathsep)
    default_paths.extend(search_paths or [])
    default_paths.insert(0, "")
    default_paths.insert(1, os.getcwd())

    dest_dir = destination
    if package:
        dest_dir = os.path.join(dest_dir, name)

    lib_dest_dir = (dest_dir if not package else
                    os.path.join(dest_dir, "libs"))

    plugin_dest_dir = (dest_dir if not package else
                       os.path.join(dest_dir, "plugin"))

    scripts_dest_dir = (dest_dir if not package else
                        os.path.join(dest_dir, "scripts"))

    for d in (dest_dir, lib_dest_dir, plugin_dest_dir,
              scripts_dest_dir):
        if not os.path.exists(d):
            os.makedirs(d)

    for script in scripts:
        if not os.path.exists(script):
            continue

        destination_scripts = os.path.join(
            scripts_dest_dir, os.path.split(script)[-1])

        if os.path.isfile(script):
            if os.path.exists(destination_scripts):
                os.remove(destination_scripts)

            shutil.copy2(script, destination_scripts)
        else:
            if os.path.exists(destination_scripts):
                shutil.rmtree(destination_scripts)

            shutil.copytree(script, destination_scripts)

    # Copy library.
    for lib in libs:
        matching_file = search_file(lib, search_paths)
        if matching_file:
            destination_file = os.path.join(
                lib_dest_dir, os.path.split(matching_file)[-1])
            shutil.copy2(matching_file, destination_file)
        else:
            if verbose:
                print(f"# Warning : Could not find lib {lib}")

            elif strict:
                raise RuntimeError(f"Could not find lib {lib}")

    # If plugin is a C++ plugin, then a file ,ust have been provided
    # as plugin argument. This plugin file should be copied to the
    # package or current directory.
    rel_plugin_file = plugin
    if type == 0 and not plugin_file:
        plugin_file = plugin

    if plugin_file:
        plugin_filepath = search_file(plugin_file, search_paths)
        if not plugin_filepath:
            raise RuntimeError(f"Could not find plugin file: {plugin_file}")

        shutil.copy2(plugin_filepath, plugin_dest_dir)
        final_plugin_file = os.path.join(plugin_dest_dir,
                                         os.path.split(plugin)[1])

        rel_plugin_file = os.path.join(".", os.path.relpath(final_plugin_file, dest_dir))

    return build_plugin_file(dest_dir, name, rel_plugin_file,
                             ptype=type, env=env,
                             python_env=python_env,
                             python_path=python_path)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="Build plugin",
        description="Builds a plugin file, and optionaly"
                    "an associated package."
    )

    parser.add_argument("-n", "--name", type=str,
                        required=True, nargs=1)

    parser.add_argument("-p", "--plugin", type=str,
                        required=True, nargs=1)

    parser.add_argument("-pf", "--plugin-file", type=str,
                        required=False, nargs=1)

    ptypes = ["C++", "Python"]
    parser.add_argument("-t", "--type", type=str, choices=ptypes,
                        nargs=1, required=True, dest="type")

    parser.add_argument("-e", "--env", type=str, nargs="*",
                        required=False)

    parser.add_argument("-pe", "--python-env", type=str, nargs="*",
                        required=False)

    parser.add_argument("-pp", "--python-path", type=str, nargs="*",
                        required=False, dest="python_path")

    parser.add_argument("-pc", "--package", action="store_true",
                        required=False, dest="package", default=False)

    parser.add_argument("-lsp", "--libs-search-path", type=str, nargs="*",
                        required=False, dest="libs_search_paths")

    parser.add_argument("-l", "--libs", type=str, nargs="*",
                        required=False, dest="libs")

    parser.add_argument("-sc", "--scripts", type=str, nargs="*",
                        required=False, dest="scripts")

    parser.add_argument("--destination", "-d", type=str, nargs="?",
                        default=os.getcwd())

    arguments = sys.argv
    print(arguments)

    namespace = parser.parse_args(arguments[1:])

    name = namespace.name[0]
    plugin = namespace.plugin[0]
    plugin_file = namespace.plugin_file[0] if namespace.plugin_file else None
    plugin_type = ptypes.index(namespace.type[0])
    env_args = [e.split(":", 1) for e in namespace.env or []]
    python_env = [e.split(":", 1) for e in namespace.python_env or []]
    python_path = namespace.python_path or []
    search_paths = namespace.libs_search_paths or []
    package = namespace.package
    libs = namespace.libs or []
    destination = namespace.destination
    scripts = namespace.scripts or []

    env = {}
    for k, v in env_args:
        env.setdefault(k, []).append(v)

    env_strs = []
    for k, v in env.items():
        env_strs.append(f"{k} : {';'.join(v)}")

    env_str = "\n".join(env_strs)

    penv_strs = []
    for k, v in python_env:
        penv_strs.append(f"{k} : {v}")
    penv_str = "\n".join(penv_strs)

    lib_strs = []
    for l in libs:
        lib_strs.append(f"{l}")
    libs_str = "\n".join(lib_strs)

    search_paths_str = "\n".join(search_paths)

    default_paths_str = "\n".join(os.environ.get("path").split(os.pathsep))

    print(
        f"""\
-----------------------
Building plugin : {name}
Plugin : {plugin}
Plugin-file : {plugin_file}
Type : {plugin_type}
Package : {package}
Destination : {destination}

-----------------------
Environment : 
{env_str or '-'}

-----------------------
Python-Environment :
{penv_str or '-'}


-----------------------
Default lib search paths:
{default_paths_str}

-----------------------
Libs search paths:
{search_paths_str or '-'}

-----------------------
Libs :
{libs_str or '-'}
        """
    )

    build_plugin(destination, name, plugin, plugin_file=plugin_file,
                 type=plugin_type, scripts=scripts, env=env,
                 python_env=python_env, python_path=python_path,
                 package=package, search_paths=search_paths,
                 libs=libs, verbose=True, strict=False)



