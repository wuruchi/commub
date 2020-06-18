# Miner for Twitter data

Set config variables in `config.js`:
* _credentials_ - Twitter API credentials
* _tweetsCount_ - Number of tweets per request
* _requestsCount_ - Number of maximum requests
* _searchQuery_ - Query for tweets filtering. It can be hashtag, user or some other criteria. More info in official Twitter API doc.

Run data miner with command `node src/index.js`.