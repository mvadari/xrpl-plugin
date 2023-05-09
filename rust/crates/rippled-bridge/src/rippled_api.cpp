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

std::unique_ptr<std::string> toBase58(const ripple::AccountID& accountId) {
    std::cout << "Size: " << sizeof(ripple::NotTEC) << " Alignement: " << alignof(ripple::NotTEC) << std::endl;
    return std::make_unique<std::string>(ripple::toBase58(accountId));
}

void push_soelement(int field_code, ripple::SOEStyle style, std::vector<ripple::FakeSOElement>& vec) {
    vec.push_back({field_code, style});
}

void push_stype_export(int tid, ParseLeafTypeFnPtr parseLeafTypeFn, STypeFromSITFnPtr sTypeFromSitFnPtr, STypeFromSFieldFnPtr sTypeFromSFieldFnPtr, std::vector<STypeExport>& vec) {
    vec.push_back({tid, parseLeafTypeFn, sTypeFromSitFnPtr, sTypeFromSFieldFnPtr});
}

void push_sfield_info(int tid, int fv, const char * txt_name, std::vector<ripple::SFieldInfo>& vec) {
    vec.push_back({tid, fv, txt_name});
}

ripple::SField const& constructSField(int tid, int fv, const char* fn) {
    if (ripple::SField const& field = ripple::SField::getField(ripple::field_code(tid, fv)); field != ripple::sfInvalid)
        return field;
    return *(new ripple::SField(tid, fv, fn));
}

/*ripple::SField const & makeTypedField(int tid, int fv, const char* fn) {
    if (ripple::SField const& field = ripple::SField::getField(ripple::field_code(tid, fv)); field != ripple::sfInvalid)
        return field;
    return *(new ripple::TypedField<ripple::STPluginType>(tid, fv, fn));
}*/
