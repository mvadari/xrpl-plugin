use std::pin::Pin;
use xrpl_rust_sdk_core::core::crypto::ToFromBase58;
use plugin_transactor::{Feature, PreclaimContext, preflight1, preflight2, PreflightContext, SField, STTx, TF_UNIVERSAL_MASK, Transactor};
use rippled_bridge::{NotTEC, rippled, TEMcodes, TER, TEScodes, XRPAmount};

struct DummyTx2;

impl Transactor for DummyTx2 {
    fn pre_flight(ctx: PreflightContext) -> NotTEC {
        let preflight1 = preflight1(&ctx);
        if preflight1 != TEScodes::tesSUCCESS {
            return preflight1;
        }

        if ctx.tx().flags() & TF_UNIVERSAL_MASK != 0{
            return TEMcodes::temINVALID_FLAG.into();
        }

        println!("RegularKey: {:?}", ctx.tx().get_account_id(&SField::sf_regular_key()).encode_base58());
        println!("Account: {:?}", ctx.tx().get_account_id(&SField::sf_account()).encode_base58());

        if ctx.rules().enabled(&Feature::fix_master_key_as_regular_key()) &&
            ctx.tx().is_field_present(&SField::sf_regular_key()) &&
            ctx.tx().get_account_id(&SField::sf_regular_key()) == ctx.tx().get_account_id(&SField::sf_account()) {
            return TEMcodes::temBAD_REGKEY.into();
        }

        preflight2(&ctx)
    }

    fn pre_claim(ctx: PreclaimContext) -> TER {
        TEScodes::tesSUCCESS.into()
    }
}


// TODO: Consider writing a macro that generates this for you given a T: Transactor
#[no_mangle]
pub fn preflight(ctx: &rippled::PreflightContext) -> NotTEC {
    DummyTx2::pre_flight(PreflightContext::new(ctx))
}

#[no_mangle]
pub fn preclaim(ctx: &rippled::PreclaimContext) -> TER {
    DummyTx2::pre_claim(PreclaimContext::new(ctx))
}

#[no_mangle]
pub fn calculateBaseFee(view: &rippled::ReadView, tx: &rippled::STTx) -> XRPAmount {
    DummyTx2::calculateBaseFee(view, STTx::new(tx)).into()
}

#[no_mangle]
pub fn doApply(mut ctx: Pin<&mut rippled::ApplyContext>, mPriorBalance: rippled::XRPAmount, mSourceBalance: rippled::XRPAmount) -> TER {
    rippled_bridge::do_apply(ctx, mPriorBalance, mSourceBalance)
}