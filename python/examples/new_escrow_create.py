from plugin_transactor import (
    tesSUCCESS,
    temINVALID_FLAG,
    preflight1,
    preflight2,
    tf_universal_mask,
    fix1543,
    zero_amount,
    account_keylet,
    sf_account,
    sf_amount,
    temBAD_AMOUNT,
    sf_cancel_after,
    temBAD_EXPIRATION,
    sf_condition,
    fix1571,
    temMALFORMED,
    tecNO_PERMISSION,
    tecINTERNAL,
    sf_balance,
    STAmount,
    sf_owner_count,
    tecUNFUNDED,
    tecINSUFFICIENT_RESERVE,
    tecNO_DST,
    make_sle,
    tecDIR_FULL,
    sf_owner_node,
    adjust_owner_count,
    soeREQUIRED,
    soeOPTIONAL,
    sf_destination_tag,
    sf_ticket_sequence,
    STUInt32,
    construct_custom_sfield,
    bad_type,
    AccountID,
    make_stplugintype,
    invalid_data,
    sf_source_tag,
    sf_previous_txn_id,
    sf_previous_txn_lgr_seq,
    sf_destination_node,
    index_hash,
    Keylet,
    parse_base58,
    ConsequencesFactoryType,
    sf_destination,
)

from utils import LedgerObject, Transactor, create_new_sfield


sf_finish_after2 = create_new_sfield(STUInt32, "FinishAfter2", 47)

STI_ACCOUNT2 = 24


def parse_account2(field, json_name, field_name, _name, value):
    if not value.is_string():
        return None, bad_type(json_name, field_name)
    str_value = value.as_string()
    try:
        account = AccountID()
        if account.parse_hex(str_value):
            return make_stplugintype(field, account.to_buffer()), None

        if result := parse_base58(str_value):
            ret = make_stplugintype(field, result.to_buffer())
            return ret, None
        return None, invalid_data(json_name, field_name)
    except Exception as err:
        print("Error in parsing Account2:", err)
        return None, invalid_data(json_name, field_name)


# sf_destination2 = construct_custom_sfield(STI_ACCOUNT2, "Destination2", 1)

# new_stypes = [(STI_ACCOUNT2, parse_account2)]
sfields = [sf_finish_after2]  # , sf_destination2]


ltNEW_ESCROW = 0x74
NEW_ESCROW_NAMESPACE = ord('t')


def visit_entry_xrp_change_escrow(is_delete, entry, is_before):
    if is_before:
        return -1 * entry[sf_amount].xrp().drops
    if is_delete:
        return 0
    return entry[sf_amount].xrp().drops


ledger_objects = [
    LedgerObject(
        object_type=ltNEW_ESCROW,
        name="NewEscrow",
        rpc_name="new_escrow",
        object_format=[
            (sf_account,              soeREQUIRED),
            (sf_destination,          soeREQUIRED),
            (sf_amount,               soeREQUIRED),
            (sf_condition,            soeOPTIONAL),
            (sf_cancel_after,          soeOPTIONAL),
            (sf_finish_after2,          soeOPTIONAL),
            (sf_source_tag,            soeOPTIONAL),
            (sf_destination_tag,       soeOPTIONAL),
            (sf_owner_node,            soeREQUIRED),
            (sf_previous_txn_id,        soeREQUIRED),
            (sf_previous_txn_lgr_seq,    soeREQUIRED),
            (sf_destination_node,      soeOPTIONAL),
        ],
        is_deletion_blocker=True,
        visit_entry_xrp_change=visit_entry_xrp_change_escrow
    )
]


def new_escrow_keylet(src, seq):
    return Keylet(ltNEW_ESCROW, index_hash(NEW_ESCROW_NAMESPACE, src, seq))


def after(now, mark):
    return now.time_since_epoch() > mark


def preflight(ctx):
    if ctx.rules.enabled(fix1543) and ctx.tx.get_flags() & tf_universal_mask:
        print("Malformed transaction: Invalid flags set.")
        return temINVALID_FLAG

    if (preflight1ret := preflight1(ctx)) != tesSUCCESS:
        return preflight1ret

    amount = ctx.tx[sf_amount]
    if not amount.is_xrp():
        return temBAD_AMOUNT

    if amount <= zero_amount:
        return temBAD_AMOUNT

    if not ctx.tx.is_field_present(sf_cancel_after) and \
            not ctx.tx.is_field_present(sf_finish_after2):
        return temBAD_EXPIRATION

    if ctx.tx.is_field_present(sf_cancel_after) and \
        ctx.tx.is_field_present(sf_finish_after2) and \
            ctx.tx[sf_cancel_after] <= ctx.tx[sf_finish_after2]:
        return temBAD_EXPIRATION

    if ctx.rules.enabled(fix1571):
        if not ctx.tx.is_field_present(sf_finish_after2) and \
                not ctx.tx.is_field_present(sf_condition):
            return temMALFORMED

    # TODO: figure out the conditions logic

    return preflight2(ctx)


def do_apply(ctx, _m_prior_balance, _m_source_balance):
    close_time = ctx.view().info().parent_close_time

    if ctx.view().rules().enabled(fix1571):
        if ctx.tx.is_field_present(sf_cancel_after) and \
                after(close_time, ctx.tx[sf_cancel_after]):
            return tecNO_PERMISSION

        if ctx.tx.is_field_present(sf_finish_after2) and \
                after(close_time, ctx.tx[sf_finish_after2]):
            return tecNO_PERMISSION

    account = ctx.tx[sf_account]
    sle = ctx.view().peek(account_keylet(account))
    if not sle:
        return tecINTERNAL

    balance = STAmount(sle[sf_balance]).xrp()
    reserve = ctx.view().fees().account_reserve(sle[sf_owner_count] + 1)
    if balance < reserve:
        return tecINSUFFICIENT_RESERVE

    if balance < reserve + STAmount(ctx.tx[sf_amount]).xrp():
        return tecUNFUNDED

    dest_acct = ctx.tx[sf_destination] # AccountID.from_buffer(ctx.tx[sf_destination])

    sled = ctx.view().peek(account_keylet(dest_acct))
    if not sled:
        return tecNO_DST
    # TODO: implement dest tag check

    # TODO: implement deposit auth check

    keylet = new_escrow_keylet(account, ctx.tx.get_seq_proxy().value())
    slep = make_sle(keylet)
    slep[sf_account] = account
    amount = ctx.tx[sf_amount]
    slep[sf_amount] = amount
    slep[sf_destination] = ctx.tx[sf_destination]
    ctx.view().insert(slep)

    page = ctx.view().dir_insert(account, keylet)
    if page is None:
        return tecDIR_FULL
    slep[sf_owner_node] = page

    if (destination := dest_acct) != account:
        page2 = ctx.view().dir_insert(destination, keylet)
        if page2 is None:
            return tecDIR_FULL
        slep[sf_owner_node] = page2

    sle[sf_balance] = sle[sf_balance] - ctx.tx[sf_amount]
    adjust_owner_count(ctx.view(), sle, 1, ctx.journal)
    ctx.view().update(sle)

    return tesSUCCESS


transactors = [
    Transactor(
        name="NewEscrowCreate",
        tx_type=47,
        tx_format=[
            (sf_destination, soeREQUIRED),
            (sf_amount, soeREQUIRED),
            (sf_condition, soeOPTIONAL),
            (sf_cancel_after, soeOPTIONAL),
            (sf_finish_after2, soeOPTIONAL),
            (sf_destination_tag, soeOPTIONAL),
            (sf_ticket_sequence, soeOPTIONAL)
        ],
        consequences_factory_type=ConsequencesFactoryType.Normal,
        preflight=preflight,
        do_apply=do_apply,
    )
]