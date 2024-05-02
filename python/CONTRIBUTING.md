# Contributing

## How to Deploy

```bash
# update version
(cd docs && make html)
python setup.py sdist
python setup.py bdist_wheel
twine upload dist/\*
```
