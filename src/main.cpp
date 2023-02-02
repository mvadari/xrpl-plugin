#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

// #include <ripple/app/tx/impl/Transactor.h>
// #include <ripple/protocol/st.h>
#include "main.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

Class::Class() {

}

Class2::Class2() {

}

// const ripple::SF_ACCOUNT* SF::sfAccount = &ripple::sfAccount;
// Class c = new Class();
Class* c1 = new Class();
const Class* Class2::var = c1;
Class2* c = new Class2();


PYBIND11_MODULE(plugin_transactor, m) {
    // py::enum_<ripple::TxType>(m, "TxType")
    //     .value("ttPAYMENT", ripple::TxType::ttPAYMENT)
    //     .value("ttESCROW_CREATE", ripple::TxType::ttESCROW_CREATE)
    //     .value("ttESCROW_FINISH", ripple::TxType::ttESCROW_FINISH)
    //     .value("ttACCOUNT_SET", ripple::TxType::ttACCOUNT_SET)
    //     .value("ttESCROW_CANCEL", ripple::TxType::ttESCROW_CANCEL)
    //     .value("ttREGULAR_KEY_SET", ripple::TxType::ttREGULAR_KEY_SET)
    //     .value("ttOFFER_CREATE", ripple::TxType::ttOFFER_CREATE)
    //     .value("ttOFFER_CANCEL", ripple::TxType::ttOFFER_CANCEL)
    //     .value("ttTICKET_CREATE", ripple::TxType::ttTICKET_CREATE)
    //     .value("ttSIGNER_LIST_SET", ripple::TxType::ttSIGNER_LIST_SET)
    //     .value("ttPAYCHAN_CREATE", ripple::TxType::ttPAYCHAN_CREATE)
    //     .value("ttPAYCHAN_FUND", ripple::TxType::ttPAYCHAN_FUND)
    //     .value("ttPAYCHAN_CLAIM", ripple::TxType::ttPAYCHAN_CLAIM)
    //     .value("ttCHECK_CREATE", ripple::TxType::ttCHECK_CREATE)
    //     .value("ttCHECK_CASH", ripple::TxType::ttCHECK_CASH)
    //     .value("ttCHECK_CANCEL", ripple::TxType::ttCHECK_CANCEL)
    //     .value("ttDEPOSIT_PREAUTH", ripple::TxType::ttDEPOSIT_PREAUTH)
    //     .value("ttTRUST_SET", ripple::TxType::ttTRUST_SET)
    //     .value("ttACCOUNT_DELETE", ripple::TxType::ttACCOUNT_DELETE)
    //     .value("ttHOOK_SET", ripple::TxType::ttHOOK_SET)
    //     .value("ttNFTOKEN_MINT", ripple::TxType::ttNFTOKEN_MINT)
    //     .value("ttNFTOKEN_BURN", ripple::TxType::ttNFTOKEN_BURN)
    //     .value("ttNFTOKEN_CREATE_OFFER", ripple::TxType::ttNFTOKEN_CREATE_OFFER)
    //     .value("ttNFTOKEN_CANCEL_OFFER", ripple::TxType::ttNFTOKEN_CANCEL_OFFER)
    //     .value("ttNFTOKEN_ACCEPT_OFFER", ripple::TxType::ttNFTOKEN_ACCEPT_OFFER)
    //     .value("ttDUMMY_TX", ripple::TxType::ttDUMMY_TX)
    //     .value("ttAMENDMENT", ripple::TxType::ttAMENDMENT)
    //     .value("ttFEE", ripple::TxType::ttFEE)
    //     .value("ttUNL_MODIFY", ripple::TxType::ttUNL_MODIFY)
    //     .export_values();

    // py::class_<ripple::SField> SField(m, "SField");

    // py::class_<ripple::STAccount> STAccount(m, "STAccount");
    // // STAccount
    // //     .def(py::init<>());
    
    // // py::class_<
    // //     ripple::TypedField<ripple::STAccount>,
    // //     std::shared_ptr< ripple::TypedField<ripple::STAccount> >,
    // //     ripple::SField
    // // >(m, "TypedFieldSTAccount");
    // //  py::class_<
    // //     ripple::TypedField<ripple::STBlob>,
    // //     std::shared_ptr< ripple::TypedField<ripple::STBlob> >,
    // //     ripple::SField
    // // >(m, "TypedFieldSTBlob");

    // // py::class_<
    // //     ripple::OptionaledField<ripple::STAccount>,
    // //     std::shared_ptr< ripple::OptionaledField<ripple::STAccount> >,
    // //     ripple::SField
    // // >(m, "OptionaledFieldSTAccount");
    // //  py::class_<
    // //     ripple::OptionaledField<ripple::STBlob>,
    // //     std::shared_ptr< ripple::OptionaledField<ripple::STBlob> >,
    // //     ripple::SField
    // // >(m, "OptionaledFieldSTBlob");

    // py::class_<ripple::STObject> STObject(m, "STObject");
    // STObject
    //     .def(
    //         "at",
    //         (
    //             typename ripple::STAccount::value_type // return type
    //             (ripple::STObject::*) // class pointer?
    //             (const ripple::TypedField<ripple::STAccount> &) // param type
    //             const // const
    //         )
    //         &ripple::STObject::at, // function name
    //         py::arg("f") // arg name
    //     );

    // py::class_<ripple::STTx, ripple::STObject> STTx(m, "STTx");
    // STTx
    //     .def("getTxnType", &ripple::STTx::getTxnType);
    
    // // template<typename T>
    // // void declare_at(std::string &typestr) {
    // //     using TF = ripple::TypedField<T>;
    // //     std::string pyclass_name = std::string("TypedField") + typestr;
    // //     STTx
    // //         .def(
    // //             "at",
    // //             py::overload_cast<TF>(&ripple::STTx::at)
    // //         )
    // // }

    // // declare_at<ripple::STAccount>("STAccount");
    // // declare_at<ripple::STBlob>("STBlob");

    // // py::class_<SF> SF(m, "SF");
    // // SF
    // //     .def_readonly_static("sfAccount", &ripple::sfAccount, py::return_value_policy::reference_internal);


    py::class_<Class> Class(m, "Class");
    py::class_<Class2> Class2(m, "Class2");
    Class2
        .def_readonly_static("var", &Class2::var, py::return_value_policy::reference);
}
