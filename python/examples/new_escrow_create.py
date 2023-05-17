from plugin_transactor import (
    tesSUCCESS,
    temINVALID_FLAG,
    preflight1,
    preflight2,
    tfUniversalMask,
    fix1543,
    zeroAmount,
    accountKeylet,
    sfAccount,
    sfAmount,
    temBAD_AMOUNT,
    sfCancelAfter,
    sfFinishAfter,
    temBAD_EXPIRATION,
    sfCondition,
    fix1571,
    temMALFORMED,
    tecNO_PERMISSION,
    tecINTERNAL,
    sfBalance,
    STAmount,
    sfBalance,
    sfOwnerCount,
    tecUNFUNDED,
    tecINSUFFICIENT_RESERVE,
    sfDestination,
    tecNO_DST,
    makeSLE,
    escrowKeylet,
    ownerDirKeylet,
    describeOwnerDir,
    tecDIR_FULL,
    sfOwnerNode,
    adjustOwnerCount,
    soeREQUIRED,
    soeOPTIONAL,
    sfDestinationTag,
    sfTicketSequence,
)

tx_name = "NewEscrowCreate"
tx_type = 47

tx_format = [
    (sfDestination, soeREQUIRED),
    (sfAmount, soeREQUIRED),
    (sfCondition, soeOPTIONAL),
    (sfCancelAfter, soeOPTIONAL),
    (sfFinishAfter, soeOPTIONAL),
    (sfDestinationTag, soeOPTIONAL),
    (sfTicketSequence, soeOPTIONAL)
]


def after(now, mark):
    return now.time_since_epoch() > mark


def preflight(ctx):
    if ctx.rules.enabled(fix1543) and ctx.tx.getFlags() & tfUniversalMask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret
    
    amount = ctx.tx.getAmount(sfAmount)
    if not amount.is_xrp():
        return temBAD_AMOUNT
    
    # if amount <= zeroAmount:  # TODO: get this part working
    #     return temBAD_AMOUNT

    if not ctx.tx.isFieldPresent(sfCancelAfter) and \
        not ctx.tx.isFieldPresent(sfFinishAfter):
        return temBAD_EXPIRATION

    if ctx.tx.isFieldPresent(sfCancelAfter) and \
        ctx.tx.isFieldPresent(sfFinishAfter) and \
            ctx.tx.getU32(sfCancelAfter) <= ctx.tx.getU32(sfFinishAfter):
        return temBAD_EXPIRATION

    if ctx.rules.enabled(fix1571):
        if not ctx.tx.isFieldPresent(sfFinishAfter) and \
            not ctx.tx.isFieldPresent(sfCondition):
            return temMALFORMED
    
    # TODO: figure out the conditions logic

    return preflight2(ctx)

def preclaim(ctx):
    return tesSUCCESS

def doApply(ctx, _mPriorBalance, _mSourceBalance):
    close_time = ctx.view().info().parent_close_time

    if ctx.view().rules().enabled(fix1571):
        if ctx.tx.isFieldPresent(sfCancelAfter) and \
            after(close_time, ctx.tx.getU32(sfCancelAfter)):
            return tecNO_PERMISSION

        if ctx.tx.isFieldPresent(sfFinishAfter) and \
            after(close_time, ctx.tx.getU32(sfFinishAfter)):
            return tecNO_PERMISSION
    
    account = ctx.tx.getAccountID(sfAccount)
    sle = ctx.view().peek(accountKeylet(account))
    if not sle:
        return tecINTERNAL
    
    balance = STAmount(sle.getAmount(sfBalance)).xrp()
    reserve = ctx.view().fees().accountReserve(sle.getU32(sfOwnerCount) + 1)
    if balance < reserve:
        return tecINSUFFICIENT_RESERVE
    
    if balance < reserve + STAmount(ctx.tx.getAmount(sfAmount)).xrp():
        return tecUNFUNDED
    
    sled = ctx.view().peek(accountKeylet(ctx.tx.getAccountID(sfDestination)))
    if not sled:
        return tecNO_DST
    # TODO: implement dest tag check

    # TODO: implement deposit auth check

    keylet = escrowKeylet(account, ctx.tx.getSeqProxy().value())
    slep = makeSLE(keylet)
    slep.setAccountID(sfAccount, account)
    slep.setAmount(sfAmount, ctx.tx.getAmount(sfAmount))
    slep.setAccountID(sfDestination, ctx.tx.getAccountID(sfDestination))
    ctx.view().insert(slep)

    page = ctx.view().dirInsert(account, keylet)
    if page is None:
        return tecDIR_FULL
    slep.setU64(sfOwnerNode, page)

    if (destination := ctx.tx.getAccountID(sfDestination)) != account:
        page2 = ctx.view().dirInsert(destination, keylet)
        if page2 is None:
            return tecDIR_FULL
        slep.setU64(sfOwnerNode, page2)

    sle.setAmount(sfBalance, sle.getAmount(sfBalance) - ctx.tx.getAmount(sfAmount))
    adjustOwnerCount(ctx.view(), sle, 1, ctx.journal)
    ctx.view().update(sle)

    return tesSUCCESS
