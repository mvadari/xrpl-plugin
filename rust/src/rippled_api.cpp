//
// Created by Noah Kramer on 4/17/23.
//

#include "rust/include/rippled_api.h"
#include "ripple/basics/base64.h"
#include "rust/src/main.rs.h"
#include <functional>
#include <string>

std::unique_ptr <std::string>
base64_decode_ptr(std::string const &data) {
    return std::make_unique<std::string>(ripple::base64_decode(data));
}

std::unique_ptr <ripple::NotTEC>
from_tefcodes(ripple::TEFcodes code) {
    return std::make_unique<ripple::NotTEC>(ripple::NotTEC(code));
}

std::unique_ptr <ripple::STTx> tx_ptr(ripple::PreflightContext const &ctx) {
    return std::make_unique<ripple::STTx>(ctx.tx);
}

// Test function to simulate the creation of an AccountID.
ripple::AccountID const &xrp_account() {
    return ripple::xrpAccount();
}

ripple::STTx const &get_tx(ripple::PreflightContext const &ctx) {
    return ctx.tx;
}

ripple::Rules const &get_rules(ripple::PreflightContext const &ctx) {
    return ctx.rules;
}

ripple::uint256 const &fixMasterKeyAsRegularKey() {
    return ripple::fixMasterKeyAsRegularKey;
}

ripple::PreflightContext const &get_dummy_ctx() {
    // TODO: Return a PreflightContext where .rules has fixMasterKeyAsRegularKey enabled
}

ripple::STTx const &get_dummy_sttx() {
    ripple::STTx tx = ripple::STTx(ripple::TxType::ttPAYMENT, [](ripple::STObject& obj) {
        obj.setFieldU16(ripple::sfTransactionType, ripple::TxType::ttPAYMENT);
    });

    return tx;
}

// Test function to simulate turning an AccountID into a Rust AccountId.

// Send in a C++ Vec of u32s, get back a std::vector
//void toAccountIdVec(rust::Vec<u32> v) {
////    for (auto shared : v) {
////        std::cout << shared.v << std::endl;
////    }
//
//    // Copy the elements to a C++ std::vector using STL algorithm.
//    std::vector<Shared> stdv;
//    std::copy(v.begin(), v.end(), std::back_inserter(stdv));
//    assert(v.size() == stdv.size());
//}