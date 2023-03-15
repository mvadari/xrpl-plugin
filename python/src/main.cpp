#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include <ripple/app/tx/impl/Transactor.h>
#include <ripple/protocol/st.h>
#include <ripple/protocol/TxFlags.h>
#include <ripple/protocol/Feature.h>
#include <map>
#include <string>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

// std::map <std::string, ripple::SF_ACCOUNT> accountSFields;
// accountSFields["sfAccount"] = ripple::sfAccount;

namespace py = pybind11;

PYBIND11_MODULE(plugin_transactor, m) {

    /*
    * Enums
    */

    py::enum_<ripple::TxType>(m, "TxType")
        .value("ttPAYMENT", ripple::TxType::ttPAYMENT)
        .value("ttESCROW_CREATE", ripple::TxType::ttESCROW_CREATE)
        .value("ttESCROW_FINISH", ripple::TxType::ttESCROW_FINISH)
        .value("ttACCOUNT_SET", ripple::TxType::ttACCOUNT_SET)
        .value("ttESCROW_CANCEL", ripple::TxType::ttESCROW_CANCEL)
        .value("ttREGULAR_KEY_SET", ripple::TxType::ttREGULAR_KEY_SET)
        .value("ttOFFER_CREATE", ripple::TxType::ttOFFER_CREATE)
        .value("ttOFFER_CANCEL", ripple::TxType::ttOFFER_CANCEL)
        .value("ttTICKET_CREATE", ripple::TxType::ttTICKET_CREATE)
        .value("ttSIGNER_LIST_SET", ripple::TxType::ttSIGNER_LIST_SET)
        .value("ttPAYCHAN_CREATE", ripple::TxType::ttPAYCHAN_CREATE)
        .value("ttPAYCHAN_FUND", ripple::TxType::ttPAYCHAN_FUND)
        .value("ttPAYCHAN_CLAIM", ripple::TxType::ttPAYCHAN_CLAIM)
        .value("ttCHECK_CREATE", ripple::TxType::ttCHECK_CREATE)
        .value("ttCHECK_CASH", ripple::TxType::ttCHECK_CASH)
        .value("ttCHECK_CANCEL", ripple::TxType::ttCHECK_CANCEL)
        .value("ttDEPOSIT_PREAUTH", ripple::TxType::ttDEPOSIT_PREAUTH)
        .value("ttTRUST_SET", ripple::TxType::ttTRUST_SET)
        .value("ttACCOUNT_DELETE", ripple::TxType::ttACCOUNT_DELETE)
        .value("ttHOOK_SET", ripple::TxType::ttHOOK_SET)
        .value("ttNFTOKEN_MINT", ripple::TxType::ttNFTOKEN_MINT)
        .value("ttNFTOKEN_BURN", ripple::TxType::ttNFTOKEN_BURN)
        .value("ttNFTOKEN_CREATE_OFFER", ripple::TxType::ttNFTOKEN_CREATE_OFFER)
        .value("ttNFTOKEN_CANCEL_OFFER", ripple::TxType::ttNFTOKEN_CANCEL_OFFER)
        .value("ttNFTOKEN_ACCEPT_OFFER", ripple::TxType::ttNFTOKEN_ACCEPT_OFFER)
        .value("ttDUMMY_TX", ripple::TxType::ttDUMMY_TX)
        .value("ttAMENDMENT", ripple::TxType::ttAMENDMENT)
        .value("ttFEE", ripple::TxType::ttFEE)
        .value("ttUNL_MODIFY", ripple::TxType::ttUNL_MODIFY)
        .export_values();
    
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
        .value("lsfDisallowIncomingNFTOffer", ripple::LedgerSpecificFlags::lsfDisallowIncomingNFTOffer)
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
    
    /*
    * Small classes
    */

    py::class_<ripple::SField> SField(m, "SField");

    py::class_<ripple::STAccount> STAccount(m, "STAccount");

    py::class_<ripple::AccountID> AccountID(m, "AccountID");
    AccountID
        .def("toBase58",
            [](const ripple::AccountID &a) {
                return ripple::toBase58(a);
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
        );
    
    py::class_<ripple::uint256> uint256(m, "uint256");
    uint256
        .def("__repr__",
            [](const ripple::uint256 &uint256) {
                return ripple::to_string(uint256);
            }
        );

    /*
    * STObjects
    */

    py::class_<ripple::STObject, std::shared_ptr<ripple::STObject>> STObject(m, "STObject");
    STObject
        .def("isFlag", &ripple::STObject::isFlag)
        .def_property_readonly("Account",
            [](const ripple::STObject &obj) {
                return obj[ripple::sfAccount];
            }
        )
        .def("isFieldPresent_sfRegularKey",
            [](const ripple::STObject &obj) {
                return obj.isFieldPresent(ripple::sfRegularKey);
            }
        )
        .def("getAccountID_sfAccount",
            [](const ripple::STObject &obj) {
                return obj.getAccountID(ripple::sfAccount);
            }
        )
        .def("getAccountID_sfRegularKey",
            [](const ripple::STObject &obj) {
                return obj.getAccountID(ripple::sfRegularKey);
            }
        )
        .def("setAccountID_sfRegularKey",
            [](ripple::STObject &obj, const ripple::AccountID accountID) {
                return obj.setAccountID(ripple::sfRegularKey, accountID);
            }
        )
        .def("makeFieldAbsent_sfRegularKey",
            [](ripple::STObject &obj) {
                return obj.makeFieldAbsent(ripple::sfRegularKey);
            }
        )
        // .def("at",
        //     [](const ripple::STObject &obj, const std::string fieldName) {
        //         if (auto it = accountSFields.find(fieldName); it != accountSFields.end())
        //         {
        //             auto const sField = it->second;
        //             return obj[sField];
        //         }
        //         ripple::Throw<std::logic_error>("No SField " + fieldName + ".");
        //     }
        // )
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__repr__",
            [](const ripple::STObject &obj) {
                return obj.getFullText();
            }
        );

    py::class_<ripple::STTx, ripple::STObject, std::shared_ptr<ripple::STTx>> STTx(m, "STTx");
    STTx
        .def("getTxnType", &ripple::STTx::getTxnType)
        .def("getFlags", &ripple::STTx::getFlags);

    py::class_<ripple::STLedgerEntry, ripple::STObject, std::shared_ptr<ripple::STLedgerEntry>> STLedgerEntry(m, "STLedgerEntry");

    /*
    * Contexts and classes that the contexts depend on
    */

    py::class_<ripple::Rules> Rules(m, "Rules");
    Rules
        .def("enabled", &ripple::Rules::enabled);
    
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
        );
    
    py::class_<ripple::OpenView, ripple::ReadView> OpenView(m, "OpenView");
    
    py::class_<ripple::PreflightContext> PreflightContext(m, "PreflightContext");
    PreflightContext
        .def_property_readonly("tx",
            [](const ripple::PreflightContext &ctx) {
                return ctx.tx;
            }
        )
        .def_property_readonly("rules",
            [](const ripple::PreflightContext &ctx) {
                return ctx.rules;
            }
        );
    
    py::class_<ripple::PreclaimContext> PreclaimContext(m, "PreclaimContext");
    PreclaimContext
        .def_property_readonly("tx",
            [](const ripple::PreclaimContext &ctx) {
                return ctx.tx;
            }
        );
        // .def_property_readonly("view",
        //     [](const ripple::PreclaimContext &ctx) {
        //         return std::shared_ptr<const ripple::ReadView>(ctx.view);
        //     }
        // );
    
    py::class_<ripple::Keylet> Keylet(m, "Keylet");
    Keylet
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
        );
    
    py::class_<ripple::ApplyView> ApplyView(m, "ApplyView");
    ApplyView
        .def("peek", &ripple::ApplyView::peek)
        .def("update", &ripple::ApplyView::update);
    
    py::class_<
        ripple::detail::ApplyViewBase,
        ripple::ApplyView
    > ApplyViewBase(m, "ApplyViewBase");
    py::class_<
        ripple::ApplyViewImpl,
        ripple::detail::ApplyViewBase
    > ApplyViewImpl(m, "ApplyViewImpl");

    py::class_<ripple::ApplyContext> ApplyContext(m, "ApplyContext");
    ApplyContext
        .def_property_readonly("tx",
            [](const ripple::ApplyContext &ctx) {
                return ctx.tx;
            }
        )
        .def("view", &ripple::ApplyContext::view, py::return_value_policy::reference);
    
    // py::register_exception<ripple::LogicError>(m, "LogicError");
    
    /*
    * Misc additional stuff
    */

    m
        .def("accountKeylet", &ripple::keylet::account)
        .def("signersKeylet", &ripple::keylet::signers)
        .def("preflight1", &ripple::preflight1)
        .def("preflight2", &ripple::preflight2)
        .def("isTesSuccess", &ripple::isTesSuccess)
        ;
    

    m.attr("tfFullyCanonicalSig") = ripple::tfFullyCanonicalSig;
    m.attr("tfUniversal") = ripple::tfUniversal;
    m.attr("tfUniversalMask") = ripple::tfUniversalMask;
    // m.attr("sfRegularKey") = &ripple::sfRegularKey;
    // m.attr("sfAccount") = &ripple::sfAccount;
    m.attr("fixMasterKeyAsRegularKey") = ripple::fixMasterKeyAsRegularKey;
}
