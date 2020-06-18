const fs = require('fs')
const {searchURL, searchQuery, tweetsCount, requestsCount} = require('../config')

const auth = require('./auth');
const getTweets = require('./miner');

(async () => {
  try {
    if (!searchURL || !searchQuery) throw 'No config data for mining'

    const filePath = `results/${searchQuery}.edges`
    if (fs.existsSync(filePath)) fs.unlinkSync(filePath)

    const token = await auth()
    const {tweets, next} = await getTweets(token)
    await writeEdgesToFile(filePath, tweets)
    let globalNext = next

    let counter = requestsCount
    while (counter > 0 && globalNext) {
      console.log(counter)
      const {tweets, next} = await getTweets(token, globalNext)
      await writeEdgesToFile(filePath, tweets)
      globalNext = next
      counter--
    }

  } catch (e) {
    console.log(e)
  }
})()

const writeEdgesToFile = async (filePath, tweets) => {
  tweets.map(tweet => {
    tweet.mentions.map(mention => {
      fs.appendFileSync(filePath, `${tweet.user}\t${mention}\t"${tweet.text.replace(/\n|\r/g, " ")}"\r\n`)
    })
  })
}
