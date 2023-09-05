const xrpl = require("xrpl");


const client = new xrpl.Client("ws://127.0.0.1:6006");

async function submit(tx, seed) {
  const result = await client.request({
      command: "submit",
      tx_json: tx,
      secret: seed,
  })
  console.log("SUBMITTED " + tx.TransactionType)
  console.log(result.result ?? result)

  await client.request({command: 'ledger_accept'})

  return await client.request({command: 'tx', transaction: result.result.tx_json.hash})
}

async function test() {
  await client.connect();
  const wallet = xrpl.Wallet.generate();
  console.log("connected");
  seed = "snoPBrXtMeMyMHUVTgbuqAfg1SUTb"

  submit({
    TransactionType: 'Payment',
    Account: "rHb9CJAWyB4rj91VRWn96DkukG4bwdtyTh",
    Amount: xrpl.xrpToDrops(1000000),
    Destination: wallet.address,
  }, seed)

  const response = await submit({
    TransactionType: 'NFTokenMint',
    Account: "rHb9CJAWyB4rj91VRWn96DkukG4bwdtyTh",
    NFTokenTaxon: 0,
    URI: xrpl.convertStringToHex("nft_example"),
    Flags: 0x00000008,
  }, seed)
  const nftoken_id = response.result.meta.nftoken_id
  console.log(nftoken_id)

  const response2 = await submit({
    TransactionType: 'NFTokenEscrowCreate',
    Account: "rHb9CJAWyB4rj91VRWn96DkukG4bwdtyTh",
    NFTokenID: nftoken_id,
    Destination: wallet.address,
    FinishAfter: 999999999,
  }, seed)
  console.log(response)
  console.log(JSON.stringify(response2.result, null, 4))

  await client.disconnect()
}

test()
