use std::pin::Pin;
use xrpl_rust_sdk_core::core::types::XrpAmount;
use rippled_bridge::{NotTEC, TER};
use crate::{PreclaimContext, PreflightContext, STTx};

pub trait Transactor {
    fn pre_flight(ctx: PreflightContext) -> NotTEC;
    fn pre_claim(ctx: PreclaimContext) -> TER;
    // TODO: Wrap ReadView
    fn calculateBaseFee(view: &rippled_bridge::rippled::ReadView, tx: STTx) -> XrpAmount {
        rippled_bridge::rippled::defaultCalculateBaseFee(view, tx.instance).into()
    }
}