import xrpl_plugin.rippled_py.sfields as rippled_sfields
from xrpl_plugin.rippled_py.sfields import *
from xrpl_plugin.stypes import STBase, STObject, STPluginType

__doc__ = rippled_sfields.__doc__


def create_new_sfield(cls, field_name, field_value):
    if not issubclass(cls, STBase) and cls != STObject:
        # TODO: fix inheritance
        raise Exception("SField must be of an `ST` type.")
    if cls == STPluginType:
        raise Exception("Must use `construct_custom_sfield` for custom STypes.")
    fn_name = f"_create_new_sfield_{cls.__name__}"
    create_fn = getattr(rippled_sfields, fn_name, None)
    if create_fn is None:
        # NOTE: This should never be hit in prod
        # It may be hit during dev work since not everything is implemented yet
        raise Exception(
            f"`_create_new_sfield` function does not exist for {cls.__name__}"
        )
    return create_fn(field_value, field_name)


sf_tick_size = rippled_sfields.sf_tick_size
"""
The number of significant digits in the exchange rate of an offer.
"""


__all__ = list(rippled_sfields.__dict__.keys()) + ["create_new_sfield"]
