# XRPL Plugins

XRPL Plugins ([XLS-42d](https://github.com/XRPLF/XRPL-Standards/discussions/116)) is a project to allow people to build sidechains and experimental features for the XRPL with programming languages other than C++, without needing to modify the rippled repo at all.

The current supported languages are C++ and Python, with some early (unfinished) progress in JavaScript support.

## Setup

Compile and build this branch of rippled: https://github.com/mvadari/rippled/tree/plugin

```bash
git clone --recurse-submodules https://github.com/mvadari/xrpl-plugin.git
cd xrpl-plugin
cd language-of-choice
# follow the instructions in the folder for further setup
```

See the individual per-language folders for more information about how to run plugins written in that language.

## Debugging

### Import Error

Error (usually shows up on Linux):

```
ImportError: libxrpl_plugin.so: cannot open shared object file: No such file or directory
```

Solution:

```
export LD_LIBRARY_PATH=/usr/local/lib/python3.10/dist-packages/xrpl_plugin
```
