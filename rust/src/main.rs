use cxx::let_cxx_string;
use crate::rippled::{PreflightContext, STTx, TxType};

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

    #[derive(Debug)]
    pub struct PreflightContext<'a> {
        pub tx: &'a STTx,
    }

    #[derive(Debug)]
    pub struct STTx {
        // TODO: Add other fields
        pub tx_type_: TxType,
    }

    // pub struct AccountId {
    //     id: u16,
    // }


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

        // Bits ==> 160 (20 bytes)
        // WIDTH ==> 5
        // data_ is an array of 5 uint32's in bigendian order.
        // In C++, we want to copy the 5 bytes into a Rust Vec and return by value.
        // Then, in Rust, we can pass this pointer to that function and get an AccountID.


        // : error: cannot initialize a variable of type
        // '::std::unique_ptr< ::ripple::AccountID> (*)()' (aka 'unique_ptr<base_uint<160, detail::AccountIDTag>> (*)()')
        // 'const ripple::AccountID &()'                   (aka 'const base_uint<160, detail::AccountIDTag> &()'):
        // different return type (
        // '::std::unique_ptr< ::ripple::AccountID>' (aka 'unique_ptr<base_uint<160, detail::AccountIDTag>>')
        // 'const ripple::AccountID &'               (aka 'const base_uint<160, detail::AccountIDTag> &'))

        // pub fn getTxnType(
        //     self: &STTx
        // ) -> TxType;
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

        /*#[namespace = "ripple"]
        type STTx;

        #[namespace = "ripple"]
        type PreflightContext;
        pub fn tx_ptr(ctx: &PreflightContext) -> UniquePtr<STTx>;*/

        // #[rust_name = "get_tx_type"]
        // #[namespace = "ripple"]
    }
}

fn main() {
    use std::ops::Deref;
    /*let client = ffi::new_blobstore_client();

    let chunks = vec![b"fearless".to_vec(), b"concurrency".to_vec()];
    let mut buf = MultiBuf { chunks, pos: 0 };
    let blobid = client.put(&mut buf);
    println!("blobid = {}", blobid);*/
    // let size = unsafe {
    //     rippled::encoded_size(4)
    // };
    // println!("Size: {}", size);

    let_cxx_string!(b64 = "dGhlIGNha2UgaXMgYSBsaWU");
    let decoded = rippled::base64_decode_ptr(&b64);
    println!("decoded: {}", decoded);

    let tef = rippled::TEFcodes::tefALREADY;
    let not_tec = rippled::from_tefcodes(tef);
    // println!("NotTEC: {:?}", not_tec.deref());
    // assert_eq!(not_tec.deref().get_value(), -198);

    let tx = STTx {
        tx_type_: TxType::ttACCOUNT_SET
    };
    println!("tx={:?}", tx);

    let preflight = PreflightContext {
        tx: &tx
    };
    println!("preflight={:?}", preflight);


    let xrp_account_cxx: &rippled::AccountID = rippled::xrpAccount();
    unsafe {
        let first_byte = xrp_account_cxx.data();
        println!("first_byte: {:?}", *first_byte);
    }
    // let xrp_account_id: xrpl_rust_sdk_core::core::types::account_id::AccountID
    //     = rust::to_account_id(xrp_account_cxx);

    /*   let tx_type = preflight.tx.getTxnType();
       println!("tx_type: {:?}", tx_type);*/
}
