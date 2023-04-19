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

std::unique_ptr<std::string>
base64_decode_ptr(std::string const& data);

std::unique_ptr<ripple::NotTEC>
from_tefcodes(ripple::TEFcodes code);

std::unique_ptr<ripple::STTx> tx_ptr(ripple::PreflightContext const& ctx);

#endif //PLUGIN_TRANSACTOR_BLOBSTORE_H
