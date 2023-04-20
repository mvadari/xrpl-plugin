pub mod dummy_tx_rs;

use cxx::let_cxx_string;
use crate::rippled::{TxType};

#[cxx::bridge]
mod rippled {
    // These are Rust functions that can be called by C++.
    extern "Rust" {
        ////////////////////////////////
        // Zero or more opaque types which both languages can pass around but only Rust can see
        // the fields.
        ////////////////////////////////

        // type MultiBuf;

        ////////////////////////////////
        // Functions implemented in Rust.
        ////////////////////////////////

        /*type NotTEC;
        fn preflight() -> NotTEC*/
    }

    #[namespace = "ripple"]
    #[repr(i32)]
    #[derive(Clone, Copy)]
    pub enum TEFcodes {
        tefFAILURE = -199,
        tefALREADY,
        tefBAD_ADD_AUTH,
        tefBAD_AUTH,
        tefBAD_LEDGER,
        tefCREATED,
        tefEXCEPTION,
        tefINTERNAL,
        tefNO_AUTH_REQUIRED,
        // Can't set auth if auth is not required.
        tefPAST_SEQ,
        tefWRONG_PRIOR,
        tefMASTER_DISABLED,
        tefMAX_LEDGER,
        tefBAD_SIGNATURE,
        tefBAD_QUORUM,
        tefNOT_MULTI_SIGNING,
        tefBAD_AUTH_MASTER,
        tefINVARIANT_FAILED,
        tefTOO_BIG,
        tefNO_TICKET,
        tefNFTOKEN_IS_NOT_TRANSFERABLE,
    }

    #[namespace = "ripple"]
    #[repr(u16)]
    #[derive(Debug, Clone, Copy)]
    pub enum TxType {
        /// This transaction type executes a payment.
        ttPAYMENT = 0,

        /// This transaction type creates an escrow object.
        ttESCROW_CREATE = 1,

        /// This transaction type completes an existing escrow. */
        ttESCROW_FINISH = 2,

        /// This transaction type adjusts various account settings. */
        ttACCOUNT_SET = 3,

        /// This transaction type cancels an existing escrow. */
        ttESCROW_CANCEL = 4,

        /// This transaction type sets or clears an account's "regular key". */
        ttREGULAR_KEY_SET = 5,

        /// This transaction type is deprecated; it is retained for historical purposes. */
        ttNICKNAME_SET = 6,

        /// This transaction type creates an offer to trade one asset for another. */
        ttOFFER_CREATE = 7,

        /// This transaction type cancels existing offers to trade one asset for another. */
        ttOFFER_CANCEL = 8,

        /// This transaction type is deprecated; it is retained for historical purposes. */
        ttCONTRACT = 9,

        /// This transaction type creates a new set of tickets. */
        ttTICKET_CREATE = 10,

        /// This identifier was never used, but the slot is reserved for historical purposes. */
        ttSPINAL_TAP = 11,

        /// This transaction type modifies the signer list associated with an account. */
        ttSIGNER_LIST_SET = 12,

        /// This transaction type creates a new unidirectional XRP payment channel. */
        ttPAYCHAN_CREATE = 13,

        /// This transaction type funds an existing unidirectional XRP payment channel. */
        ttPAYCHAN_FUND = 14,

        /// This transaction type submits a claim against an existing unidirectional payment channel. */
        ttPAYCHAN_CLAIM = 15,

        /// This transaction type creates a new check. */
        ttCHECK_CREATE = 16,

        /// This transaction type cashes an existing check. */
        ttCHECK_CASH = 17,

        /// This transaction type cancels an existing check. */
        ttCHECK_CANCEL = 18,

        /// This transaction type grants or revokes authorization to transfer funds. */
        ttDEPOSIT_PREAUTH = 19,

        /// This transaction type modifies a trustline between two accounts. */
        ttTRUST_SET = 20,

        /// This transaction type deletes an existing account. */
        ttACCOUNT_DELETE = 21,

        /// This transaction type installs a hook. */
        ttHOOK_SET = 22,

        /// This transaction mints a new NFT. */
        ttNFTOKEN_MINT = 25,

        /// This transaction burns (i.e. destroys) an existing NFT. */
        ttNFTOKEN_BURN = 26,

        /// This transaction creates a new offer to buy or sell an NFT. */
        ttNFTOKEN_CREATE_OFFER = 27,

        /// This transaction cancels an existing offer to buy or sell an existing NFT. */
        ttNFTOKEN_CANCEL_OFFER = 28,

        /// This transaction accepts an existing offer to buy or sell an existing  NFT. */
        ttNFTOKEN_ACCEPT_OFFER = 29,

        /// This transactions is a dummy transaction */
        ttDUMMY_TX = 30,

        /// This system-generated transaction type is used to update the status of the various amendments.
        ///
        /// For details, see: https://xrpl.org/amendments.html
        ttAMENDMENT = 100,

        /// This system-generated transaction type is used to update the network's fee settings.
        ///
        /// For details, see: https://xrpl.org/fee-voting.html
        ttFEE = 101,

        /// This system-generated transaction type is used to update the network's negative UNL
        ///
        /// For details, see: https://xrpl.org/negative-unl.html
        ttUNL_MODIFY = 102,
    }


    // Safety: the extern "C++" block is responsible for deciding whether to expose each signature
    // inside as safe-to-call or unsafe-to-call. If an extern block contains at least one
    // safe-to-call signature, it must be written as an unsafe extern block, which serves as
    // an item level unsafe block to indicate that an unchecked safety claim is being made about
    // the contents of the block.

    // These are C++ functions that can be called by Rust.
    // Within the extern "C++" part, we list types and functions for which C++ is the source of
    // truth, as well as the header(s) that declare those APIs.
    #[namespace = "ripple"]
    unsafe extern "C++" {
        ////////////////////////////////
        // One or more headers with the matching C++ declarations for the
        // enclosing extern "C++" block. Our code generators don't read it
        // but it gets #include'd and used in static assertions to ensure
        // our picture of the FFI boundary is accurate.
        ////////////////////////////////
        include!("rust/include/rippled_api.h");

        ////////////////////////////////
        // Zero or more opaque types which both languages can pass around
        // but only C++ can see the fields.
        // type BlobstoreClient;
        ////////////////////////////////
        type TxType;
        type TEFcodes;

        #[namespace = "ripple"]
        type AccountID;

        ////////////////////////////////
        // Functions implemented in C++.
        ////////////////////////////////

        // In AccountId.h --> AccountID const & xrpAccount();
        fn xrpAccount<'a>() -> &'a AccountID;

        fn data(self: &AccountID) -> *const u8;
        fn begin(self: &AccountID) -> *const u8;
        fn end(self: &AccountID) -> *const u8;
    }

    unsafe extern "C++" {
        include!("rust/include/rippled_api.h");

        #[namespace = "ripple"]
        type NotTEC;
        // TODO: Add other codes enums and constructors in rippled_api.h and .cpp
        pub fn from_tefcodes(code: TEFcodes) -> UniquePtr<NotTEC>;
        // TODO: I added this function to TER.h in rippled just to test things out. We should
        //  get rid of this and the function in TER.h later
        //pub fn get_value(self: &NotTEC) -> i32;

        pub(crate) fn base64_decode_ptr(s: &CxxString) -> UniquePtr<CxxString>;

        #[namespace = "ripple"]
        type PreflightContext;
        #[namespace = "ripple"]
        type STTx;
        #[namespace = "ripple"]
        type Rules;
        #[namespace = "ripple"]
        type uint256;

        fn fixMasterKeyAsRegularKey() -> &'static uint256;

        fn get_tx(ctx: &PreflightContext) -> &STTx;
        fn get_rules(ctx: &PreflightContext) -> &Rules;

        #[namespace = "ripple"]
        fn getTxnType(self: &STTx) -> TxType;

        #[namespace = "ripple"]
        fn enabled(self: &Rules, s_field: &uint256) -> bool;

        fn get_dummy_sttx<'a>() -> &'a STTx;
        fn get_dummy_ctx<'a>() -> &'a PreflightContext;
    }
}

fn main() {
    use std::ops::Deref;

    let_cxx_string!(b64 = "dGhlIGNha2UgaXMgYSBsaWU");
    let decoded = rippled::base64_decode_ptr(&b64);
    println!("decoded: {}", decoded);

    let tef = rippled::TEFcodes::tefALREADY;
    let not_tec = rippled::from_tefcodes(tef);
    // println!("NotTEC: {:?}", not_tec.deref());
    // assert_eq!(not_tec.deref().get_value(), -198);


    let xrp_account_cxx: &rippled::AccountID = rippled::xrpAccount();

    let account_id = to_account_id(xrp_account_cxx);
    println!("account_id: {:?}", account_id);
    assert_eq!(account_id, ACCOUNT_ZERO);

    let tx: &rippled::STTx = rippled::get_dummy_sttx();
    let tx_type: TxType = tx.getTxnType();
    println!("tx_type: {:?}", tx_type);
    assert_eq!(tx_type, rippled::TxType::ttPAYMENT);

    let ctx = rippled::get_dummy_ctx();
    let res = dummy_tx_rs::preFlight(ctx);

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
