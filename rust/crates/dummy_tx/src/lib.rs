use std::ffi::CString;
use std::pin::Pin;
use std::vec;
use cxx::{CxxString, CxxVector, let_cxx_string, UniquePtr};
use once_cell::sync::OnceCell;
use xrpl_rust_sdk_core::core::crypto::ToFromBase58;
use plugin_transactor::{Feature, PreclaimContext, preflight1, preflight2, PreflightContext, SField, STTx, TF_UNIVERSAL_MASK, Transactor};
use plugin_transactor::transactor::SOElement;
use rippled_bridge::{NotTEC, ParseLeafTypeFnPtr, rippled, SOEStyle, STypeFromSFieldFnPtr, STypeFromSITFnPtr, TEMcodes, TER, TEScodes, XRPAmount};
use rippled_bridge::rippled::{FakeSOElement, OptionalSTVar, push_soelement, SerialIter, SFieldInfo, sfRegularKey, sfTicketSequence, STBase, STypeExport, Value};

struct DummyTx2;

impl Transactor for DummyTx2 {
    fn pre_flight(ctx: PreflightContext) -> NotTEC {
        let preflight1 = preflight1(&ctx);
        if preflight1 != TEScodes::tesSUCCESS {
            return preflight1;
        }

        if ctx.tx().flags() & TF_UNIVERSAL_MASK != 0 {
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

    fn tx_format() -> Vec<SOElement> {
        vec![
            SOElement {
                field_code: field_code(24, 1),
                style: SOEStyle::soeOPTIONAL,
            },
            SOElement {
                field_code: sfTicketSequence().getCode(),
                style: SOEStyle::soeOPTIONAL,
            },
        ]
    }
}

pub fn field_code(type_id: i32, field_id: i32) -> i32 {
    (type_id << 16) | field_id
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

#[no_mangle]
pub fn getTxType() -> u16 {
    31
}

#[no_mangle]
extern "C" fn parseLeafTypeNew(
    field: &rippled::SField,
    json_name: &CxxString,
    field_name: &CxxString,
    name: &rippled::SField,
    value: &Value,
    error: Pin<&mut Value>,
) -> UniquePtr<OptionalSTVar> {
    if !value.isString() {
        // bad_type(error, json_name, field_name);

    }

    todo!()
}

#[no_mangle]
extern "C" fn constructNewSType(
    sit: Pin<&mut SerialIter>,
    name: &rippled::SField
) -> *mut STBase {
    todo!()
}

#[no_mangle]
extern "C" fn constructNewSType2(
    name: &rippled::SField
) -> *mut STBase {
    todo!()
}

#[no_mangle]
pub fn getSTypes(mut s_types: Pin<&mut CxxVector<STypeExport>>) {
    unsafe {
        rippled::push_stype_export(
            24,
            ParseLeafTypeFnPtr(parseLeafTypeNew),
            STypeFromSITFnPtr(constructNewSType),
            STypeFromSFieldFnPtr(constructNewSType2),
            s_types,
        );
    }
    /*s_types.as_mut().push(STypeExport {
        foo: 5
    });
    s_types.as_mut().push(STypeExport {
        foo: 6
    });*/
}

static FIELD_NAMES_ONCE: OnceCell<Vec<CString>> = OnceCell::new();

#[no_mangle]
pub fn getSFields(mut s_fields: Pin<&mut CxxVector<SFieldInfo>>) {
    let field_names = FIELD_NAMES_ONCE.get_or_init(|| {
        vec![CString::new("RegularKey2").unwrap()]
    });
    unsafe {
        rippled::push_sfield_info(24, 1, field_names.get(0).unwrap().as_ptr(), s_fields)
    }
    /*s_fields.as_mut().push(SFieldInfo {
        type_id: 24,
        field_value: 1,
        txt_name: CString::new("RegularKey2").unwrap().as_ptr()
    });*/

    /*s_fields.as_mut().push(SFieldInfo {
        foo: 4
    });*/
}

/*#[no_mangle]
pub fn createNewSType<'a>(tid: i32, fv: i32, f_name: *const i8) -> &'a rippled::TypedSTPluginType {
    unsafe {
        rippled::makeTypedField(tid, fv, f_name)
    }
}*/


// To Register SField, need to: (All doable easily)
//   Define a type_id, ie STI_UINT32_2
//   Define field_id, ie 47
//   Define field name, ie sfQualityIn2

// To Register SType, need to:
//  Define a type_id, ie STI_UINT32_2 (already done)
//  Define a function that constructs a new SField of a given TypedField<T>
//  Define a function that parses the SF from JSON (ie parseLeafTypeNew)
//      In order to do this, need to be able to call detail::make_stvar (which is templated C++ code)
//      with an STBase somehow
//  Define a function that constructs a T: STBase from a SerialIter and SField
//

static NAME_ONCE: OnceCell<CString> = OnceCell::new();
static TT_ONCE: OnceCell<CString> = OnceCell::new();

#[no_mangle]
pub fn getTxName() -> *const i8 {
    let c_string = NAME_ONCE.get_or_init(|| {
        CString::new("DummyTx2").unwrap()
    });
    let ptr = c_string.as_ptr();
    ptr
}

#[no_mangle]
pub fn getTTName() -> *const i8 {
    let c_string = TT_ONCE.get_or_init(|| {
        CString::new("ttDUMMY_TX2").unwrap()
    });
    let ptr = c_string.as_ptr();
    ptr
}

#[no_mangle]
pub fn getTxFormat(mut elements: Pin<&mut CxxVector<FakeSOElement>>) {
    let tx_format = DummyTx2::tx_format();
    for element in tx_format {
        push_soelement(element.field_code, element.style, elements.as_mut());
    }
    // getTxFormat must take a Pin<&mut CxxVector<FakeSOElement>> that we can push FakeSOElements on to.
    // FakeSOElement must be an opaque type because CxxVectors cannot consist of shared types (CXX won't compile).
    // Therefore, we cannot construct an instance of a shared FakeSOElement and push it onto `elements`,
    // so we must call `push_soelement`, which will call a C++ function that constructs a C++ FakeSOElement
    // and pushes it onto the `elements` `std::vector`.
    // push_soelement(8, SOEStyle::soeOPTIONAL, elements.as_mut());
    // push_soelement(41, SOEStyle::soeOPTIONAL, elements.as_mut());
}