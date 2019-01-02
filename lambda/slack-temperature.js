var fetch = require('node-fetch');

const {
  DEVICE_ID,
  ACCESS_TOKEN
} = process.env;

exports.handler = async (event, context) => {
  return fetch(`https://api.particle.io/v1/devices/${DEVICE_ID}/slack`, {
    headers: {
      access_token: ACCESS_TOKEN
    },
    method: 'POST'
  })
    .then(() => ({ statusCode: 200 }))
    .catch(error => ({ statusCode: 422 }));
};
