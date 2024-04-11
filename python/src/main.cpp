#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include <ripple/protocol/Serializer.h>
#include <ripple/protocol/st.h>
#include <ripple/protocol/TxFlags.h>
#include <ripple/protocol/Feature.h>
#include <ripple/protocol/nft.h>
#include <ripple/ledger/View.h>
#include <ripple/ledger/ApplyViewImpl.h>
#include <ripple/app/main/Application.h>
#include <ripple/app/tx/TxConsequences.h>
#include <ripple/app/tx/impl/PreflightContext.h>
#include <ripple/app/tx/impl/PreclaimContext.h>
#include <ripple/app/tx/impl/details/NFTokenUtils.h>
#include <ripple/app/tx/impl/ApplyContext.h>
#include <ripple/protocol/digest.h>
#include <ripple/plugin/createSFields.h>
#include <ripple/protocol/InnerObjectFormats.h>
#include <map>
#include <iostream>
#include <string>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

ripple::SField const&
newUntypedSField(const int fieldValue, std::string fieldName)
{
    if (ripple::SField const& field = ripple::SField::getField(fieldName); field != ripple::sfInvalid)
    {
        return field;
    }
    throw std::runtime_error(
        "SField " + std::string(fieldName) + " doesn't exist.");
}

struct WSF {
  mutable void *f_;
  const int fieldValue_;
  std::string const fieldName_;
  const int tid_;
  operator ripple::SField const &() const {
    if (f_ == nullptr) {
        ripple::SField const& sfield = newUntypedSField(fieldValue_, fieldName_);
        f_ = (void *)&sfield;
    }
    return *static_cast<ripple::SField *>(f_);
  };
};

template <class T> struct TWSF : WSF {
  operator ripple::TypedField<T> const &() const {
    if (f_ == nullptr) {
        ripple::TypedField<T> const& sfield = ripple::newSField<ripple::TypedField<T>>(fieldValue_, fieldName_);
        f_ = (void *)&sfield;
    }
    return *static_cast<ripple::TypedField<T> *>(f_);
  };
};

struct CustomTWSF : TWSF<ripple::STPluginType> {
  operator ripple::TypedField<ripple::STPluginType> const &() const {
    if (f_ == nullptr) {
        ripple::SF_PLUGINTYPE const& sfield = ripple::constructCustomSField(tid_, fieldValue_, fieldName_.c_str());
        f_ = (void *)&sfield;
    }
    return *static_cast<ripple::TypedField<ripple::STPluginType> *>(f_);
  };
};

template <class T>
TWSF<T>
wrappedNewSField(const int fieldValue, const char* fieldName)
{
    return TWSF<T>{nullptr, fieldValue, std::string(fieldName), ripple::getSTId<ripple::TypedField<T>>()};
}

// used only for STArray and STObject
template <class T>
WSF
wrappedNewUntypedSField(const int fieldValue, std::string const fieldName)
{
    return WSF{nullptr, fieldValue, fieldName, ripple::getSTId<T>()};
}

TWSF<ripple::STPluginType>
constructCustomWrappedSField(int tid, const char* fn, int fv)
{
    return CustomTWSF{nullptr, fv, std::string(fn), tid};
}

template <class... Args>
static ripple::uint256
indexHash(std::uint16_t space, Args const&... args)
{
    return ripple::sha512Half(space, args...);
}

extern "C" void initializePluginPointers(
    void* pluginTxFormatPtr,
    void* pluginObjectsMapPtr,
    void* pluginInnerObjectFormatsPtr,
    void* knownCodeToFieldPtr,
    void* pluginSFieldCodesPtr,
    void* pluginSTypesPtr,
    void*pluginLeafParserMapPtr,
    void* pluginTERcodes)
{
    ripple::registerTxFormats(static_cast<std::map<std::uint16_t, ripple::PluginTxFormat>*>(pluginTxFormatPtr));
    ripple::registerLedgerObjects(static_cast<std::map<std::uint16_t, ripple::PluginLedgerFormat>*>(pluginObjectsMapPtr));
    ripple::registerPluginInnerObjectFormats(static_cast<std::map<std::uint16_t, ripple::PluginInnerObjectFormat>*>(pluginInnerObjectFormatsPtr));
    ripple::registerSFields(
        static_cast<std::map<int, ripple::SField const*>*>(knownCodeToFieldPtr),
        static_cast<std::vector<int>*>(pluginSFieldCodesPtr));
    ripple::registerSTypes(static_cast<std::map<int, ripple::STypeFunctions>*>(pluginSTypesPtr));
    ripple::registerLeafTypes(static_cast<std::map<int, ripple::parsePluginValuePtr>* >(pluginLeafParserMapPtr));
    ripple::registerPluginTERs(static_cast<std::vector<ripple::TERExport>*>(pluginTERcodes));
}

namespace py = pybind11;

PYBIND11_MODULE(rippled_py, m) {

    m.doc() = "Wrappers for rippled functionality.";

    py::options options;
    options.disable_enum_members_docstring();

    // This function is left uncommented on purpose
    // It should not be touched in Python code
    m.def("initializePluginPointers", &initializePluginPointers);

    /*
    * Enums
    */

   py::module_ ledgerObjectModule = m.def_submodule("ledger_objects", "Info pertaining to ledger object manipulation.");
    
    py::enum_<ripple::LedgerEntryType>(ledgerObjectModule, "LedgerEntryType", "Identifiers for on-ledger objects.\
\
    Each ledger object requires a unique type identifier, which is stored \
    within the object itself; this makes it possible to iterate the entire \
    ledger and determine each object's type and verify that the object you \
    retrieved from a given hash matches the expected type.")
        .value("ltACCOUNT_ROOT", ripple::LedgerEntryType::ltACCOUNT_ROOT)
        .value("ltDIR_NODE", ripple::LedgerEntryType::ltDIR_NODE)
        .value("ltRIPPLE_STATE", ripple::LedgerEntryType::ltRIPPLE_STATE)
        .value("ltTICKET", ripple::LedgerEntryType::ltTICKET)
        .value("ltSIGNER_LIST", ripple::LedgerEntryType::ltSIGNER_LIST)
        .value("ltOFFER", ripple::LedgerEntryType::ltOFFER)
        .value("ltLEDGER_HASHES", ripple::LedgerEntryType::ltLEDGER_HASHES)
        .value("ltAMENDMENTS", ripple::LedgerEntryType::ltAMENDMENTS)
        .value("ltFEE_SETTINGS", ripple::LedgerEntryType::ltFEE_SETTINGS)
        .value("ltESCROW", ripple::LedgerEntryType::ltESCROW)
        .value("ltPAYCHAN", ripple::LedgerEntryType::ltPAYCHAN)
        .value("ltCHECK", ripple::LedgerEntryType::ltCHECK)
        .value("ltDEPOSIT_PREAUTH", ripple::LedgerEntryType::ltDEPOSIT_PREAUTH)
        .value("ltNEGATIVE_UNL", ripple::LedgerEntryType::ltNEGATIVE_UNL)
        .value("ltNFTOKEN_PAGE", ripple::LedgerEntryType::ltNFTOKEN_PAGE)
        .value("ltNFTOKEN_OFFER", ripple::LedgerEntryType::ltNFTOKEN_OFFER)
        .value("ltANY", ripple::LedgerEntryType::ltANY)
        .value("ltCHILD", ripple::LedgerEntryType::ltCHILD)
        .export_values();
    
    py::enum_<ripple::LedgerSpecificFlags>(ledgerObjectModule, "LedgerSpecificFlags", "Valid flag values for ledger objects.")
        .value("lsfPasswordSpent", ripple::LedgerSpecificFlags::lsfPasswordSpent)
        .value("lsfRequireDestTag", ripple::LedgerSpecificFlags::lsfRequireDestTag)
        .value("lsfRequireAuth", ripple::LedgerSpecificFlags::lsfRequireAuth)
        .value("lsfDisallowXRP", ripple::LedgerSpecificFlags::lsfDisallowXRP)
        .value("lsfDisableMaster", ripple::LedgerSpecificFlags::lsfDisableMaster)
        .value("lsfNoFreeze", ripple::LedgerSpecificFlags::lsfNoFreeze)
        .value("lsfGlobalFreeze", ripple::LedgerSpecificFlags::lsfGlobalFreeze)
        .value("lsfDefaultRipple", ripple::LedgerSpecificFlags::lsfDefaultRipple)
        .value("lsfDepositAuth", ripple::LedgerSpecificFlags::lsfDepositAuth)
        .value("lsfDisallowIncomingNFTokenOffer", ripple::LedgerSpecificFlags::lsfDisallowIncomingNFTokenOffer)
        .value("lsfDisallowIncomingCheck", ripple::LedgerSpecificFlags::lsfDisallowIncomingCheck)
        .value("lsfDisallowIncomingPayChan", ripple::LedgerSpecificFlags::lsfDisallowIncomingPayChan)
        .value("lsfDisallowIncomingTrustline", ripple::LedgerSpecificFlags::lsfDisallowIncomingTrustline)
        .value("lsfPassive", ripple::LedgerSpecificFlags::lsfPassive)
        .value("lsfSell", ripple::LedgerSpecificFlags::lsfSell)
        .value("lsfLowReserve", ripple::LedgerSpecificFlags::lsfLowReserve)
        .value("lsfHighReserve", ripple::LedgerSpecificFlags::lsfHighReserve)
        .value("lsfLowAuth", ripple::LedgerSpecificFlags::lsfLowAuth)
        .value("lsfHighAuth", ripple::LedgerSpecificFlags::lsfHighAuth)
        .value("lsfLowNoRipple", ripple::LedgerSpecificFlags::lsfLowNoRipple)
        .value("lsfHighNoRipple", ripple::LedgerSpecificFlags::lsfHighNoRipple)
        .value("lsfLowFreeze", ripple::LedgerSpecificFlags::lsfLowFreeze)
        .value("lsfHighFreeze", ripple::LedgerSpecificFlags::lsfHighFreeze)
        .value("lsfOneOwnerCount", ripple::LedgerSpecificFlags::lsfOneOwnerCount)
        .value("lsfNFTokenBuyOffers", ripple::LedgerSpecificFlags::lsfNFTokenBuyOffers)
        .value("lsfNFTokenSellOffers", ripple::LedgerSpecificFlags::lsfNFTokenSellOffers)
        .value("lsfSellNFToken", ripple::LedgerSpecificFlags::lsfSellNFToken)
        .export_values();
    
    /*
    * Return code enums
    */

   py::module_ terModule = m.def_submodule("return_codes", "Existing TER return codes.");

    py::enum_<ripple::TEScodes>(terModule, "TEScodes", py::arithmetic(), "Codes that indicate a transaction succeeded.")
        .value("tesSUCCESS", ripple::TEScodes::tesSUCCESS)
        .export_values();
    
    py::enum_<ripple::TEMcodes>(terModule, "TEMcodes", py::arithmetic(), "Codes that  indicate that the transaction was malformed, and cannot succeed according to the XRP Ledger protocol.")
        .value("temMALFORMED", ripple::TEMcodes::temMALFORMED)
        .value("temBAD_AMOUNT", ripple::TEMcodes::temBAD_AMOUNT)
        .value("temBAD_CURRENCY", ripple::TEMcodes::temBAD_CURRENCY)
        .value("temBAD_EXPIRATION", ripple::TEMcodes::temBAD_EXPIRATION)
        .value("temBAD_FEE", ripple::TEMcodes::temBAD_FEE)
        .value("temBAD_ISSUER", ripple::TEMcodes::temBAD_ISSUER)
        .value("temBAD_LIMIT", ripple::TEMcodes::temBAD_LIMIT)
        .value("temBAD_OFFER", ripple::TEMcodes::temBAD_OFFER)
        .value("temBAD_PATH", ripple::TEMcodes::temBAD_PATH)
        .value("temBAD_PATH_LOOP", ripple::TEMcodes::temBAD_PATH_LOOP)
        .value("temBAD_REGKEY", ripple::TEMcodes::temBAD_REGKEY)
        .value("temBAD_SEND_XRP_LIMIT", ripple::TEMcodes::temBAD_SEND_XRP_LIMIT)
        .value("temBAD_SEND_XRP_MAX", ripple::TEMcodes::temBAD_SEND_XRP_MAX)
        .value("temBAD_SEND_XRP_NO_DIRECT", ripple::TEMcodes::temBAD_SEND_XRP_NO_DIRECT)
        .value("temBAD_SEND_XRP_PARTIAL", ripple::TEMcodes::temBAD_SEND_XRP_PARTIAL)
        .value("temBAD_SEND_XRP_PATHS", ripple::TEMcodes::temBAD_SEND_XRP_PATHS)
        .value("temBAD_SEQUENCE", ripple::TEMcodes::temBAD_SEQUENCE)
        .value("temBAD_SIGNATURE", ripple::TEMcodes::temBAD_SIGNATURE)
        .value("temBAD_SRC_ACCOUNT", ripple::TEMcodes::temBAD_SRC_ACCOUNT)
        .value("temBAD_TRANSFER_RATE", ripple::TEMcodes::temBAD_TRANSFER_RATE)
        .value("temDST_IS_SRC", ripple::TEMcodes::temDST_IS_SRC)
        .value("temDST_NEEDED", ripple::TEMcodes::temDST_NEEDED)
        .value("temINVALID", ripple::TEMcodes::temINVALID)
        .value("temINVALID_FLAG", ripple::TEMcodes::temINVALID_FLAG)
        .value("temREDUNDANT", ripple::TEMcodes::temREDUNDANT)
        .value("temRIPPLE_EMPTY", ripple::TEMcodes::temRIPPLE_EMPTY)
        .value("temDISABLED", ripple::TEMcodes::temDISABLED)
        .value("temBAD_SIGNER", ripple::TEMcodes::temBAD_SIGNER)
        .value("temBAD_QUORUM", ripple::TEMcodes::temBAD_QUORUM)
        .value("temBAD_WEIGHT", ripple::TEMcodes::temBAD_WEIGHT)
        .value("temBAD_TICK_SIZE", ripple::TEMcodes::temBAD_TICK_SIZE)
        .value("temINVALID_ACCOUNT_ID", ripple::TEMcodes::temINVALID_ACCOUNT_ID)
        .value("temCANNOT_PREAUTH_SELF", ripple::TEMcodes::temCANNOT_PREAUTH_SELF)
        .value("temINVALID_COUNT", ripple::TEMcodes::temINVALID_COUNT)
        .value("temUNCERTAIN", ripple::TEMcodes::temUNCERTAIN)
        .value("temUNKNOWN", ripple::TEMcodes::temUNKNOWN)
        .value("temSEQ_AND_TICKET", ripple::TEMcodes::temSEQ_AND_TICKET)
        .value("temBAD_NFTOKEN_TRANSFER_FEE", ripple::TEMcodes::temBAD_NFTOKEN_TRANSFER_FEE)
        .export_values();
    
    py::enum_<ripple::TECcodes>(terModule, "TECcodes", py::arithmetic(), "Codes that indicate that the transaction failed, but it was applied to a ledger to apply the transaction cost.")
        .value("tecCLAIM", ripple::TECcodes::tecCLAIM)
        .value("tecPATH_PARTIAL", ripple::TECcodes::tecPATH_PARTIAL)
        .value("tecUNFUNDED_ADD", ripple::TECcodes::tecUNFUNDED_ADD)
        .value("tecUNFUNDED_OFFER", ripple::TECcodes::tecUNFUNDED_OFFER)
        .value("tecUNFUNDED_PAYMENT", ripple::TECcodes::tecUNFUNDED_PAYMENT)
        .value("tecFAILED_PROCESSING", ripple::TECcodes::tecFAILED_PROCESSING)
        .value("tecDIR_FULL", ripple::TECcodes::tecDIR_FULL)
        .value("tecINSUF_RESERVE_LINE", ripple::TECcodes::tecINSUF_RESERVE_LINE)
        .value("tecINSUF_RESERVE_OFFER", ripple::TECcodes::tecINSUF_RESERVE_OFFER)
        .value("tecNO_DST", ripple::TECcodes::tecNO_DST)
        .value("tecNO_DST_INSUF_XRP", ripple::TECcodes::tecNO_DST_INSUF_XRP)
        .value("tecNO_LINE_INSUF_RESERVE", ripple::TECcodes::tecNO_LINE_INSUF_RESERVE)
        .value("tecNO_LINE_REDUNDANT", ripple::TECcodes::tecNO_LINE_REDUNDANT)
        .value("tecPATH_DRY", ripple::TECcodes::tecPATH_DRY)
        .value("tecUNFUNDED", ripple::TECcodes::tecUNFUNDED)
        .value("tecNO_ALTERNATIVE_KEY", ripple::TECcodes::tecNO_ALTERNATIVE_KEY)
        .value("tecNO_REGULAR_KEY", ripple::TECcodes::tecNO_REGULAR_KEY)
        .value("tecOWNERS", ripple::TECcodes::tecOWNERS)
        .value("tecNO_ISSUER", ripple::TECcodes::tecNO_ISSUER)
        .value("tecNO_AUTH", ripple::TECcodes::tecNO_AUTH)
        .value("tecNO_LINE", ripple::TECcodes::tecNO_LINE)
        .value("tecINSUFF_FEE", ripple::TECcodes::tecINSUFF_FEE)
        .value("tecFROZEN", ripple::TECcodes::tecFROZEN)
        .value("tecNO_TARGET", ripple::TECcodes::tecNO_TARGET)
        .value("tecNO_PERMISSION", ripple::TECcodes::tecNO_PERMISSION)
        .value("tecNO_ENTRY", ripple::TECcodes::tecNO_ENTRY)
        .value("tecINSUFFICIENT_RESERVE", ripple::TECcodes::tecINSUFFICIENT_RESERVE)
        .value("tecNEED_MASTER_KEY", ripple::TECcodes::tecNEED_MASTER_KEY)
        .value("tecDST_TAG_NEEDED", ripple::TECcodes::tecDST_TAG_NEEDED)
        .value("tecINTERNAL", ripple::TECcodes::tecINTERNAL)
        .value("tecOVERSIZE", ripple::TECcodes::tecOVERSIZE)
        .value("tecCRYPTOCONDITION_ERROR", ripple::TECcodes::tecCRYPTOCONDITION_ERROR)
        .value("tecINVARIANT_FAILED", ripple::TECcodes::tecINVARIANT_FAILED)
        .value("tecEXPIRED", ripple::TECcodes::tecEXPIRED)
        .value("tecDUPLICATE", ripple::TECcodes::tecDUPLICATE)
        .value("tecKILLED", ripple::TECcodes::tecKILLED)
        .value("tecHAS_OBLIGATIONS", ripple::TECcodes::tecHAS_OBLIGATIONS)
        .value("tecTOO_SOON", ripple::TECcodes::tecTOO_SOON)
        .value("tecMAX_SEQUENCE_REACHED", ripple::TECcodes::tecMAX_SEQUENCE_REACHED)
        .value("tecNO_SUITABLE_NFTOKEN_PAGE", ripple::TECcodes::tecNO_SUITABLE_NFTOKEN_PAGE)
        .value("tecNFTOKEN_BUY_SELL_MISMATCH", ripple::TECcodes::tecNFTOKEN_BUY_SELL_MISMATCH)
        .value("tecNFTOKEN_OFFER_TYPE_MISMATCH", ripple::TECcodes::tecNFTOKEN_OFFER_TYPE_MISMATCH)
        .value("tecCANT_ACCEPT_OWN_NFTOKEN_OFFER", ripple::TECcodes::tecCANT_ACCEPT_OWN_NFTOKEN_OFFER)
        .value("tecINSUFFICIENT_FUNDS", ripple::TECcodes::tecINSUFFICIENT_FUNDS)
        .value("tecOBJECT_NOT_FOUND", ripple::TECcodes::tecOBJECT_NOT_FOUND)
        .value("tecINSUFFICIENT_PAYMENT", ripple::TECcodes::tecINSUFFICIENT_PAYMENT)
        .export_values();
    
    py::enum_<ripple::TERcodes>(terModule, "TERcodes", py::arithmetic(), "Codes that indicate that the transaction failed, but it could apply successfully in the future, usually if some other hypothetical transaction applies first.")
        .value("terRETRY", ripple::TERcodes::terRETRY)
        .value("terFUNDS_SPENT", ripple::TERcodes::terFUNDS_SPENT)
        .value("terINSUF_FEE_B", ripple::TERcodes::terINSUF_FEE_B)
        .value("terNO_ACCOUNT", ripple::TERcodes::terNO_ACCOUNT)
        .value("terNO_AUTH", ripple::TERcodes::terNO_AUTH)
        .value("terNO_LINE", ripple::TERcodes::terNO_LINE)
        .value("terOWNERS", ripple::TERcodes::terOWNERS)
        .value("terPRE_SEQ", ripple::TERcodes::terPRE_SEQ)
        .value("terLAST", ripple::TERcodes::terLAST)
        .value("terNO_RIPPLE", ripple::TERcodes::terNO_RIPPLE)
        .value("terQUEUED", ripple::TERcodes::terQUEUED)
        .value("terPRE_TICKET", ripple::TERcodes::terPRE_TICKET)
        .export_values();

    py::enum_<ripple::TEFcodes>(terModule, "TEFcodes", py::arithmetic(), "Codes that indicate that the transaction failed and was not included in a ledger, but the transaction could have succeeded in some theoretical ledger.")
        .value("tefFAILURE", ripple::TEFcodes::tefFAILURE)
        .value("tefALREADY", ripple::TEFcodes::tefALREADY)
        .value("tefBAD_ADD_AUTH", ripple::TEFcodes::tefBAD_ADD_AUTH)
        .value("tefBAD_AUTH", ripple::TEFcodes::tefBAD_AUTH)
        .value("tefBAD_LEDGER", ripple::TEFcodes::tefBAD_LEDGER)
        .value("tefCREATED", ripple::TEFcodes::tefCREATED)
        .value("tefEXCEPTION", ripple::TEFcodes::tefEXCEPTION)
        .value("tefINTERNAL", ripple::TEFcodes::tefINTERNAL)
        .value("tefNO_AUTH_REQUIRED", ripple::TEFcodes::tefNO_AUTH_REQUIRED)
        .value("tefPAST_SEQ", ripple::TEFcodes::tefPAST_SEQ)
        .value("tefWRONG_PRIOR", ripple::TEFcodes::tefWRONG_PRIOR)
        .value("tefMASTER_DISABLED", ripple::TEFcodes::tefMASTER_DISABLED)
        .value("tefMAX_LEDGER", ripple::TEFcodes::tefMAX_LEDGER)
        .value("tefBAD_SIGNATURE", ripple::TEFcodes::tefBAD_SIGNATURE)
        .value("tefBAD_QUORUM", ripple::TEFcodes::tefBAD_QUORUM)
        .value("tefNOT_MULTI_SIGNING", ripple::TEFcodes::tefNOT_MULTI_SIGNING)
        .value("tefBAD_AUTH_MASTER", ripple::TEFcodes::tefBAD_AUTH_MASTER)
        .value("tefINVARIANT_FAILED", ripple::TEFcodes::tefINVARIANT_FAILED)
        .value("tefTOO_BIG", ripple::TEFcodes::tefTOO_BIG)
        .value("tefNO_TICKET", ripple::TEFcodes::tefNO_TICKET)
        .value("tefNFTOKEN_IS_NOT_TRANSFERABLE", ripple::TEFcodes::tefNFTOKEN_IS_NOT_TRANSFERABLE)
        .export_values();
    
    py::enum_<ripple::TELcodes>(terModule, "TELcodes", py::arithmetic(), "Codes that indicate an error in the local server processing the transaction; it is possible that another server with a different configuration or load level could process the transaction successfully.")
        .value("telLOCAL_ERROR", ripple::TELcodes::telLOCAL_ERROR)
        .value("telBAD_DOMAIN", ripple::TELcodes::telBAD_DOMAIN)
        .value("telBAD_PATH_COUNT", ripple::TELcodes::telBAD_PATH_COUNT)
        .value("telBAD_PUBLIC_KEY", ripple::TELcodes::telBAD_PUBLIC_KEY)
        .value("telFAILED_PROCESSING", ripple::TELcodes::telFAILED_PROCESSING)
        .value("telINSUF_FEE_P", ripple::TELcodes::telINSUF_FEE_P)
        .value("telNO_DST_PARTIAL", ripple::TELcodes::telNO_DST_PARTIAL)
        .value("telCAN_NOT_QUEUE", ripple::TELcodes::telCAN_NOT_QUEUE)
        .value("telCAN_NOT_QUEUE_BALANCE", ripple::TELcodes::telCAN_NOT_QUEUE_BALANCE)
        .value("telCAN_NOT_QUEUE_BLOCKS", ripple::TELcodes::telCAN_NOT_QUEUE_BLOCKS)
        .value("telCAN_NOT_QUEUE_BLOCKED", ripple::TELcodes::telCAN_NOT_QUEUE_BLOCKED)
        .value("telCAN_NOT_QUEUE_FEE", ripple::TELcodes::telCAN_NOT_QUEUE_FEE)
        .value("telCAN_NOT_QUEUE_FULL", ripple::TELcodes::telCAN_NOT_QUEUE_FULL)
        .value("telWRONG_NETWORK", ripple::TELcodes::telWRONG_NETWORK)
        .value("telREQUIRES_NETWORK_ID", ripple::TELcodes::telREQUIRES_NETWORK_ID)
        .value("telNETWORK_ID_MAKES_TX_NON_CANONICAL", ripple::TELcodes::telNETWORK_ID_MAKES_TX_NON_CANONICAL)
        .export_values();
    
    py::class_<ripple::NotTEC> NotTEC(terModule, "NotTEC", "A subset of return codes that includes all return codes *except* `tec` codes.");
    NotTEC
        .def(py::init<ripple::TEScodes>())
        .def(py::init<ripple::TEMcodes>())
        .def(py::init<ripple::TERcodes>())
        .def(py::init<ripple::TEFcodes>())
        .def(py::init<ripple::TELcodes>())
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::implicitly_convertible<ripple::TEScodes, ripple::NotTEC>();
    py::implicitly_convertible<ripple::TEMcodes, ripple::NotTEC>();
    py::implicitly_convertible<ripple::TERcodes, ripple::NotTEC>();
    py::implicitly_convertible<ripple::TEFcodes, ripple::NotTEC>();
    py::implicitly_convertible<ripple::TERcodes, ripple::NotTEC>();
    py::implicitly_convertible<ripple::TEFcodes, ripple::NotTEC>();
    py::implicitly_convertible<ripple::TELcodes, ripple::NotTEC>();

    py::class_<ripple::TER> TER(terModule, "TER", "An overarching class representing all possible return types.");
    TER
        .def(py::init<ripple::TEScodes>())
        .def(py::init<ripple::TEMcodes>())
        .def(py::init<ripple::TERcodes>())
        .def(py::init<ripple::TEFcodes>())
        .def(py::init<ripple::TELcodes>())
        .def(py::init<ripple::TECcodes>())
        .def(py::init<ripple::NotTEC>())
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::implicitly_convertible<ripple::TEScodes, ripple::TER>();
    py::implicitly_convertible<ripple::TEMcodes, ripple::TER>();
    py::implicitly_convertible<ripple::TERcodes, ripple::TER>();
    py::implicitly_convertible<ripple::TEFcodes, ripple::TER>();
    py::implicitly_convertible<ripple::TELcodes, ripple::TER>();
    py::implicitly_convertible<ripple::TECcodes, ripple::TER>();
    py::implicitly_convertible<ripple::NotTEC, ripple::TER>();
    
    /*
    * Small classes
    */

    py::module_ sfieldModule = m.def_submodule("sfields", "All existing SFields.");

    py::class_<WSF> PythonWSF(sfieldModule, "SField", "A serialized field that can be included in transactions and ledger objects.");
    PythonWSF
        .def_property_readonly("fieldCode",
            [](const WSF &wsf) {
                return wsf.tid_ ? (wsf.tid_ << 16) | wsf.fieldValue_ : static_cast<ripple::SField const&>(wsf).fieldCode;
            },
            "(fieldType<<16)|fieldValue"
        )
        .def_property_readonly("fieldNum",
            [](const WSF &wsf) {
                return static_cast<ripple::SField const&>(wsf).fieldNum;
            }
        )
        .def_property_readonly("fieldName",
            [](const WSF &wsf) {
                return wsf.tid_ ? wsf.fieldName_ : static_cast<ripple::SField const&>(wsf).fieldName;
            },
            "The name of the field."
        )
        .def_property_readonly("fieldType",
            [](const WSF &wsf) {
                return wsf.tid_ ? wsf.tid_ : static_cast<ripple::SField const&>(wsf).fieldType;
            },
            "The SType of the field."
        )
        .def_property_readonly("fieldValue",
            [](const WSF &wsf) {
                return wsf.tid_ ? wsf.fieldValue_ : static_cast<ripple::SField const&>(wsf).fieldValue;
            },
            "The unique value of the field for that SType."
        )
        .def("__repr__",
            [](const WSF &wsf) {
                return "sf" + (wsf.tid_ ? wsf.fieldName_ : static_cast<ripple::SField const&>(wsf).fieldName);
            }
        )
        ;
    py::class_<TWSF<ripple::STAccount>, WSF> TWSF_STAccount(sfieldModule, "SF_ACCOUNT", "An SField representing an Account type.");
    py::class_<TWSF<ripple::STAmount>, WSF> TWSF_STAmount(sfieldModule, "SF_AMOUNT", "An SField representing an Amount type.");
    py::class_<TWSF<ripple::STCurrency>, WSF> TWSF_STCurrency(sfieldModule, "SF_CURRENCY", "An SField representing a Currency type.");
    py::class_<TWSF<ripple::STIssue>, WSF> TWSF_STIssue(sfieldModule, "SF_ISSUE", "An SField representing an Issue type.");
    py::class_<TWSF<ripple::STUInt8>, WSF> TWSF_STUInt8(sfieldModule, "SF_UINT8", "An SField representing a uint8 type.");
    py::class_<TWSF<ripple::STUInt16>, WSF> TWSF_STUInt16(sfieldModule, "SF_UINT16", "An SField representing a uint16 type.");
    py::class_<TWSF<ripple::STUInt32>, WSF> TWSF_STUInt32(sfieldModule, "SF_UINT32", "An SField representing a uint32 type.");
    py::class_<TWSF<ripple::STUInt64>, WSF> TWSF_STUInt64(sfieldModule, "SF_UINT64", "An SField representing a uint64 type.");
    py::class_<TWSF<ripple::STUInt128>, WSF> TWSF_STUInt128(sfieldModule, "SF_UINT128", "An SField representing a uint128 type.");
    py::class_<TWSF<ripple::STUInt160>, WSF> TWSF_STUInt160(sfieldModule, "SF_UINT160", "An SField representing a uint160 type.");
    py::class_<TWSF<ripple::STUInt256>, WSF> TWSF_STUInt256(sfieldModule, "SF_UINT256", "An SField representing a uint256 type.");
    py::class_<TWSF<ripple::STBlob>, WSF> TWSF_STBlob(sfieldModule, "SF_VL", "An SField representing a variable length type.");
    py::class_<TWSF<ripple::STPluginType>, WSF> TWSF_STPluginType(sfieldModule, "SF_PLUGINTYPE", "An SField representing a type that has been imported via a plugin.");
    py::class_<TWSF<ripple::STVector256>, WSF> TWSF_STVector256(sfieldModule, "SF_VECTOR256", "An SField representing a 256-bit vector type.");
    py::class_<TWSF<ripple::STXChainBridge>, WSF> TWSF_STXChainBridge(sfieldModule, "SF_XCHAINBRIDGE", "An SField representing an XChainBridge type.");

    py::module_ basicsModule = m.def_submodule("basic_types", "Basic/fundamental types.");

    py::enum_<ripple::SOEStyle>(basicsModule, "SOEStyle", "The kind of parameter in each entry of a transactor/ledger object template.")
        .value("soeINVALID", ripple::SOEStyle::soeINVALID)
        .value("soeREQUIRED", ripple::SOEStyle::soeREQUIRED)
        .value("soeOPTIONAL", ripple::SOEStyle::soeOPTIONAL)
        .value("soeDEFAULT", ripple::SOEStyle::soeDEFAULT)
        .export_values();

    py::class_<ripple::Buffer> Buffer(basicsModule, "Buffer", "A custom char vector implementation.");
    Buffer
        .def("__repr__",
            [](const ripple::Buffer &buf) {
                return strHex(buf);
            }
        );

    py::class_<ripple::AccountID> AccountID(basicsModule, "AccountID", "A 160-bit unsigned int that uniquely identifies an account.");
    AccountID
        .def(py::init<>())
        .def(py::init<ripple::Slice>())
        .def("parse_hex",
            [](ripple::AccountID acct, std::string const& hex) {
                return acct.parseHex(hex);
            },
            "Parse a hex representation of an account."
        )
        .def("to_base58",
            [](const ripple::AccountID &a) {
                return ripple::toBase58(a);
            },
            "Get the base58 representation of the account."
        )
        .def("to_buffer",
            [](const ripple::AccountID &a) {
                return ripple::Buffer(a.data(), a.size());
            },
            py::return_value_policy::move,
            "Convert the AccountID data to a buffer."
        )
        .def("from_buffer",
            [](const ripple::Buffer &buf) {
                return ripple::AccountID::fromVoid(std::move(buf.data()));
            },
            "Convert the data in a buffer to an AccountID."
        )
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__repr__",
            [](const ripple::AccountID &a) {
                return "<AccountID:'" + ripple::toBase58(a) + "'>";
            }
        );
    
    py::class_<ripple::Seed> Seed(basicsModule, "Seed", "A secret used to generate deterministic secret keys.");

    py::class_<ripple::XRPAmount> XRPAmount(basicsModule, "XRPAmount", "A class representing an amount of XRP.");
    XRPAmount
        .def(py::init<std::int64_t &>())
        .def_property_readonly("drops",
            [](const ripple::XRPAmount &xrpAmount) {
                return xrpAmount.drops();
            },
            "The amount of XRP represented in drops."
        )
        .def(py::self += py::self)
        .def(py::self + py::self)
        .def(py::self -= py::self)
        .def(py::self - py::self)
        .def(py::self *= int())
        .def(py::self * int())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self)
        .def(py::self > py::self)
        .def(py::self <= py::self)
        .def(py::self >= py::self)
        .def(py::self == int())
        .def(py::self != int())
        .def(-py::self)
        .def("__repr__",
            [](const ripple::XRPAmount &xrp) {
                return ripple::to_string(xrp);
            }
        );
    
    py::class_<ripple::IOUAmount> IOUAmount(basicsModule, "IOUAmount", "A class representing an amount of an IOU.");
    IOUAmount
        .def(py::self += py::self)
        .def(py::self + py::self)
        .def(py::self -= py::self)
        .def(py::self - py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self)
        .def(py::self > py::self)
        .def(py::self <= py::self)
        .def(py::self >= py::self)
        .def(-py::self)
        .def("__repr__",
            [](const ripple::IOUAmount &iou) {
                return ripple::to_string(iou);
            }
        );

    py::class_<ripple::Issue> Issue(basicsModule, "Issue", "A currency issued by an account.");
    py::class_<ripple::Currency> Currency(basicsModule, "Currency", "A hash representing a specific currency.");

    py::class_<ripple::uint256> uint256(basicsModule, "uint256", "A 256-bit uint.");
    uint256
        .def("__repr__",
            [](const ripple::uint256 &uint256) {
                return ripple::to_string(uint256);
            }
        );

        py::class_<ripple::uint128> uint128(basicsModule, "uint128", "A 128-bit uint.");
    uint128
        .def("__repr__",
            [](const ripple::uint128 &uint128) {
                return ripple::to_string(uint128);
            }
        );
    
    py::class_<ripple::uint160> uint160(basicsModule, "uint160", "A 160-bit uint.");
    uint160
        .def("__repr__",
            [](const ripple::uint160 &uint160) {
                return ripple::to_string(uint160);
            }
        );

    py::class_<ripple::Slice> Slice(basicsModule, "Slice", "An immutable linear range of bytes.");
    Slice
        .def("to_buffer",
            [](const ripple::Slice &slice) {
                return ripple::Buffer(slice.data(), slice.size());
            },
            py::return_value_policy::move,
            "Convert the Slice data to a buffer."
        )
        .def("from_buffer",
            [](const ripple::Buffer &buf) {
                return ripple::Slice(std::move(buf.data()), buf.size());
            },
            "Convert Buffer data to a Slice."
        )
        .def("__repr__",
            [](const ripple::Slice &slice) {
                return strHex(slice);
            }
        )
    ;

    py::class_<beast::Zero> BeastZero(basicsModule, "BeastZero");
    py::class_<beast::Journal> Journal(basicsModule, "Journal", "A generic endpoint for log messages.");
    py::class_<ripple::SeqProxy> SeqProxy(basicsModule, "SeqProxy", "A type that represents either a sequence value or a ticket value.");
    SeqProxy
        .def("value", &ripple::SeqProxy::value, "The integer value.")
    ;

    py::class_<Json::Value> JsonValue(basicsModule, "JsonValue", "Represents a JSON value.");
    JsonValue
        .def("is_string", &Json::Value::isString, "Returns True if the value is a string.")
        .def("as_string", &Json::Value::asString, "Converts the value to a string.")
        .def("__repr__", &Json::Value::asCString);

    py::enum_<ripple::VoteBehavior>(basicsModule, "VoteBehavior", "An enum representing the default vote for an amendment.")
        .value("OBSOLETE", ripple::VoteBehavior::Obsolete)
        .value("DEFAULT_NO", ripple::VoteBehavior::DefaultNo)
        .value("DEFAULT_YES", ripple::VoteBehavior::DefaultYes);

    /*
    * STObjects
    */
    py::module_ sTypeModule = m.def_submodule("stypes", "All existing Serialized Types.");

    py::class_<ripple::STBase> STBase(sTypeModule, "STBase", "A type which can be exported to a well-known binary format.");
    STBase
        .def("get_fname",
            [](const ripple::STBase &obj) {
                ripple::SField const& f = obj.getFName();
                return TWSF<ripple::STPluginType>{(void *)&f};
            },
            "Get the associated SField."
        )
        .def("__repr__",
            [](const ripple::STBase &obj) {
                return obj.getFullText();
            }
        );
    py::class_<ripple::STAccount, ripple::STBase> STAccount(sTypeModule, "STAccount", "A serialized type representing an Account ID.");

    py::class_<ripple::STAmount, ripple::STBase> STAmount(sTypeModule, "STAmount", "A serialized type representing an Amount (XRP/IOU).");
    STAmount
        .def(py::init<ripple::STAmount &>())
        .def("is_xrp",
            [](const ripple::STAmount &amt) {
                return ripple::isXRP(amt);
            },
            "Returns True if the STAmount represents an XRP amount."
        )
        .def("issuer",
            [](const ripple::STAmount &amt) {
                return amt.issue();
            },
            "Returns the issue of a IOUAmount (if possible)"
        )
        .def("xrp", &ripple::STAmount::xrp, "Converts the STAmount to an XRPAmount (if possible).")
        .def("iou", &ripple::STAmount::iou, "Converts the STAmount to an IOUAmount (if possible).")
        .def("native", &ripple::STAmount::native, "Returns True if the STAmount represents an XRP amount (the native currency).")
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self + py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self)
        .def(py::self > py::self)
        .def(py::self <= py::self)
        .def(py::self >= py::self)
        .def("__eq__", [](const ripple::STAmount &a, beast::Zero b) {
            return a == b;
        }, py::is_operator())
        .def("__ne__", [](const ripple::STAmount &a, beast::Zero b) {
            return a != b;
        }, py::is_operator())
        .def("__lt__", [](const ripple::STAmount &a, beast::Zero b) {
            return a < b;
        }, py::is_operator())
        .def("__le__", [](const ripple::STAmount &a, beast::Zero b) {
            return a <= b;
        }, py::is_operator())
        .def("__gt__", [](const ripple::STAmount &a, beast::Zero b) {
            return a > b;
        }, py::is_operator())
        .def("__ge__", [](const ripple::STAmount &a, beast::Zero b) {
            return a >= b;
        }, py::is_operator())
        .def(-py::self)
        .def(py::self - py::self)
    ;

    py::class_<ripple::STBlob, ripple::STBase> STBlob(sTypeModule, "STBlob", "A serializeable type representing a variable-length element.");
    py::class_<ripple::STUInt8, ripple::STBase> STUInt8(sTypeModule, "STUInt8", "A serializeable type representing a uint8.");
    py::class_<ripple::STUInt16, ripple::STBase> STUInt16(sTypeModule, "STUInt16", "A serializeable type representing a uint16.");
    py::class_<ripple::STUInt32, ripple::STBase> STUInt32(sTypeModule, "STUInt32", "A serializeable type representing a uint32.");
    py::class_<ripple::STUInt64, ripple::STBase> STUInt64(sTypeModule, "STUInt64", "A serializeable type representing a uint64.");
    py::class_<ripple::STUInt128, ripple::STBase> STUInt128(sTypeModule, "STUInt128", "A serializeable type representing a uint128.");
    py::class_<ripple::STUInt160, ripple::STBase> STUInt160(sTypeModule, "STUInt160", "A serializeable type representing a uint160.");
    py::class_<ripple::STUInt256, ripple::STBase> STUInt256(sTypeModule, "STUInt256", "A serializeable type representing a uint256.");
    py::class_<ripple::STPathSet, ripple::STBase> STPathSet(sTypeModule, "STPathSet", "A serializeable type representing a path set in a cross-currency payment.");
    py::class_<ripple::STPluginType, ripple::STBase> STPluginType(sTypeModule, "STPluginType", "A serializeable type that is imported at runtime via a plugin.");
    py::class_<ripple::STVector256, ripple::STBase> STVector256(sTypeModule, "STVector256", "A serializeable type representing a 256-bit vector.");
    py::class_<ripple::STIssue, ripple::STBase> STIssue(sTypeModule, "STIssue", "A serializeable type representing an issued currency.");
    py::class_<ripple::STXChainBridge, ripple::STBase> STXChainBridge(sTypeModule, "STXChainBridge", "A serializeable type representing a cross-chain bridge.");
    py::class_<ripple::STCurrency, ripple::STBase> STCurrency(sTypeModule, "STCurrency", "A serializeable type representing a currency.");

    py::class_<ripple::STArray, ripple::STBase> STArray(sTypeModule, "STArray", "A serializeable type representing an array of other serialized types.");
    STArray
        .def(py::init<>())
        .def("append", [](ripple::STArray &arr, ripple::STObject& obj) {
            arr.push_back(std::move(obj));
        })
    ;

    py::class_<ripple::STObject, std::shared_ptr<ripple::STObject>> STObject(sTypeModule, "STObject", "A serializeable type representing a map of other serialized types.");
    STObject
        .def("is_flag", &ripple::STObject::isFlag, "Returns True if the object has the given flag.")
        .def("get_flags", &ripple::STTx::getFlags, "Returns the flags on the object.")
        .def("__getitem__", [](const ripple::STObject &obj, TWSF<ripple::STAccount> sf) {
            return obj[static_cast<ripple::TypedField<ripple::STAccount> const&>(sf)];
        }, py::return_value_policy::move)
        .def("__getitem__", [](const ripple::STObject &obj, TWSF<ripple::STAmount> sf) {
            return obj[static_cast<ripple::TypedField<ripple::STAmount> const&>(sf)];
        }, py::return_value_policy::move)
        .def("__getitem__", [](const ripple::STObject &obj, TWSF<ripple::STUInt32> sf) {
            return obj[static_cast<ripple::TypedField<ripple::STUInt32> const&>(sf)];
        }, py::return_value_policy::move)
        .def("__getitem__", [](const ripple::STObject &obj, TWSF<ripple::STUInt64> sf) {
            return obj[static_cast<ripple::TypedField<ripple::STUInt64> const&>(sf)];
        }, py::return_value_policy::move)
        .def("__getitem__", [](const ripple::STObject &obj, TWSF<ripple::STUInt256> sf) {
            return obj[static_cast<ripple::TypedField<ripple::STUInt256> const&>(sf)];
        }, py::return_value_policy::move)
        .def("__getitem__", [](const ripple::STObject &obj, TWSF<ripple::STBlob> sf) {
            return obj[static_cast<ripple::TypedField<ripple::STBlob> const&>(sf)];
        }, py::return_value_policy::move)
        .def("__getitem__", [](const ripple::STObject &obj, TWSF<ripple::STPluginType> sf) {
            return obj[static_cast<ripple::TypedField<ripple::STPluginType> const&>(sf)];
        }, py::return_value_policy::move) // TODO: avoid doing a memory allocation every time this is called
        .def("__setitem__", [](ripple::STObject &obj, TWSF<ripple::STAccount> sf, ripple::STAccount::value_type value) {
            obj[static_cast<ripple::TypedField<ripple::STAccount> const&>(sf)] = value;
        })
        .def("__setitem__", [](ripple::STObject &obj, TWSF<ripple::STAmount> sf, ripple::STAmount value) {
            obj[static_cast<ripple::TypedField<ripple::STAmount> const&>(sf)] = value;
        })
        .def("__setitem__", [](ripple::STObject &obj, TWSF<ripple::STUInt32> sf, std::uint32_t value) {
            obj[static_cast<ripple::TypedField<ripple::STUInt32> const&>(sf)] = value;
        })
        .def("__setitem__", [](ripple::STObject &obj, TWSF<ripple::STUInt64> sf, std::uint64_t value) {
            obj[static_cast<ripple::TypedField<ripple::STUInt64> const&>(sf)] = value;
        })
        .def("__setitem__", [](ripple::STObject &obj, TWSF<ripple::STUInt256> sf, ripple::uint256 value) {
            obj[static_cast<ripple::TypedField<ripple::STUInt256> const&>(sf)] = value;
        })
        .def("__setitem__", [](ripple::STObject &obj, TWSF<ripple::STBlob> sf, ripple::STBlob::value_type value) {
            obj[static_cast<ripple::TypedField<ripple::STBlob> const&>(sf)] = value;
        })
        .def("__setitem__", [](ripple::STObject &obj, TWSF<ripple::STPluginType> sf, ripple::STPluginType::value_type value) {
            obj[static_cast<ripple::TypedField<ripple::STPluginType> const&>(sf)] = value;
        })
        .def("set_field_array", [](ripple::STObject &obj, const WSF& sf, ripple::STArray value) {
            obj.setFieldArray(sf, value);
        })
        .def("is_field_present",
            [](const ripple::STObject &obj, const WSF &wsf) {
                return obj.isFieldPresent(static_cast<ripple::SField const&>(wsf));
            },
            "Returns True if the field is present."
        )
        .def("make_field_absent",
            [](ripple::STObject &obj, const WSF &wsf) {
                return obj.makeFieldAbsent(static_cast<ripple::SField const&>(wsf));
            },
            "Modifies the object to delete the field."
        )
        .def("__delitem__",  // del obj[sfSField]
            [](ripple::STObject &obj, const WSF &wsf) {
                return obj.makeFieldAbsent(static_cast<ripple::SField const&>(wsf));
            }
        )
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__repr__",
            [](const ripple::STObject &obj) {
                return obj.getFullText();
            }
        );

    py::class_<ripple::STTx, ripple::STObject, std::shared_ptr<ripple::STTx>> STTx(sTypeModule, "STTx", "A serializeable type representing a transaction.");
    STTx
        .def("get_txn_type", &ripple::STTx::getTxnType, "Get the transaction type of the object.")
        .def("get_seq_proxy", &ripple::STTx::getSeqProxy, "Get the sequence number/ticket number of the object.")
    ;

    py::class_<ripple::STLedgerEntry, ripple::STObject, std::shared_ptr<ripple::STLedgerEntry>> STLedgerEntry(sTypeModule, "STLedgerEntry", "A serializeable type representing a ledger entry.");
    STLedgerEntry
        .def("get_type", &ripple::STLedgerEntry::getType, "Get the ledger entry type of the object.")
    ;

    // Keylets
    py::module_ keyletModule = m.def_submodule("keylets", "Keylets for getting access to ledger objects.");
    
    py::class_<ripple::Keylet> Keylet(keyletModule, "Keylet", "A pair of SHAMap key and LedgerEntryType. \
\
    A Keylet identifies both a key in the state map and its ledger \
    entry type. `Keylet` is a portmanteau of the words key and LET, \
    an acronym for LedgerEntryType.");
    Keylet
        .def(py::init<std::uint16_t, ripple::uint256>())
        .def_property_readonly("type",
            [](const ripple::Keylet &keylet) {
                return keylet.type;
            },
            "The ledger entry type."
        )
        .def_property_readonly("key",
            [](const ripple::Keylet &keylet) {
                return keylet.key.data();
            },
            "The unique key of the object."
        );

    keyletModule
        .def("account_keylet", &ripple::keylet::account)
        .def("escrow_keylet", &ripple::keylet::escrow)
        .def("signers_keylet", &ripple::keylet::signers)
        .def("owner_dir_keylet", &ripple::keylet::ownerDir)
        .def("child_keylet", &ripple::keylet::child)
        .def("skip_keylet", py::overload_cast<>(&ripple::keylet::skip))
        .def("skip_keylet", py::overload_cast<ripple::LedgerIndex>(&ripple::keylet::skip))
        .def("amendments_keylet", &ripple::keylet::amendments)
        .def("fees_keylet", &ripple::keylet::fees)
        .def("negative_unl_keylet", &ripple::keylet::negativeUNL)
        .def("trustline_keylet", py::overload_cast<ripple::AccountID const&, ripple::AccountID const&, ripple::Currency const&>(&ripple::keylet::line))
        .def("trustline_keylet", py::overload_cast<ripple::AccountID const&, ripple::Issue const&>(&ripple::keylet::line))
        .def("offer_keylet", py::overload_cast<ripple::AccountID const&, std::uint32_t>(&ripple::keylet::offer))
        .def("offer_keylet", py::overload_cast<ripple::uint256 const&>(&ripple::keylet::offer))
        .def("quality_keylet", &ripple::keylet::quality)
        .def("check_keylet", py::overload_cast<ripple::AccountID const&, std::uint32_t>(&ripple::keylet::check))
        .def("check_keylet", py::overload_cast<ripple::uint256 const&>(&ripple::keylet::check))
        .def("deposit_preauth_keylet", py::overload_cast<ripple::AccountID const&, ripple::AccountID const&>(&ripple::keylet::depositPreauth))
        .def("deposit_preauth_keylet", py::overload_cast<ripple::uint256 const&>(&ripple::keylet::depositPreauth))
        .def("unchecked_keylet", &ripple::keylet::unchecked)
        .def("page_keylet", py::overload_cast<ripple::uint256 const&, std::uint64_t>(&ripple::keylet::page))
        .def("page_keylet", py::overload_cast<ripple::Keylet const&, std::uint64_t>(&ripple::keylet::page))
        .def("paychan_keylet", &ripple::keylet::payChan)
        .def("nftpage_min_keylet", &ripple::keylet::nftpage_min)
        .def("nftpage_max_keylet", &ripple::keylet::nftpage_max)
        .def("nftpage_keylet", &ripple::keylet::nftpage)
        .def("nft_offer_keylet", py::overload_cast<ripple::AccountID const&, std::uint32_t>(&ripple::keylet::nftoffer))
        .def("nft_offer_keylet", py::overload_cast<ripple::uint256 const&>(&ripple::keylet::nftoffer))
        .def("nft_buys_keylet", &ripple::keylet::nft_buys)
        .def("nft_sells_keylet", &ripple::keylet::nft_sells)
    ;

    /*
    * Contexts and classes that the contexts depend on
    */

   py::module_ transactorModule = m.def_submodule("transactors", "Contexts and other classes used in transaction processing.");

    py::class_<ripple::Rules> Rules(transactorModule, "Rules", "Rules controlling protocol behavior, such as amendments.");
    Rules
        .def("enabled", &ripple::Rules::enabled, "Whether an amendment is enabled.")
        .def("enabled",
            [](const ripple::Rules &rules, py::object amendment) {
                auto const name = amendment.attr("name").cast<std::string>();
                return rules.enabled(ripple::sha512Half(ripple::Slice(name.data(), name.size())));
            },
            "Whether an amendment is enabled."
        );
    
    py::class_<ripple::Fees> Fees(transactorModule, "Fees", "Reflects the fee settings for a particular ledger. \
\
    The fees are always the same for any transactions applied \
    to a ledger. Changes to fees occur in between ledgers.");
    Fees
        .def_property_readonly("base",
            [](const ripple::Fees &fees) {
                return fees.base;
            },
            "Reference tx cost (drops)."
        )
        .def_property_readonly("reserve",
            [](const ripple::Fees &fees) {
                return fees.reserve;
            },
            "Reserve base (drops)."
        )
        .def_property_readonly("increment",
            [](const ripple::Fees &fees) {
                return fees.increment;
            },
            "Reserve increment (drops)."
        )
        .def("account_reserve", &ripple::Fees::accountReserve, "Returns the account reserve given the owner count, in drops. \
\
        The reserve is calculated as the reserve base plus the reserve increment times the number of increments.");
    
    py::class_<ripple::NetClock::time_point> TimePoint(transactorModule, "TimePoint");
    TimePoint
        .def("time_since_epoch",
            [](const ripple::NetClock::time_point &tp) {
                return tp.time_since_epoch().count();
            },
            "Returns the amount of time that has passed since the UNIX epoch."
        )
        .def("__repr__",
            [](const ripple::NetClock::time_point &tp) {
                return ripple::to_string(tp);
            }
        )
    ;
    
    py::class_<ripple::LedgerInfo> LedgerInfo(transactorModule, "LedgerInfo", "Information about the notional ledger backing the view.");
    LedgerInfo
        .def_readonly("seq", &ripple::LedgerInfo::seq, "The ledger index.")
        .def_readonly("parent_close_time", &ripple::LedgerInfo::parentCloseTime, "The time the previous ledger closed.")
        .def_readonly("hash", &ripple::LedgerInfo::hash, "The hash of the ledger.")
        .def_readonly("tx_hash", &ripple::LedgerInfo::txHash, "The hash of the transactions in the ledger.")
        .def_readonly("account_hash", &ripple::LedgerInfo::accountHash)
        .def_readonly("parent_hash", &ripple::LedgerInfo::parentHash)
        .def_readonly("drops", &ripple::LedgerInfo::drops)
        .def_readonly("validated", &ripple::LedgerInfo::validated)
        .def_readonly("accepted", &ripple::LedgerInfo::accepted)
        .def_readonly("close_flags", &ripple::LedgerInfo::closeFlags)
        .def_readonly("close_time_resolution", &ripple::LedgerInfo::closeTimeResolution)
        .def_readonly("close_time", &ripple::LedgerInfo::closeTime)
    ;

    py::enum_<ripple::ConsequencesFactoryType>(transactorModule, "ConsequencesFactoryType", "The type of TxConsequences that the transactor needs.")
        .value("Normal", ripple::ConsequencesFactoryType::Normal)
        .value("Blocker", ripple::ConsequencesFactoryType::Blocker)
        .value("Custom", ripple::ConsequencesFactoryType::Custom);

    py::enum_<ripple::TxConsequences::Category>(transactorModule, "TxConsequencesCategory", "Describes how the transaction affects subsequent transactions.")
        .value("normal", ripple::TxConsequences::Category::normal)
        .value("blocker", ripple::TxConsequences::Category::blocker);

    py::class_<ripple::TxConsequences> TxConsequences(transactorModule, "TxConsequences", "Class describing the consequences to the account of applying a transaction if the transaction consumes the maximum XRP allowed.");
    TxConsequences
        .def(py::init<ripple::NotTEC>())
        .def(py::init<ripple::STTx>())
        .def(py::init<ripple::STTx, ripple::TxConsequences::Category>())
        .def(py::init<ripple::STTx, ripple::XRPAmount>())
        .def(py::init<ripple::STTx, std::uint32_t>())
        .def("fee", &ripple::TxConsequences::fee, "The fee.")
        .def("potential_spend", &ripple::TxConsequences::potentialSpend, "The potential spend.")
        .def("seq_proxy", &ripple::TxConsequences::seqProxy, "The sequence/ticket number.")
        .def("sequences_consumed", &ripple::TxConsequences::sequencesConsumed, "The number of sequences consumed.")
        .def("is_blocker", &ripple::TxConsequences::isBlocker, "Whether the TxConsequences is a blocker.")
        .def("following_seq", &ripple::TxConsequences::followingSeq, "The next sequence number.")
    ;

    py::class_<ripple::Serializer> Serializer(transactorModule, "Serializer", "A serializer to the rippled serialization format.");
    Serializer
        .def("add8", &ripple::Serializer::add8, "Add 8 bits.")
        .def("add16", &ripple::Serializer::add16, "Add 16 bits.")
        .def("add32", py::overload_cast<std::uint32_t>(&ripple::Serializer::add32), "Add 32 bits.")
        .def("add64", &ripple::Serializer::add64, "Add 64 bits.")
        .def("add_raw", py::overload_cast<ripple::Blob const&>(&ripple::Serializer::addRaw), "Add raw bytes as a blob.")
        .def("add_raw", py::overload_cast<ripple::Slice>(&ripple::Serializer::addRaw), "Add raw bytes as a slice.")
        .def("add_raw", py::overload_cast<const ripple::Serializer&>(&ripple::Serializer::addRaw), "Add raw bytes from another Serializer.")
        .def("add_vl", [](ripple::Serializer& self, const ripple::Blob& vector) {
            return self.addVL(vector);
        }, "Add raw bytes as a variable length blob.")
        .def("add_vl", [](ripple::Serializer& self, const ripple::Slice& slice) {
            return self.addVL(slice);
        }, "Add raw bytes as a variable length slice.")
    ;

    py::class_<ripple::SerialIter> SerialIter(transactorModule, "SerialIter", "Transitional adapter to new serialization interfaces.");
    SerialIter
        .def("get8", &ripple::SerialIter::get8, "Get the next 8 bits.")
        .def("get16", &ripple::SerialIter::get16, "Get the next 16 bits.")
        .def("get32", &ripple::SerialIter::get32, "Get the next 32 bits.")
        .def("get64", &ripple::SerialIter::get64, "Get the next 64 bits.")
        .def("get128", &ripple::SerialIter::get128, "Get the next 128 bits.")
        .def("get160", &ripple::SerialIter::get160, "Get the next 160 bits.")
        .def("get256", &ripple::SerialIter::get256, "Get the next 256 bits.")
        .def("get_vl_buffer", &ripple::SerialIter::getVLBuffer, "Assuming a variable-length buffer is next, get the buffer.")
    ;

    py::enum_<ripple::ApplyFlags>(basicsModule, "ApplyFlags", "An enum representing the type of transaction, in terms of how to process it.")
        .value("tapNONE", ripple::ApplyFlags::tapNONE)
        .value("tapFAIL_HARD", ripple::ApplyFlags::tapFAIL_HARD)
        .value("tapRETRY", ripple::ApplyFlags::tapRETRY)
        .value("tapUNLIMITED", ripple::ApplyFlags::tapUNLIMITED);

    py::class_<ripple::Application> Application(transactorModule, "Application", "The ledger and transaction processor.");

    py::class_<ripple::ReadView> ReadView(transactorModule, "ReadView", "A view into a ledger. \
\
    This interface provides read access to state \
    and transaction items. There is no checkpointing \
    or calculation of metadata.");
    ReadView
        .def_property_readonly("fees",
            [](const ripple::ReadView &view) {
                return view.fees();
            },
            "The fees for the base ledger."
        )
        .def_property_readonly("rules",
            [](const ripple::ReadView &view) {
                return view.rules();
            },
            "The tx processing rules."
        )
        .def("info", &ripple::ReadView::info, "Returns information about the ledger.")
        .def("open", &ripple::ReadView::open, "Returns true if this reflects an open ledger.")
        .def("parent_close_time", &ripple::ReadView::parentCloseTime, "Returns the close time of the previous ledger.")
        .def("seq", &ripple::ReadView::seq, "Returns the sequence number of the base ledger.")
        .def("exists", &ripple::ReadView::exists, "Determine if a state item exists.")
        .def("read", &ripple::ReadView::read, "Get an unmodifiable version of a SLE.")
        ;
    
    py::class_<ripple::OpenView, ripple::ReadView> OpenView(transactorModule, "OpenView", "Writable ledger view that accumulates state and tx changes.");
    OpenView
        .def("tx_count", &ripple::OpenView::txCount, "Return the number of transactions inserted since creation.")
    ;

    py::class_<ripple::ApplyView, ripple::ReadView> ApplyView(transactorModule, "ApplyView", "Writeable view to a ledger, for applying a transaction.");
    ApplyView
        .def("peek", &ripple::ApplyView::peek, "Get a modifiable version of a SLE.")
        .def("update", &ripple::ApplyView::update, "Indicate changes to a peeked SLE.")
        .def("flags", &ripple::ApplyView::flags, "Returns the tx apply flags.")
        .def("erase", &ripple::ApplyView::erase, "Remove a peeked SLE.")
        .def("insert", &ripple::ApplyView::insert, "Insert a new state SLE.")
        .def("dir_remove", py::overload_cast<ripple::Keylet const&, std::uint64_t, ripple::uint256 const&, bool>(&ripple::ApplyView::dirRemove), "Remove an entry from a directory.")
        .def("dir_remove", py::overload_cast<ripple::Keylet const&, std::uint64_t, ripple::Keylet const&, bool>(&ripple::ApplyView::dirRemove), "Remove an entry from a directory.")
        .def("dir_insert",
            [](ripple::ApplyView &view, ripple::AccountID account, ripple::Keylet keylet) {
                return view.dirInsert(ripple::keylet::ownerDir(account), keylet, ripple::describeOwnerDir(account));
            },
            "Insert an entry to a directory."
        )
        // .def("dirInsert", py::overload_cast<
        //     ripple::Keylet, ripple::Keylet, std::function<void(std::shared_ptr<ripple::SLE> const&)>
        // >(&ripple::ApplyView::dirInsert))
        // .def("dirInsert", py::overload_cast<
        //     ripple::Keylet, ripple::uint256, std::function<void(std::shared_ptr<ripple::SLE> const&)>
        // >(&ripple::ApplyView::dirInsert))
    ;
    
    py::class_<
        ripple::detail::ApplyViewBase,
        ripple::ApplyView
    > ApplyViewBase(transactorModule, "ApplyViewBase", ":meta private:");
    py::class_<
        ripple::ApplyViewImpl,
        ripple::detail::ApplyViewBase
    > ApplyViewImpl(transactorModule, "ApplyViewImpl", ":meta private:");
    
    py::class_<ripple::PreflightContext> PreflightContext(transactorModule, "PreflightContext", "State information when preflighting a transaction.");
    PreflightContext
        .def_property_readonly("app",
            [](const ripple::PreflightContext &ctx) -> const ripple::Application& {
                return ctx.app;
            }, py::return_value_policy::reference, "The application.")
        .def_property_readonly("tx",
            [](const ripple::PreflightContext &ctx) {
                return ctx.tx;
            },
            "The transaction."
        )
        .def_readonly("rules", &ripple::PreflightContext::rules, "The current governing rules of the network.")
        .def_readonly("flags", &ripple::PreflightContext::flags, "The flags on the transaction.")
        .def_readonly("j", &ripple::PreflightContext::j, "The journal logger.")
    ;
    
    py::class_<ripple::PreclaimContext> PreclaimContext(transactorModule, "PreclaimContext", "State information when preclaiming a transaction.");
    PreclaimContext
        .def_property_readonly("app",
            [](const ripple::PreclaimContext &ctx) -> const ripple::Application& {
                return ctx.app;
            }, py::return_value_policy::reference, "The application.")
        .def_property_readonly("tx",
            [](const ripple::PreclaimContext &ctx) {
                return ctx.tx;
            },
            "The transaction."
        )
        .def_property_readonly("view",
            [](const ripple::PreclaimContext &ctx) -> const ripple::ReadView& {
                return ctx.view;
            }, py::return_value_policy::reference, "A read-only view into the current state of the ledger.")
        .def_readonly("preflight_result", &ripple::PreclaimContext::preflightResult, "The result from the preflight processing.")
        .def_readonly("flags", &ripple::PreclaimContext::flags, "The flags on the transaction.")
        .def_readonly("j", &ripple::PreclaimContext::j, "The journal logger.")
        ;

    py::class_<ripple::ApplyContext> ApplyContext(transactorModule, "ApplyContext", "State information when applying a transaction.");
    ApplyContext
        .def_property_readonly("app",
            [](const ripple::ApplyContext &ctx) -> const ripple::Application& {
                return ctx.app;
            }, py::return_value_policy::reference, "The application.")
        .def_property_readonly("tx",
            [](const ripple::ApplyContext &ctx) {
                return ctx.tx;
            },
            "The transaction."
        )
        .def_readonly("preclaim_result", &ripple::ApplyContext::preclaimResult, "The result from the preclaim processing.")
        .def_readonly("base_fee", &ripple::ApplyContext::baseFee, "The base fee on the transaction.")
        .def_readonly("journal", &ripple::ApplyContext::journal, "The journal logger.")
        .def("deliver", &ripple::ApplyContext::deliver, "Set the DeliveredAmount field in the metadata.")
        .def("view",
            [](const ripple::ApplyContext &ctx) -> const ripple::ApplyView& {
                return ctx.view();
            }, py::return_value_policy::reference, "Get an editable view into the current state of the ledger.");
    
    // py::register_exception<ripple::LogicError>(m, "LogicError");
    
    /*
    * Misc additional stuff
    */

    transactorModule
        .def("preflight1", &ripple::preflight1, "Performs early sanity checks on the account and fee fields")
        .def("preflight2", &ripple::preflight2, "Checks whether the signature appears valid");
    
    terModule
        .def("is_tes_success", &ripple::isTesSuccess, "Returns true if the TER passed in is ``tesSUCCESS`` or not.");
    
    ledgerObjectModule
        .def("make_sle",
            [](const ripple::Keylet &keylet) {
                return std::make_shared<ripple::SLE>(keylet);
            },
            py::return_value_policy::move,
            "Create a new SLE object with the provided keylet."
        )
        .def("adjust_owner_count", &ripple::adjustOwnerCount, "Adjust the owner count of an account after creating/deleting a new ledger object.");
    
    py::module_ parseErrorModule = sTypeModule.def_submodule("parser_errors", "Helper functions for throwing parsing errors when creating a new plugin SType.");

    parseErrorModule
        .def("make_name", &ripple::make_name, "Get the name of the object being parsed.")
        .def("not_an_object", py::overload_cast<std::string const&>(&ripple::not_an_object))
        .def("not_an_object", py::overload_cast<std::string const&, std::string const&>(&ripple::not_an_object))
        .def("not_an_array", &ripple::not_an_array)
        .def("unknown_field", &ripple::unknown_field)
        .def("out_of_range", &ripple::out_of_range)
        .def("bad_type", &ripple::bad_type)
        .def("unknown_type", &ripple::unknown_type)
        .def("invalid_data", py::overload_cast<std::string const&>(&ripple::invalid_data))
        .def("invalid_data", py::overload_cast<std::string const&, std::string const&>(&ripple::invalid_data))
        .def("array_expected", &ripple::array_expected)
        .def("string_expected", &ripple::string_expected)
        .def("too_deep", &ripple::too_deep)
        .def("singleton_expected", &ripple::singleton_expected)
        .def("template_mismatch", &ripple::template_mismatch)
        .def("non_object_in_array", &ripple::non_object_in_array)
        .def("make_stplugintype", 
            [](const WSF &wsf, ripple::Buffer& b) {
                ripple::SField const &f = static_cast<ripple::SField const&>(wsf);
                auto const stVar = ripple::detail::make_stvar<ripple::STPluginType>(f, b.data(), b.size());
                return stVar;
            },
            py::return_value_policy::move
        )
    ;

    sTypeModule
        // TODO: replace `indexHash` with a native Python implementation
        .def("index_hash", &indexHash<>, "Hash the provided values together.")
        .def("index_hash", &indexHash<ripple::AccountID, std::uint32_t>);

    basicsModule
        .def("parse_base58", py::overload_cast<std::string const&>(&ripple::parseBase58<ripple::AccountID>))
        .def("parse_base58", py::overload_cast<std::string const&>(&ripple::parseBase58<ripple::Seed>))
        // .def("parse_base58", py::overload_cast<std::string const&>(&ripple::parseBase58<ripple::PublicKey>))
    ;

    py::module_ nftModule = m.def_submodule("nfts", "Utility methods for working with NFTs.");
    py::class_<ripple::nft::TokenAndPage> TokenAndPage(nftModule, "TokenAndPage", "A helper struct containing the token info and page of an NFT.");
    TokenAndPage
        .def_property_readonly("token",
            [](const ripple::nft::TokenAndPage &tokenAndPage) {
                return tokenAndPage.token;
            },
            "The token."
        )
        .def_property_readonly("page",
            [](const ripple::nft::TokenAndPage &tokenAndPage) {
                return tokenAndPage.page;
            },
            "The page."
        );

    nftModule
        .def("find_token", ripple::nft::findToken)
        .def("find_token_and_page", ripple::nft::findTokenAndPage)
        .def("get_flags", ripple::nft::getFlags)
        .def("get_transfer_fee", ripple::nft::getTransferFee)
        .def("get_serial", ripple::nft::getSerial)
        .def("get_taxon", ripple::nft::getTaxon)
        .def("get_issuer", ripple::nft::getIssuer)
        .def("insert_token", 
            [](ripple::ApplyView& view, ripple::AccountID& owner, ripple::STObject& nft) {
                return ripple::nft::insertToken(view, owner, std::move(nft));
            })
        .def("remove_token", py::overload_cast<ripple::ApplyView&, ripple::AccountID const&, ripple::uint256 const&>(&ripple::nft::removeToken))
        .def("remove_token",
            [](ripple::ApplyView& view, ripple::AccountID const& account, ripple::uint256 const& id, std::shared_ptr<ripple::SLE>& sle) {
                return ripple::nft::removeToken(view, account, id, std::move(sle));
            })
    ;
    nftModule.attr("flag_burnable") = ripple::nft::flagBurnable;
    nftModule.attr("flag_only_xrp") = ripple::nft::flagOnlyXRP;
    nftModule.attr("flag_create_trustlines") = ripple::nft::flagCreateTrustLines;
    nftModule.attr("flag_transferable") = ripple::nft::flagTransferable;

    sfieldModule
        .def("_create_new_sfield_STAccount", &wrappedNewSField<ripple::STAccount>)
        .def("_create_new_sfield_STAmount", &wrappedNewSField<ripple::STAmount>)
        .def("_create_new_sfield_STUInt8", &wrappedNewSField<ripple::STUInt8>)
        .def("_create_new_sfield_STUInt16", &wrappedNewSField<ripple::STUInt16>)
        .def("_create_new_sfield_STUInt32", &wrappedNewSField<ripple::STUInt32>)
        .def("_create_new_sfield_STUInt64", &wrappedNewSField<ripple::STUInt64>)
        .def("_create_new_sfield_STUInt128", &wrappedNewSField<ripple::STUInt128>)
        .def("_create_new_sfield_STUInt160", &wrappedNewSField<ripple::STUInt160>)
        .def("_create_new_sfield_STUInt256", &wrappedNewSField<ripple::STUInt256>)
        .def("_create_new_sfield_STBlob", &wrappedNewSField<ripple::STBlob>)
        .def("_create_new_sfield_STPluginType", &wrappedNewSField<ripple::STPluginType>)
        .def("_create_new_sfield_STVector256", &wrappedNewSField<ripple::STVector256>)
        .def("_create_new_sfield_STIssue", &wrappedNewSField<ripple::STIssue>)
        .def("_create_new_sfield_STCurrency", &wrappedNewSField<ripple::STCurrency>)
        .def("_create_new_sfield_STArray", &wrappedNewUntypedSField<ripple::STArray>)
        .def("_create_new_sfield_STObject", &wrappedNewUntypedSField<ripple::STObject>)
        .def("construct_custom_sfield", &constructCustomWrappedSField)
    ;

    // Flags
    transactorModule.attr("tf_fully_canonical_sig") = ripple::tfFullyCanonicalSig;
    transactorModule.attr("tf_universal") = ripple::tfUniversal;
    transactorModule.attr("tf_universal_mask") = ripple::tfUniversalMask;

    // SFields

    sfieldModule.attr("sf_invalid") = WSF{(void *)&ripple::sfInvalid};
    sfieldModule.attr("sf_generic") = WSF{(void *)&ripple::sfGeneric};
    sfieldModule.attr("sf_ledger_entry") = WSF{(void *)&ripple::sfLedgerEntry};
    sfieldModule.attr("sf_transaction") = WSF{(void *)&ripple::sfTransaction};
    sfieldModule.attr("sf_validation") = WSF{(void *)&ripple::sfValidation};
    sfieldModule.attr("sf_metadata") = WSF{(void *)&ripple::sfMetadata};
    sfieldModule.attr("sf_close_resolution") = TWSF<ripple::STUInt8>{(void *)&ripple::sfCloseResolution};
    sfieldModule.attr("sf_method") = TWSF<ripple::STUInt8>{(void *)&ripple::sfMethod};
    sfieldModule.attr("sf_transaction_result") = TWSF<ripple::STUInt8>{(void *)&ripple::sfTransactionResult};
    sfieldModule.attr("sf_was_locking_chain_send") = TWSF<ripple::STUInt8>{(void *)&ripple::sfWasLockingChainSend};
    sfieldModule.attr("sf_scale") = TWSF<ripple::STUInt8>{(void *)&ripple::sfScale};
    sfieldModule.attr("sf_tick_size") = TWSF<ripple::STUInt8>{(void *)&ripple::sfTickSize};
    sfieldModule.attr("sf_unl_modify_disabling") = TWSF<ripple::STUInt8>{(void *)&ripple::sfUNLModifyDisabling};
    sfieldModule.attr("sf_hook_result") = TWSF<ripple::STUInt8>{(void *)&ripple::sfHookResult};
    sfieldModule.attr("sf_ledger_entry_type") = TWSF<ripple::STUInt16>{(void *)&ripple::sfLedgerEntryType};
    sfieldModule.attr("sf_transaction_type") = TWSF<ripple::STUInt16>{(void *)&ripple::sfTransactionType};
    sfieldModule.attr("sf_signer_weight") = TWSF<ripple::STUInt16>{(void *)&ripple::sfSignerWeight};
    sfieldModule.attr("sf_transfer_fee") = TWSF<ripple::STUInt16>{(void *)&ripple::sfTransferFee};
    sfieldModule.attr("sf_trading_fee") = TWSF<ripple::STUInt16>{(void *)&ripple::sfTradingFee};
    sfieldModule.attr("sf_version") = TWSF<ripple::STUInt16>{(void *)&ripple::sfVersion};
    sfieldModule.attr("sf_hook_state_change_count") = TWSF<ripple::STUInt16>{(void *)&ripple::sfHookStateChangeCount};
    sfieldModule.attr("sf_hook_emit_count") = TWSF<ripple::STUInt16>{(void *)&ripple::sfHookEmitCount};
    sfieldModule.attr("sf_hook_execution_index") = TWSF<ripple::STUInt16>{(void *)&ripple::sfHookExecutionIndex};
    sfieldModule.attr("sf_hook_api_version") = TWSF<ripple::STUInt16>{(void *)&ripple::sfHookApiVersion};
    sfieldModule.attr("sf_discounted_fee") = TWSF<ripple::STUInt16>{(void *)&ripple::sfDiscountedFee};
    sfieldModule.attr("sf_network_id") = TWSF<ripple::STUInt32>{(void *)&ripple::sfNetworkID};
    sfieldModule.attr("sf_flags") = TWSF<ripple::STUInt32>{(void *)&ripple::sfFlags};
    sfieldModule.attr("sf_source_tag") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSourceTag};
    sfieldModule.attr("sf_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSequence};
    sfieldModule.attr("sf_previous_txn_lgr_seq") = TWSF<ripple::STUInt32>{(void *)&ripple::sfPreviousTxnLgrSeq};
    sfieldModule.attr("sf_ledger_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLedgerSequence};
    sfieldModule.attr("sf_close_time") = TWSF<ripple::STUInt32>{(void *)&ripple::sfCloseTime};
    sfieldModule.attr("sf_parent_close_time") = TWSF<ripple::STUInt32>{(void *)&ripple::sfParentCloseTime};
    sfieldModule.attr("sf_signing_time") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSigningTime};
    sfieldModule.attr("sf_expiration") = TWSF<ripple::STUInt32>{(void *)&ripple::sfExpiration};
    sfieldModule.attr("sf_transfer_rate") = TWSF<ripple::STUInt32>{(void *)&ripple::sfTransferRate};
    sfieldModule.attr("sf_wallet_size") = TWSF<ripple::STUInt32>{(void *)&ripple::sfWalletSize};
    sfieldModule.attr("sf_owner_count") = TWSF<ripple::STUInt32>{(void *)&ripple::sfOwnerCount};
    sfieldModule.attr("sf_destination_tag") = TWSF<ripple::STUInt32>{(void *)&ripple::sfDestinationTag};
    sfieldModule.attr("sf_last_update_time") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLastUpdateTime};
    sfieldModule.attr("sf_high_quality_in") = TWSF<ripple::STUInt32>{(void *)&ripple::sfHighQualityIn};
    sfieldModule.attr("sf_high_quality_out") = TWSF<ripple::STUInt32>{(void *)&ripple::sfHighQualityOut};
    sfieldModule.attr("sf_low_quality_in") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLowQualityIn};
    sfieldModule.attr("sf_low_quality_out") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLowQualityOut};
    sfieldModule.attr("sf_quality_in") = TWSF<ripple::STUInt32>{(void *)&ripple::sfQualityIn};
    sfieldModule.attr("sf_quality_out") = TWSF<ripple::STUInt32>{(void *)&ripple::sfQualityOut};
    sfieldModule.attr("sf_stamp_escrow") = TWSF<ripple::STUInt32>{(void *)&ripple::sfStampEscrow};
    sfieldModule.attr("sf_bond_amount") = TWSF<ripple::STUInt32>{(void *)&ripple::sfBondAmount};
    sfieldModule.attr("sf_load_fee") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLoadFee};
    sfieldModule.attr("sf_offer_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfOfferSequence};
    sfieldModule.attr("sf_first_ledger_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfFirstLedgerSequence};
    sfieldModule.attr("sf_last_ledger_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLastLedgerSequence};
    sfieldModule.attr("sf_transaction_index") = TWSF<ripple::STUInt32>{(void *)&ripple::sfTransactionIndex};
    sfieldModule.attr("sf_operation_limit") = TWSF<ripple::STUInt32>{(void *)&ripple::sfOperationLimit};
    sfieldModule.attr("sf_reference_fee_units") = TWSF<ripple::STUInt32>{(void *)&ripple::sfReferenceFeeUnits};
    sfieldModule.attr("sf_reserve_base") = TWSF<ripple::STUInt32>{(void *)&ripple::sfReserveBase};
    sfieldModule.attr("sf_reserve_increment") = TWSF<ripple::STUInt32>{(void *)&ripple::sfReserveIncrement};
    sfieldModule.attr("sf_set_flag") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSetFlag};
    sfieldModule.attr("sf_clear_flag") = TWSF<ripple::STUInt32>{(void *)&ripple::sfClearFlag};
    sfieldModule.attr("sf_signer_quorum") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSignerQuorum};
    sfieldModule.attr("sf_cancel_after") = TWSF<ripple::STUInt32>{(void *)&ripple::sfCancelAfter};
    sfieldModule.attr("sf_finish_after") = TWSF<ripple::STUInt32>{(void *)&ripple::sfFinishAfter};
    sfieldModule.attr("sf_signer_list_id") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSignerListID};
    sfieldModule.attr("sf_settle_delay") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSettleDelay};
    sfieldModule.attr("sf_ticket_count") = TWSF<ripple::STUInt32>{(void *)&ripple::sfTicketCount};
    sfieldModule.attr("sf_ticket_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfTicketSequence};
    sfieldModule.attr("sf_nftoken_taxon") = TWSF<ripple::STUInt32>{(void *)&ripple::sfNFTokenTaxon};
    sfieldModule.attr("sf_minted_nftokens") = TWSF<ripple::STUInt32>{(void *)&ripple::sfMintedNFTokens};
    sfieldModule.attr("sf_burned_nftokens") = TWSF<ripple::STUInt32>{(void *)&ripple::sfBurnedNFTokens};
    sfieldModule.attr("sf_hook_state_count") = TWSF<ripple::STUInt32>{(void *)&ripple::sfHookStateCount};
    sfieldModule.attr("sf_emit_generation") = TWSF<ripple::STUInt32>{(void *)&ripple::sfEmitGeneration};
    sfieldModule.attr("sf_vote_weight") = TWSF<ripple::STUInt32>{(void *)&ripple::sfVoteWeight};
    sfieldModule.attr("sf_first_nftoken_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfFirstNFTokenSequence};
    sfieldModule.attr("sf_oracle_document_id") = TWSF<ripple::STUInt32>{(void *)&ripple::sfOracleDocumentID};
    sfieldModule.attr("sf_index_next") = TWSF<ripple::STUInt64>{(void *)&ripple::sfIndexNext};
    sfieldModule.attr("sf_index_previous") = TWSF<ripple::STUInt64>{(void *)&ripple::sfIndexPrevious};
    sfieldModule.attr("sf_book_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfBookNode};
    sfieldModule.attr("sf_owner_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfOwnerNode};
    sfieldModule.attr("sf_base_fee") = TWSF<ripple::STUInt64>{(void *)&ripple::sfBaseFee};
    sfieldModule.attr("sf_exchange_rate") = TWSF<ripple::STUInt64>{(void *)&ripple::sfExchangeRate};
    sfieldModule.attr("sf_low_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfLowNode};
    sfieldModule.attr("sf_high_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfHighNode};
    sfieldModule.attr("sf_destination_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfDestinationNode};
    sfieldModule.attr("sf_cookie") = TWSF<ripple::STUInt64>{(void *)&ripple::sfCookie};
    sfieldModule.attr("sf_server_version") = TWSF<ripple::STUInt64>{(void *)&ripple::sfServerVersion};
    sfieldModule.attr("sf_nftoken_offer_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfNFTokenOfferNode};
    sfieldModule.attr("sf_emit_burden") = TWSF<ripple::STUInt64>{(void *)&ripple::sfEmitBurden};
    sfieldModule.attr("sf_hook_on") = TWSF<ripple::STUInt64>{(void *)&ripple::sfHookOn};
    sfieldModule.attr("sf_hook_instruction_count") = TWSF<ripple::STUInt64>{(void *)&ripple::sfHookInstructionCount};
    sfieldModule.attr("sf_hook_return_code") = TWSF<ripple::STUInt64>{(void *)&ripple::sfHookReturnCode};
    sfieldModule.attr("sf_reference_count") = TWSF<ripple::STUInt64>{(void *)&ripple::sfReferenceCount};
    sfieldModule.attr("sf_xchain_claim_id") = TWSF<ripple::STUInt64>{(void *)&ripple::sfXChainClaimID};
    sfieldModule.attr("sf_xchain_account_create_count") = TWSF<ripple::STUInt64>{(void *)&ripple::sfXChainAccountCreateCount};
    sfieldModule.attr("sf_xchain_account_claim_count") = TWSF<ripple::STUInt64>{(void *)&ripple::sfXChainAccountClaimCount};
    sfieldModule.attr("sf_asset_price") = TWSF<ripple::STUInt64>{(void *)&ripple::sfAssetPrice};
    sfieldModule.attr("sf_email_hash") = TWSF<ripple::STUInt128>{(void *)&ripple::sfEmailHash};
    sfieldModule.attr("sf_taker_pays_currency") = TWSF<ripple::STUInt160>{(void *)&ripple::sfTakerPaysCurrency};
    sfieldModule.attr("sf_taker_pays_issuer") = TWSF<ripple::STUInt160>{(void *)&ripple::sfTakerPaysIssuer};
    sfieldModule.attr("sf_taker_gets_currency") = TWSF<ripple::STUInt160>{(void *)&ripple::sfTakerGetsCurrency};
    sfieldModule.attr("sf_taker_gets_issuer") = TWSF<ripple::STUInt160>{(void *)&ripple::sfTakerGetsIssuer};
    sfieldModule.attr("sf_ledger_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfLedgerHash};
    sfieldModule.attr("sf_parent_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfParentHash};
    sfieldModule.attr("sf_transaction_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfTransactionHash};
    sfieldModule.attr("sf_account_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfAccountHash};
    sfieldModule.attr("sf_previous_txn_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfPreviousTxnID};
    sfieldModule.attr("sf_ledger_index") = TWSF<ripple::STUInt256>{(void *)&ripple::sfLedgerIndex};
    sfieldModule.attr("sf_wallet_locator") = TWSF<ripple::STUInt256>{(void *)&ripple::sfWalletLocator};
    sfieldModule.attr("sf_root_index") = TWSF<ripple::STUInt256>{(void *)&ripple::sfRootIndex};
    sfieldModule.attr("sf_account_txn_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfAccountTxnID};
    sfieldModule.attr("sf_nftoken_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfNFTokenID};
    sfieldModule.attr("sf_emit_parent_txn_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfEmitParentTxnID};
    sfieldModule.attr("sf_emit_nonce") = TWSF<ripple::STUInt256>{(void *)&ripple::sfEmitNonce};
    sfieldModule.attr("sf_emit_hook_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfEmitHookHash};
    sfieldModule.attr("sf_amm_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfAMMID};
    sfieldModule.attr("sf_book_directory") = TWSF<ripple::STUInt256>{(void *)&ripple::sfBookDirectory};
    sfieldModule.attr("sf_invoice_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfInvoiceID};
    sfieldModule.attr("sf_nickname") = TWSF<ripple::STUInt256>{(void *)&ripple::sfNickname};
    sfieldModule.attr("sf_amendment") = TWSF<ripple::STUInt256>{(void *)&ripple::sfAmendment};
    sfieldModule.attr("sf_digest") = TWSF<ripple::STUInt256>{(void *)&ripple::sfDigest};
    sfieldModule.attr("sf_channel") = TWSF<ripple::STUInt256>{(void *)&ripple::sfChannel};
    sfieldModule.attr("sf_consensus_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfConsensusHash};
    sfieldModule.attr("sf_check_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfCheckID};
    sfieldModule.attr("sf_validated_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfValidatedHash};
    sfieldModule.attr("sf_previous_page_min") = TWSF<ripple::STUInt256>{(void *)&ripple::sfPreviousPageMin};
    sfieldModule.attr("sf_next_page_min") = TWSF<ripple::STUInt256>{(void *)&ripple::sfNextPageMin};
    sfieldModule.attr("sf_nftoken_buy_offer") = TWSF<ripple::STUInt256>{(void *)&ripple::sfNFTokenBuyOffer};
    sfieldModule.attr("sf_nftoken_sell_offer") = TWSF<ripple::STUInt256>{(void *)&ripple::sfNFTokenSellOffer};
    sfieldModule.attr("sf_hook_state_key") = TWSF<ripple::STUInt256>{(void *)&ripple::sfHookStateKey};
    sfieldModule.attr("sf_hook_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfHookHash};
    sfieldModule.attr("sf_hook_namespace") = TWSF<ripple::STUInt256>{(void *)&ripple::sfHookNamespace};
    sfieldModule.attr("sf_hook_set_txn_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfHookSetTxnID};
    sfieldModule.attr("sf_amount") = TWSF<ripple::STAmount>{(void *)&ripple::sfAmount};
    sfieldModule.attr("sf_balance") = TWSF<ripple::STAmount>{(void *)&ripple::sfBalance};
    sfieldModule.attr("sf_limit_amount") = TWSF<ripple::STAmount>{(void *)&ripple::sfLimitAmount};
    sfieldModule.attr("sf_taker_pays") = TWSF<ripple::STAmount>{(void *)&ripple::sfTakerPays};
    sfieldModule.attr("sf_taker_gets") = TWSF<ripple::STAmount>{(void *)&ripple::sfTakerGets};
    sfieldModule.attr("sf_low_limit") = TWSF<ripple::STAmount>{(void *)&ripple::sfLowLimit};
    sfieldModule.attr("sf_high_limit") = TWSF<ripple::STAmount>{(void *)&ripple::sfHighLimit};
    sfieldModule.attr("sf_fee") = TWSF<ripple::STAmount>{(void *)&ripple::sfFee};
    sfieldModule.attr("sf_send_max") = TWSF<ripple::STAmount>{(void *)&ripple::sfSendMax};
    sfieldModule.attr("sf_deliver_min") = TWSF<ripple::STAmount>{(void *)&ripple::sfDeliverMin};
    sfieldModule.attr("sf_amount2") = TWSF<ripple::STAmount>{(void *)&ripple::sfAmount2};
    sfieldModule.attr("sf_e_price") = TWSF<ripple::STAmount>{(void *)&ripple::sfEPrice};
    sfieldModule.attr("sf_bid_min") = TWSF<ripple::STAmount>{(void *)&ripple::sfBidMin};
    sfieldModule.attr("sf_bid_max") = TWSF<ripple::STAmount>{(void *)&ripple::sfBidMax};
    sfieldModule.attr("sf_price") = TWSF<ripple::STAmount>{(void *)&ripple::sfPrice};
    sfieldModule.attr("sf_lp_token_balance") = TWSF<ripple::STAmount>{(void *)&ripple::sfLPTokenBalance};
    sfieldModule.attr("sf_minimum_offer") = TWSF<ripple::STAmount>{(void *)&ripple::sfMinimumOffer};
    sfieldModule.attr("sf_ripple_escrow") = TWSF<ripple::STAmount>{(void *)&ripple::sfRippleEscrow};
    sfieldModule.attr("sf_delivered_amount") = TWSF<ripple::STAmount>{(void *)&ripple::sfDeliveredAmount};
    sfieldModule.attr("sf_nftoken_broker_fee") = TWSF<ripple::STAmount>{(void *)&ripple::sfNFTokenBrokerFee};
    sfieldModule.attr("sf_lp_token_out") = TWSF<ripple::STAmount>{(void *)&ripple::sfLPTokenOut};
    sfieldModule.attr("sf_lp_token_in") = TWSF<ripple::STAmount>{(void *)&ripple::sfLPTokenIn};
    sfieldModule.attr("sf_base_fee_drops") = TWSF<ripple::STAmount>{(void *)&ripple::sfBaseFeeDrops};
    sfieldModule.attr("sf_reserve_base_drops") = TWSF<ripple::STAmount>{(void *)&ripple::sfReserveBaseDrops};
    sfieldModule.attr("sf_reserve_increment_drops") = TWSF<ripple::STAmount>{(void *)&ripple::sfReserveIncrementDrops};
    sfieldModule.attr("sf_signature_reward") = TWSF<ripple::STAmount>{(void *)&ripple::sfSignatureReward};
    sfieldModule.attr("sf_min_account_create_amount") = TWSF<ripple::STAmount>{(void *)&ripple::sfMinAccountCreateAmount};
    sfieldModule.attr("sf_public_key") = TWSF<ripple::STBlob>{(void *)&ripple::sfPublicKey};
    sfieldModule.attr("sf_message_key") = TWSF<ripple::STBlob>{(void *)&ripple::sfMessageKey};
    sfieldModule.attr("sf_signing_pub_key") = TWSF<ripple::STBlob>{(void *)&ripple::sfSigningPubKey};
    sfieldModule.attr("sf_txn_signature") = TWSF<ripple::STBlob>{(void *)&ripple::sfTxnSignature};
    sfieldModule.attr("sf_uri") = TWSF<ripple::STBlob>{(void *)&ripple::sfURI};
    sfieldModule.attr("sf_signature") = TWSF<ripple::STBlob>{(void *)&ripple::sfSignature};
    sfieldModule.attr("sf_domain") = TWSF<ripple::STBlob>{(void *)&ripple::sfDomain};
    sfieldModule.attr("sf_fund_code") = TWSF<ripple::STBlob>{(void *)&ripple::sfFundCode};
    sfieldModule.attr("sf_remove_code") = TWSF<ripple::STBlob>{(void *)&ripple::sfRemoveCode};
    sfieldModule.attr("sf_expire_code") = TWSF<ripple::STBlob>{(void *)&ripple::sfExpireCode};
    sfieldModule.attr("sf_create_code") = TWSF<ripple::STBlob>{(void *)&ripple::sfCreateCode};
    sfieldModule.attr("sf_memo_type") = TWSF<ripple::STBlob>{(void *)&ripple::sfMemoType};
    sfieldModule.attr("sf_memo_data") = TWSF<ripple::STBlob>{(void *)&ripple::sfMemoData};
    sfieldModule.attr("sf_memo_format") = TWSF<ripple::STBlob>{(void *)&ripple::sfMemoFormat};
    sfieldModule.attr("sf_did_document") = TWSF<ripple::STBlob>{(void *)&ripple::sfDIDDocument};
    sfieldModule.attr("sf_data") = TWSF<ripple::STBlob>{(void *)&ripple::sfData};
    sfieldModule.attr("sf_asset_class") = TWSF<ripple::STBlob>{(void *)&ripple::sfAssetClass};
    sfieldModule.attr("sf_provider") = TWSF<ripple::STBlob>{(void *)&ripple::sfProvider};
    sfieldModule.attr("sf_fulfillment") = TWSF<ripple::STBlob>{(void *)&ripple::sfFulfillment};
    sfieldModule.attr("sf_condition") = TWSF<ripple::STBlob>{(void *)&ripple::sfCondition};
    sfieldModule.attr("sf_master_signature") = TWSF<ripple::STBlob>{(void *)&ripple::sfMasterSignature};
    sfieldModule.attr("sf_unl_modify_validator") = TWSF<ripple::STBlob>{(void *)&ripple::sfUNLModifyValidator};
    sfieldModule.attr("sf_validator_to_disable") = TWSF<ripple::STBlob>{(void *)&ripple::sfValidatorToDisable};
    sfieldModule.attr("sf_validator_to_re_enable") = TWSF<ripple::STBlob>{(void *)&ripple::sfValidatorToReEnable};
    sfieldModule.attr("sf_hook_state_data") = TWSF<ripple::STBlob>{(void *)&ripple::sfHookStateData};
    sfieldModule.attr("sf_hook_return_string") = TWSF<ripple::STBlob>{(void *)&ripple::sfHookReturnString};
    sfieldModule.attr("sf_hook_parameter_name") = TWSF<ripple::STBlob>{(void *)&ripple::sfHookParameterName};
    sfieldModule.attr("sf_hook_parameter_value") = TWSF<ripple::STBlob>{(void *)&ripple::sfHookParameterValue};
    sfieldModule.attr("sf_account") = TWSF<ripple::STAccount>{(void *)&ripple::sfAccount};
    sfieldModule.attr("sf_owner") = TWSF<ripple::STAccount>{(void *)&ripple::sfOwner};
    sfieldModule.attr("sf_destination") = TWSF<ripple::STAccount>{(void *)&ripple::sfDestination};
    sfieldModule.attr("sf_issuer") = TWSF<ripple::STAccount>{(void *)&ripple::sfIssuer};
    sfieldModule.attr("sf_authorize") = TWSF<ripple::STAccount>{(void *)&ripple::sfAuthorize};
    sfieldModule.attr("sf_unauthorize") = TWSF<ripple::STAccount>{(void *)&ripple::sfUnauthorize};
    sfieldModule.attr("sf_regular_key") = TWSF<ripple::STAccount>{(void *)&ripple::sfRegularKey};
    sfieldModule.attr("sf_nftoken_minter") = TWSF<ripple::STAccount>{(void *)&ripple::sfNFTokenMinter};
    sfieldModule.attr("sf_emit_callback") = TWSF<ripple::STAccount>{(void *)&ripple::sfEmitCallback};
    sfieldModule.attr("sf_hook_account") = TWSF<ripple::STAccount>{(void *)&ripple::sfHookAccount};
    sfieldModule.attr("sf_other_chain_source") = TWSF<ripple::STAccount>{(void *)&ripple::sfOtherChainSource};
    sfieldModule.attr("sf_other_chain_destination") = TWSF<ripple::STAccount>{(void *)&ripple::sfOtherChainDestination};
    sfieldModule.attr("sf_attestation_signer_account") = TWSF<ripple::STAccount>{(void *)&ripple::sfAttestationSignerAccount};
    sfieldModule.attr("sf_attestation_reward_account") = TWSF<ripple::STAccount>{(void *)&ripple::sfAttestationRewardAccount};
    sfieldModule.attr("sf_locking_chain_door") = TWSF<ripple::STAccount>{(void *)&ripple::sfLockingChainDoor};
    sfieldModule.attr("sf_issuing_chain_door") = TWSF<ripple::STAccount>{(void *)&ripple::sfIssuingChainDoor};
    sfieldModule.attr("sf_paths") = WSF{(void *)&ripple::sfPaths};
    sfieldModule.attr("sf_base_asset") = TWSF<ripple::STCurrency>{(void *)&ripple::sfBaseAsset};
    sfieldModule.attr("sf_quote_asset") = TWSF<ripple::STCurrency>{(void *)&ripple::sfQuoteAsset};
    sfieldModule.attr("sf_asset") = TWSF<ripple::STIssue>{(void *)&ripple::sfAsset};
    sfieldModule.attr("sf_asset2") = TWSF<ripple::STIssue>{(void *)&ripple::sfAsset2};
    sfieldModule.attr("sf_locking_chain_issue") = TWSF<ripple::STIssue>{(void *)&ripple::sfLockingChainIssue};
    sfieldModule.attr("sf_issuing_chain_issue") = TWSF<ripple::STIssue>{(void *)&ripple::sfIssuingChainIssue};
    sfieldModule.attr("sf_xchain_bridge") = TWSF<ripple::STXChainBridge>{(void *)&ripple::sfXChainBridge};
    sfieldModule.attr("sf_indexes") = TWSF<ripple::STVector256>{(void *)&ripple::sfIndexes};
    sfieldModule.attr("sf_hashes") = TWSF<ripple::STVector256>{(void *)&ripple::sfHashes};
    sfieldModule.attr("sf_amendments") = TWSF<ripple::STVector256>{(void *)&ripple::sfAmendments};
    sfieldModule.attr("sf_nftoken_offers") = TWSF<ripple::STVector256>{(void *)&ripple::sfNFTokenOffers};
    sfieldModule.attr("sf_transaction_meta_data") = WSF{(void *)&ripple::sfTransactionMetaData};
    sfieldModule.attr("sf_created_node") = WSF{(void *)&ripple::sfCreatedNode};
    sfieldModule.attr("sf_deleted_node") = WSF{(void *)&ripple::sfDeletedNode};
    sfieldModule.attr("sf_modified_node") = WSF{(void *)&ripple::sfModifiedNode};
    sfieldModule.attr("sf_previous_fields") = WSF{(void *)&ripple::sfPreviousFields};
    sfieldModule.attr("sf_final_fields") = WSF{(void *)&ripple::sfFinalFields};
    sfieldModule.attr("sf_new_fields") = WSF{(void *)&ripple::sfNewFields};
    sfieldModule.attr("sf_template_entry") = WSF{(void *)&ripple::sfTemplateEntry};
    sfieldModule.attr("sf_memo") = WSF{(void *)&ripple::sfMemo};
    sfieldModule.attr("sf_signer_entry") = WSF{(void *)&ripple::sfSignerEntry};
    sfieldModule.attr("sf_nftoken") = WSF{(void *)&ripple::sfNFToken};
    sfieldModule.attr("sf_emit_details") = WSF{(void *)&ripple::sfEmitDetails};
    sfieldModule.attr("sf_hook") = WSF{(void *)&ripple::sfHook};
    sfieldModule.attr("sf_vote_entry") = WSF{(void *)&ripple::sfVoteEntry};
    sfieldModule.attr("sf_auction_slot") = WSF{(void *)&ripple::sfAuctionSlot};
    sfieldModule.attr("sf_auth_account") = WSF{(void *)&ripple::sfAuthAccount};
    sfieldModule.attr("sf_price_data") = WSF{(void *)&ripple::sfPriceData};
    sfieldModule.attr("sf_signer") = WSF{(void *)&ripple::sfSigner};
    sfieldModule.attr("sf_majority") = WSF{(void *)&ripple::sfMajority};
    sfieldModule.attr("sf_disabled_validator") = WSF{(void *)&ripple::sfDisabledValidator};
    sfieldModule.attr("sf_emitted_txn") = WSF{(void *)&ripple::sfEmittedTxn};
    sfieldModule.attr("sf_hook_execution") = WSF{(void *)&ripple::sfHookExecution};
    sfieldModule.attr("sf_hook_definition") = WSF{(void *)&ripple::sfHookDefinition};
    sfieldModule.attr("sf_hook_parameter") = WSF{(void *)&ripple::sfHookParameter};
    sfieldModule.attr("sf_hook_grant") = WSF{(void *)&ripple::sfHookGrant};
    sfieldModule.attr("sf_xchain_claim_proof_sig") = WSF{(void *)&ripple::sfXChainClaimProofSig};
    sfieldModule.attr("sf_xchain_create_account_proof_sig") = WSF{(void *)&ripple::sfXChainCreateAccountProofSig};
    sfieldModule.attr("sf_xchain_claim_attestation_collection_element") = WSF{(void *)&ripple::sfXChainClaimAttestationCollectionElement};
    sfieldModule.attr("sf_xchain_create_account_attestation_collection_element") = WSF{(void *)&ripple::sfXChainCreateAccountAttestationCollectionElement};
    sfieldModule.attr("sf_signers") = WSF{(void *)&ripple::sfSigners};
    sfieldModule.attr("sf_signer_entries") = WSF{(void *)&ripple::sfSignerEntries};
    sfieldModule.attr("sf_template") = WSF{(void *)&ripple::sfTemplate};
    sfieldModule.attr("sf_necessary") = WSF{(void *)&ripple::sfNecessary};
    sfieldModule.attr("sf_sufficient") = WSF{(void *)&ripple::sfSufficient};
    sfieldModule.attr("sf_affected_nodes") = WSF{(void *)&ripple::sfAffectedNodes};
    sfieldModule.attr("sf_memos") = WSF{(void *)&ripple::sfMemos};
    sfieldModule.attr("sf_nftokens") = WSF{(void *)&ripple::sfNFTokens};
    sfieldModule.attr("sf_hooks") = WSF{(void *)&ripple::sfHooks};
    sfieldModule.attr("sf_vote_slots") = WSF{(void *)&ripple::sfVoteSlots};
    sfieldModule.attr("sf_auth_accounts") = WSF{(void *)&ripple::sfAuthAccounts};
    sfieldModule.attr("sf_price_data_series") = WSF{(void *)&ripple::sfPriceDataSeries};
    sfieldModule.attr("sf_majorities") = WSF{(void *)&ripple::sfMajorities};
    sfieldModule.attr("sf_disabled_validators") = WSF{(void *)&ripple::sfDisabledValidators};
    sfieldModule.attr("sf_hook_executions") = WSF{(void *)&ripple::sfHookExecutions};
    sfieldModule.attr("sf_hook_parameters") = WSF{(void *)&ripple::sfHookParameters};
    sfieldModule.attr("sf_hook_grants") = WSF{(void *)&ripple::sfHookGrants};
    sfieldModule.attr("sf_xchain_claim_attestations") = WSF{(void *)&ripple::sfXChainClaimAttestations};
    sfieldModule.attr("sf_xchain_create_account_attestations") = WSF{(void *)&ripple::sfXChainCreateAccountAttestations};

    // Amendments
    transactorModule.attr("fixMasterKeyAsRegularKey") = ripple::fixMasterKeyAsRegularKey;
    transactorModule.attr("fix1543") = ripple::fix1543;
    transactorModule.attr("fix1571") = ripple::fix1571;

    // Other
    basicsModule.attr("zero_amount") = beast::zero;
};
