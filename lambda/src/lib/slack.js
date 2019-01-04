class Slack {
  static inChannel(text) {
    return {
      'response_type': 'in_channel',
      'text': text
    };
  }
}

module.exports = {
  Slack
}
