use cxx::UniquePtr;
use super::rippled::*;
// type PreflightContext = super::rippled::PreflightContext;
// type NotTEC = super::rippled::NotTEC;

pub fn pre_flight(ctx: &PreflightContext) -> i32 {
    let enabled = get_rules(ctx).enabled(fixMasterKeyAsRegularKey());
    println!("enabled = {}", enabled);
    // from_tefcodes(TEFcodes::tefALREADY)
    return 0;
}

pub fn pre_claim(ctx: &PreclaimContext) -> i32 {
    return 0;
}

pub fn do_apply(ctx: &ApplyContext, m_prior_balance: XRPAmount, m_source_balance: XRPAmount) -> i32 {
    return 0;
}