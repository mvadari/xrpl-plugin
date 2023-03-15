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

std::map <std::string, ripple::SF_ACCOUNT> accountSFields;
accountSFields["sfAccount"] = ripple::sfAccount;

namespace py = pybind11;

PYBIND11_MODULE(plugin_transactor, m) {

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
    
    py::enum_<ripple::TEScodes>(m, "TEScodes")
        .value("tesSUCCESS", ripple::TEScodes::tesSUCCESS)
        .export_values();
    
    py::enum_<ripple::TEMcodes>(m, "TEMcodes")
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
    
    py::class_<ripple::NotTEC> NotTEC(m, "NotTEC");
    NotTEC
        .def(py::init<ripple::TEScodes>())
        .def(py::init<ripple::TEMcodes>())
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::implicitly_convertible<ripple::TEScodes, ripple::NotTEC>();
    py::implicitly_convertible<ripple::TEMcodes, ripple::NotTEC>();

    py::class_<ripple::TER> TER(m, "TER");
    TER
        .def(py::init<ripple::TEScodes>())
        .def(py::init<ripple::TEMcodes>())
        .def(py::init<ripple::NotTEC>())
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::implicitly_convertible<ripple::TEScodes, ripple::TER>();
    py::implicitly_convertible<ripple::TEMcodes, ripple::TER>();
    py::implicitly_convertible<ripple::NotTEC, ripple::TER>();
    

    py::class_<ripple::SField> SField(m, "SField");

    py::class_<ripple::STAccount> STAccount(m, "STAccount");

    py::class_<ripple::AccountID> AccountID(m, "AccountID");
    AccountID
        .def("toBase58",
            [](const ripple::AccountID &a) {
                return ripple::toBase58(a);
            }
        )
        .def("__repr__",
            [](const ripple::AccountID &a) {
                return "<AccountID:'" + ripple::toBase58(a) + "'>";
            }
        );

    py::class_<ripple::STObject, std::shared_ptr<ripple::STObject>> STObject(m, "STObject");
    STObject
        .def_property_readonly("Account",
            [](const ripple::STObject &obj) {
                return obj[ripple::sfAccount];
            }
        )
        .def("isFieldPresent", &ripple::STObject::isFieldPresent)
        .def("getAccountID", &ripple::STObject::getAccountID)
        .def("setRegularKey",
            [](ripple::STObject &obj) {
                auto const accountID = obj[ripple::sfAccount];
                obj.setAccountID(ripple::sfRegularKey, accountID);
            }
        )
        .def("at",
            [](const ripple::STObject &obj, const std::string fieldName) {
                if (auto it = accountSFields.find(fieldName); it != accountSFields.end())
                {
                    auto const sField = it->second;
                    return obj[sField];
                }
                ripple::Throw<std::logic_error>("No SField " + fieldName + ".");
            }
        )
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
    
    py::class_<ripple::XRPAmount> XRPAmount(m, "XRPAmount");
    XRPAmount
        .def(py::init<std::int64_t &>())
        .def_property_readonly("drops",
            [](const ripple::XRPAmount &xrpAmount) {
                return xrpAmount.drops();
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
    
    m
        .def("accountKeylet", &ripple::keylet::account)
        .def("preflight1", &ripple::preflight1)
        .def("preflight2", &ripple::preflight2)
        .def("isTesSuccess", &ripple::isTesSuccess)
        ;
    

    m.attr("tfFullyCanonicalSig") = ripple::tfFullyCanonicalSig;
    m.attr("tfUniversal") = ripple::tfUniversal;
    m.attr("tfUniversalMask") = ripple::tfUniversalMask;
    // m.attr("sfRegularKey") = &ripple::sfRegularKey;
    // m.attr("sfAccount") = &ripple::sfAccount;
    // m.attr("fixMasterKeyAsRegularKey") = ripple::fixMasterKeyAsRegularKey;
    // m.attr("lsfPasswordSpent") = ripple::lsfPasswordSpent;
    // m.attr("lsfDisableMaster") = ripple::lsfDisableMaster;
}
