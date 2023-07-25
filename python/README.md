# Python Plugins

```bash
pip install ./python
cd python
python build_scripts/build.py path/to/plugin.py
# edit rippled.cfg to include the plugin.dylib in a [plugins] stanza
# run rippled with the same branch and submit plugin transactions to it
```
