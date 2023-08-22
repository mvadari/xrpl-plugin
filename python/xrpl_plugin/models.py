import json
from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Callable, List, Optional, Tuple, Union

from xrpl_plugin.transactors import (
    Application,
    ApplyContext,
    ApplyView,
    ConsequencesFactoryType,
    PreclaimContext,
    PreflightContext,
    ReadView,
    SerialIter,
    Serializer,
    TxConsequences,
)
from xrpl_plugin.basic_types import (
    AccountID,
    Buffer,
    Journal,
    JsonValue,
    VoteBehavior,
    XRPAmount,
    uint256,
    SOEStyle,
)
from xrpl_plugin.sfields import SField
from xrpl_plugin.stypes import (
    STLedgerEntry,
    STTx,
)
from xrpl_plugin.return_codes import TER, NotTEC


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


@dataclass(frozen=True)
class SType:
    type_id: int
    parse_value: Callable[
        [SField, str, str, SField, JsonValue], Union[Buffer, JsonValue]
    ]
    to_string: Callable[[Buffer], str]
    to_serializer: Callable[[Buffer, Serializer], None]
    from_serial_iter: Callable[[SerialIter], Buffer]
    to_json: Optional[Callable[[Buffer], JsonValue]] = None


@dataclass(frozen=True)
class Amendment:
    name: str
    supported: bool
    vote: VoteBehavior


@dataclass(frozen=True)
class TERCode:
    code: int
    code_str: str
    description: str


class InvariantCheck(ABC):
    def process_data(self, data):
        if data is None:
            return
        data_dict = json.loads(data)
        self.__dict__.update(**data_dict)

    def visit_entry_actual(self, data, is_delete, before, after):
        self.process_data(data)

        self.visit_entry(is_delete, before, after)

        return json.dumps(self.__dict__)

    def finalize_actual(self, data, tx, result, fee, view, j):
        self.process_data(data)
        return self.finalize(tx, result, fee, view, j)

    @abstractmethod
    def visit_entry(self, is_delete, before, after):
        pass

    @abstractmethod
    def finalize(self, tx, result, fee, view, j):
        pass


@dataclass(frozen=True)
class LedgerObject:
    object_type: int
    name: str
    rpc_name: str
    object_format: List[Tuple[SField, SOEStyle]]
    is_deletion_blocker: bool = False
    delete_object: Optional[
        Callable[
            [Application, ApplyView, AccountID, uint256, STLedgerEntry, Journal], TER
        ]
    ] = None
    visit_entry_xrp_change: Optional[Callable[[bool, STLedgerEntry, bool], int]] = None

    def __post_init__(self):
        if self.is_deletion_blocker and self.delete_object is not None:
            raise Exception(
                "Cannot have `delete_object` if `is_deletion_blocker` is True"
            )
        if not self.is_deletion_blocker and self.delete_object is None:
            raise Exception(
                "Must have `delete_object` if `is_deletion_blocker` is False"
            )


@dataclass(frozen=True)
class InnerObject:
    field: SField
    format: List[Tuple[SField, SOEStyle]]
