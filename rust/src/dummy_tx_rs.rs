use cxx::UniquePtr;
use super::rippled::*;
// type PreflightContext = super::rippled::PreflightContext;
// type NotTEC = super::rippled::NotTEC;

pub fn pre_flight(ctx: &PreflightContext) -> UniquePtr<NotTEC> {
    let enabled = get_rules(ctx).enabled(fixMasterKeyAsRegularKey());
    println!("enabled = {}", enabled);
    from_tefcodes(TEFcodes::tefALREADY)
}

