
function preflight(ctx) {
  log("This is a JavaScript plugin")
  const preflight1ret = preflight1(ctx)
  log(preflight1ret)
  if (!isTesSuccess(preflight1ret)) return preflight1ret
  log(ctx)
  log(ctx.tx)
  log(ctx.tx.getFlags())
  log(ctx.tx.toString())
  log(fixMasterKeyAsRegularKey)
  // log(ctx.enabled({1:1}))
  return 0
}

function preclaim(ctx) {
  log("Preclaim Success!!!")
  log(ctx)
  log(ctx.tx)
  // log(ctx.enabled({1:1}))
  log(ctx.tx.account)
  return 0
}

function doApply(ctx, mPriorBalance, mSourceBalance) {
  log("doApply success!!!!")
  log(ctx)
  log(mPriorBalance)
  log(mSourceBalance)
  return 0
}
