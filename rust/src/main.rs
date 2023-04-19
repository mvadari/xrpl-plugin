use cxx::let_cxx_string;
use crate::rippled::{PreflightContext, STTx, TxType};

#[cxx::bridge]
mod rippled {

    // These are Rust functions that can be called by C++
    extern "Rust" {

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
        tefNO_AUTH_REQUIRED,  // Can't set auth if auth is not required.
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

    // These are C++ functions that can be called by Rust
    #[namespace = "ripple"]
    unsafe extern "C++" {
        include!("rust/include/rippled_api.h");

        type TxType;
        type TEFcodes;

        pub fn getTxnType(
            self: &STTx
        ) -> TxType;

    }

    // #[namespace = "ripple"]
    pub struct PreflightContext<'a> {
        pub tx: &'a STTx
    }

    #[namespace = "ripple"]
    pub struct STTx {
        // TODO: Add other fields
        pub tx_type_: TxType
    }

    unsafe extern "C++" {
        include!("rust/include/rippled_api.h");

        #[namespace = "ripple"]
        type NotTEC;
        // TODO: Add other codes enums and constructors in rippled_api.h and .cpp
        pub fn from_tefcodes(code: TEFcodes) -> UniquePtr<NotTEC>;
        // TODO: I added this function to TER.h in rippled just to test things out. We should
        //  get rid of this and the function in TER.h later
        pub fn get_value(self: &NotTEC) -> i32;

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
    println!("NotTEC: {:?}", not_tec.deref().get_value());
    assert_eq!(not_tec.deref().get_value(), -198);

    /*let tx = STTx {
        tx_type_: TxType::ttACCOUNT_SET
    };

    let preflight = PreflightContext {
        tx: &tx
    };

    let tx_type = preflight.tx.getTxnType();
    println!("tx_type: {:?}", tx_type);*/
}
