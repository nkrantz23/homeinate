const https = require('https')

const {
  DEVICE_ID,
  ACCESS_TOKEN = 'empty'
} = process.env;

exports.handler = async (event, context, callback) => {
  return new Promise(particle);
};

function headers() {
  return {
    'Content-Type': 'application/json'
  };
};

function error(reject, err, code = 500) {
  reject({
    statusCode: code,
    headers: headers(),
    body: JSON.stringify({ error: err })
  })
};

function temperature(json) {
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

function particle(resolve, reject) {
  const options = {
    hostname: 'api.particle.io',
    port: 443,
    path: `/v1/devices/${DEVICE_ID}/temperature?access_token=${ACCESS_TOKEN}`,
    method: 'GET',
    json: true
  };

  const req = https.request(options, (res) => {
    var json = '',
      onResponseEnd = () => {
        const temp = temperature(JSON.parse(json));

        if (temp === false) {
          error(reject, 'Bad response from Particle', res.statusCode);
        } else {
          var msg = `It's currently ${stringTemperature(fahrenheit(temp))} in the domicile.`;
          resolve({
            statusCode: res.statusCode,
            headers: headers(),
            body: slack(msg)
          });
        }
      };
    res.on('data', (chunk) => {
      json += chunk;
    });

    res.on('end', onResponseEnd);
  });

  req.on('error', (error) => {
    error(reject, error);
  });

  req.write('');
  req.end();
}

function slack(text) {
  return JSON.stringify({
    'response_type': 'in_channel',
    'text': text
  });
}

function fahrenheit(cel) {
  var cel = Number(cel);

  return (cel * (9 / 5)) + 32;
}

function stringTemperature(temp, unit = 'F') {
  return `${temp.toFixed(0)} ${unit}`;
}
