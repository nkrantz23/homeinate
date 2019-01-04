const https = require('https')

class Particle {
  constructor(deviceId, accessToken) {
    this.deviceId = deviceId;
    this.accessToken = accessToken;
  }

  getVariable(name, onFinished, onError) {
    const options = this._options(name);

    this._request(
      options,
      onFinished,
      onError
    );
  }

  runFunction(name, onFinished, onErrort) {
    const options = this._options(name, 'POST');

    this._request(
      options,
      onFinished,
      onError
    );
  }

  _request(options, onFinished, onError) {
    const req = https.request(options, (res) => {
      var json = '';

      res.on('data', (chunk) => { json += chunk; });
      res.on('end', () => {
        onFinished(res, JSON.parse(json));
      });
    });

    req.on('error', (err) => { onError(err); });
    req.write('');
    req.end();
  }

  _options(path, method = 'GET') {
    return {
      hostname: 'api.particle.io',
      port: 443,
      path: `/v1/devices/${this.deviceId}/${path}?access_token=${this.accessToken}`,
      method: method,
      json: true
    };
  }

  static getVariableValue(json) {
    const {
      cmd,
      name,
      result
    } = json;

    if (cmd === 'VarReturn' && name === 'temperature') {
      return result;
    }

    return false;
  };
}

module.exports = {
  Particle
}
