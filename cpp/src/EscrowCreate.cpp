//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#include "EscrowCreate.h"
#include <ripple/conditions/Condition.h>
#include <ripple/conditions/Fulfillment.h>
#include <ripple/basics/Log.h>
#include <ripple/ledger/View.h>
#include <ripple/protocol/Feature.h>
#include <ripple/protocol/Indexes.h>
#include <ripple/protocol/Quality.h>
#include <ripple/protocol/st.h>
#include <ripple/protocol/ErrorCodes.h>
#include <ripple/beast/core/LexicalCast.h>
#include <ripple/protocol/digest.h>
#include <ripple/protocol/TxFlags.h>


typedef ripple::SField const& (*createNewSFieldPtr)(
    int tid,
    int fv,
    const char* fn);
typedef ripple::STBase* (*constructSTypePtr)(ripple::SerialIter& sit, ripple::SField const& name);
typedef ripple::STBase* (*constructSTypePtr2)(ripple::SField const& name);
typedef std::optional<ripple::detail::STVar> (*parseLeafTypePtr)(
    ripple::SField const&,
    std::string const&,
    std::string const&,
    ripple::SField const*,
    Json::Value const&,
    Json::Value&);

typedef std::int64_t (*visitEntryXRPChangePtr)(
    bool isDelete,
    std::shared_ptr<ripple::SLE const> const& entry,
    bool isBefore);

struct STypeExport {
    int typeId;
    createNewSFieldPtr createPtr;
    parseLeafTypePtr parsePtr;
    constructSTypePtr constructPtr;
    constructSTypePtr2 constructPtr2;
};

static const std::uint16_t ltNEW_ESCROW = 0x0074;
static const std::uint16_t NEW_ESCROW_NAMESPACE = 't';


namespace ripple {

template <class... Args>
static uint256
indexHash(std::uint16_t space, Args const&... args)
{
    return sha512Half(space, args...);
}

Keylet
new_escrow(AccountID const& src, std::uint32_t seq) noexcept
{
    return {ltNEW_ESCROW, indexHash(NEW_ESCROW_NAMESPACE, src, seq)};
}

const int STI_UINT32_2 = 24;

class STUInt32_2 : public STUInt32
{
using STUInt32::STUInt32;

STUInt32_2(STUInt32 num) : STUInt32(num.value())
{
}

STBase*
copy(std::size_t n, void* buf) const
{
    return emplace(n, buf, *this);
}

STBase*
move(std::size_t n, void* buf)
{
    return emplace(n, buf, std::move(*this));
}

int
getSType() const
{
    return STI_UINT32_2;
}
};

using SF_UINT32_2 = TypedField<STUInt32_2>;

template <class T>
STBase*
constructNewSType(SerialIter& sit, SField const& name)
{
    T* stype = new T(sit, name);
    return stype;
}

template <class T>
STBase*
constructNewSType2(SField const& name)
{
    return new T(name);
}

template <class T>
SField const&
createNewSType(int tid, int fv, const char* fn)
{
    if (SField const& field = SField::getField(field_code(tid, fv)); field != sfInvalid)
        return field;
    // TODO: refactor
    // probably not a memory leak because the constructor adds the object to a map
    return *(new T(tid, fv, fn));
}

std::optional<detail::STVar>
parseLeafTypeNew(
    SField const& field,
    std::string const& json_name,
    std::string const& fieldName,
    SField const* name,
    Json::Value const& value,
    Json::Value& error)
{
    // copied from parseLeafType<STUInt32>
    std::optional<detail::STVar> ret;
    try
    {
        if (value.isString())
        {
            ret = detail::make_stvar<STUInt32_2>(
                field,
                beast::lexicalCastThrow<std::uint32_t>(
                    value.asString()));
        }
        else if (value.isInt())
        {
            ret = detail::make_stvar<STUInt32_2>(
                field, to_unsigned<std::uint32_t>(value.asInt()));
        }
        else if (value.isUInt())
        {
            ret = detail::make_stvar<STUInt32_2>(
                field, safe_cast<std::uint32_t>(value.asUInt()));
        }
        else
        {
            error = bad_type(json_name, fieldName);
            return ret;
        }
        return ret;
    }
    catch (std::exception const&)
    {
        error = invalid_data(json_name, fieldName);
        return ret;
    }
}

// helper stuff that needs to be moved to rippled

template <typename T>
int getSTId() { return 0; }

template <>
int getSTId<SF_AMOUNT>() { return STI_AMOUNT; }

template <> 
int getSTId<SF_ACCOUNT>() { return STI_ACCOUNT; }

template <> 
int getSTId<SF_UINT32>() { return STI_UINT32; }

template <> 
int getSTId<SF_UINT32_2>() { return STI_UINT32_2; }



template <class T>
T const&
newSField(const int fieldValue, char const* fieldName)
{
    if (SField const& field = SField::getField(fieldName); field != sfInvalid)
        return static_cast<T const&>(field);
    T const* newSField = new T(getSTId<T>(), fieldValue, fieldName);
    return *newSField;
}

template <class T>
T const&
newSField(const int fieldValue, std::string const fieldName)
{
    return newSField<T>(fieldValue, fieldName.c_str());
}

// end of helper stuff

SF_UINT32_2 const&
sfQualityIn2()
{
    return newSField<SF_UINT32_2>(47, "QualityIn2");
}

/** Has the specified time passed?

    @param now  the current time
    @param mark the cutoff point
    @return true if \a now refers to a time strictly after \a mark, else false.
*/
static inline bool
after(NetClock::time_point now, std::uint32_t mark)
{
    return now.time_since_epoch().count() > mark;
}

NotTEC
preflight(PreflightContext const& ctx)
{
    if (ctx.rules.enabled(fix1543) && ctx.tx.getFlags() & tfUniversalMask)
        return temINVALID_FLAG;

    if (auto const ret = preflight1(ctx); !isTesSuccess(ret))
        return ret;

    if (!isXRP(ctx.tx[sfAmount]))
        return temBAD_AMOUNT;

    if (ctx.tx[sfAmount] < beast::zero)
        return temBAD_AMOUNT;

    // We must specify at least one timeout value
    if (!ctx.tx[~sfCancelAfter] && !ctx.tx[~sfFinishAfter])
        return temBAD_EXPIRATION;

    // If both finish and cancel times are specified then the cancel time must
    // be strictly after the finish time.
    if (ctx.tx[~sfCancelAfter] && ctx.tx[~sfFinishAfter] &&
        ctx.tx[sfCancelAfter] <= ctx.tx[sfFinishAfter])
        return temBAD_EXPIRATION;

    if (ctx.rules.enabled(fix1571))
    {
        // In the absence of a FinishAfter, the escrow can be finished
        // immediately, which can be confusing. When creating an escrow,
        // we want to ensure that either a FinishAfter time is explicitly
        // specified or a completion condition is attached.
        if (!ctx.tx[~sfFinishAfter] && !ctx.tx[~sfCondition])
            return temMALFORMED;
    }

    if (auto const cb = ctx.tx[~sfCondition])
    {
        using namespace ripple::cryptoconditions;

        std::error_code ec;

        auto condition = Condition::deserialize(*cb, ec);
        if (!condition)
        {
            JLOG(ctx.j.debug())
                << "Malformed condition during escrow creation: "
                << ec.message();
            return temMALFORMED;
        }

        // Conditions other than PrefixSha256 require the
        // "CryptoConditionsSuite" amendment:
        if (condition->type != Type::preimageSha256 &&
            !ctx.rules.enabled(featureCryptoConditionsSuite))
            return temDISABLED;
    }

    return preflight2(ctx);
}

TER
preclaim(PreclaimContext const& ctx)
{
    return tesSUCCESS;
}

TER
doApply(ApplyContext& ctx, XRPAmount mPriorBalance, XRPAmount mSourceBalance)
{
    auto const closeTime = ctx.view().info().parentCloseTime;

    // Prior to fix1571, the cancel and finish times could be greater
    // than or equal to the parent ledgers' close time.
    //
    // With fix1571, we require that they both be strictly greater
    // than the parent ledgers' close time.
    if (ctx.view().rules().enabled(fix1571))
    {
        if (ctx.tx[~sfCancelAfter] && after(closeTime, ctx.tx[sfCancelAfter]))
            return tecNO_PERMISSION;

        if (ctx.tx[~sfFinishAfter] && after(closeTime, ctx.tx[sfFinishAfter]))
            return tecNO_PERMISSION;
    }
    else
    {
        if (ctx.tx[~sfCancelAfter])
        {
            auto const cancelAfter = ctx.tx[sfCancelAfter];

            if (closeTime.time_since_epoch().count() >= cancelAfter)
                return tecNO_PERMISSION;
        }

        if (ctx.tx[~sfFinishAfter])
        {
            auto const finishAfter = ctx.tx[sfFinishAfter];

            if (closeTime.time_since_epoch().count() >= finishAfter)
                return tecNO_PERMISSION;
        }
    }

    auto const account = ctx.tx[sfAccount];
    auto const sle = ctx.view().peek(keylet::account(account));
    if (!sle)
        return tefINTERNAL;

    // Check reserve and funds availability
    {
        auto const balance = STAmount((*sle)[sfBalance]).xrp();
        auto const reserve =
            ctx.view().fees().accountReserve((*sle)[sfOwnerCount] + 1);

        if (balance < reserve)
            return tecINSUFFICIENT_RESERVE;

        if (balance < reserve + STAmount(ctx.tx[sfAmount]).xrp())
            return tecUNFUNDED;
    }

    // Check destination account
    {
        auto const sled =
            ctx.view().read(keylet::account(ctx.tx[sfDestination]));
        if (!sled)
            return tecNO_DST;
        if (((*sled)[sfFlags] & lsfRequireDestTag) &&
            !ctx.tx[~sfDestinationTag])
            return tecDST_TAG_NEEDED;

        // Obeying the lsfDissalowXRP flag was a bug.  Piggyback on
        // featureDepositAuth to remove the bug.
        if (!ctx.view().rules().enabled(featureDepositAuth) &&
            ((*sled)[sfFlags] & lsfDisallowXRP))
            return tecNO_TARGET;
    }

    // Create escrow in ledger.  Note that we we use the value from the
    // sequence or ticket.  For more explanation see comments in SeqProxy.h.
    Keylet const escrowKeylet =
        new_escrow(account, ctx.tx.getSeqProxy().value());
    auto const slep = std::make_shared<SLE>(escrowKeylet);
    (*slep)[sfAmount] = ctx.tx[sfAmount];
    (*slep)[sfAccount] = account;
    (*slep)[~sfCondition] = ctx.tx[~sfCondition];
    (*slep)[~sfSourceTag] = ctx.tx[~sfSourceTag];
    (*slep)[sfDestination] = ctx.tx[sfDestination];
    (*slep)[~sfCancelAfter] = ctx.tx[~sfCancelAfter];
    (*slep)[~sfFinishAfter] = ctx.tx[~sfFinishAfter];
    (*slep)[~sfDestinationTag] = ctx.tx[~sfDestinationTag];

    ctx.view().insert(slep);

    // Add escrow to sender's owner directory
    {
        auto page = ctx.view().dirInsert(
            keylet::ownerDir(account), escrowKeylet, describeOwnerDir(account));
        if (!page)
            return tecDIR_FULL;
        (*slep)[sfOwnerNode] = *page;
    }

    // If it's not a self-send, add escrow to recipient's owner directory.
    if (auto const dest = ctx.tx[sfDestination]; dest != ctx.tx[sfAccount])
    {
        auto page = ctx.view().dirInsert(
            keylet::ownerDir(dest), escrowKeylet, describeOwnerDir(dest));
        if (!page)
            return tecDIR_FULL;
        (*slep)[sfDestinationNode] = *page;
    }

    // Deduct owner's balance, increment owner count
    (*sle)[sfBalance] = (*sle)[sfBalance] - ctx.tx[sfAmount];
    adjustOwnerCount(ctx.view(), sle, 1, ctx.journal);
    ctx.view().update(sle);

    return tesSUCCESS;
}

}  // namespace ripple



extern "C"
ripple::NotTEC
preflight(ripple::PreflightContext const& ctx)
{
    return ripple::preflight(ctx);
}

extern "C"
ripple::TER
preclaim(ripple::PreclaimContext const& ctx)
{
    return ripple::preclaim(ctx);
}

extern "C"
ripple::XRPAmount
calculateBaseFee(ripple::ReadView const& view, ripple::STTx const& tx)
{
    return ripple::Transactor::calculateBaseFee(view, tx);
}

extern "C"
ripple::TER
doApply(
    ripple::ApplyContext& ctx,
    ripple::XRPAmount mPriorBalance,
    ripple::XRPAmount mSourceBalance)
{
    return ripple::doApply(ctx, mPriorBalance, mSourceBalance);
}

extern "C"
char const*
getTxName()
{
    return "NewEscrowCreate";
}

struct FakeSOElement {
    int fieldCode;
    ripple::SOEStyle style;
};

extern "C"
std::vector<FakeSOElement>
getTxFormat()
{
    return std::vector<FakeSOElement>{
        {ripple::sfDestination.getCode(), ripple::soeREQUIRED},
        {ripple::sfAmount.getCode(), ripple::soeREQUIRED},
        {ripple::sfCondition.getCode(), ripple::soeOPTIONAL},
        {ripple::sfCancelAfter.getCode(), ripple::soeOPTIONAL},
        {ripple::sfFinishAfter.getCode(), ripple::soeOPTIONAL},
        {ripple::sfDestinationTag.getCode(), ripple::soeOPTIONAL},
        {ripple::sfTicketSequence.getCode(), ripple::soeOPTIONAL},
    };
}

struct SFieldInfo {
    int typeId;
    int fieldValue;
    const char * txtName;
};

extern "C"
std::vector<STypeExport>
getSTypes()
{
    // registerSType(ripple::STI_UINT32_2, ripple::createNewSType<ripple::SF_UINT32_2>);
    return std::vector<STypeExport>{
        // {
        //     ripple::STI_UINT32_2,
        //     ripple::createNewSType<ripple::SF_UINT32_2>,
        //     ripple::parseLeafTypeNew,
        //     ripple::constructNewSType<ripple::STUInt32_2>,
        //     ripple::constructNewSType2<ripple::STUInt32_2>
        // },
    };
}

extern "C"
std::vector<SFieldInfo>
getSFields()
{
    // auto const& sfQualityIn2 = ripple::sfQualityIn2();
    return std::vector<SFieldInfo>{
        // {sfQualityIn2.fieldType, sfQualityIn2.fieldValue, sfQualityIn2.fieldName.c_str()},
    };
}

extern "C"
std::uint16_t
getTxType()
{
    return 30;
}

extern "C"
std::string
getTTName()
{
    return "ttESCROW_CREATE2";
}

std::int64_t visitEntryXRPChange(
    bool isDelete,
    std::shared_ptr<ripple::SLE const> const& entry,
    bool isBefore)
{
    if (isBefore) {
        return -1 * (*entry)[ripple::sfAmount].xrp().drops();
    }
    if (isDelete) return 0;
    return (*entry)[ripple::sfAmount].xrp().drops();
}

struct LedgerObjectInfo {
    std::uint16_t objectType;
    char const* objectName; // CamelCase
    char const* objectRpcName; // snake_case
    std::vector<FakeSOElement> objectFormat;
    bool isDeletionBlocker;
    visitEntryXRPChangePtr visitEntryXRPChange;
    // FakeSOElement[] innerObjectFormat; // optional
};

extern "C"
std::vector<LedgerObjectInfo>
getLedgerObjects()
{
    return std::vector<LedgerObjectInfo>{
        {
            ltNEW_ESCROW,
            "NewEscrow",
            "new_escrow",
            std::vector<FakeSOElement>{
                {ripple::sfAccount.getCode(),              ripple::soeREQUIRED},
                {ripple::sfDestination.getCode(),          ripple::soeREQUIRED},
                {ripple::sfAmount.getCode(),               ripple::soeREQUIRED},
                {ripple::sfCondition.getCode(),            ripple::soeOPTIONAL},
                {ripple::sfCancelAfter.getCode(),          ripple::soeOPTIONAL},
                {ripple::sfFinishAfter.getCode(),          ripple::soeOPTIONAL},
                {ripple::sfSourceTag.getCode(),            ripple::soeOPTIONAL},
                {ripple::sfDestinationTag.getCode(),       ripple::soeOPTIONAL},
                {ripple::sfOwnerNode.getCode(),            ripple::soeREQUIRED},
                {ripple::sfPreviousTxnID.getCode(),        ripple::soeREQUIRED},
                {ripple::sfPreviousTxnLgrSeq.getCode(),    ripple::soeREQUIRED},
                {ripple::sfDestinationNode.getCode(),      ripple::soeOPTIONAL},
            },
            false,
            visitEntryXRPChange
        }
    };
}
