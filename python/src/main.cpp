#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include <ripple/app/tx/impl/Transactor.h>
#include <ripple/protocol/Serializer.h>
#include <ripple/protocol/st.h>
#include <ripple/protocol/TxFlags.h>
#include <ripple/protocol/Feature.h>
#include <ripple/ledger/View.h>
#include <ripple/protocol/digest.h>
#include <map>
#include <iostream>
#include <string>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

struct WSF {
  void *f_;
  operator ripple::SField const &() const {
    return *static_cast<ripple::SField *>(f_);
  };
};

template <class T> struct TWSF : WSF {
  operator ripple::TypedField<T> const &() const {
    return *static_cast<ripple::TypedField<T> *>(f_);
  };
};

template <typename T>
int getSTId() { return 0; }

template <>
int getSTId<ripple::SF_UINT8>() { return ripple::STI_UINT8; }

template <>
int getSTId<ripple::SF_UINT16>() { return ripple::STI_UINT16; }

template <>
int getSTId<ripple::SF_UINT32>() { return ripple::STI_UINT32; }

template <>
int getSTId<ripple::SF_UINT64>() { return ripple::STI_UINT64; }

template <>
int getSTId<ripple::SF_UINT128>() { return ripple::STI_UINT128; }

template <>
int getSTId<ripple::SF_UINT256>() { return ripple::STI_UINT256; }

template <>
int getSTId<ripple::SF_UINT160>() { return ripple::STI_UINT160; }

template <>
int getSTId<ripple::SF_AMOUNT>() { return ripple::STI_AMOUNT; }

template <>
int getSTId<ripple::SF_VL>() { return ripple::STI_VL; }

template <> 
int getSTId<ripple::SF_ACCOUNT>() { return ripple::STI_ACCOUNT; }

template <> 
int getSTId<ripple::STObject>() { return ripple::STI_OBJECT; }

template <> 
int getSTId<ripple::STArray>() { return ripple::STI_ARRAY; }

template <>
int getSTId<ripple::SF_VECTOR256>() { return ripple::STI_VECTOR256; }

template <>
int getSTId<ripple::SF_UINT96>() { return ripple::STI_UINT96; }

template <>
int getSTId<ripple::SF_UINT192>() { return ripple::STI_UINT192; }

template <>
int getSTId<ripple::SF_UINT384>() { return ripple::STI_UINT384; }

template <>
int getSTId<ripple::SF_UINT512>() { return ripple::STI_UINT512; }

template <>
int getSTId<ripple::SF_ISSUE>() { return ripple::STI_ISSUE; }

template <class T>
T const&
newSField(const int fieldValue, char const* fieldName)
{
    if (ripple::SField const& field = ripple::SField::getField(fieldName); field != ripple::sfInvalid)
        return static_cast<T const&>(field);
    T const* newSField = new T(getSTId<T>(), fieldValue, fieldName);
    return *newSField;
}

template <class T>
ripple::SField const&
newUntypedSField(const int fieldValue, char const* fieldName)
{
    if (ripple::SField const& field = ripple::SField::getField(fieldName); field != ripple::sfInvalid)
        return field;
    ripple::SField const* newSField = new ripple::SField(getSTId<T>(), fieldValue, fieldName);
    return *newSField;
}

template <class T>
TWSF<T>
wrappedNewSField(const int fieldValue, std::string const fieldName)
{
    ripple::TypedField<T> const& sfield = newSField<ripple::TypedField<T>>(fieldValue, fieldName.c_str());
    return TWSF<T>{(void *)&sfield};
}

template <class T>
WSF
wrappedNewUntypedSField(const int fieldValue, std::string const fieldName)
{
    ripple::SField const& sfield = newUntypedSField<T>(fieldValue, fieldName.c_str());
    return WSF{(void *)&sfield};
}

ripple::SField const& constructCustomSField(int tid, int fv, const char* fn) {
    if (ripple::SField const& field = ripple::SField::getField(ripple::field_code(tid, fv)); field != ripple::sfInvalid)
        return field;
    return *(new ripple::TypedField<ripple::STPluginType>(tid, fv, fn));
}

TWSF<ripple::STPluginType>
constructCustomWrappedSField(int tid, const char* fn, int fv)
{
    ripple::SField const& sfield = constructCustomSField(tid, fv, fn);
    return TWSF<ripple::STPluginType>{(void *)&sfield};
}

template <class... Args>
static ripple::uint256
indexHash(std::uint16_t space, Args const&... args)
{
    return ripple::sha512Half(space, args...);
}

namespace py = pybind11;

PYBIND11_MODULE(xrpl_plugin_py, m) {

    /*
    * Enums
    */
    
    py::enum_<ripple::LedgerEntryType>(m, "LedgerEntryType")
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
    
    py::enum_<ripple::LedgerSpecificFlags>(m, "LedgerSpecificFlags")
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

    py::enum_<ripple::TEScodes>(m, "TEScodes", py::arithmetic())
        .value("tesSUCCESS", ripple::TEScodes::tesSUCCESS)
        .export_values();
    
    py::enum_<ripple::TEMcodes>(m, "TEMcodes", py::arithmetic())
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
    
    py::enum_<ripple::TECcodes>(m, "TECcodes", py::arithmetic())
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
        .value("tecHOOK_ERROR", ripple::TECcodes::tecHOOK_ERROR)
        .value("tecMAX_SEQUENCE_REACHED", ripple::TECcodes::tecMAX_SEQUENCE_REACHED)
        .value("tecNO_SUITABLE_NFTOKEN_PAGE", ripple::TECcodes::tecNO_SUITABLE_NFTOKEN_PAGE)
        .value("tecNFTOKEN_BUY_SELL_MISMATCH", ripple::TECcodes::tecNFTOKEN_BUY_SELL_MISMATCH)
        .value("tecNFTOKEN_OFFER_TYPE_MISMATCH", ripple::TECcodes::tecNFTOKEN_OFFER_TYPE_MISMATCH)
        .value("tecCANT_ACCEPT_OWN_NFTOKEN_OFFER", ripple::TECcodes::tecCANT_ACCEPT_OWN_NFTOKEN_OFFER)
        .value("tecINSUFFICIENT_FUNDS", ripple::TECcodes::tecINSUFFICIENT_FUNDS)
        .value("tecOBJECT_NOT_FOUND", ripple::TECcodes::tecOBJECT_NOT_FOUND)
        .value("tecINSUFFICIENT_PAYMENT", ripple::TECcodes::tecINSUFFICIENT_PAYMENT)
        .export_values();
    
    py::enum_<ripple::TERcodes>(m, "TERcodes", py::arithmetic())
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

    py::enum_<ripple::TEFcodes>(m, "TEFcodes", py::arithmetic())
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
    
    py::class_<ripple::NotTEC> NotTEC(m, "NotTEC");
    NotTEC
        .def(py::init<ripple::TEScodes>())
        .def(py::init<ripple::TEMcodes>())
        .def(py::init<ripple::TERcodes>())
        .def(py::init<ripple::TEFcodes>())
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::implicitly_convertible<ripple::TEScodes, ripple::NotTEC>();
    py::implicitly_convertible<ripple::TEMcodes, ripple::NotTEC>();
    py::implicitly_convertible<ripple::TERcodes, ripple::NotTEC>();
    py::implicitly_convertible<ripple::TEFcodes, ripple::NotTEC>();

    py::class_<ripple::TER> TER(m, "TER");
    TER
        .def(py::init<ripple::TEScodes>())
        .def(py::init<ripple::TEMcodes>())
        .def(py::init<ripple::TERcodes>())
        .def(py::init<ripple::TEFcodes>())
        .def(py::init<ripple::TECcodes>())
        .def(py::init<ripple::NotTEC>())
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::implicitly_convertible<ripple::TEScodes, ripple::TER>();
    py::implicitly_convertible<ripple::TEMcodes, ripple::TER>();
    py::implicitly_convertible<ripple::TERcodes, ripple::TER>();
    py::implicitly_convertible<ripple::TEFcodes, ripple::TER>();
    py::implicitly_convertible<ripple::TECcodes, ripple::TER>();
    py::implicitly_convertible<ripple::NotTEC, ripple::TER>();

    py::enum_<ripple::SOEStyle>(m, "SOEStyle")
        .value("soeINVALID", ripple::SOEStyle::soeINVALID)
        .value("soeREQUIRED", ripple::SOEStyle::soeREQUIRED)
        .value("soeOPTIONAL", ripple::SOEStyle::soeOPTIONAL)
        .value("soeDEFAULT", ripple::SOEStyle::soeDEFAULT)
        .export_values();
    
    /*
    * Small classes
    */

    py::class_<WSF> PythonWSF(m, "SField");
    PythonWSF
        .def_property_readonly("fieldCode",
            [](const WSF &wsf) {
                return static_cast<ripple::SField const&>(wsf).fieldCode;
            }
        )
        .def("__repr__",
            [](const WSF &wsf) {
                return "sf" + static_cast<ripple::SField const&>(wsf).getName();
            }
        )
        ;
    py::class_<TWSF<ripple::STAccount>, WSF> TWSF_STAccount(m, "SF_ACCOUNT");
    py::class_<TWSF<ripple::STAmount>, WSF> TWSF_STAmount(m, "SF_AMOUNT");
    py::class_<TWSF<ripple::STUInt8>, WSF> TWSF_STUInt8(m, "SF_UINT8");
    py::class_<TWSF<ripple::STUInt16>, WSF> TWSF_STUInt16(m, "SF_UINT16");
    py::class_<TWSF<ripple::STUInt32>, WSF> TWSF_STUInt32(m, "SF_UINT32");
    py::class_<TWSF<ripple::STUInt64>, WSF> TWSF_STUInt64(m, "SF_UINT64");
    py::class_<TWSF<ripple::STUInt128>, WSF> TWSF_STUInt128(m, "SF_UINT128");
    py::class_<TWSF<ripple::STUInt160>, WSF> TWSF_STUInt160(m, "SF_UINT160");
    py::class_<TWSF<ripple::STUInt256>, WSF> TWSF_STUInt256(m, "SF_UINT256");
    py::class_<TWSF<ripple::STBlob>, WSF> TWSF_STBlob(m, "SF_VL");
    py::class_<TWSF<ripple::STPluginType>, WSF> TWSF_STPluginType(m, "SF_PLUGINTYPE");
    py::class_<TWSF<ripple::STVector256>, WSF> TWSF_STVector256(m, "SF_VECTOR256");


    py::class_<ripple::AccountID> AccountID(m, "AccountID");
    AccountID
        .def(py::init<>())
        .def(py::init<ripple::Slice>())
        .def("parse_hex",
            [](ripple::AccountID acct, std::string const& hex) {
                return acct.parseHex(hex);
            }
        )
        .def("to_base58",
            [](const ripple::AccountID &a) {
                return ripple::toBase58(a);
            }
        )
        .def("to_buffer",
            [](const ripple::AccountID &a) {
                return ripple::Buffer(a.data(), a.size());
            },
            py::return_value_policy::move
        )
        .def("from_buffer",
            [](const ripple::Buffer &buf) {
                return ripple::AccountID::fromVoid(std::move(buf.data()));
            }
        )
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__repr__",
            [](const ripple::AccountID &a) {
                return "<AccountID:'" + ripple::toBase58(a) + "'>";
            }
        );
    
    py::class_<ripple::XRPAmount> XRPAmount(m, "XRPAmount");
    XRPAmount
        .def(py::init<std::int64_t &>())
        .def_property_readonly("drops",
            [](const ripple::XRPAmount &xrpAmount) {
                return xrpAmount.drops();
            }
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
    
    py::class_<ripple::Issue> Issue(m, "Issue");
    py::class_<ripple::Currency> Currency(m, "Currency");
    
    py::class_<ripple::uint256> uint256(m, "uint256");
    uint256
        .def("__repr__",
            [](const ripple::uint256 &uint256) {
                return ripple::to_string(uint256);
            }
        );
    
    py::class_<ripple::Slice> Slice(m, "Slice");
    Slice
        .def("to_buffer",
            [](const ripple::Slice &slice) {
                return ripple::Buffer(slice.data(), slice.size());
            },
            py::return_value_policy::move
        )
        .def("from_buffer",
            [](const ripple::Buffer &buf) {
                return ripple::Slice(std::move(buf.data()), buf.size());
            }
        )
        .def("__repr__",
            [](const ripple::Slice &slice) {
                return strHex(slice);
            }
        )
    ;
    
    py::class_<ripple::Buffer> Buffer(m, "Buffer");
    Buffer
        .def("__repr__",
            [](const ripple::Buffer &buf) {
                return strHex(buf);
            }
        );
    
    py::class_<beast::Zero> BeastZero(m, "BeastZero");
    py::class_<beast::Journal> Journal(m, "Journal");

    py::class_<ripple::SeqProxy> SeqProxy(m, "SeqProxy");
    SeqProxy
        .def("value", &ripple::SeqProxy::value)
    ;

    py::class_<Json::Value> JsonValue(m, "JsonValue");
    JsonValue
        .def("is_string", &Json::Value::isString)
        .def("as_string", &Json::Value::asString)
        .def("__repr__", &Json::Value::asCString);
    
    py::enum_<ripple::VoteBehavior>(m, "VoteBehavior")
        .value("OBSOLETE", ripple::VoteBehavior::Obsolete)
        .value("DEFAULT_NO", ripple::VoteBehavior::DefaultNo)
        .value("DEFAULT_YES", ripple::VoteBehavior::DefaultYes);

    /*
    * STObjects
    */

    py::class_<ripple::STBase> STBase(m, "STBase");
    STBase
        .def("get_fname",
            [](const ripple::STBase &obj) {
                ripple::SField const& f = obj.getFName();
                // TODO: make this general
                return TWSF<ripple::STPluginType>{(void *)&f};
            }
        )
        .def("__repr__",
            [](const ripple::STBase &obj) {
                return obj.getFullText();
            }
        );
    py::class_<ripple::STAccount, ripple::STBase> STAccount(m, "STAccount");

    py::class_<ripple::STAmount, ripple::STBase> STAmount(m, "STAmount");
    STAmount
        .def(py::init<ripple::STAmount &>())
        .def("is_xrp",
            [](const ripple::STAmount &amt) {
                return ripple::isXRP(amt);
            }
        )
        .def("xrp", &ripple::STAmount::xrp)
        .def("native", &ripple::STAmount::native)
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

    py::class_<ripple::STBlob, ripple::STBase> STBlob(m, "STBlob");
    py::class_<ripple::STUInt8, ripple::STBase> STUInt8(m, "STUInt8");
    py::class_<ripple::STUInt16, ripple::STBase> STUInt16(m, "STUInt16");
    py::class_<ripple::STUInt32, ripple::STBase> STUInt32(m, "STUInt32");
    py::class_<ripple::STUInt64, ripple::STBase> STUInt64(m, "STUInt64");
    py::class_<ripple::STUInt128, ripple::STBase> STUInt128(m, "STUInt128");
    py::class_<ripple::STUInt160, ripple::STBase> STUInt160(m, "STUInt160");
    py::class_<ripple::STUInt256, ripple::STBase> STUInt256(m, "STUInt256");
    py::class_<ripple::STPathSet, ripple::STBase> STPathSet(m, "STPathSet");
    py::class_<ripple::STPluginType, ripple::STBase> STPluginType(m, "STPluginType");
    py::class_<ripple::STVector256, ripple::STBase> STVector256(m, "STVector256");

    py::class_<ripple::STArray, ripple::STBase> STArray(m, "STArray");

    py::class_<ripple::STObject, std::shared_ptr<ripple::STObject>> STObject(m, "STObject");
    STObject
        .def("is_flag", &ripple::STObject::isFlag)
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
        .def("__setitem__", [](ripple::STObject &obj, TWSF<ripple::STBlob> sf, ripple::STBlob::value_type value) {
            obj[static_cast<ripple::TypedField<ripple::STBlob> const&>(sf)] = value;
        })
        .def("__setitem__", [](ripple::STObject &obj, TWSF<ripple::STPluginType> sf, ripple::STPluginType::value_type value) {
            obj[static_cast<ripple::TypedField<ripple::STPluginType> const&>(sf)] = value;
        })
        .def("is_field_present",
            [](const ripple::STObject &obj, const WSF &wsf) {
                return obj.isFieldPresent(static_cast<ripple::SField const&>(wsf));
            }
        )
        .def("make_field_absent",
            [](ripple::STObject &obj, const WSF &wsf) {
                return obj.makeFieldAbsent(static_cast<ripple::SField const&>(wsf));
            }
        )
        .def("__delitem__",  // del obj[sfSField]
            [](ripple::STObject &obj, const WSF &wsf) {
                return obj.makeFieldAbsent(static_cast<ripple::SField const&>(wsf));
            }
        )
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::class_<ripple::STTx, ripple::STObject, std::shared_ptr<ripple::STTx>> STTx(m, "STTx");
    STTx
        .def("get_txn_type", &ripple::STTx::getTxnType)
        .def("get_flags", &ripple::STTx::getFlags)
        .def("get_seq_proxy", &ripple::STTx::getSeqProxy)
    ;

    py::class_<ripple::STLedgerEntry, ripple::STObject, std::shared_ptr<ripple::STLedgerEntry>> STLedgerEntry(m, "STLedgerEntry");
    STLedgerEntry
        .def("get_type", &ripple::STLedgerEntry::getType)
    ;

    /*
    * Contexts and classes that the contexts depend on
    */

    py::class_<ripple::Rules> Rules(m, "Rules");
    Rules
        .def("enabled", &ripple::Rules::enabled)
        .def("enabled",
            [](const ripple::Rules &rules, py::object amendment) {
                auto const name = amendment.attr("name").cast<std::string>();
                return rules.enabled(ripple::sha512Half(ripple::Slice(name.data(), name.size())));
            }
        );
    
    py::class_<ripple::Keylet> Keylet(m, "Keylet");
    Keylet
        .def(py::init<std::uint16_t, ripple::uint256>())
        .def_property_readonly("type",
            [](const ripple::Keylet &keylet) {
                return keylet.type;
            }
        )
        .def_property_readonly("key",
            [](const ripple::Keylet &keylet) {
                return keylet.key.data();
            }
        );
    
    py::class_<ripple::Fees> Fees(m, "Fees");
    Fees
        .def_property_readonly("base",
            [](const ripple::Fees &fees) {
                return fees.base;
            }
        )
        .def_property_readonly("reserve",
            [](const ripple::Fees &fees) {
                return fees.reserve;
            }
        )
        .def_property_readonly("increment",
            [](const ripple::Fees &fees) {
                return fees.increment;
            }
        )
        .def("account_reserve", &ripple::Fees::accountReserve);
    
    py::class_<ripple::NetClock::time_point> TimePoint(m, "TimePoint");
    TimePoint
        .def("time_since_epoch",
            [](const ripple::NetClock::time_point &tp) {
                return tp.time_since_epoch().count();
            }
        )
        .def("__repr__",
            [](const ripple::NetClock::time_point &tp) {
                return ripple::to_string(tp);
            }
        )
    ;
    
    py::class_<ripple::LedgerInfo> LedgerInfo(m, "LedgerInfo");
    LedgerInfo
        .def_readonly("seq", &ripple::LedgerInfo::seq)
        .def_readonly("parent_close_time", &ripple::LedgerInfo::parentCloseTime)
        .def_readonly("hash", &ripple::LedgerInfo::hash)
        .def_readonly("tx_hash", &ripple::LedgerInfo::txHash)
        .def_readonly("account_hash", &ripple::LedgerInfo::accountHash)
        .def_readonly("parent_hash", &ripple::LedgerInfo::parentHash)
        .def_readonly("drops", &ripple::LedgerInfo::drops)
        .def_readonly("validated", &ripple::LedgerInfo::validated)
        .def_readonly("accepted", &ripple::LedgerInfo::accepted)
        .def_readonly("close_flags", &ripple::LedgerInfo::closeFlags)
        .def_readonly("close_time_resolution", &ripple::LedgerInfo::closeTimeResolution)
        .def_readonly("close_time", &ripple::LedgerInfo::closeTime)
    ;

    py::enum_<ripple::Transactor::ConsequencesFactoryType>(m, "ConsequencesFactoryType")
        .value("Normal", ripple::Transactor::ConsequencesFactoryType::Normal)
        .value("Blocker", ripple::Transactor::ConsequencesFactoryType::Blocker)
        .value("Custom", ripple::Transactor::ConsequencesFactoryType::Custom);

    py::enum_<ripple::TxConsequences::Category>(m, "TxConsequencesCategory")
        .value("normal", ripple::TxConsequences::Category::normal)
        .value("blocker", ripple::TxConsequences::Category::blocker);

    py::class_<ripple::TxConsequences> TxConsequences(m, "TxConsequences");
    TxConsequences
        .def(py::init<ripple::NotTEC>())
        .def(py::init<ripple::STTx>())
        .def(py::init<ripple::STTx, ripple::TxConsequences::Category>())
        .def(py::init<ripple::STTx, ripple::XRPAmount>())
        .def(py::init<ripple::STTx, std::uint32_t>())
        .def("fee", &ripple::TxConsequences::fee)
        .def("potential_spend", &ripple::TxConsequences::potentialSpend)
        .def("seq_proxy", &ripple::TxConsequences::seqProxy)
        .def("sequences_consumed", &ripple::TxConsequences::sequencesConsumed)
        .def("is_blocker", &ripple::TxConsequences::isBlocker)
        .def("following_seq", &ripple::TxConsequences::followingSeq)
    ;

    py::class_<ripple::Serializer> Serializer(m, "Serializer");
    Serializer
        .def("add8", &ripple::Serializer::add8)
        .def("add16", &ripple::Serializer::add16)
        .def("add32", py::overload_cast<std::uint32_t>(&ripple::Serializer::add32))
        .def("add64", &ripple::Serializer::add64)
        .def("add_raw", py::overload_cast<ripple::Blob const&>(&ripple::Serializer::addRaw))
        .def("add_raw", py::overload_cast<ripple::Slice>(&ripple::Serializer::addRaw))
        .def("add_raw", py::overload_cast<const ripple::Serializer&>(&ripple::Serializer::addRaw))
        .def("add_vl", [](ripple::Serializer& self, const ripple::Blob& vector) {
            return self.addVL(vector);
        })
        .def("add_vl", [](ripple::Serializer& self, const ripple::Slice& slice) {
            return self.addVL(slice);
        })
    ;

    py::class_<ripple::SerialIter> SerialIter(m, "SerialIter");
    SerialIter
        .def("get8", &ripple::SerialIter::get8)
        .def("get16", &ripple::SerialIter::get16)
        .def("get32", &ripple::SerialIter::get32)
        .def("get64", &ripple::SerialIter::get64)
        .def("get128", &ripple::SerialIter::get128)
        .def("get160", &ripple::SerialIter::get160)
        .def("get256", &ripple::SerialIter::get256)
        .def("get_vl_buffer", &ripple::SerialIter::getVLBuffer)
    ;

    py::class_<ripple::Application> Application(m, "Application");

    py::class_<ripple::ReadView> ReadView(m, "ReadView");
    ReadView
        .def_property_readonly("fees",
            [](const ripple::ReadView &view) {
                return view.fees();
            }
        )
        .def_property_readonly("rules",
            [](const ripple::ReadView &view) {
                return view.rules();
            }
        )
        .def("info", &ripple::ReadView::info)
        .def("open", &ripple::ReadView::open)
        .def("parent_close_time", &ripple::ReadView::parentCloseTime)
        .def("seq", &ripple::ReadView::seq)
        .def("exists", &ripple::ReadView::exists)
        ;
    
    py::class_<ripple::OpenView, ripple::ReadView> OpenView(m, "OpenView");
    OpenView
        .def("tx_count", &ripple::OpenView::txCount)
    ;

    py::class_<ripple::ApplyView> ApplyView(m, "ApplyView");
    ApplyView
        .def("peek", &ripple::ApplyView::peek)
        .def("update", &ripple::ApplyView::update)
        .def("info", &ripple::ApplyView::info)
        .def("rules", &ripple::ApplyView::rules)
        .def("fees", &ripple::ApplyView::fees)
        .def("flags", &ripple::ApplyView::flags)
        .def("erase", &ripple::ApplyView::erase)
        .def("insert", &ripple::ApplyView::insert)
        .def("dir_remove", py::overload_cast<ripple::Keylet const&, std::uint64_t, ripple::uint256 const&, bool>(&ripple::ApplyView::dirRemove))
        .def("dir_remove", py::overload_cast<ripple::Keylet const&, std::uint64_t, ripple::Keylet const&, bool>(&ripple::ApplyView::dirRemove))
        .def("dir_insert",
            [](ripple::ApplyView &view, ripple::AccountID account, ripple::Keylet keylet) {
                return view.dirInsert(ripple::keylet::ownerDir(account), keylet, ripple::describeOwnerDir(account));;
            }
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
    > ApplyViewBase(m, "ApplyViewBase");
    py::class_<
        ripple::ApplyViewImpl,
        ripple::detail::ApplyViewBase
    > ApplyViewImpl(m, "ApplyViewImpl");
    
    py::class_<ripple::PreflightContext> PreflightContext(m, "PreflightContext");
    PreflightContext
        .def_property_readonly("app",
            [](const ripple::PreflightContext &ctx) -> const ripple::Application& {
                return ctx.app;
            }, py::return_value_policy::reference)
        .def_property_readonly("tx",
            [](const ripple::PreflightContext &ctx) {
                return ctx.tx;
            }
        )
        .def_readonly("rules", &ripple::PreflightContext::rules)
        .def_readonly("flags", &ripple::PreflightContext::flags)
        .def_readonly("j", &ripple::PreflightContext::j)
    ;
    
    py::class_<ripple::PreclaimContext> PreclaimContext(m, "PreclaimContext");
    PreclaimContext
        .def_property_readonly("app",
            [](const ripple::PreclaimContext &ctx) -> const ripple::Application& {
                return ctx.app;
            }, py::return_value_policy::reference)
        .def_property_readonly("view",
            [](const ripple::PreclaimContext &ctx) -> const ripple::ReadView& {
                return ctx.view;
            }, py::return_value_policy::reference)
        .def_readonly("preflight_result", &ripple::PreclaimContext::preflightResult)
        .def_readonly("flags", &ripple::PreclaimContext::flags)
        .def_readonly("j", &ripple::PreclaimContext::j)
        ;

    py::class_<ripple::ApplyContext> ApplyContext(m, "ApplyContext");
    ApplyContext
        .def_property_readonly("app",
            [](const ripple::ApplyContext &ctx) -> const ripple::Application& {
                return ctx.app;
            }, py::return_value_policy::reference)
        .def_property_readonly("tx",
            [](const ripple::ApplyContext &ctx) {
                return ctx.tx;
            }
        )
        .def_readonly("preclaim_result", &ripple::ApplyContext::preclaimResult)
        .def_readonly("base_fee", &ripple::ApplyContext::baseFee)
        .def_readonly("journal", &ripple::ApplyContext::journal)
        .def("deliver", &ripple::ApplyContext::deliver)
        .def("view",
            [](const ripple::ApplyContext &ctx) -> const ripple::ApplyView& {
                return ctx.view();
            }, py::return_value_policy::reference);
    
    // py::register_exception<ripple::LogicError>(m, "LogicError");
    
    /*
    * Misc additional stuff
    */

    m
        // Keylets
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

        // Other
        .def("preflight1", &ripple::preflight1)
        .def("preflight2", &ripple::preflight2)
        .def("is_tes_success", &ripple::isTesSuccess)
        .def("make_sle",
            [](const ripple::Keylet &keylet) {
                return std::make_shared<ripple::SLE>(keylet);
            },
            py::return_value_policy::move
        )
        .def("describe_owner_dir", &ripple::describeOwnerDir)
        .def("adjust_owner_count", &ripple::adjustOwnerCount)
        .def("create_new_sfield_STAccount", &wrappedNewSField<ripple::STAccount>)
        .def("create_new_sfield_STAmount", &wrappedNewSField<ripple::STAmount>)
        .def("create_new_sfield_STUInt8", &wrappedNewSField<ripple::STUInt8>)
        .def("create_new_sfield_STUInt16", &wrappedNewSField<ripple::STUInt16>)
        .def("create_new_sfield_STUInt32", &wrappedNewSField<ripple::STUInt32>)
        .def("create_new_sfield_STUInt64", &wrappedNewSField<ripple::STUInt64>)
        .def("create_new_sfield_STUInt128", &wrappedNewSField<ripple::STUInt128>)
        .def("create_new_sfield_STUInt160", &wrappedNewSField<ripple::STUInt160>)
        .def("create_new_sfield_STUInt256", &wrappedNewSField<ripple::STUInt256>)
        .def("create_new_sfield_STBlob", &wrappedNewSField<ripple::STBlob>)
        .def("create_new_sfield_STPluginType", &wrappedNewSField<ripple::STPluginType>)
        .def("create_new_sfield_STVector256", &wrappedNewSField<ripple::STVector256>)
        .def("create_new_sfield_STPluginType", &wrappedNewSField<ripple::STPluginType>)
        .def("create_new_sfield_STArray", &wrappedNewUntypedSField<ripple::STArray>)
        .def("create_new_sfield_STObject", &wrappedNewUntypedSField<ripple::STObject>)
        .def("construct_custom_sfield", &constructCustomWrappedSField)
        .def("make_name", &ripple::make_name)
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
        .def("parse_base58", py::overload_cast<std::string const&>(&ripple::parseBase58<ripple::AccountID>))
        .def("parse_base58", py::overload_cast<std::string const&>(&ripple::parseBase58<ripple::Seed>))
        // .def("parse_base58", py::overload_cast<std::string const&>(&ripple::parseBase58<ripple::PublicKey>))
        .def("make_stplugintype", 
            [](const WSF &wsf, ripple::Buffer& b) {
                ripple::SField const &f = static_cast<ripple::SField const&>(wsf);
                auto const stVar = ripple::detail::make_stvar<ripple::STPluginType>(f, b.data(), b.size());
                return stVar;
            },
            py::return_value_policy::move
        )
        // TODO: replace `indexHash` with a native Python implementation
        .def("index_hash", &indexHash<>)
        .def("index_hash", &indexHash<ripple::AccountID, std::uint32_t>)
    ;
    

    // Flags
    m.attr("tf_fully_canonical_sig") = ripple::tfFullyCanonicalSig;
    m.attr("tf_universal") = ripple::tfUniversal;
    m.attr("tf_universal_mask") = ripple::tfUniversalMask;

    // SFields
    m.attr("sf_close_resolution") = TWSF<ripple::STUInt8>{(void *)&ripple::sfCloseResolution};
    m.attr("sf_method") = TWSF<ripple::STUInt8>{(void *)&ripple::sfMethod};
    m.attr("sf_transaction_result") = TWSF<ripple::STUInt8>{(void *)&ripple::sfTransactionResult};
    m.attr("sf_tick_size") = TWSF<ripple::STUInt8>{(void *)&ripple::sfTickSize};
    m.attr("sf_unl_modify_disabling") = TWSF<ripple::STUInt8>{(void *)&ripple::sfUNLModifyDisabling};
    m.attr("sf_hook_result") = TWSF<ripple::STUInt8>{(void *)&ripple::sfHookResult};
    m.attr("sf_ledger_entry_type") = TWSF<ripple::STUInt16>{(void *)&ripple::sfLedgerEntryType};
    m.attr("sf_transaction_type") = TWSF<ripple::STUInt16>{(void *)&ripple::sfTransactionType};
    m.attr("sf_signer_weight") = TWSF<ripple::STUInt16>{(void *)&ripple::sfSignerWeight};
    m.attr("sf_transfer_fee") = TWSF<ripple::STUInt16>{(void *)&ripple::sfTransferFee};
    m.attr("sf_version") = TWSF<ripple::STUInt16>{(void *)&ripple::sfVersion};
    m.attr("sf_hook_state_change_count") = TWSF<ripple::STUInt16>{(void *)&ripple::sfHookStateChangeCount};
    m.attr("sf_hook_emit_count") = TWSF<ripple::STUInt16>{(void *)&ripple::sfHookEmitCount};
    m.attr("sf_hook_execution_index") = TWSF<ripple::STUInt16>{(void *)&ripple::sfHookExecutionIndex};
    m.attr("sf_hook_api_version") = TWSF<ripple::STUInt16>{(void *)&ripple::sfHookApiVersion};
    m.attr("sf_network_id") = TWSF<ripple::STUInt32>{(void *)&ripple::sfNetworkID};
    m.attr("sf_flags") = TWSF<ripple::STUInt32>{(void *)&ripple::sfFlags};
    m.attr("sf_source_tag") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSourceTag};
    m.attr("sf_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSequence};
    m.attr("sf_previous_txn_lgr_seq") = TWSF<ripple::STUInt32>{(void *)&ripple::sfPreviousTxnLgrSeq};
    m.attr("sf_ledger_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLedgerSequence};
    m.attr("sf_close_time") = TWSF<ripple::STUInt32>{(void *)&ripple::sfCloseTime};
    m.attr("sf_parent_close_time") = TWSF<ripple::STUInt32>{(void *)&ripple::sfParentCloseTime};
    m.attr("sf_signing_time") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSigningTime};
    m.attr("sf_expiration") = TWSF<ripple::STUInt32>{(void *)&ripple::sfExpiration};
    m.attr("sf_transfer_rate") = TWSF<ripple::STUInt32>{(void *)&ripple::sfTransferRate};
    m.attr("sf_wallet_size") = TWSF<ripple::STUInt32>{(void *)&ripple::sfWalletSize};
    m.attr("sf_owner_count") = TWSF<ripple::STUInt32>{(void *)&ripple::sfOwnerCount};
    m.attr("sf_destination_tag") = TWSF<ripple::STUInt32>{(void *)&ripple::sfDestinationTag};
    m.attr("sf_high_quality_in") = TWSF<ripple::STUInt32>{(void *)&ripple::sfHighQualityIn};
    m.attr("sf_high_quality_out") = TWSF<ripple::STUInt32>{(void *)&ripple::sfHighQualityOut};
    m.attr("sf_low_quality_in") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLowQualityIn};
    m.attr("sf_low_quality_out") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLowQualityOut};
    m.attr("sf_quality_in") = TWSF<ripple::STUInt32>{(void *)&ripple::sfQualityIn};
    m.attr("sf_quality_out") = TWSF<ripple::STUInt32>{(void *)&ripple::sfQualityOut};
    m.attr("sf_stamp_escrow") = TWSF<ripple::STUInt32>{(void *)&ripple::sfStampEscrow};
    m.attr("sf_bond_amount") = TWSF<ripple::STUInt32>{(void *)&ripple::sfBondAmount};
    m.attr("sf_load_fee") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLoadFee};
    m.attr("sf_offer_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfOfferSequence};
    m.attr("sf_first_ledger_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfFirstLedgerSequence};
    m.attr("sf_last_ledger_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfLastLedgerSequence};
    m.attr("sf_transaction_index") = TWSF<ripple::STUInt32>{(void *)&ripple::sfTransactionIndex};
    m.attr("sf_operation_limit") = TWSF<ripple::STUInt32>{(void *)&ripple::sfOperationLimit};
    m.attr("sf_reference_fee_units") = TWSF<ripple::STUInt32>{(void *)&ripple::sfReferenceFeeUnits};
    m.attr("sf_reserve_base") = TWSF<ripple::STUInt32>{(void *)&ripple::sfReserveBase};
    m.attr("sf_reserve_increment") = TWSF<ripple::STUInt32>{(void *)&ripple::sfReserveIncrement};
    m.attr("sf_set_flag") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSetFlag};
    m.attr("sf_clear_flag") = TWSF<ripple::STUInt32>{(void *)&ripple::sfClearFlag};
    m.attr("sf_signer_quorum") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSignerQuorum};
    m.attr("sf_cancel_after") = TWSF<ripple::STUInt32>{(void *)&ripple::sfCancelAfter};
    m.attr("sf_finish_after") = TWSF<ripple::STUInt32>{(void *)&ripple::sfFinishAfter};
    m.attr("sf_signer_list_id") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSignerListID};
    m.attr("sf_settle_delay") = TWSF<ripple::STUInt32>{(void *)&ripple::sfSettleDelay};
    m.attr("sf_ticket_count") = TWSF<ripple::STUInt32>{(void *)&ripple::sfTicketCount};
    m.attr("sf_ticket_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfTicketSequence};
    m.attr("sf_nftoken_taxon") = TWSF<ripple::STUInt32>{(void *)&ripple::sfNFTokenTaxon};
    m.attr("sf_minted_nftokens") = TWSF<ripple::STUInt32>{(void *)&ripple::sfMintedNFTokens};
    m.attr("sf_burned_nftokens") = TWSF<ripple::STUInt32>{(void *)&ripple::sfBurnedNFTokens};
    m.attr("sf_hook_state_count") = TWSF<ripple::STUInt32>{(void *)&ripple::sfHookStateCount};
    m.attr("sf_emit_generation") = TWSF<ripple::STUInt32>{(void *)&ripple::sfEmitGeneration};
    m.attr("sf_first_nftoken_sequence") = TWSF<ripple::STUInt32>{(void *)&ripple::sfFirstNFTokenSequence};
    m.attr("sf_index_next") = TWSF<ripple::STUInt64>{(void *)&ripple::sfIndexNext};
    m.attr("sf_index_previous") = TWSF<ripple::STUInt64>{(void *)&ripple::sfIndexPrevious};
    m.attr("sf_book_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfBookNode};
    m.attr("sf_owner_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfOwnerNode};
    m.attr("sf_base_fee") = TWSF<ripple::STUInt64>{(void *)&ripple::sfBaseFee};
    m.attr("sf_exchange_rate") = TWSF<ripple::STUInt64>{(void *)&ripple::sfExchangeRate};
    m.attr("sf_low_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfLowNode};
    m.attr("sf_high_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfHighNode};
    m.attr("sf_destination_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfDestinationNode};
    m.attr("sf_cookie") = TWSF<ripple::STUInt64>{(void *)&ripple::sfCookie};
    m.attr("sf_server_version") = TWSF<ripple::STUInt64>{(void *)&ripple::sfServerVersion};
    m.attr("sf_nftoken_offer_node") = TWSF<ripple::STUInt64>{(void *)&ripple::sfNFTokenOfferNode};
    m.attr("sf_emit_burden") = TWSF<ripple::STUInt64>{(void *)&ripple::sfEmitBurden};
    m.attr("sf_hook_on") = TWSF<ripple::STUInt64>{(void *)&ripple::sfHookOn};
    m.attr("sf_hook_instruction_count") = TWSF<ripple::STUInt64>{(void *)&ripple::sfHookInstructionCount};
    m.attr("sf_hook_return_code") = TWSF<ripple::STUInt64>{(void *)&ripple::sfHookReturnCode};
    m.attr("sf_reference_count") = TWSF<ripple::STUInt64>{(void *)&ripple::sfReferenceCount};
    m.attr("sf_email_hash") = TWSF<ripple::STUInt128>{(void *)&ripple::sfEmailHash};
    m.attr("sf_taker_pays_currency") = TWSF<ripple::STUInt160>{(void *)&ripple::sfTakerPaysCurrency};
    m.attr("sf_taker_pays_issuer") = TWSF<ripple::STUInt160>{(void *)&ripple::sfTakerPaysIssuer};
    m.attr("sf_taker_gets_currency") = TWSF<ripple::STUInt160>{(void *)&ripple::sfTakerGetsCurrency};
    m.attr("sf_taker_gets_issuer") = TWSF<ripple::STUInt160>{(void *)&ripple::sfTakerGetsIssuer};
    m.attr("sf_ledger_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfLedgerHash};
    m.attr("sf_parent_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfParentHash};
    m.attr("sf_transaction_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfTransactionHash};
    m.attr("sf_account_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfAccountHash};
    m.attr("sf_previous_txn_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfPreviousTxnID};
    m.attr("sf_ledger_index") = TWSF<ripple::STUInt256>{(void *)&ripple::sfLedgerIndex};
    m.attr("sf_wallet_locator") = TWSF<ripple::STUInt256>{(void *)&ripple::sfWalletLocator};
    m.attr("sf_root_index") = TWSF<ripple::STUInt256>{(void *)&ripple::sfRootIndex};
    m.attr("sf_account_txn_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfAccountTxnID};
    m.attr("sf_nftoken_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfNFTokenID};
    m.attr("sf_emit_parent_txn_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfEmitParentTxnID};
    m.attr("sf_emit_nonce") = TWSF<ripple::STUInt256>{(void *)&ripple::sfEmitNonce};
    m.attr("sf_emit_hook_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfEmitHookHash};
    m.attr("sf_book_directory") = TWSF<ripple::STUInt256>{(void *)&ripple::sfBookDirectory};
    m.attr("sf_invoice_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfInvoiceID};
    m.attr("sf_nickname") = TWSF<ripple::STUInt256>{(void *)&ripple::sfNickname};
    m.attr("sf_amendment") = TWSF<ripple::STUInt256>{(void *)&ripple::sfAmendment};
    m.attr("sf_digest") = TWSF<ripple::STUInt256>{(void *)&ripple::sfDigest};
    m.attr("sf_channel") = TWSF<ripple::STUInt256>{(void *)&ripple::sfChannel};
    m.attr("sf_consensus_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfConsensusHash};
    m.attr("sf_check_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfCheckID};
    m.attr("sf_validated_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfValidatedHash};
    m.attr("sf_previous_page_min") = TWSF<ripple::STUInt256>{(void *)&ripple::sfPreviousPageMin};
    m.attr("sf_next_page_min") = TWSF<ripple::STUInt256>{(void *)&ripple::sfNextPageMin};
    m.attr("sf_nftoken_buy_offer") = TWSF<ripple::STUInt256>{(void *)&ripple::sfNFTokenBuyOffer};
    m.attr("sf_nftoken_sell_offer") = TWSF<ripple::STUInt256>{(void *)&ripple::sfNFTokenSellOffer};
    m.attr("sf_hook_state_key") = TWSF<ripple::STUInt256>{(void *)&ripple::sfHookStateKey};
    m.attr("sf_hook_hash") = TWSF<ripple::STUInt256>{(void *)&ripple::sfHookHash};
    m.attr("sf_hook_namespace") = TWSF<ripple::STUInt256>{(void *)&ripple::sfHookNamespace};
    m.attr("sf_hook_set_txn_id") = TWSF<ripple::STUInt256>{(void *)&ripple::sfHookSetTxnID};
    m.attr("sf_amount") = TWSF<ripple::STAmount>{(void *)&ripple::sfAmount};
    m.attr("sf_balance") = TWSF<ripple::STAmount>{(void *)&ripple::sfBalance};
    m.attr("sf_limit_amount") = TWSF<ripple::STAmount>{(void *)&ripple::sfLimitAmount};
    m.attr("sf_taker_pays") = TWSF<ripple::STAmount>{(void *)&ripple::sfTakerPays};
    m.attr("sf_taker_gets") = TWSF<ripple::STAmount>{(void *)&ripple::sfTakerGets};
    m.attr("sf_low_limit") = TWSF<ripple::STAmount>{(void *)&ripple::sfLowLimit};
    m.attr("sf_high_limit") = TWSF<ripple::STAmount>{(void *)&ripple::sfHighLimit};
    m.attr("sf_fee") = TWSF<ripple::STAmount>{(void *)&ripple::sfFee};
    m.attr("sf_send_max") = TWSF<ripple::STAmount>{(void *)&ripple::sfSendMax};
    m.attr("sf_deliver_min") = TWSF<ripple::STAmount>{(void *)&ripple::sfDeliverMin};
    m.attr("sf_minimum_offer") = TWSF<ripple::STAmount>{(void *)&ripple::sfMinimumOffer};
    m.attr("sf_ripple_escrow") = TWSF<ripple::STAmount>{(void *)&ripple::sfRippleEscrow};
    m.attr("sf_delivered_amount") = TWSF<ripple::STAmount>{(void *)&ripple::sfDeliveredAmount};
    m.attr("sf_nftoken_broker_fee") = TWSF<ripple::STAmount>{(void *)&ripple::sfNFTokenBrokerFee};
    m.attr("sf_base_fee_drops") = TWSF<ripple::STAmount>{(void *)&ripple::sfBaseFeeDrops};
    m.attr("sf_reserve_base_drops") = TWSF<ripple::STAmount>{(void *)&ripple::sfReserveBaseDrops};
    m.attr("sf_reserve_increment_drops") = TWSF<ripple::STAmount>{(void *)&ripple::sfReserveIncrementDrops};
    m.attr("sf_public_key") = TWSF<ripple::STBlob>{(void *)&ripple::sfPublicKey};
    m.attr("sf_message_key") = TWSF<ripple::STBlob>{(void *)&ripple::sfMessageKey};
    m.attr("sf_signing_pub_key") = TWSF<ripple::STBlob>{(void *)&ripple::sfSigningPubKey};
    m.attr("sf_txn_signature") = TWSF<ripple::STBlob>{(void *)&ripple::sfTxnSignature};
    m.attr("sf_uri") = TWSF<ripple::STBlob>{(void *)&ripple::sfURI};
    m.attr("sf_signature") = TWSF<ripple::STBlob>{(void *)&ripple::sfSignature};
    m.attr("sf_domain") = TWSF<ripple::STBlob>{(void *)&ripple::sfDomain};
    m.attr("sf_fund_code") = TWSF<ripple::STBlob>{(void *)&ripple::sfFundCode};
    m.attr("sf_remove_code") = TWSF<ripple::STBlob>{(void *)&ripple::sfRemoveCode};
    m.attr("sf_expire_code") = TWSF<ripple::STBlob>{(void *)&ripple::sfExpireCode};
    m.attr("sf_create_code") = TWSF<ripple::STBlob>{(void *)&ripple::sfCreateCode};
    m.attr("sf_memo_type") = TWSF<ripple::STBlob>{(void *)&ripple::sfMemoType};
    m.attr("sf_memo_data") = TWSF<ripple::STBlob>{(void *)&ripple::sfMemoData};
    m.attr("sf_memo_format") = TWSF<ripple::STBlob>{(void *)&ripple::sfMemoFormat};
    m.attr("sf_fulfillment") = TWSF<ripple::STBlob>{(void *)&ripple::sfFulfillment};
    m.attr("sf_condition") = TWSF<ripple::STBlob>{(void *)&ripple::sfCondition};
    m.attr("sf_master_signature") = TWSF<ripple::STBlob>{(void *)&ripple::sfMasterSignature};
    m.attr("sf_unl_modify_validator") = TWSF<ripple::STBlob>{(void *)&ripple::sfUNLModifyValidator};
    m.attr("sf_validator_to_disable") = TWSF<ripple::STBlob>{(void *)&ripple::sfValidatorToDisable};
    m.attr("sf_validator_to_re_enable") = TWSF<ripple::STBlob>{(void *)&ripple::sfValidatorToReEnable};
    m.attr("sf_hook_state_data") = TWSF<ripple::STBlob>{(void *)&ripple::sfHookStateData};
    m.attr("sf_hook_return_string") = TWSF<ripple::STBlob>{(void *)&ripple::sfHookReturnString};
    m.attr("sf_hook_parameter_name") = TWSF<ripple::STBlob>{(void *)&ripple::sfHookParameterName};
    m.attr("sf_hook_parameter_value") = TWSF<ripple::STBlob>{(void *)&ripple::sfHookParameterValue};
    m.attr("sf_account") = TWSF<ripple::STAccount>{(void *)&ripple::sfAccount};
    m.attr("sf_owner") = TWSF<ripple::STAccount>{(void *)&ripple::sfOwner};
    m.attr("sf_destination") = TWSF<ripple::STAccount>{(void *)&ripple::sfDestination};
    m.attr("sf_issuer") = TWSF<ripple::STAccount>{(void *)&ripple::sfIssuer};
    m.attr("sf_authorize") = TWSF<ripple::STAccount>{(void *)&ripple::sfAuthorize};
    m.attr("sf_unauthorize") = TWSF<ripple::STAccount>{(void *)&ripple::sfUnauthorize};
    m.attr("sf_regular_key") = TWSF<ripple::STAccount>{(void *)&ripple::sfRegularKey};
    m.attr("sf_nftoken_minter") = TWSF<ripple::STAccount>{(void *)&ripple::sfNFTokenMinter};
    m.attr("sf_emit_callback") = TWSF<ripple::STAccount>{(void *)&ripple::sfEmitCallback};
    m.attr("sf_hook_account") = TWSF<ripple::STAccount>{(void *)&ripple::sfHookAccount};
    m.attr("sf_paths") = WSF{(void *)&ripple::sfPaths};
    m.attr("sf_indexes") = TWSF<ripple::STVector256>{(void *)&ripple::sfIndexes};
    m.attr("sf_hashes") = TWSF<ripple::STVector256>{(void *)&ripple::sfHashes};
    m.attr("sf_amendments") = TWSF<ripple::STVector256>{(void *)&ripple::sfAmendments};
    m.attr("sf_nftoken_offers") = TWSF<ripple::STVector256>{(void *)&ripple::sfNFTokenOffers};
    m.attr("sf_transaction_meta_data") = WSF{(void *)&ripple::sfTransactionMetaData};
    m.attr("sf_created_node") = WSF{(void *)&ripple::sfCreatedNode};
    m.attr("sf_deleted_node") = WSF{(void *)&ripple::sfDeletedNode};
    m.attr("sf_modified_node") = WSF{(void *)&ripple::sfModifiedNode};
    m.attr("sf_previous_fields") = WSF{(void *)&ripple::sfPreviousFields};
    m.attr("sf_final_fields") = WSF{(void *)&ripple::sfFinalFields};
    m.attr("sf_new_fields") = WSF{(void *)&ripple::sfNewFields};
    m.attr("sf_template_entry") = WSF{(void *)&ripple::sfTemplateEntry};
    m.attr("sf_memo") = WSF{(void *)&ripple::sfMemo};
    m.attr("sf_signer_entry") = WSF{(void *)&ripple::sfSignerEntry};
    m.attr("sf_nftoken") = WSF{(void *)&ripple::sfNFToken};
    m.attr("sf_emit_details") = WSF{(void *)&ripple::sfEmitDetails};
    m.attr("sf_hook") = WSF{(void *)&ripple::sfHook};
    m.attr("sf_signer") = WSF{(void *)&ripple::sfSigner};
    m.attr("sf_majority") = WSF{(void *)&ripple::sfMajority};
    m.attr("sf_disabled_validator") = WSF{(void *)&ripple::sfDisabledValidator};
    m.attr("sf_emitted_txn") = WSF{(void *)&ripple::sfEmittedTxn};
    m.attr("sf_hook_execution") = WSF{(void *)&ripple::sfHookExecution};
    m.attr("sf_hook_definition") = WSF{(void *)&ripple::sfHookDefinition};
    m.attr("sf_hook_parameter") = WSF{(void *)&ripple::sfHookParameter};
    m.attr("sf_hook_grant") = WSF{(void *)&ripple::sfHookGrant};
    m.attr("sf_signers") = WSF{(void *)&ripple::sfSigners};
    m.attr("sf_signer_entries") = WSF{(void *)&ripple::sfSignerEntries};
    m.attr("sf_template") = WSF{(void *)&ripple::sfTemplate};
    m.attr("sf_necessary") = WSF{(void *)&ripple::sfNecessary};
    m.attr("sf_sufficient") = WSF{(void *)&ripple::sfSufficient};
    m.attr("sf_affected_nodes") = WSF{(void *)&ripple::sfAffectedNodes};
    m.attr("sf_memos") = WSF{(void *)&ripple::sfMemos};
    m.attr("sf_nftokens") = WSF{(void *)&ripple::sfNFTokens};
    m.attr("sf_hooks") = WSF{(void *)&ripple::sfHooks};
    m.attr("sf_majorities") = WSF{(void *)&ripple::sfMajorities};
    m.attr("sf_disabled_validators") = WSF{(void *)&ripple::sfDisabledValidators};
    m.attr("sf_hook_executions") = WSF{(void *)&ripple::sfHookExecutions};
    m.attr("sf_hook_parameters") = WSF{(void *)&ripple::sfHookParameters};
    m.attr("sf_hook_grants") = WSF{(void *)&ripple::sfHookGrants};

    // Amendments
    m.attr("fixMasterKeyAsRegularKey") = ripple::fixMasterKeyAsRegularKey;
    m.attr("fix1543") = ripple::fix1543;
    m.attr("fix1571") = ripple::fix1571;

    // Other
    m.attr("zero_amount") = beast::zero;
}
