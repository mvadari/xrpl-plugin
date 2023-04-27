use std::ops::Deref;
use std::pin::Pin;
use cxx::{ExternType, SharedPtr, type_id, UniquePtr};
use cxx::kind::Trivial;

pub mod dummy_tx_rs;

pub use dummy_tx_rs::pre_flight;
pub use dummy_tx_rs::pre_claim;
pub use dummy_tx_rs::do_apply;

// Also try this
#[no_mangle]
pub fn preflight(ctx: &rippled::PreflightContext) -> NotTEC {
    pre_flight(ctx)
}

#[no_mangle]
pub fn preclaim(ctx: &rippled::PreclaimContext) -> TER {
    pre_claim(ctx)
}

#[no_mangle]
pub fn calculateBaseFee(view: &rippled::ReadView, tx: &rippled::STTx) -> XRPAmount {
    rippled::defaultCalculateBaseFee(view, tx)
}

#[no_mangle]
pub fn doApply(mut ctx: Pin<&mut rippled::ApplyContext>, mPriorBalance: rippled::XRPAmount, mSourceBalance: rippled::XRPAmount) -> TER {
    do_apply(ctx, mPriorBalance, mSourceBalance)
}

#[repr(i32)]
#[derive(Clone, Copy)]
pub enum TELcodes {
    telLOCAL_ERROR = -399,
    telBAD_DOMAIN,
    telBAD_PATH_COUNT,
    telBAD_PUBLIC_KEY,
    telFAILED_PROCESSING,
    telINSUF_FEE_P,
    telNO_DST_PARTIAL,
    telCAN_NOT_QUEUE,
    telCAN_NOT_QUEUE_BALANCE,
    telCAN_NOT_QUEUE_BLOCKS,
    telCAN_NOT_QUEUE_BLOCKED,
    telCAN_NOT_QUEUE_FEE,
    telCAN_NOT_QUEUE_FULL,
}

unsafe impl cxx::ExternType for TELcodes {
    type Id = type_id!("ripple::TELcodes");
    type Kind = Trivial;
}

#[repr(i32)]
#[derive(Clone, Copy)]
pub enum TEMcodes {
    temMALFORMED = -299,

    temBAD_AMOUNT,
    temBAD_CURRENCY,
    temBAD_EXPIRATION,
    temBAD_FEE,
    temBAD_ISSUER,
    temBAD_LIMIT,
    temBAD_OFFER,
    temBAD_PATH,
    temBAD_PATH_LOOP,
    temBAD_REGKEY,
    temBAD_SEND_XRP_LIMIT,
    temBAD_SEND_XRP_MAX,
    temBAD_SEND_XRP_NO_DIRECT,
    temBAD_SEND_XRP_PARTIAL,
    temBAD_SEND_XRP_PATHS,
    temBAD_SEQUENCE,
    temBAD_SIGNATURE,
    temBAD_SRC_ACCOUNT,
    temBAD_TRANSFER_RATE,
    temDST_IS_SRC,
    temDST_NEEDED,
    temINVALID,
    temINVALID_FLAG,
    temREDUNDANT,
    temRIPPLE_EMPTY,
    temDISABLED,
    temBAD_SIGNER,
    temBAD_QUORUM,
    temBAD_WEIGHT,
    temBAD_TICK_SIZE,
    temINVALID_ACCOUNT_ID,
    temCANNOT_PREAUTH_SELF,
    temINVALID_COUNT,

    temUNCERTAIN,  // An internal intermediate result; should never be returned.
    temUNKNOWN,    // An internal intermediate result; should never be returned.

    temSEQ_AND_TICKET,
    temBAD_NFTOKEN_TRANSFER_FEE,
}

unsafe impl cxx::ExternType for TEMcodes {
    type Id = type_id!("ripple::TEMcodes");
    type Kind = Trivial;
}

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

unsafe impl cxx::ExternType for TEFcodes {
    type Id = type_id!("ripple::TEFcodes");
    type Kind = Trivial;
}

#[repr(i32)]
#[derive(Clone, Copy)]
pub enum TERcodes {
    terRETRY = -99,
    terFUNDS_SPENT,  // DEPRECATED.
    terINSUF_FEE_B,  // Can't pay fee, therefore don't burden network.
    terNO_ACCOUNT,   // Can't pay fee, therefore don't burden network.
    terNO_AUTH,      // Not authorized to hold IOUs.
    terNO_LINE,      // Internal flag.
    terOWNERS,       // Can't succeed with non-zero owner count.
    terPRE_SEQ,      // Can't pay fee, no point in forwarding, so don't
    // burden network.
    terLAST,         // DEPRECATED.
    terNO_RIPPLE,    // Rippling not allowed
    terQUEUED,       // Transaction is being held in TxQ until fee drops
    terPRE_TICKET,   // Ticket is not yet in ledger but might be on its way
}

unsafe impl cxx::ExternType for TERcodes {
    type Id = type_id!("ripple::TERcodes");
    type Kind = Trivial;
}

#[repr(i32)]
#[derive(Clone, Copy)]
pub enum TEScodes {
    tesSUCCESS = 0
}

unsafe impl cxx::ExternType for TEScodes {
    type Id = type_id!("ripple::TEScodes");
    type Kind = Trivial;
}

#[repr(i32)]
#[derive(Clone, Copy)]
pub enum TECcodes {
    tecCLAIM = 100,
    tecPATH_PARTIAL = 101,
    tecUNFUNDED_ADD = 102,  // Unused legacy code
    tecUNFUNDED_OFFER = 103,
    tecUNFUNDED_PAYMENT = 104,
    tecFAILED_PROCESSING = 105,
    tecDIR_FULL = 121,
    tecINSUF_RESERVE_LINE = 122,
    tecINSUF_RESERVE_OFFER = 123,
    tecNO_DST = 124,
    tecNO_DST_INSUF_XRP = 125,
    tecNO_LINE_INSUF_RESERVE = 126,
    tecNO_LINE_REDUNDANT = 127,
    tecPATH_DRY = 128,
    tecUNFUNDED = 129,
    tecNO_ALTERNATIVE_KEY = 130,
    tecNO_REGULAR_KEY = 131,
    tecOWNERS = 132,
    tecNO_ISSUER = 133,
    tecNO_AUTH = 134,
    tecNO_LINE = 135,
    tecINSUFF_FEE = 136,
    tecFROZEN = 137,
    tecNO_TARGET = 138,
    tecNO_PERMISSION = 139,
    tecNO_ENTRY = 140,
    tecINSUFFICIENT_RESERVE = 141,
    tecNEED_MASTER_KEY = 142,
    tecDST_TAG_NEEDED = 143,
    tecINTERNAL = 144,
    tecOVERSIZE = 145,
    tecCRYPTOCONDITION_ERROR = 146,
    tecINVARIANT_FAILED = 147,
    tecEXPIRED = 148,
    tecDUPLICATE = 149,
    tecKILLED = 150,
    tecHAS_OBLIGATIONS = 151,
    tecTOO_SOON = 152,
    tecHOOK_ERROR = 153,
    tecMAX_SEQUENCE_REACHED = 154,
    tecNO_SUITABLE_NFTOKEN_PAGE = 155,
    tecNFTOKEN_BUY_SELL_MISMATCH = 156,
    tecNFTOKEN_OFFER_TYPE_MISMATCH = 157,
    tecCANT_ACCEPT_OWN_NFTOKEN_OFFER = 158,
    tecINSUFFICIENT_FUNDS = 159,
    tecOBJECT_NOT_FOUND = 160,
    tecINSUFFICIENT_PAYMENT = 161,
}

unsafe impl cxx::ExternType for TECcodes {
    type Id = type_id!("ripple::TECcodes");
    type Kind = Trivial;
}

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

unsafe impl cxx::ExternType for TxType {
    type Id = type_id!("ripple::TxType");
    type Kind = Trivial;
}

#[cxx::bridge]
pub mod rippled {
    // These are Rust functions that can be called by C++.
    // step 0: try preflight above
    // step 1: try to get rid of cxxbridge1$ prefix on externed generated pre_flight function
    // step 1b: See if you can tell cargo to not exclude unused code from dylib?
    // step 2: create a new C++ project that depends on the static lib generated by this crate that calls pre_flight, then compile to dylib
    //
    extern "Rust" {
        // This function is unused, but exists only to ensure that line 11's interface is bridge
        // compatible.
        pub fn preflight(ctx: &PreflightContext) -> NotTEC;
        pub fn preclaim(ctx: &PreclaimContext) -> TER;
        pub fn calculateBaseFee(view: &ReadView, tx: &STTx) -> XRPAmount;
        pub fn doApply(mut ctx: Pin<&mut ApplyContext>, mPriorBalance: XRPAmount, mSourceBalance: XRPAmount) -> TER;
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
        type TxType = super::TxType;
        type TEFcodes = super::TEFcodes;
        type TEMcodes = super::TEMcodes;
        type TELcodes = super::TELcodes;
        type TERcodes = super::TERcodes;
        type TEScodes = super::TEScodes;
        type TECcodes = super::TECcodes;
        type AccountID = super::AccountID;
        type NotTEC = super::NotTEC;
        type TER = super::TER;
        pub type PreflightContext;
        pub type PreclaimContext;
        pub type ApplyContext;
        pub type XRPAmount = super::XRPAmount;
        pub type ReadView;
        pub type ApplyView;
        pub(crate) type STTx;
        type Rules;
        type uint256;
        type Transactor;
        type SField;
        type STObject;
        type Keylet = super::Keylet;
        type LedgerEntryType = super::LedgerEntryType;
        pub(crate) type SLE;
        type Application;
        type Fees;
        type ApplyFlags = super::ApplyFlags;

        ////////////////////////////////
        // Functions implemented in C++.
        ////////////////////////////////

        pub fn preflight1(ctx: &PreflightContext) -> NotTEC;
        pub fn preflight2(ctx: &PreflightContext) -> NotTEC;

        // In AccountId.h --> AccountID const & xrpAccount();
        pub fn xrpAccount<'a>() -> &'a AccountID;

        pub fn data(self: &AccountID) -> *const u8;
        pub fn begin(self: &AccountID) -> *const u8;
        pub fn end(self: &AccountID) -> *const u8;

        #[namespace = "ripple::keylet"]
        pub fn account(id: &AccountID) -> Keylet;

        #[namespace = "ripple::keylet"]
        pub fn signers(id: &AccountID) -> Keylet;
    }

    unsafe extern "C++" {
        include!("rust/include/rippled_api.h");

        pub fn base64_decode_ptr(s: &CxxString) -> UniquePtr<CxxString>;

        pub fn fixMasterKeyAsRegularKey() -> &'static uint256;

        pub fn tfUniversalMask() -> u32;

        pub fn getTxnType(self: &STTx) -> TxType;

        pub fn enabled(self: &Rules, s_field: &uint256) -> bool;
        pub fn getRules(self: &PreflightContext) -> &Rules;

        pub fn get_dummy_sttx<'a>() -> &'a STTx;
        pub fn get_dummy_ctx<'a>() -> &'a PreflightContext;

        pub fn drops(self: &XRPAmount) -> i64;

        pub fn baseFeeDrops(self: &ApplyContext) -> i64;

        pub fn defaultCalculateBaseFee(view: &ReadView, tx: &STTx) -> XRPAmount;

        pub fn getTx(self: &PreflightContext) -> &STTx;

        // Note: getFlags is a method on STObject. In C++, we can call getFlags on anything
        // that extends STObject, but we can't call getFlags on an STTx in rust. We _could_
        // pass in self: &STTx here, but we'd have to duplicate this function for everything
        // we want to map that extends STObject. Instead, I defined an `upcast` function
        // in rippled_api.h that casts an STTx to an STObject, which I can call and then
        // use the returned STObject to call getFlags on.
        // See solution here: https://github.com/dtolnay/cxx/issues/797
        pub fn getFlags(self: &STObject) -> u32;

        pub fn isFieldPresent(self: &STObject, field: &SField) -> bool;
        pub fn isFlag(self: &SLE, f: u32) -> bool;

        pub fn getAccountID(self: &STObject, field: &SField) -> AccountID;

        pub fn sfRegularKey() -> &'static SField;
        pub fn sfAccount() -> &'static SField;

        pub fn upcast(stTx: &STTx) -> &STObject;

        pub fn toBase58(v: &AccountID) -> UniquePtr<CxxString>;
        pub fn view(self: Pin<&mut ApplyContext>) -> Pin<&mut ApplyView>;
        pub fn getBaseFee<'a>(self: Pin<&'a mut ApplyContext>) -> XRPAmount;
        pub fn getTx(self: &ApplyContext) -> &STTx;
        pub fn getApp<'a, 'b>(self: Pin<&'a mut ApplyContext>) -> Pin<&'b mut Application>;

        pub fn peek(self: Pin<&mut ApplyView>, k: &Keylet) -> SharedPtr<SLE>;
        pub fn fees(self: &ApplyView) -> &Fees;
        pub fn flags(self: &ApplyView) -> ApplyFlags;

        // pub fn setFlag(self: Pin<&mut SLE>, f: u32) -> bool;

        pub fn setFlag(sle: &SharedPtr<SLE>, f: u32) -> bool;
        pub fn setAccountID(sle: &SharedPtr<SLE>, field: &SField, v: &AccountID);
        pub fn makeFieldAbsent(sle: &SharedPtr<SLE>, field: &SField);
        pub fn minimumFee(app: Pin<&mut Application>, baseFee: XRPAmount, fees: &Fees, flags: ApplyFlags) -> XRPAmount;
    }
}

/*pub struct SLE {
    sle: SharedPtr<rippled::SLE>
}

impl SLE {
    pub fn setFlag(&self, f: u32) -> bool {
        rippled::setFlag(&self.sle, f)
    }
}*/

#[repr(C)]
#[derive(PartialEq)]
pub struct AccountID {
    data_: [u8; 20]
}

unsafe impl cxx::ExternType for AccountID {
    type Id = type_id!("ripple::AccountID");
    type Kind = Trivial;
}

#[repr(C)]
#[derive(PartialEq)]
pub struct XRPAmount {
    drops_: i64
}

impl XRPAmount {
    pub fn zero() -> Self {
        XRPAmount { drops_: 0 }
    }
}

unsafe impl cxx::ExternType for XRPAmount {
    type Id = type_id!("ripple::XRPAmount");
    type Kind = Trivial;
}

/////////////////////////////
// NotTEC
/////////////////////////////

#[repr(C)]
#[derive(PartialEq)]
pub struct NotTEC {
    code: i32
}

unsafe impl cxx::ExternType for NotTEC {
    type Id = type_id!("ripple::NotTEC");
    type Kind = Trivial;
}

impl NotTEC {
    pub fn new(code: i32) -> Self {
        NotTEC { code }
    }
}

impl From<TEScodes> for NotTEC {
    fn from(value: TEScodes) -> Self {
        NotTEC::new(value as i32)
    }
}

impl From<TELcodes> for NotTEC {
    fn from(value: TELcodes) -> Self {
        NotTEC::new(value as i32)
    }
}


impl From<TEMcodes> for NotTEC {
    fn from(value: TEMcodes) -> Self {
        NotTEC::new(value as i32)
    }
}

impl From<TEFcodes> for NotTEC {
    fn from(value: TEFcodes) -> Self {
        NotTEC::new(value as i32)
    }
}

impl From<TERcodes> for NotTEC {
    fn from(value: TERcodes) -> Self {
        NotTEC::new(value as i32)
    }
}

impl PartialEq<TEScodes> for NotTEC {
    fn eq(&self, other: &TEScodes) -> bool {
        self.code == (*other as i32)
    }
}

impl PartialEq<TELcodes> for NotTEC {
    fn eq(&self, other: &TELcodes) -> bool {
        self.code == (*other as i32)
    }
}

impl PartialEq<TEMcodes> for NotTEC {
    fn eq(&self, other: &TEMcodes) -> bool {
        self.code == (*other as i32)
    }
}

impl PartialEq<TEFcodes> for NotTEC {
    fn eq(&self, other: &TEFcodes) -> bool {
        self.code == (*other as i32)
    }
}

impl PartialEq<TERcodes> for NotTEC {
    fn eq(&self, other: &TERcodes) -> bool {
        self.code == (*other as i32)
    }
}

impl PartialEq<TECcodes> for NotTEC {
    fn eq(&self, other: &TECcodes) -> bool {
        self.code == (*other as i32)
    }
}


/////////////////////////////
// NotTEC
/////////////////////////////

#[repr(C)]
#[derive(PartialEq)]
pub struct TER {
    code: i32
}

unsafe impl cxx::ExternType for TER {
    type Id = type_id!("ripple::TER");
    type Kind = Trivial;
}

impl TER {
    pub fn new(code: i32) -> Self {
        TER { code }
    }
}

impl From<TEScodes> for TER {
    fn from(value: TEScodes) -> Self {
        TER::new(value as i32)
    }
}

impl From<TELcodes> for TER {
    fn from(value: TELcodes) -> Self {
        TER::new(value as i32)
    }
}


impl From<TEMcodes> for TER {
    fn from(value: TEMcodes) -> Self {
        TER::new(value as i32)
    }
}

impl From<TEFcodes> for TER {
    fn from(value: TEFcodes) -> Self {
        TER::new(value as i32)
    }
}

impl From<TERcodes> for TER {
    fn from(value: TERcodes) -> Self {
        TER::new(value as i32)
    }
}

impl From<TECcodes> for TER {
    fn from(value: TECcodes) -> Self {
        TER::new(value as i32)
    }
}

///////////////////
impl PartialEq<TEScodes> for TER {
    fn eq(&self, other: &TEScodes) -> bool {
        self.code == (*other as i32)
    }
}

impl PartialEq<TELcodes> for TER {
    fn eq(&self, other: &TELcodes) -> bool {
        self.code == (*other as i32)
    }
}

impl PartialEq<TEMcodes> for TER {
    fn eq(&self, other: &TEMcodes) -> bool {
        self.code == (*other as i32)
    }
}

impl PartialEq<TEFcodes> for TER {
    fn eq(&self, other: &TEFcodes) -> bool {
        self.code == (*other as i32)
    }
}

impl PartialEq<TERcodes> for TER {
    fn eq(&self, other: &TERcodes) -> bool {
        self.code == (*other as i32)
    }
}

impl PartialEq<TECcodes> for TER {
    fn eq(&self, other: &TECcodes) -> bool {
        self.code == (*other as i32)
    }
}

#[repr(i16)]
#[derive(Clone, Copy)]
pub enum LedgerEntryType {
    /// A ledger object which describes an account.
    /// \sa keylet::account

    ltACCOUNT_ROOT = 0x0061,

    /// A ledger object which contains a list of object identifiers.
    ///       \sa keylet::page, keylet::quality, keylet::book, keylet::next and
    ///           keylet::ownerDir

    ltDIR_NODE = 0x0064,

    /// A ledger object which describes a bidirectional trust line.
    ///       @note Per Vinnie Falco this should be renamed to ltTRUST_LINE
    ///       \sa keylet::line

    ltRIPPLE_STATE = 0x0072,

    /// A ledger object which describes a ticket.

    ///    \sa keylet::ticket
    ltTICKET = 0x0054,

    /// A ledger object which contains a signer list for an account.
    /// \sa keylet::signers
    ltSIGNER_LIST = 0x0053,

    /// A ledger object which describes an offer on the DEX.
    /// \sa keylet::offer
    ltOFFER = 0x006f,

    /// A ledger object that contains a list of ledger hashes.
    ///
    ///       This type is used to store the ledger hashes which the protocol uses
    ///       to implement skip lists that allow for efficient backwards (and, in
    ///       theory, forward) forward iteration across large ledger ranges.
    ///       \sa keylet::skip
    ltLEDGER_HASHES = 0x0068,

    /// The ledger object which lists details about amendments on the network.
    ///       \note This is a singleton: only one such object exists in the ledger.
    ///
    ///       \sa keylet::amendments
    ltAMENDMENTS = 0x0066,

    /// The ledger object which lists the network's fee settings.
    ///
    ///       \note This is a singleton: only one such object exists in the ledger.
    ///
    ///       \sa keylet::fees
    ltFEE_SETTINGS = 0x0073,

    /// A ledger object describing a single escrow.
    ///
    ///       \sa keylet::escrow
    ltESCROW = 0x0075,

    /// A ledger object describing a single unidirectional XRP payment channel.
    ///
    ///       \sa keylet::payChan
    ltPAYCHAN = 0x0078,

    /// A ledger object which describes a check.
    ///       \sa keylet::check
    ltCHECK = 0x0043,

    /// A ledger object which describes a deposit preauthorization.
    ///
    ///       \sa keylet::depositPreauth
    ltDEPOSIT_PREAUTH = 0x0070,

    /// The ledger object which tracks the current negative UNL state.
    ///
    ///       \note This is a singleton: only one such object exists in the ledger.
    ///
    ///       \sa keylet::negativeUNL

    ltNEGATIVE_UNL = 0x004e,

    /// A ledger object which contains a list of NFTs
    ///       \sa keylet::nftpage_min, keylet::nftpage_max, keylet::nftpage
    ltNFTOKEN_PAGE = 0x0050,

    /// A ledger object which identifies an offer to buy or sell an NFT.
    ///       \sa keylet::nftoffer
    ltNFTOKEN_OFFER = 0x0037,

    //---------------------------------------------------------------------------
    /// A special type, matching any ledger entry type.
    ///
    ///       The value does not represent a concrete type, but rather is used in
    ///       contexts where the specific type of a ledger object is unimportant,
    ///       unknown or unavailable.
    ///
    ///      Objects with this special type cannot be created or stored on the
    ///      ledger.
    ///
    ///      \sa keylet::unchecked
    ltANY = 0,

    /// A special type, matching any ledger type except directory nodes.
    ///
    ///      The value does not represent a concrete type, but rather is used in
    ///       contexts where the ledger object must not be a directory node but
    ///       its specific type is otherwise unimportant, unknown or unavailable.
    ///
    ///      Objects with this special type cannot be created or stored on the
    ///     ledger.
    ///
    ///      \sa keylet::child
    ltCHILD = 0x1CD2,

    //---------------------------------------------------------------------------
    /// A legacy, deprecated type.
    ///
    ///      \deprecated **This object type is not supported and should not be used.**
    ///                   Support for this type of object was never implemented.
    ///                  No objects of this type were ever created.
    ltNICKNAME = 0x006e,

    /// A legacy, deprecated type.
    ///
    ///   \deprecated **This object type is not supported and should not be used.**
    ///               Support for this type of object was never implemented.
    ///               No objects of this type were ever created.
    ltCONTRACT = 0x0063,

    /// A legacy, deprecated type.
    ///
    ///   \deprecated **This object type is not supported and should not be used.**
    ///             Support for this type of object was never implemented.
    ///            No objects of this type were ever created.
    ltGENERATOR_MAP = 0x0067,
}

unsafe impl cxx::ExternType for LedgerEntryType {
    type Id = type_id!("ripple::LedgerEntryType");
    type Kind = Trivial;
}

#[repr(C)]
pub struct Keylet {
    // TODO: Consider making uint256 a shared struct
    // Also test this, since key is a uint256 which has a data_ field
    key: [u8; 32],
    r#type: LedgerEntryType
}

unsafe impl cxx::ExternType for Keylet {
    type Id = type_id!("ripple::Keylet");
    type Kind = Trivial;
}

#[repr(i32)]
#[derive(Copy, Clone)]
pub enum ApplyFlags {
    tapNONE = 0x00,

    // This is a local transaction with the
    // fail_hard flag set.
    tapFAIL_HARD = 0x10,

    // This is not the transaction's last pass
    // Transaction can be retried, soft failures allowed
    tapRETRY = 0x20,

    // Transaction came from a privileged source
    tapUNLIMITED = 0x400,
}

unsafe impl cxx::ExternType for ApplyFlags {
    type Id = type_id!("ripple::ApplyFlags");
    type Kind = Trivial;
}


pub enum LedgerSpecificFlags {
    // ltACCOUNT_ROOT
    lsfPasswordSpent,  // True, if password set fee is spent.
    lsfRequireDestTag,  // True, to require a DestinationTag for payments.
    lsfRequireAuth,  // True, to require a authorization to hold IOUs.
    lsfDisallowXRP,    // True, to disallow sending XRP.
    lsfDisableMaster,  // True, force regular key
    lsfNoFreeze,       // True, cannot freeze ripple states
    lsfGlobalFreeze,   // True, all assets frozen
    lsfDefaultRipple,               // True, trust lines allow rippling by default
    lsfDepositAuth,  // True, all deposits require authorization
    /*  // reserved for Hooks amendment
        lsfTshCollect = 0x02000000,     // True, allow TSH collect-calls to acc hooks
    */
    lsfDisallowIncomingNFTokenOffer,// True, reject new incoming NFT offers
    lsfDisallowIncomingCheck,               // True, reject new checks
    lsfDisallowIncomingPayChan,               // True, reject new paychans
    lsfDisallowIncomingTrustline,               // True, reject new trustlines (only if no issued assets)

    // ltOFFER
    lsfPassive,
    lsfSell,  // True, offer was placed as a sell.

    // ltRIPPLE_STATE
    lsfLowReserve,  // True, if entry counts toward reserve.
    lsfHighReserve,
    lsfLowAuth,
    lsfHighAuth,
    lsfLowNoRipple,
    lsfHighNoRipple,
    lsfLowFreeze,   // True, low side has set freeze flag
    lsfHighFreeze,  // True, high side has set freeze flag

    // ltSIGNER_LIST
    lsfOneOwnerCount,  // True, uses only one OwnerCount

    // ltDIR_NODE
    lsfNFTokenBuyOffers,
    lsfNFTokenSellOffers,

    // ltNFTOKEN_OFFER
    lsfSellNFToken,
}

impl From<LedgerSpecificFlags> for u32 {
    fn from(value: LedgerSpecificFlags) -> Self {
        match value {
            LedgerSpecificFlags::lsfPasswordSpent => 0x00010000,
            LedgerSpecificFlags::lsfRequireDestTag => 0x00020000,
            LedgerSpecificFlags::lsfRequireAuth => 0x00040000,
            LedgerSpecificFlags::lsfDisallowXRP => 0x00080000,
            LedgerSpecificFlags::lsfDisableMaster => 0x00100000,
            LedgerSpecificFlags::lsfNoFreeze => 0x00200000,
            LedgerSpecificFlags::lsfGlobalFreeze => 0x00400000,
            LedgerSpecificFlags::lsfDefaultRipple => 0x00800000,
            LedgerSpecificFlags::lsfDepositAuth => 0x01000000,
            LedgerSpecificFlags::lsfDisallowIncomingNFTokenOffer => 0x04000000,
            LedgerSpecificFlags::lsfDisallowIncomingCheck => 0x08000000,
            LedgerSpecificFlags::lsfDisallowIncomingPayChan => 0x10000000,
            LedgerSpecificFlags::lsfDisallowIncomingTrustline => 0x20000000,
            LedgerSpecificFlags::lsfPassive => 0x00010000,
            LedgerSpecificFlags::lsfSell => 0x00020000,
            LedgerSpecificFlags::lsfLowReserve => 0x00010000,
            LedgerSpecificFlags::lsfHighReserve => 0x00020000,
            LedgerSpecificFlags::lsfLowAuth => 0x00040000,
            LedgerSpecificFlags::lsfHighAuth => 0x00080000,
            LedgerSpecificFlags::lsfLowNoRipple => 0x00100000,
            LedgerSpecificFlags::lsfHighNoRipple => 0x00200000,
            LedgerSpecificFlags::lsfLowFreeze => 0x00400000,
            LedgerSpecificFlags::lsfHighFreeze => 0x00800000,
            LedgerSpecificFlags::lsfOneOwnerCount => 0x00010000,
            LedgerSpecificFlags::lsfNFTokenBuyOffers => 0x00000001,
            LedgerSpecificFlags::lsfNFTokenSellOffers => 0x00000002,
            LedgerSpecificFlags::lsfSellNFToken => 0x00000001,
        }
    }
}