use rust::rippled;

fn main() {
    use std::ops::Deref;

    /*let_cxx_string!(b64 = "dGhlIGNha2UgaXMgYSBsaWU");
    let decoded = rippled::base64_decode_ptr(&b64);
    println!("decoded: {}", decoded);
*/
    let tef = rippled::TEFcodes::tefALREADY;
    let not_tec = rippled::from_tefcodes(tef);
    // println!("NotTEC: {:?}", not_tec.deref());
    // assert_eq!(not_tec.deref().get_value(), -198);


    let xrp_account_cxx: &rippled::AccountID = rippled::xrpAccount();

    let account_id = to_account_id(xrp_account_cxx);
    println!("account_id: {:?}", account_id);
    assert_eq!(account_id, ACCOUNT_ZERO);

    let tx: &rippled::STTx = rippled::get_dummy_sttx();
    let tx_type: rippled::TxType = tx.getTxnType();
    println!("tx_type: {:?}", tx_type);
    assert_eq!(tx_type, rippled::TxType::ttPAYMENT);

}

use xrpl_rust_sdk_core::core::types::{AccountId, ACCOUNT_ZERO};

pub fn to_account_id(cxx_account_id: &rippled::AccountID) -> AccountId {
    unsafe {
        let begin: *const u8 = cxx_account_id.begin();
        let end: *const u8 = cxx_account_id.end();
        let offset = end.offset_from(begin);
        assert!(!offset.is_negative());
        let account_id_slice = std::slice::from_raw_parts(begin, offset as usize);

        return AccountId::try_from(account_id_slice).unwrap();
    }
}
