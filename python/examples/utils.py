from dataclasses import dataclass
from typing import Callable, List, Optional, Tuple

from plugin_transactor import (
    SField,
    SOEStyle,
    PreflightContext,
    TxConsequences,
    ConsequencesFactoryType,
    ReadView,
    STTx,
    NotTEC,
    TER,
    PreclaimContext,
    XRPAmount,
    ApplyContext,
    Journal,
    STBase,
    STLedgerEntry,
    register_ledger_object,
    Application,
    ApplyView,
    AccountID,
    uint256,
)
import plugin_transactor

# TODO: move this file into the package


@dataclass(frozen=True)
class Transactor:
    name: str
    tx_type: int
    tx_format: List[Tuple[SField, SOEStyle]]
    consequences_factory_type: ConsequencesFactoryType
    make_tx_consequences: Optional[Callable[[PreflightContext], TxConsequences]] = None
    calculate_base_fee: Optional[Callable[[ReadView, STTx], XRPAmount]] = None
    preflight: Optional[Callable[[PreflightContext], NotTEC]] = None
    preclaim: Optional[Callable[[PreclaimContext], TER]] = None
    do_apply: Optional[Callable[[ApplyContext, XRPAmount, XRPAmount], TER]] = None
    check_seq_proxy: Optional[Callable[[ReadView, STTx, Journal], NotTEC]] = None
    check_prior_tx_and_last_ledger: Optional[Callable[[PreclaimContext], NotTEC]] = None
    check_fee: Optional[Callable[[PreclaimContext, XRPAmount], TER]] = None
    check_sign: Optional[Callable[[PreclaimContext], TER]] = None


def create_new_sfield(cls, field_name, field_value):
    if not issubclass(cls, STBase):
        raise Exception("SField must be of an `ST` type.")
    fn_name = f"create_new_sfield_{cls.__name__}"
    create_fn = getattr(plugin_transactor, fn_name, None)
    if create_fn is None:
        # NOTE: This should never be hit in prod
        # It may be hit during dev work since not everything is implemented yet
        raise Exception(
            f"`create_new_sfield` function does not exist for {cls.__name__}"
        )
    return create_fn(field_value, field_name)


# TODO: helper class, move this to the package
@dataclass(frozen=True)
class LedgerObject:
    object_type: int
    name: str
    rpc_name: str
    format: List[Tuple[SField, SOEStyle]]
    is_deletion_blocker: bool = False
    deleter_function: Optional[Callable[
        Application,
        ApplyView,
        AccountID,
        uint256,
        STLedgerEntry,
        Journal
    ]] = None
    visit_entry_xrp_change: Optional[Callable[[bool, STLedgerEntry, bool], int]] = None

    def __post_init__(self):
        register_ledger_object(self.object_type, self.name, self.format)
