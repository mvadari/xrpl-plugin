#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include <ripple/app/tx/impl/Transactor.h>
#include <ripple/basics/Log.h>
#include <ripple/basics/XRPAmount.h>
#include <ripple/ledger/ApplyView.h>
#include <ripple/ledger/View.h>
#include <ripple/protocol/Feature.h>
#include <ripple/protocol/TER.h>
#include <ripple/protocol/TxFlags.h>
#include <ripple/protocol/st.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

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

    py::class_<ripple::STObject> STObject(m, "STObject");
    STObject
        .def("getAccount",
            [](const ripple::STObject &obj) {
                return obj[ripple::sfAccount];
            }
        );

    py::class_<ripple::STTx, ripple::STObject> STTx(m, "STTx");
    STTx
        .def("getTxnType", &ripple::STTx::getTxnType);
}
