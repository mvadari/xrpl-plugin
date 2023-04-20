
type PreflightContext = super::rippled::PreflightContext;
type NotTEC = super::rippled::NotTEC;
pub fn preFlight(ctx: &PreflightContext) -> super::rippled::NotTEC {
    let enabled = super::rippled::get_rules(ctx).enabled(super::rippled::fixMasterKeyAsRegularKey());
    println!("enabled = {}", enabled);
    todo!()
    /*if enabled {
        return super::rippled::NotTEC
    }*/
}

