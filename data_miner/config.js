const credentials = {
  key: "APP_KEY",
  secret: "APP_SECRET",
}

const authURL = "https://api.twitter.com/oauth2/token"
const searchURL = "https://api.twitter.com/1.1/tweets/search/30day/ENV.json"

const tweetsCount = 100
const requestsCount = 20
const searchQuery = "#BLM"

module.exports = {
  searchQuery,
  authURL,
  searchURL,
  credentials,
  tweetsCount,
  requestsCount,
}
