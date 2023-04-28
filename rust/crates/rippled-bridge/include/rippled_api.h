//
// Created by Noah Kramer on 4/17/23.
//

#ifndef PLUGIN_TRANSACTOR_BLOBSTORE_H
#define PLUGIN_TRANSACTOR_BLOBSTORE_H

#pragma once

#include <memory>
#include "ripple/basics/base64.h"
#include <ripple/app/tx/impl/Transactor.h>
#include <ripple/protocol/st.h>
#include <ripple/protocol/TxFlags.h>
#include <ripple/protocol/Feature.h>

std::unique_ptr <std::string>
base64_decode_ptr(std::string const &data);

std::unique_ptr <ripple::NotTEC>
from_tefcodes(ripple::TEFcodes code);

std::unique_ptr <ripple::STTx> tx_ptr(ripple::PreflightContext const &ctx);

// Return the XRP Issuer as an AccountID
ripple::AccountID const &xrp_account();

ripple::STTx const &get_dummy_sttx();

ripple::uint256 const &fixMasterKeyAsRegularKey();

ripple::PreflightContext const &get_dummy_ctx();

ripple::XRPAmount defaultCalculateBaseFee(ripple::ReadView const& view, ripple::STTx const& tx);

ripple::XRPAmount minimumFee(
        ripple::Application& app,
        ripple::XRPAmount baseFee,
        ripple::Fees const& fees,
        ripple::ApplyFlags flags
        );

bool setFlag(
        std::shared_ptr<ripple::SLE>const & sle,
        std::uint32_t f);

void setAccountID(
        std::shared_ptr<ripple::SLE>const & sle,
        ripple::SField const& field,
                ripple::AccountID const& v
);

void makeFieldAbsent(
        std::shared_ptr<ripple::SLE>const & sle,
        ripple::SField const& field
        );

/*std::int32_t preflight1(ripple::PreflightContext const& ctx);
std::int32_t preflight2(ripple::PreflightContext const& ctx);*/

inline const ripple::STObject & upcast(const ripple::STTx &stTx) {
    return stTx;
}

constexpr std::uint32_t tfUniversalMask() {
    return ripple::tfUniversalMask;
}

constexpr ripple::SField const& sfRegularKey() {
    return ripple::sfRegularKey;
}

constexpr ripple::SField const& sfAccount() {
    return ripple::sfAccount;
}

std::unique_ptr<std::string> toBase58(const ripple::AccountID& accountId);

namespace ripple {

    class RustDummyTx : public Transactor
    {
    public:
        static constexpr ConsequencesFactoryType ConsequencesFactory{Normal};

        explicit RustDummyTx(ApplyContext& ctx) : Transactor(ctx)
        {
        }

        static NotTEC
        preflight(PreflightContext const& ctx);

        static TER
        preclaim(PreclaimContext const& ctx);

        TER
        doApply() override;
    };

//------------------------------------------------------------------------------

}

//ripple::uint256 const &foo(ripple::uint256 const* bar);

//void toAccountIdVec(rust::Vec<u32> v);

#endif //PLUGIN_TRANSACTOR_BLOBSTORE_H
