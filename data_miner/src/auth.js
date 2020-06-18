const request = require('request-promise')
const {authURL, credentials} = require('../config')

const auth = async () => {
  if (!authURL || !credentials) throw 'No config data for authorization'

  const credentialsString = `${encodeURIComponent(credentials.key)}:${encodeURIComponent(credentials.secret)}`

  const options = {
    method: 'POST',
    uri: authURL,
    form: { grant_type: 'client_credentials' },
    headers: {
      Authorization: `Basic ${Buffer.from(credentialsString).toString('base64')}`,
      'content-type': "application/x-www-form-urlencoded;charset=UTF-8"
    },
    json: true,
  }
  const res = await request(options)
  console.log(res.access_token)
  return res.access_token
}
 
module.exports = auth
