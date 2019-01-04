const https = require('https')

const {
  DEVICE_ID,
  ACCESS_TOKEN = 'empty'
} = process.env;

exports.handler = async (event, context, callback) => {
  return new Promise(particle);
};

function particle(resolve, reject) {
  const headers = {
    'Content-Type': 'application/json'
  };

  const options = {
    hostname: 'api.particle.io',
    port: 443,
    path: `/v1/devices/${DEVICE_ID}/slack?access_token=${ACCESS_TOKEN}`,
    method: 'POST',
    json: true
  };

  const req = https.request(options, (res) => {
    var json = '',
      onResponseEnd = () => {
        if (res.ok) {
          resolve({
            statusCode: 200,
            headers: headers,
            body: JSON.stringify({ status: 'ok' })
          });
        } else {
          resolve({
            statusCode: res.statusCode,
            headers: headers,
            body: json
          });
        }
      };
    res.on('data', (chunk) => {
      json += chunk;
    });

    res.on('end', onResponseEnd);
  });

  req.on('error', (error) => {
    console.log(error);
    reject({
      statusCode: 500,
      headers: headers,
      body: JSON.stringify({ error: error })
    });
  });

  req.write('');
  req.end();
}
