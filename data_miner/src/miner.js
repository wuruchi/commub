const request = require('request-promise')
const {searchURL, searchQuery} = require('../config')
const data = require('../data.json')

const getTweets = async (token, queryNext) => {

  const options = {
    uri: searchURL,
    qs: {
      query: searchQuery,
      next: queryNext,
    },
    headers: {
      Authorization: `Bearer ${token}`,
    },
    json: true,
  }
  const {results, next, requestParameters} = await request(options)

  const tweets = results
    .map(status => ({
      user: status.user.screen_name,
      text: status.text,
      mentions: status.entities.user_mentions.map(mention => mention.screen_name),
    }))
    .filter(tweet => tweet.mentions.length > 0)
  
  console.log(results.length, tweets.length, next, requestParameters)
  return {tweets, next}
}
 
module.exports = getTweets30
