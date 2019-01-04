const { Particle } = require('./lib/particle');
const { Slack } = require('./lib/slack');
const { Response } = require('./lib/http');
const { Temperature } = require('./lib/temperature');

const {
  DEVICE_ID,
  ACCESS_TOKEN = 'empty'
} = process.env;

exports.handler = async (event, context, callback) => {
  return new Promise((resolve, reject) => {
    var particle = new Particle(DEVICE_ID, ACCESS_TOKEN);

    particle.getVariable('temperature',
      function (res, json) {
        var celsius = Particle.getVariableValue(json);

        if (celsius !== false) {
          var temp = new Temperature(celsius);

          resolve(
            Response.success(
              Slack.inChannel(`It is currently *${temp.toStringFahrenheit()}* inside the domicile.`)
            )
          );
        } else {
          reject(Response.error('Unable to obtain valid data from the device API.', res.statusCode));
        }
      },
      function (err) {
        reject(Response.error(err));
      }
    );
  });
};
