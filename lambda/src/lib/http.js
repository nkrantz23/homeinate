class Response {
  static success(json, headers = { 'Content-Type': 'application/json' }) {
    return {
      statusCode: 200,
      headers: headers,
      body: JSON.stringify(json)
    }
  }

  static error(err, statusCode = 500, headers = { 'Content-Type': 'application/json' }) {
    return {
      statusCode: statusCode,
      headers: headers,
      body: JSON.stringify({ error: err })
    }
  }
}

module.exports = {
  Response
}