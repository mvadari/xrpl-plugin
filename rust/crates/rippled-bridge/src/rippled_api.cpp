//
// Created by Noah Kramer on 4/17/23.
//

#include "rippled-bridge/include/rippled_api.h"
#include "ripple/basics/base64.h"
#include "rippled-bridge/src/lib.rs.h"
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

ripple::XRPAmount defaultCalculateBaseFee(ripple::ReadView const& view, ripple::STTx const& tx) {
    return ripple::Transactor::calculateBaseFee(view, tx);
}

ripple::XRPAmount minimumFee(
        ripple::Application& app,
        ripple::XRPAmount baseFee,
        ripple::Fees const& fees,
        ripple::ApplyFlags flags
) {
    return ripple::Transactor::minimumFee(app, baseFee, fees, flags);
}

bool setFlag(
        std::shared_ptr<ripple::SLE>const & sle,
        std::uint32_t f) {
    return sle->setFlag(f);
}

void setAccountID(
        std::shared_ptr<ripple::SLE>const & sle,
        ripple::SField const& field,
        ripple::AccountID const& v
) {
    sle->setAccountID(field, v);
}

void makeFieldAbsent(
        std::shared_ptr<ripple::SLE>const & sle,
ripple::SField const& field
) {
    sle->makeFieldAbsent(field);
}

/*std::int32_t preflight1(ripple::PreflightContext const& ctx) {
    return ripple::preflight1(ctx).getUnderlyingCode();
}

std::int32_t preflight2(ripple::PreflightContext const& ctx) {
    return ripple::preflight2(ctx).getUnderlyingCode();
}*/

std::unique_ptr<std::string> toBase58(const ripple::AccountID& accountId) {
    std::cout << "Size: " << sizeof(ripple::NotTEC) << " Alignement: " << alignof(ripple::NotTEC) << std::endl;
    return std::make_unique<std::string>(ripple::toBase58(accountId));
}

namespace ripple {

    /*extern "C" {
        NotTEC
        preflight(PreflightContext const &ctx) {
            return *pre_flight(ctx);
        }
    }*/

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