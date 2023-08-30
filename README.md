# XRPL Plugins

Compile and build this branch of rippled: https://github.com/mvadari/rippled/tree/plugin

Setup:

```bash
git clone --recurse-submodules https://github.com/mvadari/xrpl-plugin.git
cd xrpl-plugin
cd language-of-choice
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
