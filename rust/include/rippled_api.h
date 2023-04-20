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

ripple::STTx const &get_tx(ripple::PreflightContext const &ctx);
ripple::Rules const &get_rules(ripple::PreflightContext const &ctx);

ripple::STTx const &get_dummy_sttx();

ripple::uint256 const &fixMasterKeyAsRegularKey();

ripple::PreflightContext const &get_dummy_ctx();

//ripple::uint256 const &foo(ripple::uint256 const* bar);

//void toAccountIdVec(rust::Vec<u32> v);

#endif //PLUGIN_TRANSACTOR_BLOBSTORE_H
